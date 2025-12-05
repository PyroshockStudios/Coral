#pragma once

#include "Object.hpp"

namespace Coral {
    class alignas(8) String : private Object
    {
    public:
        String() = default;
        static String CreateEmptyString(int32_t InLength);
        static String CreateStringUtf8(StdStringView InString);
        static String CreateStringUtf16(StdWStringView InString);

        StdString GetStringUtf8() const;
        StdWString GetStringUtf16() const;

        int32_t GetLength() const;
        
        using Object::GetType;
        using Object::Destroy;
        using Object::IsValid;
        using Object::operator bool;
        using Object::operator=;

    private:
        String(int32_t InLength);
        String(const char16_t* InString, int32_t InLength);
    };
}
