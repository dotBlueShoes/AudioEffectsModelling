#include "Framework.hpp"

namespace Display {

    // Display in Console Bits.
    auto PrintFromTo (
        int16_t* data,
        const uint16_t& to = 0,
        const uint16_t& from = 0
    ) {
        std::bitset<16> bitset;

        for (uint16_t current = from; current <= to; ++current) {
            bitset = data[current];
            std::cout << current << "\t: " << bitset << "\n";
        }

        std::cout << std::endl;
        
    }
}