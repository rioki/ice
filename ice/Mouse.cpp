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

#include "Mouse.h"

#include "stdng.h"

namespace ice
{
    glm::vec2 Mouse::get_position() const
    {
        auto x = 0.0f;
        auto y = 0.0f;
        SDL_GetMouseState(&x, &y);
        return {x, y};
    }

    bool Mouse::is_pressed(MouseButton button) const
    {
        auto state = SDL_GetMouseState(nullptr, nullptr);
        return (state & SDL_BUTTON_MASK(stdng::to_underlying(button))) != 0;
    }

    rsig::connection Mouse::on_press(const std::function<void (MouseButton, glm::vec2)>& callback)
    {
        return press_signal.connect(callback);
    }

    rsig::connection Mouse::on_release(const std::function<void (MouseButton, glm::vec2)>& callback)
    {
        return release_signal.connect(callback);
    }

    rsig::connection Mouse::on_move(const std::function<void (glm::vec2, glm::vec2)>& callback)
    {
        return move_signal.connect(callback);
    }

    rsig::connection Mouse::on_wheel(const std::function<void (glm::vec2)>& callback)
    {
        return wheel_signal.connect(callback);
    }

    void Mouse::handle_event(const SDL_Event& event)
    {
        switch (event.type)
        {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                press_signal.emit(static_cast<MouseButton>(event.button.button), {event.button.x, event.button.y});
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                release_signal.emit(static_cast<MouseButton>(event.button.button), {event.button.x, event.button.y});
                break;
            case SDL_EVENT_MOUSE_MOTION:
                move_signal.emit({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel});
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                wheel_signal.emit({event.wheel.x, event.wheel.y});
                break;
        }
    }
}
