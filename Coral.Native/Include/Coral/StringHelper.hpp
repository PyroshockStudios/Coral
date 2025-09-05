#pragma once

#include "Core.hpp"

namespace Coral {
    class String;
    class StringHelper
    {
    public:
        static UCString ConvertUtf8ToWide(std::string_view InString);
        static std::string ConvertWideToUtf8(UCStringView InString);
        static std::string UCCharToString(const UCChar* InString);
        static std::string ConsumeNativeString(String& InString);
    };

}
