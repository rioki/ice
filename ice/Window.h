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

#include <string>

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <rsig/rsig.h>

namespace ice
{
    class ICE_API Window
    {
    public:

        static constexpr glm::ivec2 CENTER_POSITION    = glm::ivec2(SDL_WINDOWPOS_CENTERED);
        static constexpr glm::ivec2 UNDEFINED_POSITION = glm::ivec2(SDL_WINDOWPOS_UNDEFINED);

        struct Init
        {
            std::string title            = "Ice Engine";
            glm::uvec2  size             = glm::uvec2(800, 600);
            glm::ivec2  position         = CENTER_POSITION;
            bool        fullscreen       = false;
        };

        Window(Init init);
        ~Window();

        void set_size(glm::uvec2 value);
        glm::uvec2 get_size() const;
        glm::uvec2 get_resolution() const;
        float get_pixel_scale() const;

        void set_position(glm::ivec2 value);
        glm::ivec2 get_position() const;

        void set_fullscreen(bool value);
        bool get_fullscreen() const;

        void set_background_color(const glm::vec3& value);
        const glm::vec3& get_background_color() const;

        void draw();
        rsig::connection on_draw(const std::function<void ()>& cb);

        rsig::connection on_resize(const std::function<void ()>& cb);

    private:
        SDL_Window*    window           = nullptr;
        SDL_GLContext  context          = nullptr;
        glm::vec3      background_color = glm::vec3(0.0f);

        rsig::signal<> draw_signal;
        rsig::signal<> resize_signal;

        void handle_event(const SDL_Event& event);

        Window(const Window&) = delete;
        Window& operator = (const Window&) = delete;
        friend class Engine;
    };
}
