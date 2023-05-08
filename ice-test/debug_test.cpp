// Ice Engine
// Copyright 2023 Sean Farrell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <ice/debug.h>
#include <ice/utils.h>

#include <gtest/gtest.h>

#include "DebugMonitor.h"

using namespace std::chrono_literals;

TEST(debug, trace)
{
    auto debug_monitor = ice::test::DebugMonitor{};

    ice::trace("Ups! Did I do that?");
    ice::trace("Ok... So, what now?");

    std::this_thread::sleep_for(100ms);
    const auto ref = "debug_test.cpp(35): TestBody: Ups! Did I do that?\n"
                     "debug_test.cpp(36): TestBody: Ok... So, what now?\n";
    EXPECT_EQ(ref, debug_monitor.get_output());
}


TEST(debug, fail)
{
    auto debug_monitor = ice::test::DebugMonitor{true};

    EXPECT_DEATH(ice::fail(), "");

    std::this_thread::sleep_for(100ms);
    const auto ref = "debug_test.cpp(49): TestBody: failed\n";
    EXPECT_EQ(ref, debug_monitor.get_output());
}

TEST(debug, ensure_true)
{
    auto debug_monitor = ice::test::DebugMonitor{true};

    ice::ensure(true);

    std::this_thread::sleep_for(100ms);
    const auto ref = "";
    EXPECT_EQ(ref, debug_monitor.get_output());
}

TEST(debug, ensure_false)
{
    auto debug_monitor = ice::test::DebugMonitor{true};

    EXPECT_DEATH(ice::ensure(false), "");

    std::this_thread::sleep_for(100ms);
    const auto ref = "debug_test.cpp(71): TestBody: ensure failed\n";
    EXPECT_EQ(ref, debug_monitor.get_output());
}

TEST(debug, require_true)
{
    auto debug_monitor = ice::test::DebugMonitor{true};

    ice::require(true);

    std::this_thread::sleep_for(100ms);
    const auto ref = "";
    EXPECT_EQ(ref, debug_monitor.get_output());
}

TEST(debug, require_false)
{
    auto debug_monitor = ice::test::DebugMonitor{true};

    EXPECT_DEATH(ice::require(false), "");

    std::this_thread::sleep_for(100ms);
    const auto ref = "debug_test.cpp(93): TestBody: require failed\n";
    EXPECT_EQ(ref, debug_monitor.get_output());
}

