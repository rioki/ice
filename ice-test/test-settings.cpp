// ice engine
// Copyright 2026 Sean Farrell
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

#include <ice/ice.h>
#include <ice/glm_njson.h>

#include "test-env.h"

#include <filesystem>
#include <fstream>
#include <random>
#include <string>

#include <gtest/gtest.h>
#include "glm_gtest.h"

TEST(Settings, fall_back)
{
    auto settings = ice::Settings{};

    EXPECT_EQ("some string value", settings.get<std::string>("test", "string", "some string value"));
    EXPECT_EQ(6.9f, settings.get("test", "float", 6.9f));
    EXPECT_EQ(42, settings.get("test", "int", 42));
}

TEST(Settings, set_get)
{
    auto settings = ice::Settings{};

    settings.set("test", "string", "currect string");
    settings.set("test", "float", -13.13f);
    settings.set("test", "int", -1337);

    EXPECT_EQ("currect string", settings.get<std::string>("test", "string", "some string value"));
    EXPECT_EQ(-13.13f, settings.get("test", "float", 6.9f));
    EXPECT_EQ(-1337, settings.get("test", "int", 42));
}

TEST(Settings, save_load)
{
    auto file = testing::get_test_output() / (testing::get_test_name() + ".json");

    auto a = ice::Settings{};

    a.set("test", "string", "currect string");
    a.set("test", "float", -13.13f);
    a.set("test", "int", -1337);
    a.save(file);

    auto b = ice::Settings{file};

    EXPECT_EQ("currect string", b.get<std::string>("test", "string"));
    EXPECT_EQ(-13.13f, b.get<float>("test", "float"));
    EXPECT_EQ(-1337, b.get<int>("test", "int"));
}


TEST(Settings, glm)
{
    auto settings = ice::Settings{};

    settings.set("test", "uvec2", glm::uvec2(800u, 600u));
    settings.set("test", "vec4",  glm::vec4(1.2f, 2.3f, 3.4f, 4.5f));

    EXPECT_GLM_EQ(glm::uvec2(800, 600), settings.get("test", "uvec2", glm::uvec2(0u)));
    EXPECT_GLM_EQ(glm::vec4(1.2f, 2.3f, 3.4f, 4.5f), settings.get("test", "vec4",glm::vec4(0.0f)));
}

TEST(Settings, glm_save_load)
{
    auto file = testing::get_test_output() / (testing::get_test_name() + ".json");

    auto a = ice::Settings{};

    a.set("test", "uvec2", glm::uvec2(800u, 600u));
    a.set("test", "vec4",  glm::vec4(1.2f, 2.3f, 3.4f, 4.5f));
    a.save(file);

    auto b = ice::Settings{file};

    EXPECT_GLM_EQ(glm::uvec2(800, 600), b.get("test", "uvec2", glm::uvec2(0u)));
    EXPECT_GLM_EQ(glm::vec4(1.2f, 2.3f, 3.4f, 4.5f), b.get("test", "vec4",glm::vec4(0.0f)));
}

