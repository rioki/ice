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

#include "Window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "debug.h"

namespace ice
{
    Uint32 get_sdl_window_flags(WindowMode mode)
    {
        switch (mode)
        {
        case WindowMode::FULLSCREEN:
            return SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
        case WindowMode::DESKTOP_FULLSCREEN:
            return SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP;
        case WindowMode::RESIZABLE:
            return SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        case WindowMode::BORDERLESS:
            return SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS;
        case WindowMode::STATIC:
            return SDL_WINDOW_OPENGL;
        default:
            fail("Unknown window flags.");
        }
    }

    Window::Window(const glm::uvec2& size, WindowMode mode, const std::string_view caption)
    {
        check(SDL_WasInit(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        Uint32 sdl_flags = get_sdl_window_flags(mode);
        window = SDL_CreateWindow(caption.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, sdl_flags);
        if (window == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }

        glcontext = SDL_GL_CreateContext(window);
        if (glcontext == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    Window::~Window()
    {
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(window);
    }

    void Window::set_caption(const std::string_view text) noexcept
    {
        SDL_SetWindowTitle(window, text.data());
    }

    std::string Window::get_caption() const noexcept
    {
        auto caption = SDL_GetWindowTitle(window);
        if (caption != nullptr)
        {
            return {caption};
        }
        else
        {
            return {};
        }
    }

    void Window::resize(const glm::uvec2& size, WindowMode mode) noexcept
    {
        if (mode == get_mode())
        {
            SDL_SetWindowSize(window, size.x, size.y);
        }
        else
        {
            switch (mode)
            {
                case WindowMode::STATIC:
                    SDL_SetWindowFullscreen(window, 0);
                    SDL_SetWindowResizable(window, SDL_FALSE);
                    SDL_SetWindowBordered(window, SDL_FALSE);
                    SDL_SetWindowSize(window, size.x, size.y);
                    break;
                case WindowMode::RESIZABLE:
                    SDL_SetWindowFullscreen(window, 0);
                    SDL_SetWindowResizable(window, SDL_TRUE);
                    SDL_SetWindowBordered(window, SDL_FALSE);
                    SDL_SetWindowSize(window, size.x, size.y);
                    break;
                case WindowMode::BORDERLESS:
                    SDL_SetWindowFullscreen(window, 0);
                    SDL_SetWindowResizable(window, SDL_FALSE);
                    SDL_SetWindowBordered(window, SDL_TRUE);
                    SDL_SetWindowSize(window, size.x, size.y);
                    break;
                case WindowMode::FULLSCREEN:
                    SDL_SetWindowSize(window, size.x, size.y);
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                    break;
                case WindowMode::DESKTOP_FULLSCREEN:
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    break;
            }

            SDL_SetWindowResizable(window, (mode == WindowMode::RESIZABLE) ? SDL_TRUE : SDL_FALSE);
        }
    }

    glm::uvec2 Window::get_size() const noexcept
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return {w, h};
    }

    glm::uvec2 Window::get_drawable_size() const noexcept
    {
        int w, h;
        SDL_GL_GetDrawableSize(window, &w, &h);
        return {w, h};
    }

    WindowMode Window::get_mode() const noexcept
    {
        auto sdl_flags = SDL_GetWindowFlags(window);
        if (sdl_flags & SDL_WINDOW_FULLSCREEN)
        {
            return WindowMode::FULLSCREEN;
        }
        if (sdl_flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
        {
            return WindowMode::DESKTOP_FULLSCREEN;
        }
        if (sdl_flags & SDL_WINDOW_BORDERLESS)
        {
            return WindowMode::BORDERLESS;
        }
        if (sdl_flags & SDL_WINDOW_RESIZABLE)
        {
            return WindowMode::RESIZABLE;
        }
        return WindowMode::STATIC;
    }

    void Window::close()
    {
        SDL_DestroyWindow(window);
    }

    void Window::draw() const noexcept
    {
        int w, h;
        SDL_GL_GetDrawableSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        draw_signal.emit();

        SDL_GL_SwapWindow(window);
    }

    /*Texture Window::save() const noexcept
    {
        auto s = get_size();
        std::vector<std::byte> buffer(s.x * s.y * 3);
        glReadPixels(0, 0, s.x, s.y, GL_BGR, GL_UNSIGNED_BYTE, buffer.data());
        return Texture(s, ColorMode::RGB, DataType::UINT8, buffer.data(), "screen");
    }*/

    rsig::signal<>& Window::get_draw_sginal() noexcept
    {
        return draw_signal;
    }

    rsig::connection Window::on_draw(const std::function<void()>& cb) noexcept
    {
        return draw_signal.connect(cb);
    }

    rsig::signal<>& Window::get_close_sginal() noexcept
    {
        return close_signal;
    }

    rsig::connection Window::on_close(const std::function<void()>& cb) noexcept
    {
        return close_signal.connect(cb);
    }

    rsig::signal<glm::uvec2>& Window::get_resize_sginal() noexcept
    {
        return resize_signal;
    }

    rsig::connection Window::on_resize(const std::function<void(glm::uvec2)>& cb) noexcept
    {
        return resize_signal.connect(cb);
    }

    void Window::handle_event(SDL_Event& event)
    {
        switch (event.type)
        {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            resize_signal.emit({event.window.data1, event.window.data2});
            break;
        case SDL_WINDOWEVENT_CLOSE:
            close_signal.emit();
            break;
        default:
            fail("Unexpected SDL event.");
        }
    }
}
