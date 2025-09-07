#include "Coral/StringHelper.hpp"
#include "Coral/NativeString.hpp"

#include <codecvt>
#include <locale>

namespace Coral {
#ifndef _WIN32
    size_t Utf16CharToUtf8(char16_t ch, char* buffer)
    {
        if (ch <= 0x7F)
        {
            buffer[0] = (static_cast<char>(ch));
            return 1;
        }
        else if (ch <= 0x7FF)
        {
            buffer[0] = (static_cast<char>(0xC0 | ((ch >> 6) & 0x1F)));
            buffer[1] = (static_cast<char>(0x80 | (ch & 0x3F)));
            return 2;
        }
        else
        {
           buffer[0] = (static_cast<char>(0xE0 | ((ch >> 12) & 0x0F)));
           buffer[1] = (static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
           buffer[2] = (static_cast<char>(0x80 | (ch & 0x3F)));
           return 3;
        }
    }
    char16_t Utf8ToUtf16Char(const char* utf8, size_t& consumedBytes)
    {
        unsigned char c = static_cast<unsigned char>(utf8[0]);
        consumedBytes = 1;

        if (c <= 0x7F)
        {
            return static_cast<char16_t>(c);
        }
        else if ((c & 0xE0) == 0xC0)
        {
            // 2-byte sequence
            char16_t ch = ((c & 0x1F) << 6) | (utf8[1] & 0x3F);
            consumedBytes = 2;
            return ch;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            // 3-byte sequence
            char16_t ch = ((c & 0x0F) << 12) |
                          ((utf8[1] & 0x3F) << 6) |
                          (utf8[2] & 0x3F);
            consumedBytes = 3;
            return ch;
        }
        else
        {
            throw std::runtime_error("UTF-8 character too large for single UTF-16 code unit");
        }
    }
#endif

    UCString StringHelper::ConvertUtf8ToUC(StdStringView InString)
    {
#ifdef CORAL_WIDE_CHARS
        return ConvertUtf8ToWide(InString);
#else
        return UCString(InString);
#endif
    }

    StdString StringHelper::ConvertUCToUtf8(UCStringView InString)
    {
#ifdef CORAL_WIDE_CHARS
        return ConvertWideToUtf8(InString);
#else
        return StdString(InString);
#endif
    }

    StdWString StringHelper::ConvertUtf8ToWide(StdStringView InString)
    {
#ifdef _WIN32
        int length = MultiByteToWideChar(CP_UTF8, 0, InString.data(), static_cast<int>(InString.length()), nullptr, 0);
        auto result = StdWString(length, char16_t(0));
        MultiByteToWideChar(CP_UTF8, 0, InString.data(), static_cast<int>(InString.length()), result.data(), length);
        return result;
#else
        StdWString result;
        result.reserve(InString.size()); // rough estimate

        const char* src = InString.data();
        const char* srcEnd = src + InString.size();

        while (src < srcEnd)
        {
            size_t consumed;

            char16_t outChar = Utf8ToUtf16Char(src, consumed);
            if (consumed == (size_t)-1 || consumed == (size_t)-2)
                throw std::runtime_error("Invalid UTF-8 sequence");
            result.push_back(outChar);
            src += consumed;
        }

        return result;
#endif
    }

    StdString StringHelper::ConvertWideToUtf8(StdWStringView InString)
    {
#ifdef _WIN32
        int requiredLength = WideCharToMultiByte(CP_UTF8, 0, InString.data(), static_cast<int>(InString.length()), nullptr, 0, nullptr, nullptr);
        StdString result(requiredLength, 0);
        (void)WideCharToMultiByte(CP_UTF8, 0, InString.data(), static_cast<int>(InString.length()), result.data(), requiredLength, nullptr, nullptr);
        return result;
#else
        StdString result;
        result.reserve(InString.size()); // rough estimate

        char buffer[5]; // temporary buffer for encoded UTF-8

        for (char16_t ch : InString)
        {
            size_t len = Utf16CharToUtf8(ch, buffer);
            result.append(buffer, len);
        }

        return result;
#endif
    }

    StdString StringHelper::UCCharToString(const UCChar* InString)
    {
#ifdef CORAL_WIDE_CHARS
        return ConvertWideToUtf8(InString);
#else
        return StdString(InString);
#endif
    }
    StdString StringHelper::ConsumeNativeString(NativeString& InString)
    {
        StdString str = UCCharToString(InString.Data());
        NativeString::Free(InString);
        return str;
    }
}
