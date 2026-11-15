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
#include "test-env.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>


void write_file(const std::filesystem::path& name, const std::string_view contents)
{
    std::ofstream file;
    file.exceptions(std::ios::failbit | std::ios::badbit);
    file.open(name, std::ios::binary | std::ios::trunc);

    file.write(contents.data(),
               static_cast<std::streamsize>(contents.size()));
    file.close();
}

TEST(Settings, init)
{
    const auto settings = ice::Settings{};

    EXPECT_FALSE(settings.has("video", "width"));
    EXPECT_THROW(settings.get<int>("video", "width"), std::runtime_error);
    EXPECT_EQ(800, settings.get<int>("video", "width", 800));
}

TEST(Settings, stores_supported_value_types)
{
    auto settings = ice::Settings{};
    settings.set("values", "integer", -42);
    settings.set("values", "unsigned", std::numeric_limits<std::uint64_t>::max());
    settings.set("values", "real", 0.125);
    settings.set("values", "boolean", true);
    settings.set("values", "string", std::string("Ice Engine"));
    settings.set("values", "literal", "hello");
    settings.set("values", "array", std::vector<int>{1, 2, 3});
    settings.set("values", "object", nlohmann::json{{"nested", 42}});

    EXPECT_EQ(-42, settings.get<int>("values", "integer"));
    EXPECT_EQ(std::numeric_limits<std::uint64_t>::max(),
        settings.get<std::uint64_t>("values", "unsigned"));
    EXPECT_DOUBLE_EQ(0.125, settings.get<double>("values", "real"));
    EXPECT_TRUE(settings.get<bool>("values", "boolean"));
    EXPECT_EQ("Ice Engine", settings.get<std::string>("values", "string"));
    EXPECT_EQ("hello", settings.get<std::string>("values", "literal"));
    EXPECT_EQ((std::vector<int>{1, 2, 3}), settings.get<std::vector<int>>("values", "array"));
    EXPECT_EQ((nlohmann::json{{"nested", 42}}), settings.get<nlohmann::json>("values", "object"));
}

TEST(Settings, has_checks_both_section_and_key)
{
    auto settings = ice::Settings{};
    settings.set("video", "width", 1920);

    EXPECT_TRUE(settings.has("video", "width"));
    EXPECT_FALSE(settings.has("video", "height"));
    EXPECT_FALSE(settings.has("audio", "width"));
    EXPECT_FALSE(settings.has("Video", "width"));
    EXPECT_FALSE(settings.has("video", "Width"));
}

TEST(Settings, sections_keep_values_independent)
{
    auto settings = ice::Settings{};
    settings.set("video", "enabled", true);
    settings.set("audio", "enabled", false);
    settings.set("video", "width", 1920);

    EXPECT_TRUE(settings.get<bool>("video", "enabled"));
    EXPECT_FALSE(settings.get<bool>("audio", "enabled"));
    EXPECT_EQ(1920, settings.get<int>("video", "width"));
}

TEST(Settings, set_replaces_value_and_type)
{
    auto settings = ice::Settings{};
    settings.set("video", "width", 800);
    settings.set("video", "height", 600);
    settings.set("video", "width", 1920);
    EXPECT_EQ(1920, settings.get<int>("video", "width"));

    settings.set("video", "width", "automatic");
    EXPECT_EQ("automatic", settings.get<std::string>("video", "width"));
    EXPECT_EQ(600, settings.get<int>("video", "height"));
}

TEST(Settings, names_are_literal_and_may_be_empty)
{
    auto settings = ice::Settings{};
    settings.set("", "", 1);
    settings.set("video.display", "size/width", 2);
    settings.set("/video", "~width", 3);
    settings.set("日本語", "名前", "氷");

    EXPECT_TRUE(settings.has("", ""));
    EXPECT_EQ(1, settings.get<int>("", ""));
    EXPECT_EQ(2, settings.get<int>("video.display", "size/width"));
    EXPECT_EQ(3, settings.get<int>("/video", "~width"));
    EXPECT_EQ("氷", settings.get<std::string>("日本語", "名前"));
    EXPECT_FALSE(settings.has("video", "display.size/width"));
}

