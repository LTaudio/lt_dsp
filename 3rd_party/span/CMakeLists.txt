project(lt_span)

add_library(lt_span INTERFACE)
add_library(lt::lt_span ALIAS lt_span)
target_sources(lt_span INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}//tcbspan/span.hpp)
target_include_directories(lt_span INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})