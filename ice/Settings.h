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

#pragma once
#include "config.h"

#include <filesystem>
#include <string>

#include <tinyformat.h>
#include <nlohmann/json.hpp>

namespace ice
{
    class ICE_API Settings
    {
    public:
        Settings() = default;
        Settings(const std::filesystem::path& file);
        Settings(const Settings& rhs) = default;
        Settings(Settings&& rhs) noexcept = default;
        ~Settings() = default;
        Settings& operator = (const Settings& rhs) = default;
        Settings& operator = (Settings&& rhs) noexcept = default;

        bool has(const std::string& section, const std::string& key) const;

        template <typename Type>
        Type get(const std::string& section, const std::string& key) const;

        template <typename Type>
        Type get(const std::string& section, const std::string& key, const Type& fallback) const;

        template <typename Type>
        void set(const std::string& section, const std::string& key, const Type& value);

        void save(const std::filesystem::path& file);

    private:
        nlohmann::json data;
    };

    template <typename Type>
    Type Settings::get(const std::string& section, const std::string& key) const
    {
        auto si = data.find(section);
        if (si == end(data))
        {
            throw std::runtime_error(tfm::format("No value for %s.%s", section, key));
        }

        auto ki = si->find(key);
        if (ki == end(*si))
        {
            throw std::runtime_error(tfm::format("No value for %s.%s", section, key));
        }

        return ki->get<Type>();
    }

    template <typename Type>
    Type Settings::get(const std::string& section, const std::string& key, const Type& fallback) const
    {
        auto si = data.find(section);
        if (si == end(data))
        {
            return fallback;
        }

        auto ki = si->find(key);
        if (ki == end(*si))
        {
            return fallback;
        }

        return ki->get<Type>();
    }

    template <typename Type>
    void Settings::set(const std::string& section, const std::string& key, const Type& value)
    {
        data[section][key] = value;
    }
}
