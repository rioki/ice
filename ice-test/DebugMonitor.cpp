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

#include "DebugMonitor.h"

namespace ice::test
{
    DebugMonitor::DebugMonitor(bool all_processes) {

        auto DBWIN_BUFFER_READY = L"DBWIN_BUFFER_READY";
        buffer_ready_event = OpenEventW(EVENT_ALL_ACCESS, FALSE, DBWIN_BUFFER_READY);
        if (buffer_ready_event == NULL) {
            buffer_ready_event = CreateEventW(NULL, FALSE, TRUE, DBWIN_BUFFER_READY);

            if (buffer_ready_event == NULL) {
                throw std::runtime_error("Failed to create buffer ready event.");
            }
        }

        auto DBWIN_DATA_READY = L"DBWIN_DATA_READY";
        data_ready_event = OpenEventW(EVENT_ALL_ACCESS, FALSE, DBWIN_DATA_READY);
        if (data_ready_event == NULL) {
            data_ready_event = CreateEventW(NULL, FALSE, TRUE, DBWIN_DATA_READY);

            if (data_ready_event == NULL) {
                throw std::runtime_error("Failed to create data ready event.");
            }
        }

        auto DBWIN_BUFFER = L"DBWIN_BUFFER";
        mapped_file = OpenFileMappingW(FILE_MAP_READ, FALSE, DBWIN_BUFFER);
        if (mapped_file == NULL) {
            mapped_file = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct dbwin_buffer), DBWIN_BUFFER);
            if (mapped_file == NULL) {
                throw std::runtime_error("Failed to open mapped DBWIN buffer.");
            }
        }

        buffer = reinterpret_cast<dbwin_buffer *>(MapViewOfFile(mapped_file, SECTION_MAP_READ, 0, 0, 0));
        if (buffer == NULL) {
            throw std::runtime_error("Faile to map DBWIN buffer.");
        }

        thread = std::jthread([this, all_processes] (std::stop_token stoken) {
            while (!stoken.stop_requested()) {
                auto ret = WaitForSingleObject(data_ready_event, 100);

                if (ret == WAIT_OBJECT_0) {
                    if (all_processes || buffer->dwProcessId == GetCurrentProcessId()) {
                        auto lk = std::scoped_lock{mutex};
                        output << buffer->data;
                    }

                    // signal buffer ready
                    SetEvent(buffer_ready_event);
                }
            }
        });
    }

    DebugMonitor::~DebugMonitor() {
        thread.request_stop();
        thread.join();
        CloseHandle(buffer_ready_event);
        CloseHandle(data_ready_event);
        UnmapViewOfFile(buffer);
        CloseHandle(mapped_file);
    }

    std::string DebugMonitor::get_output() const noexcept {
        auto lk = std::scoped_lock{mutex};
        return output.str();
    }
}