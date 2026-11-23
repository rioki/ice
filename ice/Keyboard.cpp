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

#include "Keyboard.h"

namespace ice
{
    bool Keyboard::is_pressed(Scancode scancode) const
    {
        auto count = 0;
        auto state = SDL_GetKeyboardState(&count);
        auto index = static_cast<int>(scancode);
        return index >= 0 && index < count && state[index];
    }

    bool Keyboard::is_pressed(Key key) const
    {
        auto scancode = SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(key), nullptr);
        return is_pressed(static_cast<Scancode>(scancode));
    }

    KeyMod Keyboard::get_modifiers() const
    {
        return static_cast<KeyMod>(SDL_GetModState());
    }

    rsig::connection Keyboard::on_press(const std::function<void (Scancode)>& callback)
    {
        return press_scan_signal.connect(callback);
    }

    rsig::connection Keyboard::on_release(const std::function<void (Scancode)>& callback)
    {
        return release_scan_signal.connect(callback);
    }

    rsig::connection Keyboard::on_press(const std::function<void (KeyMod, Key)>& callback)
    {
        return press_key_signal.connect(callback);
    }

    rsig::connection Keyboard::on_release(const std::function<void (KeyMod, Key)>& callback)
    {
        return release_key_signal.connect(callback);
    }

    void Keyboard::handle_event(const SDL_Event& event)
    {
        switch (event.type)
        {
            case SDL_EVENT_KEY_DOWN:
                if (event.key.repeat == false)
                {
                    press_scan_signal.emit(static_cast<Scancode>(event.key.scancode));
                    press_key_signal.emit(static_cast<KeyMod>(event.key.mod), static_cast<Key>(event.key.key));
                }
                break;
            case SDL_EVENT_KEY_UP:
                release_scan_signal.emit(static_cast<Scancode>(event.key.scancode));
                release_key_signal.emit(static_cast<KeyMod>(event.key.mod), static_cast<Key>(event.key.key));
                break;
        }
    }
}
