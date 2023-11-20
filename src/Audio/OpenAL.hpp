#pragma once
#include "Framework.hpp"
#include "SoundIO.hpp"

#include <al.h>
#include <alc.h>


// According to the OpenAL documentation https://openal.org/documentation/OpenAL_Programmers_Guide.pdf 
//  If source is attached to a buffer with STEREO sound, then this source is no longer affected by distance calculations.
//  Not sure how it affects our code, but it could mean that gain setting of a STEREO sound would stay unnafected??


// Changing gain and pitch.
//  Gain and pitch can be changed per sound, source and listener!
//  To allow gain above 1.0 the "AL_MAX_GAIN" must be set to the greater value!


// There is still that bug with having 4 buffers processed if the programm runs for long enough... dunno why. Maybe its connected to the PCM end somehow?

namespace OpenAL {

    const float MAX_GAIN = 10.0f;

    auto CheckError(
        const char* message
    ) {
        const ALenum errorCode = alGetError();
        switch (errorCode) {
            case AL_NO_ERROR:
                return;
            case AL_INVALID_NAME:
                std::cerr << OPENAL_ERROR << "AL_INVALID_NAME" << " with: " << message << std::endl;
                return;
            case AL_INVALID_ENUM:
                std::cerr << OPENAL_ERROR << "AL_INVALID_ENUM" << " with: " << message << std::endl;
                return;
            case AL_INVALID_VALUE:
                std::cerr << OPENAL_ERROR << "AL_INVALID_VALUE" << " with: " << message << std::endl;
                return;
            case AL_INVALID_OPERATION:
                std::cerr << OPENAL_ERROR << "AL_INVALID_OPERATION" << " with: " << message << std::endl;
                return;
            case AL_OUT_OF_MEMORY:
                std::cerr << OPENAL_ERROR << "AL_OUT_OF_MEMORY" << " with: " << message << std::endl;
                return;
            default:
                std::cerr << OPENAL_ERROR << "UNKNOWN: " << errorCode << " with: " << message << std::endl;
                return;
        }
    }


	auto CreateAudioDevice() {

        // First arg - device, nullptr means get default.
        const ALCchar* defaultDeviceString = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
        ALCdevice* device = alcOpenDevice(defaultDeviceString);

        if (device == nullptr) {
            std::cerr << OPENAL_FAIL_GET_DEFAULT_DEVICE << std::endl;
            exit(FAILURE);
        }

        return device;
	}


    auto CreateAudioContext(ALCdevice* device) {

        // Create context for that device. ( It's a state machine with its own sound sources. )
        ALCcontext* context = alcCreateContext(device, /*attributelist*/ nullptr);

        // Activates the context!
        if (!alcMakeContextCurrent(context)) { 
            std::cerr << "Failed to make the context active." << std::endl;
            exit(FAILURE);
        }

        return context;
    }


    // Creates a listener in 3D space. ( User that hears the sound. )
    auto CreateListener3D() {

        const ALfloat forwardAndUpVectors[] {
            /* forward */   1.0f, 0.0f, 0.0f,
            /* up */        0.0f, 1.0f, 0.0f
        };

        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        OpenAL::CheckError("listener-position");
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        OpenAL::CheckError("listener-velocity");
        alListenerfv(AL_ORIENTATION, forwardAndUpVectors);
        OpenAL::CheckError("listener-orientation");

    }

    auto CreateMonoSound(SoundIO::ReadWavData& monoData) {
        ALuint monoSoundBuffer;

        alGenBuffers(1, &monoSoundBuffer);
        OpenAL::CheckError("gen-buffers-mono");

        alBufferData(
            monoSoundBuffer,
            monoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, /* 16bit depth */
            monoData.pcm.data(),
            (ALsizei) monoData.pcm.size() * 2 /* two bytes per sample */,
            monoData.sampleRate
        );

        OpenAL::CheckError("set-buffer-mono");

        return monoSoundBuffer;
    }

    namespace Buffered {

        using Buffor4 = array<ALuint, 4>;

        const Buffor4 emptyBuffor { NULL };

        const size_t BUFFER_SIZE (4096);//(2048);//(1024);

        struct BufferQueue {
            //size_t bufferSize;
            Buffor4 buffers;
        };

        //thread* ;
        bool isThreadStop (false);

        // Buffer queuing loop must operate in a new thread
        ALint iTotalBuffersProcessed = 0;

