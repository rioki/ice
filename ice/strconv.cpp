// strex - string utilities that should be standard
// Copyright 2022-2023 Sean Farrell <sean.farrell@rioki.org>
//
// This program is free software. It comes without any warranty, to
// the extent permitted by applicable law. You can redistribute it
// and/or modify it under the terms of the Do What The Fuck You Want
// To Public License, Version 2, as published by Sam Hocevar. See
// http://www.wtfpl.net/ for more details.

#include "strconv.h"

#include <cassert>
#include <array>
#include <iconv.h>

namespace strconv
{
    template <typename CharT>
    struct CharTraits {};

    template <>
    struct CharTraits<char>
    {
        // Empty string is whatever the system uses as multibyte encoding
        static constexpr const char* encoding = "";
    };

    template <>
    struct CharTraits<wchar_t>
    {
        static constexpr const char* encoding = "UCS-2LE";
    };

    template <>
    struct CharTraits<char8_t>
    {
        static constexpr const char* encoding = "UTF-8";
    };

    template <>
    struct CharTraits<char16_t>
    {
        static constexpr const char* encoding = "UTF-16LE";
    };

    template <>
    struct CharTraits<char32_t>
    {
        static constexpr const char* encoding = "UTF-32LE";
    };

    template <typename ToCharT, typename FromCharT>
    std::basic_string<ToCharT> iconv_impl(const std::basic_string_view<FromCharT>& input)
    {
        auto result = std::basic_string<ToCharT>{};

        const auto from = CharTraits<FromCharT>::encoding;
        const auto to   = CharTraits<ToCharT>::encoding;

        auto cd = iconv_open(to, from);
        assert(cd);

        char*  inbuf       = reinterpret_cast<char*>(const_cast<FromCharT*>(input.data()));
        size_t inbytesleft = input.size() * sizeof(FromCharT);

        std::array<char, 512> temp;
        char*  outbuf       = temp.data();
        size_t outbytesleft = temp.size();

        while (inbytesleft > 0u)
        {
            iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);

            if (errno == EILSEQ)
            {
                throw std::runtime_error("Invalid character sequence.");
            }

            if (errno == EINVAL)
            {
                throw std::runtime_error("Incompatible character sequence.");
            }

            result.append(reinterpret_cast<ToCharT*>(temp.data()), reinterpret_cast<ToCharT*>(outbuf));
            outbuf       = temp.data();
            outbytesleft = temp.size();
        }

        int r = iconv_close(cd);
        assert(r == 0);

        return result;
    }

    std::string narrow(const std::string_view& input)
    {
        return std::string(input);
    }

    std::string narrow(const std::wstring_view& input)
    {
        return iconv_impl<char>(input);
    }

    std::string narrow(const std::u8string_view& input)
    {
        return iconv_impl<char>(input);
    }

    std::string narrow(const std::u16string_view& input)
    {
        return iconv_impl<char>(input);
    }

    std::string narrow(const std::u32string_view& input)
    {
        return iconv_impl<char>(input);
    }

    std::wstring widen(const std::string_view& input)
    {
        return iconv_impl<wchar_t>(input);
    }

    std::wstring widen(const std::wstring_view& input)
    {
        return std::wstring(input);
    }

    std::wstring widen(const std::u8string_view& input)
    {
        return iconv_impl<wchar_t>(input);
    }

    std::wstring widen(const std::u16string_view& input)
    {
        return iconv_impl<wchar_t>(input);
    }

    std::wstring widen(const std::u32string_view& input)
    {
        return iconv_impl<wchar_t>(input);
    }

    std::u8string utf8(const std::string_view& input)
    {
        return iconv_impl<char8_t>(input);
    }
    std::u8string utf8(const std::wstring_view& input)
    {
        return iconv_impl<char8_t>(input);
    }

    std::u8string utf8(const std::u8string_view& input)
    {
        return std::u8string(input);
    }

    std::u8string utf8(const std::u16string_view& input)
    {
        return iconv_impl<char8_t>(input);
    }

    std::u8string utf8(const std::u32string_view& input)
    {
        return iconv_impl<char8_t>(input);
    }

    std::u16string utf16(const std::string_view& input)
    {
        return iconv_impl<char16_t>(input);
    }

    std::u16string utf16(const std::wstring_view& input)
    {
        return iconv_impl<char16_t>(input);
    }

    std::u16string utf16(const std::u8string_view& input)
    {
        return iconv_impl<char16_t>(input);
    }

    std::u16string utf16(const std::u16string_view& input)
    {
        return std::u16string(input);
    }

    std::u16string utf16(const std::u32string_view& input)
    {
        return iconv_impl<char16_t>(input);
    }

    std::u32string utf32(const std::string_view& input)
    {
        return iconv_impl<char32_t>(input);
    }

    std::u32string utf32(const std::wstring_view& input)
    {
        return iconv_impl<char32_t>(input);
    }

    std::u32string utf32(const std::u8string_view& input)
    {
        return iconv_impl<char32_t>(input);
    }

    std::u32string utf32(const std::u16string_view& input)
    {
        return iconv_impl<char32_t>(input);
    }

    std::u32string utf32(const std::u32string_view& input)
    {
        return std::u32string(input);
    }

    std::string u8compat(const std::u8string_view& input)
    {
        return std::string(reinterpret_cast<const char*>(input.data()), input.size());
    }

    std::u8string u8compat(const std::string_view& input)
    {
        return std::u8string(reinterpret_cast<const char8_t*>(input.data()), input.size());
    }
}
