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

#include <atomic>
#include <filesystem>
#include <string>

namespace ice
{
    class Settings;
    class Window;
    class Mouse;
    class Keyboard;

    class ICE_API Engine
    {
    public:

        struct Init
        {
            std::string id;
            std::string version;
        };

        Engine(Init init);
        ~Engine();

        const std::string& get_id() const;
        const std::string& get_version() const;

        const std::filesystem::path& get_settings_path() const;
        Settings& get_settings();
        const Settings& get_settings() const;

        Window& get_window();
        const Window& get_window() const;

        Mouse& get_mouse();
        const Mouse& get_mouse() const;

        Keyboard& get_keyboard();
        const Keyboard& get_keyboard() const;

        bool is_running() const;
        void run();
        void stop();

    private:
        const std::string id;
        const std::string version;

        std::atomic<bool> running = false;

        mutable std::filesystem::path settings_path_cache;
        std::unique_ptr<Settings>     settings;

        std::unique_ptr<Window>       window;
        std::unique_ptr<Mouse>        mouse;
        std::unique_ptr<Keyboard>     keyboard;

        void tick();
        void route_events();
        void save_settings();
    };
}