        // MONO!
        auto ThreadQueueBufforLoop(const SoundIO::ReadWavData& monoData, const ALuint& monoSource, const size_t& bufferSize) {
            while (!isThreadStop) {

                ALint iBuffersProcessed = 0;

                Sleep(10); // Sleep 10 msec periodically.
                //spdlog::info("thread-call!");

                alGetSourcei(monoSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
                iTotalBuffersProcessed += iBuffersProcessed; 

                spdlog::info(iBuffersProcessed); // AT STOP THERES A BUG! With alSourceStop(monoSource) call has enought time to be called and always sets all buffors as processed. THIS MIGHT BE DANGEROUS.

                // For each processed buffer, remove it from the source queue, read the next chunk of
                // audio data from the file, fill the buffer with new data, and add it to the source queue
                while (iBuffersProcessed != 0) {

                    // Remove the buffer from the queue (uiBuffer contains the buffer ID for the dequeued buffer)
                    ALuint uiBuffer = 0;
                    alSourceUnqueueBuffers(monoSource, 1, &uiBuffer);
                    OpenAL::CheckError("unqueue");
                    // Read more pData audio data (if there is any)

                    //spdlog::info(uiBuffer);

                    // Copy audio data to buffer 
                    alBufferData(uiBuffer, AL_FORMAT_MONO16, monoData.pcm.data(), bufferSize, monoData.sampleRate);
                    OpenAL::CheckError("buffer");

                    // Insert the audio buffer to the source queue
                    alSourceQueueBuffers(monoSource, 1, &uiBuffer);
                    OpenAL::CheckError("queue");

                    iBuffersProcessed--;
                }
            }

            spdlog::info("stopping a sound!");

            isThreadStop = false;
        }


        auto PlaySound(const ALuint& monoSource, const SoundIO::ReadWavData& monoData, const size_t& bufferSize) {

            spdlog::info("play sound call!");

            // This guard ensures that the thread finished its task!
            //  initially it allows starting this thread.
            //  when we're playing a sound this function is not callable. Only "StopSound()" is! 
            if (isThreadStop == false) { 

                alSourcePlay(monoSource);
                OpenAL::CheckError("play_sound");

                spdlog::info("playing a sound!");

                thread threadQueueBuffor(&ThreadQueueBufforLoop, monoData, monoSource, bufferSize);
                threadQueueBuffor.detach(); // see
            }
        }

        auto StopSound(const ALuint& monoSource, const ALint& state) {
            switch (state) {
                case AL_INITIAL:
                case AL_STOPPED:
                case AL_PAUSED: {
                    isThreadStop = false;
                } break;

                case AL_PLAYING:
                    isThreadStop = true;
            }

            alSourceStop(monoSource);
        }


        auto CreateMonoSound(SoundIO::ReadWavData& monoData, BufferQueue& bufferQueue) {

            alGenBuffers(bufferQueue.buffers.size(), bufferQueue.buffers.data());
            OpenAL::CheckError("gen-buffers-mono");


            for (size_t i = 0; i < bufferQueue.buffers.size(); ++i) {

                alBufferData(
                    bufferQueue.buffers[i],
                    AL_FORMAT_MONO16,
                    monoData.pcm.data(),
                    BUFFER_SIZE,
                    monoData.sampleRate
                );

                OpenAL::CheckError("create-buffer-mono");
            }

            spdlog::info("Created Mono Sound");
            //OpenAL::CheckError("call");

        }

        auto CreateMonoSource(BufferQueue& bufferQueue, const ALboolean& islooped = false, const ALfloat& pitch = 1.0f, const ALfloat& gain = 1.0f) {
            ALuint monoSource;

            alGenSources(1, &monoSource);
            OpenAL::CheckError("1");


            alSource3f(monoSource, AL_POSITION, 1.f, 0.f, 0.f);
            OpenAL::CheckError("2");
            alSource3f(monoSource, AL_VELOCITY, 0.f, 0.f, 0.f);
            OpenAL::CheckError("3");
            alSourcef(monoSource, AL_PITCH, pitch);
            OpenAL::CheckError("4");
            alSourcef(monoSource, AL_GAIN, gain);
            OpenAL::CheckError("5");
            alSourcei(monoSource, AL_LOOPING, islooped);
            OpenAL::CheckError("6");

            for (size_t i = 0; i < bufferQueue.buffers.size(); ++i) {
                alSourceQueueBuffers(monoSource, 1, &bufferQueue.buffers[i]);
                OpenAL::CheckError("set-queue-mono");
            }

            // Set max gain for that sound. // Requires source.
            alSourcef(monoSource, AL_MAX_GAIN, OpenAL::MAX_GAIN);
            OpenAL::CheckError("set-max_gain");

            return monoSource;
        }


        auto DestorySound(BufferQueue& bufferQueue) {

            for (size_t i = 0; i < bufferQueue.buffers.size(); ++i) {
                alDeleteBuffers(1, &bufferQueue.buffers[i]);
            }

            OpenAL::CheckError("delete buffer");
        }

    }

