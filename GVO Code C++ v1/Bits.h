#pragma once
#include <cstdint>

inline constexpr uint8_t NorthBit  = 0x01;
inline constexpr uint8_t SouthBit  = 0x02;
inline constexpr uint8_t EastBit   = 0x04;
inline constexpr uint8_t WestBit   = 0x08;

// ManualBit is in the *first* nibble, same numeric value but different byte
inline constexpr uint8_t ManualBit = 0x01;
