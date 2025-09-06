#pragma once

#include "Core.hpp"

namespace Coral {
    class String;
    class StringHelper
    {
    public:
        static UCString ConvertUtf8ToWide(StdStringView InString);
        static StdString ConvertWideToUtf8(UCStringView InString);
        static StdString UCCharToString(const UCChar* InString);
        static StdString ConsumeNativeString(String& InString);
    };

}
