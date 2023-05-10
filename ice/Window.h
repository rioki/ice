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

#pragma once

#include <memory>
#include <string_view>

#include <glm/glm.hpp>
#include <rsig/rsig.h>

#include "defines.h"
#include "utils.h"

struct SDL_Window;
typedef void *SDL_GLContext;
union SDL_Event;

namespace ice
{
    class EventRouter;

    //! Window Mode
    enum class WindowMode
    {
        STATIC,
        RESIZABLE,
        BORDERLESS,
        FULLSCREEN,
        DESKTOP_FULLSCREEN
    };

    //! System Window
    class ICE_EXPORT Window : private non_copyable
    {
    public:
        //! Create Event Loop
        Window(const glm::uvec2& size, WindowMode mode, const std::string_view caption);
        //! Destroy Event Loop
        ~Window();

        //! Set caption.
        void set_caption(const std::string_view text) noexcept;
        //! Get caption.
        std::string get_caption() const noexcept;

        //! Resize window.
        void resize(const glm::uvec2& size, WindowMode mode) noexcept;

        //! Get size.
        glm::uvec2 get_size() const noexcept;

        glm::uvec2 get_drawable_size() const noexcept;

        //! Get the window mode.
        WindowMode get_mode() const noexcept;

        //! Close the Window.
        void close();

        //! Draw frame
        void draw() const noexcept;

        //! Save the current window contents as texture.
        //std::shared_ptr<Texture> save() const noexcept;

        //! Signal emitted each time the window needs to be redrawn
        //! @{
        rsig::signal<>& get_draw_sginal() noexcept;
        rsig::connection on_draw(const std::function<void ()>& cb) noexcept;
        //! @}

        //! Signal emitted when the window is closed.
        //! @{
        rsig::signal<>& get_close_sginal() noexcept;
        rsig::connection on_close(const std::function<void ()>& cb) noexcept;
        //! @}

        //! Signal emitted when the window is resized.
        //! @{
        rsig::signal<glm::uvec2>& get_resize_sginal() noexcept;
        rsig::connection on_resize(const std::function<void (glm::uvec2)>& cb) noexcept;
        //! @}

    private:
        SDL_Window*    window    = nullptr;
        SDL_GLContext  glcontext = nullptr;

        rsig::signal<> draw_signal;
        rsig::signal<> close_signal;
        rsig::signal<glm::uvec2> resize_signal;

        void handle_event(SDL_Event& event);

        friend class Engine;
        friend class Mouse;
    };
}
