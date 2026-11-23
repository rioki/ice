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

#include "Window.h"

#include "stdng.h"
#include "trace.h"
#include "opengl.h"

namespace ice
{
    void GLAPIENTRY opengl_trace(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
        {
            trace(message);
        }
    }

    Window::Window(Init init)
    {
        auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        if (init.fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

        auto props = SDL_CreateProperties();
        auto props_cleanup = stdng::scope_exit([&] { SDL_DestroyProperties(props);});

        SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING,        init.title.data());
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,        init.size.x);
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER,       init.size.y);
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER,            init.position.y);
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER,            init.position.y);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, init.fullscreen);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN,     true);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN,  true);

        window = SDL_CreateWindowWithProperties(props);
        if (window == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }
        auto window_cleanup = stdng::scope_fail([&] {SDL_DestroyWindow(window);});

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        context = SDL_GL_CreateContext(window);
        if (context == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }
        auto context_cleanup = stdng::scope_fail([&] {SDL_GL_DestroyContext(context);});

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
        {
            throw std::runtime_error("GraphicContext: failed to load OpenGL functions");
        }

        gl::clear_errors();

        #ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(opengl_trace, nullptr);
        #endif
    }

    Window::~Window()
    {
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
    }

    void Window::set_size(glm::uvec2 value)
    {
        auto r = SDL_SetWindowSize(window, value.x, value.y);
        if (r == false)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }
    glm::uvec2 Window::get_size() const
    {
        auto w = 0;
        auto h = 0;
        auto queried = SDL_GetWindowSize(window, &w, &h);
        if (!queried)
        {
            throw std::runtime_error(SDL_GetError());
        }
        return {static_cast<unsigned int>(w), static_cast<unsigned int>(h)};
    }

    glm::uvec2 Window::get_resolution() const
    {
        auto w = 0;
        auto h = 0;
        auto queried = SDL_GetWindowSizeInPixels(window, &w, &h);
        if (!queried)
        {
            throw std::runtime_error(SDL_GetError());
        }
        return {static_cast<unsigned int>(w), static_cast<unsigned int>(h)};
    }

    float Window::get_pixel_scale() const
    {
        return SDL_GetWindowPixelDensity(window);
    }

    void Window::set_position(glm::ivec2 value)
    {
        auto r = SDL_SetWindowPosition(window, value.x, value.y);
        if (r == false)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    glm::ivec2 Window::get_position() const
    {
        auto x = 0;
        auto y = 0;
        auto r = SDL_GetWindowPosition(window, &x, &y);
        if (r == false)
        {
            throw std::runtime_error(SDL_GetError());
        }
        return {x, y};
    }

    void Window::set_fullscreen(bool value)
    {
        auto r = SDL_SetWindowFullscreen(window, value);
        if (r == false)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    bool Window::get_fullscreen() const
    {
        auto flags = SDL_GetWindowFlags(window);
        return (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;
    }

    void Window::set_background_color(const glm::vec3& value)
    {
        background_color = value;
    }

    const glm::vec3& Window::get_background_color() const
    {
        return background_color;
    }

    void Window::draw()
    {
        gl::check_error();

        auto res = get_resolution();
        glViewport(0, 0, res.x, res.y);
        glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        draw_signal.emit();

        SDL_GL_SwapWindow(window);

        gl::check_error();
    }
    rsig::connection Window::on_draw(const std::function<void ()>& cb)
    {
        return draw_signal.connect(cb);
    }

    rsig::connection Window::on_resize(const std::function<void ()>& cb)
    {
        return resize_signal.connect(cb);
    }

    void Window::handle_event(const SDL_Event& event)
    {
        switch (event.type)
        {
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                if (event.window.windowID == SDL_GetWindowID(window))
                {
                    resize_signal.emit();
                }
                break;
            default:
                // do nothing
                break;
        }
    }
}
