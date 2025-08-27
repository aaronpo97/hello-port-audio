#pragma once
#include "../include/constants.hpp"
#include <cstdint>

#include <cmath>

/**
 * \enum MidiNote
 * MIDI note numbers, grouped by octave.
 */
enum class MidiNote : uint8_t
{
    // Octave 0
    C0 = 12,
    CX0,
    D0,
    DX0,
    E0,
    F0,
    FX0,
    G0,
    GX0,
    A0,
    AX0,
    B0,

    // Octave 1
    C1,
    CX1,
    D1,
    DX1,
    E1,
    F1,
    FX1,
    G1,
    GX1,
    A1,
    AX1,
    B1,

    // Octave 2
    C2,
    CX2,
    D2,
    DX2,
    E2,
    F2,
    FX2,
    G2,
    GX2,
    A2,
    AX2,
    B2,

    // Octave 3
    C3,
    CX3,
    D3,
    DX3,
    E3,
    F3,
    FX3,
    G3,
    GX3,
    A3,
    AX3,
    B3,

    // Octave 4 (Middle C = 60, A4 = 69)
    C4,
    CX4,
    D4,
    DX4,
    E4,
    F4,
    FX4,
    G4,
    GX4,
    A4,
    AX4,
    B4,

    // Octave 5
    C5,
    CX5,
    D5,
    DX5,
    E5,
    F5,
    FX5,
    G5,
    GX5,
    A5,
    AX5,
    B5,

    // Octave 6
    C6,
    CX6,
    D6,
    DX6,
    E6,
    F6,
    FX6,
    G6,
    GX6,
    A6,
    AX6,
    B6,

    // Octave 7
    C7,
    CX7,
    D7,
    DX7,
    E7,
    F7,
    FX7,
    G7,
    GX7,
    A7,
    AX7,
    B7,

    // Octave 8
    C8
};

constexpr float midi_to_frequency(MidiNote midi_no)
{
    float const exponent = (static_cast<float>(midi_no) - 69.0f) / 12.0f;
    return std::powf(2, exponent) *
           static_cast<float>(constants::audio::standard_A4_hz);
}
