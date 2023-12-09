#pragma once
#include "Framework.hpp"
#include "SoundIO.hpp"
#include "AudioEffect.h"

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


        const size_t INITIAL_BUFFERS_COUNT = 4;                 // Number of buffers we fill during sound creation.
        const size_t BUFFER_SIZE(4096);                         //(2048); //(1024);
        const size_t BUFFER_SIZE_HALF(BUFFER_SIZE / 2);


        using Buffor4 = array<ALuint, INITIAL_BUFFERS_COUNT>;
        using EffectsQueue = vector<std::unique_ptr<AudioEffect>>;
        using EffectsQueueCopy = vector<AudioEffect>;

        const Buffor4 emptyBuffor { NULL };


        struct BufferQueue {
            ALuint buffersTotal;
            Buffor4 buffers;
        };


        ALuint buffersProcessedTotal = INITIAL_BUFFERS_COUNT;   // Keep the buffer count so we don't go beyond it.
        EffectsQueue effectsQueue;
        bool isThreadStop(false);
        ALint isLooped = false;


        // ! It should be new/delete with sound play / stop
        // and it's size would be like the orginal times 3 or similar.
        const size WET_SOUND_RESERVED_SIZE { 48 };
        int16_t wetSoundReservedBuffor[WET_SOUND_RESERVED_SIZE * BUFFER_SIZE] { 0 };


        auto QueueBufforProcedure(const ALuint& monoSource, const SoundIO::ReadWavData& monoDrySound, const size_t& bufferSize, const size_t& bufforIndex) {
            
            // Remove buffer from the queue (uiBuffer contains the buffer ID for the dequeued buffer)
            ALuint uiBuffer = 0;
            alSourceUnqueueBuffers(monoSource, 1, &uiBuffer);
            OpenAL::CheckError("unqueue");


            //spdlog::info(bufforIndex);


            // 1. Load the dry sound in.
            // WRONG!!!!
            //std::memcpy(wetSoundReservedBuffor, monoDrySound.pcm.data(), monoDrySound.pcm.size());

            // Copy audio data to buffer 
            const int16_t* drySoundChunk = monoDrySound.pcm.data() + (bufforIndex * BUFFER_SIZE_HALF);

            for (auto&& effect : effectsQueue) {
                //effect->applyEffect(monoDrySound, bufforIndex, wetSoundChunk);
                effect->applyEffect();
            }

            alBufferData(uiBuffer, AL_FORMAT_MONO16, drySoundChunk, bufferSize, monoDrySound.sampleRate);
            OpenAL::CheckError("buffer");


            // Insert the audio buffer to the source queue
            alSourceQueueBuffers(monoSource, 1, &uiBuffer);
            OpenAL::CheckError("queue");
        }


        namespace Rewrite {


            // To ensure that data inside does not change form the moment of calculating total buffers size
            //  To the moment we're calculating processed sound as the data inside effectsQueue changes via other thread. 
            //EffectsQueueCopy effectsQueueThreadSafeCopy;


            auto BufforQueuingLoop(
                /*cpy*/ const SoundIO::ReadWavData monoDrySound,    // Original (dry) sound data.
                /*cpy*/ const ALuint monoSource,                    // Audio source for mono. 
                /*cpy*/ const size bufferSize                       // Size of each buffer
            ) {
                ALint buffersProcessed (0);                         // Initialize storage for information about processed buffers.
                size wetBuffersTotal (0);                           // Initialize storage for information about wet sound buffers.

                while (!isThreadStop) {                             // Function runs in a loop until triggerted to stop.

                    // Create a copy of effectsQueue.
                    //effectsQueueThreadSafeCopy = effectsQueue.
                    //for (auto&& effect : effectsQueue) {
                    //    effectsQueueThreadSafeCopy[0] = 
                    //}

                    // Calculate amount of wet sound buffers.
                    size wetSoundSize = monoDrySound.pcm.size();    // 1. Load the dry sound size in.

                    // 2. Add each effect size change to it.
                    for (auto&& effect : effectsQueue) {            
                        effect->getProcessedSize(wetSoundSize, monoDrySound.sampleRate, wetSoundSize);
                    }

                    // Get information about processed buffers.
                    alGetSourcei(monoSource, AL_BUFFERS_PROCESSED, &buffersProcessed);

                    // Get information about total wet sound buffers.
                    wetBuffersTotal = wetSoundSize / BUFFER_SIZE;
                    wetBuffersTotal += (wetSoundSize % BUFFER_SIZE) > 0;

                    // If total amount of effects takes to much memory to allocate then thor an error!
                    if (wetBuffersTotal > WET_SOUND_RESERVED_SIZE) {
                        spdlog::error("Error to much wet buffors. Imposible Allocation! Tot: {}, Proc: {}", wetBuffersTotal, buffersProcessedTotal);
                        throw;
                    }
                    
                    const size wetBuffersTotalMono = (wetBuffersTotal - 1) * 2;    // It's Mono! Therefore *2.

                    // The amount of total wet buffers changes. So we need to check
                    //  whether we still would have sound data in the very next buffor or not.
                    //  If not. We either end this play iteration if looped or finish the thread otherwise.
                    if (buffersProcessedTotal < wetBuffersTotalMono) {

                        if (buffersProcessedTotal < wetBuffersTotalMono - buffersProcessed) {   // For all except last queuing.

                            for (ALuint i = buffersProcessedTotal; i < buffersProcessedTotal + buffersProcessed; ++i) {
                                QueueBufforProcedure(monoSource, monoDrySound, bufferSize, i);
                            }

                            buffersProcessedTotal += buffersProcessed;

                            Sleep(10);                                                      // Sleep 10 msec periodically.

                        } else { // Last queuing

                            for (ALuint i = buffersProcessedTotal; i < wetBuffersTotalMono; ++i) {
                                QueueBufforProcedure(monoSource, monoDrySound, bufferSize, i);
                            }

                            if (isLooped) {
                                buffersProcessedTotal = 0;
                                continue;
                            }

                            spdlog::info("Sound stopped playing naturally! {}, {}", wetBuffersTotalMono, buffersProcessedTotal);
                            isThreadStop = true; // Trigger exit from thread!
                            // EXITS

                        }

                    } else {

                        if (isLooped) {
                            buffersProcessedTotal = 0;
                            continue;
                        } else {

                            spdlog::info("Sound stopped playing naturally! {}, {}", wetBuffersTotalMono, buffersProcessedTotal);
                            isThreadStop = true; // Trigger exit from thread!
                            // EXITS

                        }

                    }

                }

                buffersProcessedTotal = INITIAL_BUFFERS_COUNT; // Reset processed buffers count.
                isThreadStop = false; // Reset TRIGGER.

            }

        }

        // MONO!
        // Buffer queuing loop must operate in a new thread
        auto ThreadQueueBufforLoop(const SoundIO::ReadWavData monoDrySound, const ALuint monoSource, const size_t bufferSize, const size_t buffersTotal) {
            while (!isThreadStop) {

                ALuint buffersProcessed = 0;
                alGetSourcei(monoSource, AL_BUFFERS_PROCESSED, (ALint*)&buffersProcessed);


                
                // Calculate WetSoundSize from effectQueue so we know with how many buffors we're about to deal with.
                //
                size_t wetSoundSize = monoDrySound.pcm.size();
                size_t temp = 0;


                for (auto&& effect : effectsQueue) {
                    effect->getProcessedSize(wetSoundSize, monoDrySound.sampleRate, temp);
                    wetSoundSize = temp;
                }

                uint16_t calcBuffersTotal = wetSoundSize / BUFFER_SIZE;
                calcBuffersTotal += (wetSoundSize % BUFFER_SIZE) > 0;
                spdlog::info("CB: {}", calcBuffersTotal);
                

                const size_t buffersTotalMono = (buffersTotal - 1) * 2;
                //const size_t buffersTotalMono = (calcBuffersTotal - 1) * 2;


                if (buffersProcessedTotal < buffersTotalMono - buffersProcessed) { // Run normal without last ones
                    
                    // For each processed buffer, remove it from the source queue, read the next chunk of
                    // audio data from the file, fill the buffer with new data, and add it to the source queue
                    for (ALuint i = buffersProcessedTotal; i < buffersProcessedTotal + buffersProcessed; ++i) {
                        QueueBufforProcedure(monoSource, monoDrySound, bufferSize, i);
                    }

                    buffersProcessedTotal += buffersProcessed;

                    Sleep(10); // Sleep 10 msec periodically.

                    // If buffersTotalMono changes with applied delay effects making a shorter sound we than need to check
                    //  if we are still away from the very end of the track. Else iterate/stop.
                    if (buffersTotalMono <= buffersProcessedTotal) {

                        if (isLooped) {
                            buffersProcessedTotal = 0;
                            continue;
                        }

                        spdlog::info("Sound stopped playing shorter delay buffers! {}, {}", buffersTotalMono, buffersProcessedTotal);
                        isThreadStop = true; // Trigger exit from thread!
                        // EXITS

                    }

                } else { // Run last ones
                    
                    for (ALuint i = buffersProcessedTotal; i < buffersTotalMono; ++i) {
                        QueueBufforProcedure(monoSource, monoDrySound, bufferSize, i);
                    }

                    if (isLooped) {
                        buffersProcessedTotal = 0;
                        continue;
                    }

                    spdlog::info("Sound stopped playing naturally! {}, {}", buffersTotalMono, buffersProcessedTotal);
                    isThreadStop = true; // Trigger exit from thread!
                    // EXITS
                }

            }

            buffersProcessedTotal = INITIAL_BUFFERS_COUNT; // Reset processed buffers count.
            isThreadStop = false; // Reset TRIGGER.
        }


        auto PlaySound(
            const ALuint& monoSource, 
            const SoundIO::ReadWavData& monoWetData, 
            BufferQueue& soundQueueBuffers,
            const size_t& bufferSize, 
            const size_t& bufferSizeHalf, 
            const size_t& buffersTotal
        ) {

            spdlog::info("Hit Play Sound!");

            // This guard ensures that the thread finished its task!
            //  initially it allows starting this thread.
            //  when we're playing a sound this function is not callable. Only "StopSound()" is! 
            //if (isThreadStop == false) { 
            // spdlog::info("Thread safe Play Sound!");

            // 1. DEQUEUE all buffers.
            alSourcei(monoSource, AL_BUFFER, 0);

            // 2. Queue new initial buffers.
            for (size_t j = 0; j < soundQueueBuffers.buffers.size(); ++j) {

                ALvoid* dataPtr = (ALvoid*)(monoWetData.pcm.data() + (j * bufferSizeHalf));

                // Copy audio data to buffer 
                alBufferData(soundQueueBuffers.buffers[j], AL_FORMAT_MONO16, dataPtr, bufferSize, monoWetData.sampleRate);
                OpenAL::CheckError("create-buffer-mono");

                alSourceQueueBuffers(monoSource, 1, &soundQueueBuffers.buffers[j]);
                OpenAL::CheckError("set-queue-mono");
            }

            // 3. Play the sound.
            alSourcePlay(monoSource);
            OpenAL::CheckError("play_sound");

            spdlog::info("Playing a sound!");

            isThreadStop = false;
            thread threadQueueBuffor(&ThreadQueueBufforLoop, monoWetData, monoSource, bufferSize, buffersTotal);
            //thread threadQueueBuffor(&Rewrite::BufforQueuingLoop, monoWetData, monoSource, bufferSize);
            threadQueueBuffor.detach(); // see
        }

        auto StopSound(const ALuint& monoSource, const ALint& state) {
            isThreadStop = true;
            alSourceStop(monoSource);
        }


        // 2 bytes per sample so 1024 buffer holds 512 samples!

        auto CreateMonoSoundBuffers(BufferQueue& bufferQueue) {
            alGenBuffers(bufferQueue.buffers.size(), bufferQueue.buffers.data());
            OpenAL::CheckError("gen-buffers-mono");
            spdlog::info("Created Mono Sound");
        }

        auto CreateMonoSource(const ALboolean& islooped = false, const ALfloat& pitch = 1.0f, const ALfloat& gain = 1.0f) {
            ALuint monoSource;

            alGenSources(1, &monoSource);
            OpenAL::CheckError("1");

            // Set max gain for that sound. // Requires source.
            alSourcef(monoSource, AL_MAX_GAIN, OpenAL::MAX_GAIN);
            OpenAL::CheckError("set-max_gain");

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