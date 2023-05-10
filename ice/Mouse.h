// pkzo
// Copyright 2010-2023 Sean Farrell <sean.farrell@rioki.org>
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

#include <functional>
#include <glm/fwd.hpp>
#include <rsig/rsig.h>

#include "defines.h"
#include "utils.h"

union SDL_Event;

namespace ice
{
    class Window;

    //! Mouse Buttons
    enum class MouseButton
    {
        NONE,
        LEFT,
        MIDDLE,
        RIGHT,
        BUTTON4,
        BUTTON5
    };

    //! Mouse
    class ICE_EXPORT Mouse : private non_copyable
    {
    public:
        //! Set cursor visibility.
        void set_cursor_visible(bool value) noexcept;
        //! Get the cursor visibility.
        bool get_cursor_visible() const noexcept;

        //! Get the cursor position.
        glm::ivec2 get_cursor_position() const noexcept;

        void move_cursor(const Window& window, int x, int y);

        //! Button Down Signal
        //!
        //! @{
        rsig::signal<MouseButton, glm::ivec2>& get_button_down_signal() noexcept;
        rsig::connection on_button_down(const std::function<void (MouseButton, glm::ivec2)>& cb) noexcept;
        //! @}

        //! Button Up Signal
        //!
        //! @{
        rsig::signal<MouseButton, glm::ivec2>& get_button_up_signal() noexcept;
        rsig::connection on_button_up(const std::function<void (MouseButton, glm::ivec2)>& cb) noexcept;
        //! @}

        //! Move Signal
        //!
        //! @{
        rsig::signal<glm::ivec2, glm::ivec2>& get_move_signal() noexcept;
        rsig::connection on_move(const std::function<void (glm::ivec2, glm::ivec2)>& cb) noexcept;
        //! @}

        //! Wheel Signal
        //!
        //! @{
        rsig::signal<glm::ivec2>& get_wheel_signal() noexcept;
        rsig::connection on_wheel(const std::function<void (glm::ivec2)>& cb) noexcept;
        //! @}

    private:
        rsig::signal<MouseButton, glm::ivec2> button_down_signal;
        rsig::signal<MouseButton, glm::ivec2> button_up_signal;
        rsig::signal<glm::ivec2, glm::ivec2>  move_signal;
        rsig::signal<glm::ivec2>              wheel_signal;

        void handle_event(SDL_Event& event);

        friend class Engine;
    };
}

