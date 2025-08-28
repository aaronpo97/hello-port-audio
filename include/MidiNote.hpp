#pragma once
#include "MidiPitch.hpp"

using velocity_t = int8_t;

struct MidiNote
{
    MidiPitch midi_note;
    velocity_t velocity;
};