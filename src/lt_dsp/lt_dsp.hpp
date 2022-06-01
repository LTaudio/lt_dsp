/*
 BEGIN_JUCE_MODULE_DECLARATION

 ID:            lt_dsp
 vendor:        lt
 version:       0.1.0
 name:          DSP
 description:   JUCE dsp extensions and utilities.
 dependencies:  juce_audio_processors, juce_dsp, juce_data_structures

 END_JUCE_MODULE_DECLARATION
 */

#pragma once

#ifndef USE_LT_DSP
#define USE_LT_DSP 1
#endif

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>

// clang-format off
#include "iterator/IndexIterator.hpp"
#include "container/CircularBuffer.hpp"
#include "processor/OverlapAddProcessor.hpp"
// clang-format on