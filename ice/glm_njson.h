// glm nlohemann JSON adapters
// Copyright 2021 Sean Farrell <sean.farrell@rioki.org>
//
// This program is free software. It comes without any warranty, to
// the extent permitted by applicable law. You can redistribute it
// and/or modify it under the terms of the Do What The Fuck You Want
// To Public License, Version 2, as published by Sam Hocevar. See
// http://www.wtfpl.net/ for more details.

#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace glm
{
    template<glm::length_t N, typename T, qualifier Q>
    void to_json(nlohmann::json& j, const vec<N, T, Q>& v) noexcept
    {
        j = nlohmann::json::array();
        for (auto i = 0u; i < N; i++)
        {
            j[i] = v[i];
        }
    }

    template<glm::length_t N, glm::length_t M, typename T, qualifier Q>
    void to_json(nlohmann::json& j, const mat<N, M, T, Q>& m)
    {
        j = nlohmann::json::array();
        for (auto i = 0u; i < N; i++)
        {
            j[i] = m[i];
        }
    }

    template<glm::length_t N, typename T, qualifier Q>
    void from_json(const nlohmann::json& j, vec<N, T, Q>& v)  noexcept
    {
        for (auto i = 0u; i < N; i++)
        {
            v[i] = j.at(i).get<T>();
        }
    }

    template<glm::length_t N, glm::length_t M, typename T, qualifier Q>
    void from_json(const nlohmann::json& j, mat<N, M, T, Q>& m)
    {
        for (auto i = 0u; i < N; i++)
        {
            m[i] = j.at(i).get<vec<M, T, Q>>();
        }
    }
}