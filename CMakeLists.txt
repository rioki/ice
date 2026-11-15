# ice engine
# Copyright 2026 Sean Farrell
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 4.2)

project(adastra
    VERSION 0.1.0
    LANGUAGES CXX
)

include(CTest)
include(GoogleTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

add_library(ice SHARED
    ice/Engine.cpp
)

target_include_directories(ice PUBLIC . ${RSIG_INCLUDE_DIRS})
target_compile_definitions(ice PRIVATE ICE_BUILD)
set_target_properties(ice PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

add_executable(ice-test
    ice-test/test-engine.cpp
)
target_compile_definitions(ice-test PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_link_libraries(ice-test PRIVATE ice GTest::gtest GTest::gtest_main)
target_compile_definitions(ice-test PRIVATE PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

gtest_discover_tests(ice-test)

add_executable(ice-lab
    ice-lab/main.cpp
)

target_link_libraries(ice-lab PRIVATE ice)