    auto CreateStereoSound(SoundIO::ReadWavData& stereoData) {

        ALuint stereoSoundBuffer;
        alGenBuffers(1, &stereoSoundBuffer);
        OpenAL::CheckError("gen-buffers-stereo");

        alBufferData(
            stereoSoundBuffer,
            stereoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, /* 16bit depth */
            stereoData.pcm.data(),
            (ALsizei) stereoData.pcm.size() * 2 /* two bytes per sample */,
            stereoData.sampleRate
        );

        OpenAL::CheckError("set-buffer-stereo");

        return stereoSoundBuffer;
    }



    // Create a sound source that plays our mono sound. ( From the sound buffer. )
    auto CreateMonoSource(const ALuint& mSoundBuffer, const ALboolean& islooped = false, const ALfloat& pitch = 1.0f, const ALfloat& gain = 1.0f) {
        
        ALuint monoSource;
        alGenSources(1, &monoSource);
        OpenAL::CheckError("1");



        alSource3f(monoSource, AL_POSITION, 1.f, 0.f, 0.f);
        OpenAL::CheckError("2");
        alSource3f(monoSource, AL_VELOCITY, 0.f, 0.f, 0.f);
        OpenAL::CheckError("3");
        alSourcef(monoSource, AL_PITCH, pitch);
        OpenAL::CheckError("4");
        alSourcef(monoSource, AL_GAIN, gain);
        OpenAL::CheckError("5");
        alSourcei(monoSource, AL_LOOPING, islooped);
        OpenAL::CheckError("6");

        alSourcei(monoSource, AL_BUFFER, mSoundBuffer);
        OpenAL::CheckError("7");

        return monoSource;
    }

    // Create a sound source for our stereo sound; note 3d positioning doesn't work with stereo files because
    //  stereo files are typically used for music. stereo files come out of both ears so it is hard to know
    //  what the sound should be doing based on 3d position data.
    auto CreateStereoSource(const ALuint& sSoundBuffer, const ALfloat& pitch = 1.0f, const ALfloat& gain = 1.0f) {
        
        ALuint stereoSource;
        alGenSources(1, &stereoSource);
        OpenAL::CheckError("8");

        //alec(alSource3f(stereoSource, AL_POSITION, 0.f, 0.f, 1.f)); //NOTE: this does not work like mono sound positions!
        //alec(alSource3f(stereoSource, AL_VELOCITY, 0.f, 0.f, 0.f)); 

        alSourcef(stereoSource, AL_PITCH, pitch);
        OpenAL::CheckError("9");
        alSourcef(stereoSource, AL_GAIN, gain);
        OpenAL::CheckError("10");
        alSourcei(stereoSource, AL_LOOPING, AL_FALSE);
        OpenAL::CheckError("11");

        alSourcei(stereoSource, AL_BUFFER, sSoundBuffer);
        OpenAL::CheckError("12");

        return stereoSource;
    }

    auto PlaySound(const ALuint& source, ALint& sourceState) {

        alSourcePlay(source);
        OpenAL::CheckError("13");

        alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
        OpenAL::CheckError("14");

        //while (sourceState == AL_PLAYING) {
        //    // Loop until we're done playing the sound.
        //    // https://learn.microsoft.com/en-us/dotnet/api/opentk.audio.openal.algetsourcei?view=xamarin-ios-sdk-12
        //    // I believe that audio could be changed while its playing using these. If not the audio could always be 
        //    //  stopped at and played from a specific sample.
        //    alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
        //    OpenAL::CheckError("15");
        //}
    }

    //auto StopSound(const ALuint& source) {
    //    alSourceStop(source);
    //}


    auto DestroySource(ALuint& source) {
        alDeleteSources(1, &source);
        OpenAL::CheckError("delete source");
    }


    auto DestorySound(ALuint& soundBuffer) {
        alDeleteBuffers(1, &soundBuffer);
        OpenAL::CheckError("delete buffer");
    }


    auto DestoryContext(ALCcontext* context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }

    
    auto DestoryDevice(ALCdevice* device) {
        alcCloseDevice(device);
    }

}