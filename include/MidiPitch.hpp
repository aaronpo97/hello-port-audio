#pragma once
#include <array>
#include <cmath>
#include <string_view>

#include "../include/constants.hpp"

enum class MidiPitch : int8_t
{
  // Octave -1
  C_NEG1 = 0,
  CX_NEG1,
  D_NEG1,
  DX_NEG1,
  E_NEG1,
  F_NEG1,
  FX_NEG1,
  G_NEG1,
  GX_NEG1,
  A_NEG1,
  AX_NEG1,
  B_NEG1,

  // Octave 0
  C0,
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
  C8,
  CX8,
  D8,
  DX8,
  E8,
  F8,
  FX8,
  G8,
  GX8,
  A8,
  AX8,
  B8,

  // Octave 9
  C9,
  CX9,
  D9,
  DX9,
  E9,
  F9,
  FX9,
  G9
};

// Lookup table
static constexpr std::array<std::string_view, 128> MidiNoteNames = {
    // Octave -1
    "C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1",
    "A#-1", "B-1",
    // Octave 0
    "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
    // Octave 1
    "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
    // Octave 2
    "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
    // Octave 3
    "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
    // Octave 4
    "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
    // Octave 5
    "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
    // Octave 6
    "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
    // Octave 7
    "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
    // Octave 8
    "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
    // Octave 9
    "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9"};

constexpr float midi_pitch_to_frequency(MidiPitch midi_no)
{
  float const exponent = (static_cast<float>(midi_no) - 69.0f) / 12.0f;
  return std::powf(2, exponent) *
         static_cast<float>(constants::audio::standard_A4_hz);
}
