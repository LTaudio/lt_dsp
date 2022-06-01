#include <catch2/catch_session.hpp>

#include <juce_events/juce_events.h>

auto main(int const argc, char const* argv[]) -> int
{
    auto init = juce::ScopedJuceInitialiser_GUI{};
    return Catch::Session().run(argc, argv);
}