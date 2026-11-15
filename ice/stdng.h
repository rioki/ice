// C++ Standard Next Generation
// Stuff that will be in the C++ standard, but available today.
// Copyright 2025-2026 Sean Farrell <sean.farrell@rioki.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <concepts>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace stdng
{
    //! Invokes a callable when leaving scope unless released.
    //!
    //! @tparam EF The exit callable type.
    template <class EF>
    class scope_exit
    {
    public:
        //! Creates a scope guard.
        //!
        //! @param fn The callable to invoke on destruction.
        scope_exit(EF&& fn) noexcept
        : exitfun(std::move(fn)) {}

        //! Move-constructs a scope guard.
        //!
        //! @param other The guard to move from.
        scope_exit(scope_exit&& other) noexcept
        : exitfun(std::move(other.exitfun)), active(other.active)
        {
            other.release();
        }

        //! Destroys the guard and invokes the callable if active.
        ~scope_exit()
        {
            if (active)
            {
                exitfun();
            }
        }

        //! Releases the guard without invoking the callable.
        void release() noexcept
        {
            active = false;
        }

    private:
        EF exitfun;
        bool active = true;

        scope_exit(const scope_exit&) = delete;
        scope_exit& operator = (const scope_exit&) = delete;
    };

    //! Invokes a callable when leaving scope during exception unwinding.
    //!
    //! @tparam EF The failure callable type.
    template <class EF>
    class scope_fail
    {
    public:

        //! Creates a failure scope guard.
        //!
        //! @param fn The callable to invoke on failure.
        explicit scope_fail(EF&& fn) noexcept
        : exitfun(std::move(fn)), exception_count(std::uncaught_exceptions()) {}

        //! Move-constructs a failure scope guard.
        //!
        //! @param other The guard to move from.
        scope_fail(scope_fail&& other) noexcept
        : exitfun(std::move(other.exitfun)), active(other.active), exception_count(other.exception_count)
        {
            other.release();
        }

        //! Destroys the guard and invokes the callable if unwinding through an exception.
        ~scope_fail()
        {
            if (active && std::uncaught_exceptions() > exception_count)
            {
                exitfun();
            }
        }

        //! Releases the guard without invoking the callable.
        void release() noexcept
        {
            active = false;
        }

    private:
        EF   exitfun;
        bool active = true;
        int  exception_count;

        scope_fail(const scope_fail&)            = delete;
        scope_fail& operator=(const scope_fail&) = delete;
    };

    //! Invokes a callable when leaving scope without a new exception.
    //!
    //! @tparam EF The success callable type.
    template <class EF>
    class scope_success
    {
    public:

        //! Creates a success scope guard.
        //!
        //! @param fn The callable to invoke on success.
        explicit scope_success(EF&& fn) noexcept
        : exitfun(std::move(fn)), exception_count(std::uncaught_exceptions()) {}

        //! Move-constructs a success scope guard.
        //!
        //! @param other The guard to move from.
        scope_success(scope_success&& other) noexcept
        : exitfun(std::move(other.exitfun)), active(other.active), exception_count(other.exception_count)
        {
            other.release();
        }

        //! Destroys the guard and invokes the callable if no new exception is active.
        ~scope_success()
        {
            if (active && std::uncaught_exceptions() <= exception_count)
            {
                exitfun();
            }
        }

        //! Releases the guard without invoking the callable.
        void release() noexcept
        {
            active = false;
        }

    private:
        EF   exitfun;
        bool active = true;
        int  exception_count;

        scope_success(const scope_success&)            = delete;
        scope_success& operator=(const scope_success&) = delete;
    };

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template <typename E>
    requires std::is_enum_v<E>
    constexpr auto to_underlying(E value) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(value);
    }

    template <std::integral T>
    constexpr auto bit(T n) {
        return T(1) << n;
    }
}