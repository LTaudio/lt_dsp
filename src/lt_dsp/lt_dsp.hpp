/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 lt_dsp
  vendor:             LTaudio
  version:            0.1.0
  name:               DSP
  description:        JUCE dsp extensions and utilities.
  website:            https://github.com/LTaudio/lt_modules
  license:            Boost Software License 1.0
  minimumCppStandard: 20
  dependencies:       juce_audio_basics, juce_dsp, lt_core

 END_JUCE_MODULE_DECLARATION
*/

#pragma once

#ifndef USE_LT_DSP
#define USE_LT_DSP 1
#endif

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <lt_core/lt_core.hpp>

// clang-format off
#include "processor/OverlapAddProcessor.hpp"
// clang-format on