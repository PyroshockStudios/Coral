#pragma once

#include "Object.hpp"

namespace Coral {
    class alignas(8) String : public Object
    {
    public:
        String() = default;
        static String CreateEmptyString(int32_t InLength);
        static String CreateStringUtf8(StdStringView InString);
        static String CreateStringUtf16(StdWStringView InString);

        StdString GetStringUtf8() const;
        StdWString GetStringUtf16() const;

        int32_t GetLength() const;

    private:
        String(int32_t InLength);
        String(const char16_t* InString, int32_t InLength);
    };
}
