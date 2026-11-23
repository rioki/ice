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

#include "trace.h"

#include <tinyformat.h>

namespace ice
{
    rsig::signal<const std::string_view> trace_signal;

    rsig::connection on_trace(const std::function<void (const std::string_view )> cb)
    {
        return trace_signal.connect(cb);
    }

    std::string_view basename(const std::string_view file)
    {
        unsigned int i = file.find_last_of("\\/");
        if (i == std::string_view::npos)
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
        auto msg = tfm::format("%s(%d): %s\n", basename(location.file_name()), location.line(), message);
        trace_signal.emit(msg);
    }
}
