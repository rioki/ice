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
#include <string_view>
#include <filesystem>
#include <source_location>

#if _WIN32
#include <windows.h>
#endif

#include "defines.h"

namespace ice
{
    //! Output diagnostic message.
    ICE_EXPORT void trace(const std::string_view message, const std::source_location location = std::source_location::current());

    //! Report failure.
    [[ noreturn ]] ICE_EXPORT void fail(const std::string_view message = "failed", const std::source_location location = std::source_location::current()) noexcept;

    //! Check.
    //!
    //! This function will check the condition and if false call the failure handler.
    //! If the failure handler is not overwritten, the handler will call trace and
    //! then either break in the debugger or pull a crash dump and exit.
    ICE_EXPORT void check(bool condition, const std::source_location location = std::source_location::current()) noexcept;

    //! Entry in the Stack Trace
    struct StackFrame
    {
        uintptr_t       address;
        std::string     name;
        std::string     module;
        unsigned int    line;
        std::string     file;
    };

    //! Get the stack trace for the current location.
    ICE_EXPORT std::vector<StackFrame> get_stack_trace() noexcept;

    //! Write stack frame to output stream.
    ICE_EXPORT std::ostream& operator << (std::ostream& os, const StackFrame& frame) noexcept;

    //! Write stack trace to output stream.
    ICE_EXPORT std::ostream& operator << (std::ostream& os, const std::vector<StackFrame>& trace) noexcept;

    //! Create a unique filename for the crash dump.
    ICE_EXPORT std::filesystem::path create_crash_dump_name(const std::string& prefix = "pkzo") noexcept;

    //! Write crash dump
    ICE_EXPORT void write_crash_dump(const std::filesystem::path& filename = create_crash_dump_name()) noexcept;

    //! Install Crash Handlers
    class ICE_EXPORT CrashHandler
    {
    public:
        CrashHandler();
        ~CrashHandler();

    private:
        #ifdef _WIN32
        LPTOP_LEVEL_EXCEPTION_FILTER old_handler = nullptr;
        #else
        std::terminate_handler old_handler = nullptr;
        #endif
    };
}
