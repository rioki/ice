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

#include "debug.h"

#include <format>
#include <array>

#if _WIN32
#include <intrin.h>
#include <dbghelp.h>
#endif

#include <ice/strconv.h>

namespace ice {

    constexpr std::string basename(const std::string& file) noexcept
    {
        auto i = file.find_last_of("\\/");
        if (i == std::string::npos)
        {
            return file;
        }
        else
        {
            return file.substr(i + 1);
        }
    }

    void trace(const std::string_view message, const std::source_location location)
    {
        const auto line = std::format("{}({}): {}: {}\n", basename(location.file_name()), location.line(), location.function_name(), message);
        #ifdef _WIN32
        OutputDebugStringA(line.c_str());
        #else
        std::clog << line;
        #endif
    }

    [[noreturn]]
    void do_fail(const std::source_location location, const std::string_view message, bool write_dump = true)
    {
        trace(message, location);

        #ifdef _WIN32
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
        else
        {
            if (write_dump)
            {
                write_crash_dump();
            }
            MessageBoxA(nullptr, message.data(), "Unexpected Error", MB_ICONERROR | MB_OK);
        }
        #endif

        std::exit(EXIT_FAILURE);
    }

     void fail(const std::string_view message, const std::source_location location) noexcept
     {
        do_fail(location, message);
     }

    void require(bool condition, const std::source_location location) noexcept
    {
        if (!condition)
        {
            fail("require failed", location);
        }
    }

    void ensure(bool condition, const std::source_location location) noexcept
    {
        if (!condition)
        {
            fail("ensure failed", location);
        }
    }

    #ifdef _WIN32
    std::vector<StackFrame> get_stack_trace_win32() noexcept
    {
        #ifdef _WIN64
        auto machine = IMAGE_FILE_MACHINE_AMD64;
        #else
        auto machine = IMAGE_FILE_MACHINE_I386;
        #endif
        auto process = GetCurrentProcess();
        auto thread  = GetCurrentThread();

        if (SymInitialize(process, nullptr, TRUE) == FALSE)
        {
            trace("Failed to call SymInitialize.");
            return {};
        }

        SymSetOptions(SYMOPT_LOAD_LINES);

        auto context = CONTEXT{};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        #ifdef _WIN64
        auto frame = STACKFRAME{};
        frame.AddrPC.Offset = context.Rip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Rbp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Rsp;
        frame.AddrStack.Mode = AddrModeFlat;
        #else
        auto frame = STACKFRAME{};
        frame.AddrPC.Offset = context.Eip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Esp;
        frame.AddrStack.Mode = AddrModeFlat;
        #endif


        auto first = true;

        auto frames = std::vector<StackFrame>{};
        while (StackWalk(machine, process, thread, &frame, &context , nullptr, SymFunctionTableAccess, SymGetModuleBase, nullptr))
        {
            auto f = StackFrame{};
            f.address = frame.AddrPC.Offset;

            #ifdef _WIN64
            auto moduleBase = DWORD64{0};
            #else
            auto moduleBase = DWORD{0};
            #endif

            moduleBase = SymGetModuleBase(process, frame.AddrPC.Offset);

            auto moduelBuff = std::array<char, MAX_PATH>{};
            if (moduleBase && GetModuleFileNameA(reinterpret_cast<HINSTANCE>(moduleBase), moduelBuff.data(), static_cast<DWORD>(moduelBuff.size())))
            {
                f.module = basename(moduelBuff.data());
            }
            else
            {
                f.module = "Unknown Module";
            }
            #ifdef _WIN64
            auto offset = DWORD64{0};
            #else
            auto offset = DWORD{0};
            #endif
            auto symbolBuffer = std::array<char, sizeof(IMAGEHLP_SYMBOL) + 255>{};
            PIMAGEHLP_SYMBOL symbol = (PIMAGEHLP_SYMBOL)symbolBuffer.data();
            symbol->SizeOfStruct = static_cast<DWORD>(symbolBuffer.size());
            symbol->MaxNameLength = 254;

            if (SymGetSymFromAddr(process, frame.AddrPC.Offset, &offset, symbol))
            {
                f.name = symbol->Name;
            }
            else
            {
                f.name = "Unknown Function";
            }

            auto line = IMAGEHLP_LINE{};
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

            auto offset_ln = DWORD{0};
            if (SymGetLineFromAddr(process, frame.AddrPC.Offset, &offset_ln, &line))
            {
                f.file = line.FileName;
                f.line = line.LineNumber;
            }
            else
            {
                f.line = 0;
            }

            if (!first)
            {
                frames.push_back(f);
            }
            first = false;
        }

        SymCleanup(process);

        return frames;
    }
    #endif


