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

#include <atomic>

#include "defines.h"
#include "debug.h"
#include "Window.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace ice
{
    //! Engine
    //!
    //! The Engine class ties all bits of the ice engine together.
    class ICE_EXPORT Engine
    {
    public:
        //! Construct Engine
        Engine();

        //! Destreuct Engine
        ~Engine();

        //! Check if the engine is running.
        [[nodiscard]] bool is_running() const noexcept;

        //! Run the engine.
        //!
        //! The run function will repeatidly call tick until stop is called.
        void run();

        //! Stop engine execution.
        void stop();

    protected:
        //! Single engine tick.
        void tick();

    private:
        CrashHandler debug_handler;
        std::atomic<bool> running = false;

        std::unique_ptr<Window>   window;
        std::unique_ptr<Mouse>    mouse;
        std::unique_ptr<Keyboard> keyboard;

        void route_events();
    };
}