TEST(Settings, missing_section_throws_with_context)
{
    const auto settings = ice::Settings{};
    try
    {
        settings.get<int>("video", "width");
        FAIL() << "Expected a missing-section error";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_STREQ("No value for video.width", error.what());
    }
}

TEST(Settings, missing_key_throws_with_context)
{
    auto settings = ice::Settings{};
    settings.set("video", "height", 600);
    try
    {
        settings.get<int>("video", "width");
        FAIL() << "Expected a missing-key error";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_STREQ("No value for video.width", error.what());
    }
}

TEST(Settings, fallback_handles_missing_sections_and_keys_without_inserting)
{
    auto settings = ice::Settings{};
    settings.set("video", "height", 600);

    EXPECT_EQ(800, settings.get<int>("video", "width", 800));
    EXPECT_EQ("default", settings.get<std::string>("audio", "device", "default"));
    EXPECT_FALSE(settings.has("video", "width"));
    EXPECT_FALSE(settings.has("audio", "device"));
    EXPECT_THROW(settings.get<int>("video", "width"), std::runtime_error);
}

TEST(Settings, fallback_does_not_replace_existing_falsy_values)
{
    auto settings = ice::Settings{};
    settings.set("values", "zero", 0);
    settings.set("values", "false", false);
    settings.set("values", "empty", "");

    EXPECT_EQ(0, settings.get<int>("values", "zero", 42));
    EXPECT_FALSE(settings.get<bool>("values", "false", true));
    EXPECT_EQ("", settings.get<std::string>("values", "empty", "fallback"));
}

TEST(Settings, incompatible_types_throw_even_with_fallback)
{
    auto settings = ice::Settings{};
    settings.set("video", "width", "wide");
    settings.set("audio", "enabled", true);

    EXPECT_THROW(settings.get<int>("video", "width"), nlohmann::json::type_error);
    EXPECT_THROW(settings.get<int>("video", "width", 800), nlohmann::json::type_error);
    EXPECT_THROW(settings.get<std::string>("audio", "enabled"), nlohmann::json::type_error);
}

TEST(Settings, null_is_present_but_not_a_typed_value)
{
    auto settings = ice::Settings{};
    settings.set("video", "width", nullptr);

    EXPECT_TRUE(settings.has("video", "width"));
    EXPECT_TRUE(settings.get<nlohmann::json>("video", "width").is_null());
    EXPECT_THROW(settings.get<int>("video", "width"), nlohmann::json::type_error);
    EXPECT_THROW(settings.get<int>("video", "width", 800), nlohmann::json::type_error);
}

TEST(Settings, get_returns_independent_values)
{
    auto settings = ice::Settings{};
    settings.set("input", "keys", std::vector<int>{1, 2});
    auto keys = settings.get<std::vector<int>>("input", "keys");
    keys.push_back(3);

    EXPECT_EQ((std::vector<int>{1, 2}), settings.get<std::vector<int>>("input", "keys"));
}

TEST(Settings, copy_construction_is_independent)
{
    auto original = ice::Settings{};
    original.set("video", "width", 800);
    auto copy = ice::Settings{original};
    EXPECT_EQ(800, copy.get<int>("video", "width"));

    copy.set("video", "width", 1920);
    original.set("video", "height", 600);
    EXPECT_EQ(800, original.get<int>("video", "width"));
    EXPECT_FALSE(copy.has("video", "height"));
}

TEST(Settings, copy_assignment_replaces_data_and_is_independent)
{
    auto original = ice::Settings{};
    original.set("video", "width", 800);
    auto copy = ice::Settings{};
    copy.set("audio", "volume", 50);
    copy = original;

    EXPECT_EQ(800, copy.get<int>("video", "width"));
    EXPECT_FALSE(copy.has("audio", "volume"));
    copy.set("video", "width", 1920);
    EXPECT_EQ(800, original.get<int>("video", "width"));
}

TEST(Settings, move_construction_transfers_data)
{
    auto original = ice::Settings{};
    original.set("video", "width", 800);
    auto moved = ice::Settings{std::move(original)};

    EXPECT_TRUE(moved.has("video", "width"));
    EXPECT_EQ(800, moved.get<int>("video", "width"));
}

