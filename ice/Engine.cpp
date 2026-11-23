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

#include "Engine.h"

#include <cassert>
#include <cstdlib>

#include <stdexcept>

#include <SDL3/SDL.h>

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#include <combaseapi.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")
#endif

#include "stdng.h"
#include "trace.h"
#include "Settings.h"
#include "Window.h"

namespace ice
{
    Engine::Engine(Init init)
    : id(init.id), version(init.version)
    {
        auto r = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
        if (r == false)
        {
            throw std::runtime_error(SDL_GetError());
        }

        auto settings_file = get_settings_path();
        if (std::filesystem::exists(settings_file))
        {
            settings = std::make_unique<Settings>(settings_file);
        }
        else
        {
            settings = std::make_unique<Settings>();
        }

        window = std::make_unique<Window>(Window::Init{
            .title      = tfm::format("%s (%s)", id, version),
            .size       = settings->get("Window", "size",       glm::uvec2(800, 600)),
            .position   = settings->get("Window", "position",   Window::CENTER_POSITION),
            .fullscreen = settings->get("Window", "fullscreen", false)
        });
    }

    Engine::~Engine()
    {
        try
        {
            settings->set("Window", "size",       window->get_size());
            settings->set("Window", "position",   window->get_position());
            settings->set("Window", "fullscreen", window->get_fullscreen());
            save_settings();

            window = nullptr;
            SDL_Quit();
        }
        catch (const std::exception& ex)
        {
            trace(ex.what());
            throw;
        }
    }

    const std::string& Engine::get_id() const
    {
        return id;
    }

    const std::string& Engine::get_version() const
    {
        return version;
    }

#ifdef _WIN32
    std::filesystem::path local_app_data()
    {
        PWSTR raw = nullptr;

        auto hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &raw);
        auto cleanup = stdng::scope_exit([&] () {
            CoTaskMemFree(raw);
        });

        if (FAILED(hr))
        {
            throw std::runtime_error("Unable to determine LOCALAPPDATA.");
        }

        return std::filesystem::path(raw);
    }
#endif
#ifdef __linux__
    std::filesystem::path home()
    {
        if (const auto *value = std::getenv("HOME"))
        {
            return value;
        }

        throw std::runtime_error("HOME is not set.");
    }

    std::filesystem::path xdg_path(const char *variable, const std::filesystem::path &fallback)
    {
        if (const auto *value = std::getenv(variable); value && *value)
        {
            return value;
        }

        return home() / fallback;
    }
#endif

    const std::filesystem::path& Engine::get_settings_path() const
    {
        if (!settings_path_cache.empty())
        {
            return settings_path_cache;
        }

        #ifdef _WIN32
        settings_path_cache = local_app_data() / id / "settings.json";;
        #elif defined(__linux__)
        settings_path_cache = xdg_path("XDG_CONFIG_HOME", ".config") / id / "settings.json";
        #else
        #error "Port me"
        #endif

        return settings_path_cache;
    }

    Settings& Engine::get_settings()
    {
        assert(settings);
        return *settings;
    }

    const Settings& Engine::get_settings() const
    {
        assert(settings);
        return *settings;
    }

    Window& Engine::get_window()
    {
        assert(window);
        return *window;
    }

    const Window& Engine::get_window() const
    {
        assert(window);
        return *window;
    }

    bool Engine::is_running() const
    {
        return running;
    }

    void Engine::run()
    {
        running = true;
        while (running)
        {
            tick();
        }
    }

    void Engine::stop()
    {
        running = false;
    }

    void Engine::tick()
    {
        route_events();
        window->draw();
    }

    void Engine::route_events()
    {
        auto event = SDL_Event{};
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    stop();
                    break;
            }
        }
    }

    void Engine::save_settings() noexcept
    {
        auto settings_file = get_settings_path();
        auto settings_folder = settings_file.parent_path();
        if (!std::filesystem::exists(settings_folder))
        {
            std::filesystem::create_directories(settings_folder);
        }
        settings->save(settings_file);
    }
}
