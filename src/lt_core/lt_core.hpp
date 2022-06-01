/*
 BEGIN_JUCE_MODULE_DECLARATION

 ID:            lt_core
 vendor:        lt
 version:       0.1.0
 name:          Core
 description:   JUCE core extensions and utilities.
 dependencies:  juce_core

 END_JUCE_MODULE_DECLARATION
 */

#pragma once

#ifndef USE_LT_CORE
#define USE_LT_CORE 1
#endif

#include <juce_core/juce_core.h>

// clang-format off
#include "iterator/IndexIterator.hpp"
#include "container/CircularBuffer.hpp"
// clang-format on