TEST(Settings, move_assignment_replaces_data)
{
    auto original = ice::Settings{};
    original.set("video", "width", 800);
    auto moved = ice::Settings{};
    moved.set("audio", "volume", 50);
    moved = std::move(original);

    EXPECT_EQ(800, moved.get<int>("video", "width"));
    EXPECT_FALSE(moved.has("audio", "volume"));
}

TEST(Settings, loads_json_sections_and_types)
{
    const auto file = testing::get_test_output() / "settings.json";
    write_file(file, R"({"video":{"width":1920,"scale":1.25,"fullscreen":false},
        "audio":{"device":"default","channels":[1,2]},"empty":{}})");
    const auto settings = ice::Settings{file};

    EXPECT_EQ(1920, settings.get<int>("video", "width"));
    EXPECT_DOUBLE_EQ(1.25, settings.get<double>("video", "scale"));
    EXPECT_FALSE(settings.get<bool>("video", "fullscreen"));
    EXPECT_EQ("default", settings.get<std::string>("audio", "device"));
    EXPECT_EQ((std::vector<int>{1, 2}), settings.get<std::vector<int>>("audio", "channels"));
    EXPECT_TRUE(settings.has("video", "width"));
    EXPECT_FALSE(settings.has("empty", "key"));
    EXPECT_THROW(settings.get<int>("empty", "key"), std::runtime_error);
    EXPECT_EQ(42, settings.get<int>("empty", "key", 42));
}

TEST(Settings, missing_input_file_throws_with_path)
{
    const auto file = testing::get_test_output() / "missing.json";
    try
    {
        const auto settings = ice::Settings{file};
        FAIL() << "Expected a file-open error";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_NE(std::string::npos, std::string(error.what()).find(file.string()));
        EXPECT_NE(std::string::npos, std::string(error.what()).find("reading"));
    }
}

TEST(Settings, rejects_invalid_json)
{
    const auto file = testing::get_test_output() / "invalid.json";
    for (const auto& contents : {"", "not json", "{", "{\"video\":}", "{} trailing"})
    {
        SCOPED_TRACE(contents);
        write_file(file, contents);
        EXPECT_THROW((ice::Settings{file}), nlohmann::json::parse_error);
    }
}

TEST(Settings, loads_empty_object)
{
    const auto file = testing::get_test_output() / "empty.json";
    write_file(file, "{}");
    auto settings = ice::Settings{file};

    EXPECT_FALSE(settings.has("video", "width"));
    EXPECT_EQ(800, settings.get<int>("video", "width", 800));
    settings.set("video", "width", 1920);
    EXPECT_EQ(1920, settings.get<int>("video", "width"));
}

TEST(Settings, non_object_sections_have_no_keys)
{
    const auto file = testing::get_test_output() / "sections.json";
    write_file(file, R"({"null":null,"number":42,"string":"text","array":[1,2]})");
    const auto settings = ice::Settings{file};

    for (const auto& section : {"null", "number", "string", "array"})
    {
        SCOPED_TRACE(section);
        EXPECT_FALSE(settings.has(section, "key"));
        EXPECT_THROW(settings.get<int>(section, "key"), std::runtime_error);
        EXPECT_EQ(7, settings.get<int>(section, "key", 7));
    }
}

