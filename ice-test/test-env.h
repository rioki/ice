// Ice Engine
// Copyright 2023 Sean Farrell
// All rights reserved.

#pragma once

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

namespace testing
{
    inline
    std::filesystem::path get_test_input() noexcept
    {
        return std::filesystem::path(ICE_TEST_DIR) / "input";
    }

    inline
    std::filesystem::path get_test_output() noexcept
    {
        return std::filesystem::path(ICE_TEST_DIR) / "output";
    }

    inline
    std::string get_test_name() noexcept
    {
        auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        return std::string(test_info->test_suite_name()) + "-" + std::string(test_info->name());
    }
}
