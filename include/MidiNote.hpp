#pragma once
#include "MidiPitch.hpp"

#include <algorithm>

struct Velocity
{
  int8_t velocity;

  explicit Velocity(int8_t const vel)
      : velocity(std::clamp(
            vel, constants::audio::midi::min, constants::audio::midi::max))
  {
  }
};

struct MidiNote
{
  MidiPitch  midi_note;
  velocity_t velocity;
};