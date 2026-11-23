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

#include <functional>

#include <glm/glm.hpp>
#include <rsig/rsig.h>
#include <SDL3/SDL.h>

namespace ice
{
    enum class MouseButton : Uint8
    {
        LEFT    = SDL_BUTTON_LEFT,
        MIDDLE  = SDL_BUTTON_MIDDLE,
        RIGHT   = SDL_BUTTON_RIGHT,
        BUTTON4 = SDL_BUTTON_X1,
        BUTTON5 = SDL_BUTTON_X2
    };

    class ICE_API Mouse
    {
    public:
        Mouse() = default;
        ~Mouse() = default;

        glm::vec2 get_position() const;
        bool is_pressed(MouseButton button) const;

        rsig::connection on_press(const std::function<void (MouseButton, glm::vec2)>& callback);
        rsig::connection on_release(const std::function<void (MouseButton, glm::vec2)>& callback);
        rsig::connection on_move(const std::function<void (glm::vec2, glm::vec2)>& callback);
        rsig::connection on_wheel(const std::function<void (glm::vec2)>& callback);

    private:
        rsig::signal<MouseButton, glm::vec2> press_signal;
        rsig::signal<MouseButton, glm::vec2> release_signal;
        rsig::signal<glm::vec2, glm::vec2>   move_signal;
        rsig::signal<glm::vec2>              wheel_signal;

        void handle_event(const SDL_Event& event);

        Mouse(const Mouse&) = delete;
        Mouse& operator = (const Mouse&) = delete;

        friend class Engine;
    };
}

