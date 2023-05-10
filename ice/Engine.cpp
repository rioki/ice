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

#include "Engine.h"

#include <SDL2/SDL.h>

namespace ice
{
    Engine::Engine()
    {
        auto r = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
        if (r < 0) {
            throw std::runtime_error("Failed to init SDL.");
        }

        window = std::make_unique<Window>(glm::uvec2(800, 600), WindowMode::STATIC, "Ice Engine");

    }

    Engine::~Engine()
    {
        window = nullptr;

        SDL_Quit();
    }

    bool Engine::is_running() const noexcept
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
        if (window)
        {
            window->draw();
        }
    }

    void Engine::route_events()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    stop();
                    break;

                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_CLOSE:
                    if (window)
                    {
                        window->handle_event(event);
                    }

                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                case SDL_TEXTINPUT:
                case SDL_TEXTEDITING:
                    if (keyboard)
                    {
                        keyboard->handle_event(event);
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                    if (mouse)
                    {
                        mouse->handle_event(event);
                    }

                    break;

                default:
                    /* stfu */
                    break;
            }
        }
    }
}
