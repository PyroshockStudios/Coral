#pragma once

#include "Core.hpp"

namespace Coral {
    class NativeString;
    class StringHelper
    {
    public:
        static UCString ConvertUtf8ToUC(StdStringView InString);
        static StdString ConvertUCToUtf8(UCStringView InString);

        static StdWString ConvertUtf8ToWide(StdStringView InString);
        static StdString ConvertWideToUtf8(StdWStringView InString);

        static StdString UCCharToString(const UCChar* InString);
        static StdString ConsumeNativeString(NativeString& InString);
    };

}
