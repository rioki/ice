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

#include <stdexcept>

#include <SDL3/SDL.h>

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
    }

    Engine::~Engine()
    {
        SDL_Quit();
    }

    const std::string& Engine::get_id() const
    {
        return id;
    }

    const std::string& Engine::get_version() const
    {
        return version;
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
        // window->draw()
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
}
