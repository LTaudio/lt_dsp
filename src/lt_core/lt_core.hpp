/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 lt_core
  vendor:             LTaudio
  version:            0.1.0
  name:               Core
  description:        JUCE core extensions and utilities.
  website:            https://github.com/LTaudio/lt_modules
  license:            Boost Software License 1.0
  minimumCppStandard: 20
  dependencies:       juce_core

 END_JUCE_MODULE_DECLARATION
*/

#pragma once

#ifndef USE_LT_CORE
#define USE_LT_CORE 1
#endif

#include <juce_core/juce_core.h>

// clang-format off
#include "types/Cast.hpp"
#include "iterator/IndexIterator.hpp"
#include "container/CircularBuffer.hpp"
// clang-format on