    std::vector<StackFrame> get_stack_trace() noexcept
    {
        #ifdef _WIN32
        return get_stack_trace_win32();
        #else
        trace("Stack trace not implemented for this platform.");
        return {};
        #endif
    }

    std::ostream& operator << (std::ostream& os, const StackFrame& frame) noexcept
    {
        os << "0x" << std::hex << frame.address << ": " << frame.name << "(" << std::dec << frame.line << ") in " << frame.module << "\n";
        return os;
    }

    std::ostream& operator << (std::ostream& os, const std::vector<StackFrame>& trace) noexcept
    {
        for (const auto& line : trace)
        {
            os << line << "\n";
        }
        return os;
    }

    std::string get_env_variable(const std::string_view name) noexcept
    {
        auto buffer = std::array<char, MAX_PATH>{};
        auto len    = size_t{0};

        const auto err = getenv_s(&len, buffer.data(), buffer.size(), name.data());
        if (err == 0) // was found
        {
            return std::string(buffer.data(), len);
        }

        return {};
    }

    std::filesystem::path get_temp_folder() noexcept
    {
        #ifdef _WIN32
        std::array<wchar_t, MAX_PATH> buff;
        const auto r = GetTempPathW(static_cast<DWORD>(buff.size()), buff.data());
        if (r != 0)
        {
            return buff.data();
        }
        #endif

        return get_env_variable("TEMP");
    }

    std::filesystem::path create_crash_dump_name(const std::string& prefix) noexcept
    {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        auto filename = std::format("{}_{:%Y-%m-%d_%H-%M-%S}.dmp", prefix, time);
        return get_temp_folder() / filename;
    }

    #ifdef _WIN32
    MINIDUMP_EXCEPTION_INFORMATION GetExceptionInfo(EXCEPTION_POINTERS* exceptionPointers) noexcept
    {
        auto exceptionInfo = MINIDUMP_EXCEPTION_INFORMATION{};
        exceptionInfo.ThreadId          = GetCurrentThreadId();
        exceptionInfo.ExceptionPointers = exceptionPointers;
        exceptionInfo.ClientPointers    = TRUE;
        return exceptionInfo;
    }

    std::string GetErrorMessage(int32_t errorid) noexcept
    {
        auto buffer = std::array<char, 1024>{};
        auto landId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
        auto flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        auto stringSize = FormatMessageA(flags, NULL, errorid, landId, buffer.data(), static_cast<DWORD>(buffer.size()), NULL);
        return std::string(buffer.data(), stringSize);
    }

    std::string GetLastErrorString() noexcept
    {
        auto error = ::GetLastError();
        return GetErrorMessage(error);
    }

    void write_crash_dump_win32(const std::filesystem::path& filename, EXCEPTION_POINTERS* exceptionPointers = nullptr) noexcept
    {
        const auto handle = CreateFileW(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE)
        {
            trace(std::format("Failed to open {} for writing: {}", strconv::narrow(filename.native()), GetLastErrorString()));
            return;
        }

        const auto process   = GetCurrentProcess();
        const auto processId = GetCurrentProcessId();


        auto result = FALSE;
        if (exceptionPointers != nullptr)
        {
            auto exceptionInfo   = GetExceptionInfo(exceptionPointers);
            result = MiniDumpWriteDump(process, processId, handle, MiniDumpNormal, &exceptionInfo, 0, 0);
        }
        else
        {
            result = MiniDumpWriteDump(process, processId, handle, MiniDumpNormal, nullptr, 0, 0);
        }

        if (result == FALSE)
        {
            trace(std::format("Failed to write mini dump to {}", strconv::narrow(filename.native())));
        }

        CloseHandle(handle);
    }
    #endif

    void write_crash_dump(const std::filesystem::path& filename) noexcept
    {
        #ifdef _WIN32
        write_crash_dump_win32(filename);
        #else
        trace("Stack trace not implemented for this platform.");
        #endif
    }


    #ifdef _WIN32
    LONG __stdcall HandleUnhendledExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
    {
        write_crash_dump_win32(create_crash_dump_name(), pExPtrs);
        do_fail(std::source_location::current(), "Process Crashed.", false);
    }
    #else
    voi handle_termiante()
    {
        fail("Unexpectged Termination.");
    }
    #endif

    CrashHandler::CrashHandler()
    {
        #ifdef _WIN32
        old_handler = SetUnhandledExceptionFilter(HandleUnhendledExceptionFilter);
        #else
        old_handler = set_terminate(handle_termiante);
        #endif
    }

    CrashHandler::~CrashHandler()
    {
        #ifdef _WIN32
        SetUnhandledExceptionFilter(old_handler);
        #else
        set_terminate(old_handler);
        #endif
    }
}