TEST(Settings, save_round_trips_values_and_special_strings)
{
    const auto file = testing::get_test_output() / "settings with spaces.json";
    const auto text = std::string("quotes: \" backslash: \\ newline:\n tab:\t Unicode: 氷");
    const auto embedded_null = std::string("a\0b", 3);
    auto settings = ice::Settings{};
    settings.set("video", "width", 1920);
    settings.set("video", "scale", 1.25);
    settings.set("video", "fullscreen", false);
    settings.set("text", "special", text);
    settings.set("text", "null", embedded_null);
    settings.set("values", "array", std::vector<int>{1, -2, 3});
    settings.set("values", "null", nullptr);
    settings.set("values", "unsigned", std::numeric_limits<std::uint64_t>::max());
    settings.save(file);

    ASSERT_TRUE(std::filesystem::is_regular_file(file));
    const auto loaded = ice::Settings{file};
    EXPECT_EQ(1920, loaded.get<int>("video", "width"));
    EXPECT_DOUBLE_EQ(1.25, loaded.get<double>("video", "scale"));
    EXPECT_FALSE(loaded.get<bool>("video", "fullscreen"));
    EXPECT_EQ(text, loaded.get<std::string>("text", "special"));
    EXPECT_EQ(embedded_null, loaded.get<std::string>("text", "null"));
    EXPECT_EQ((std::vector<int>{1, -2, 3}), loaded.get<std::vector<int>>("values", "array"));
    EXPECT_TRUE(loaded.has("values", "null"));
    EXPECT_TRUE(loaded.get<nlohmann::json>("values", "null").is_null());
    EXPECT_EQ(std::numeric_limits<std::uint64_t>::max(), loaded.get<std::uint64_t>("values", "unsigned"));
    EXPECT_EQ(1920, settings.get<int>("video", "width"));
}

TEST(Settings, save_produces_expected_json_structure)
{
    const auto file = testing::get_test_output() / "settings.json";
    auto settings = ice::Settings{};
    settings.set("video", "width", 1920);
    settings.set("audio", "enabled", true);
    settings.save(file);

    auto input = std::ifstream{file};
    const auto expected = nlohmann::json{{"video", {{"width", 1920}}}, {"audio", {{"enabled", true}}}};
    EXPECT_EQ(expected, nlohmann::json::parse(input));
}

TEST(Settings, save_overwrites_existing_file)
{
    const auto file = testing::get_test_output() / "settings.json";
    write_file(file, R"({"obsolete":{"long_key":"a long value that must not survive"}})");
    auto settings = ice::Settings{};
    settings.set("new", "key", 1);
    settings.save(file);

    const auto loaded = ice::Settings{file};
    EXPECT_EQ(1, loaded.get<int>("new", "key"));
    EXPECT_FALSE(loaded.has("obsolete", "long_key"));
}

TEST(Settings, empty_settings_round_trip)
{
    const auto file = testing::get_test_output() / "empty.json";
    auto settings = ice::Settings{};
    settings.save(file);
    auto loaded = ice::Settings{file};

    EXPECT_FALSE(loaded.has("video", "width"));
    EXPECT_THROW(loaded.get<int>("video", "width"), std::runtime_error);
    EXPECT_EQ(800, loaded.get<int>("video", "width", 800));
    loaded.set("video", "width", 1920);
    EXPECT_EQ(1920, loaded.get<int>("video", "width"));
}

TEST(Settings, loaded_settings_can_be_modified_and_saved)
{
    const auto file = testing::get_test_output() / "input.json";
    const auto output = testing::get_test_output() / "output.json";
    write_file(file, R"({"video":{"width":800,"height":600}})");
    auto settings = ice::Settings{file};
    settings.set("video", "width", 1920);
    settings.set("audio", "enabled", true);
    settings.save(output);

    const auto loaded = ice::Settings{output};
    EXPECT_EQ(1920, loaded.get<int>("video", "width"));
    EXPECT_EQ(600, loaded.get<int>("video", "height"));
    EXPECT_TRUE(loaded.get<bool>("audio", "enabled"));
    EXPECT_EQ(800, ice::Settings{file}.get<int>("video", "width"));
}

TEST(Settings, save_to_missing_parent_throws_with_path)
{
    const auto file = testing::get_test_output() / "missing" / "settings.json";
    auto settings = ice::Settings{};
    settings.set("video", "width", 800);
    try
    {
        settings.save(file);
        FAIL() << "Expected a file-open error";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_NE(std::string::npos, std::string(error.what()).find(file.string()));
        EXPECT_NE(std::string::npos, std::string(error.what()).find("writing"));
    }
    EXPECT_FALSE(std::filesystem::exists(file.parent_path()));
    EXPECT_EQ(800, settings.get<int>("video", "width"));
}

TEST(Settings, save_to_directory_throws)
{
    auto settings = ice::Settings{};
    EXPECT_THROW(settings.save(testing::get_test_output()), std::runtime_error);
}
