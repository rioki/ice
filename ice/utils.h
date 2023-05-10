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

#pragma once

#include <functional>
#include <type_traits>

#include "debug.h"

namespace ice
{
    //! Mark a class as non copyable.
    class non_copyable
    {
    public:
        non_copyable() = default;
        non_copyable(const non_copyable&) = delete;
        ~non_copyable() = default;
        non_copyable& operator + (const non_copyable&) = delete;
    };

    //! Scoped Cleanup
    //!
    //! This class helps in situations where you want to ensure that something
    //! happens at the end of a scope.
    class cleanup : private non_copyable
    {
    public:
        cleanup(const std::function<void ()>& fun) noexcept
        : cleanup_fun(fun)
        {
            check(static_cast<bool>(cleanup_fun));
        }

        ~cleanup()
        {
            check(static_cast<bool>(cleanup_fun));
            cleanup_fun();
        }

    private:
        std::function<void ()> cleanup_fun;
    };

    template <typename T>
    constexpr T bit(T n)
    {
        return (1 << n);
    }

    #define ICE_ENUM_BIT_OPERATORS(ENUM) \
        constexpr ENUM operator | (ENUM a, ENUM b) \
        { \
            return static_cast<ENUM>(static_cast<std::underlying_type_t<ENUM>>(a) | static_cast<std::underlying_type_t<ENUM>>(b)); \
        } \
        constexpr ENUM operator & (ENUM a, ENUM b) \
        { \
            return static_cast<ENUM>(static_cast<std::underlying_type_t<ENUM>>(a) & static_cast<std::underlying_type_t<ENUM>>(b)); \
        } \
        constexpr ENUM operator ^ (ENUM a, ENUM b) \
        { \
            return static_cast<ENUM>(static_cast<std::underlying_type_t<ENUM>>(a) ^ static_cast<std::underlying_type_t<ENUM>>(b)); \
        } \
        constexpr ENUM operator ~ (ENUM val) \
        { \
            return static_cast<ENUM>(~ static_cast<std::underlying_type_t<ENUM>>(val)); \
        } \
        constexpr ENUM& operator |= (ENUM& a, ENUM b) \
        { \
            return a = a | b; \
        } \
        constexpr ENUM& operator &= (ENUM& a, ENUM b) \
        { \
            return a = a & b; \
        } \
        constexpr ENUM& operator ^= (ENUM& a, ENUM b) \
        { \
            return a = a ^ b; \
        }
}