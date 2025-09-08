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
        using Object::Box;
        using Object::BoxRaw;
        using Object::GetFieldValue;
        using Object::GetFieldValueObject;
        using Object::GetFieldValueRaw;
        using Object::GetPropertyValue;
        using Object::GetPropertyValueObject;
        using Object::GetPropertyValueRaw;
        using Object::InvokeDelegate;
        using Object::InvokeDelegateRaw;
        using Object::InvokeDelegateRetRaw;
        using Object::SetFieldValue;
        using Object::SetFieldValueObject;
        using Object::SetFieldValueRaw;
        using Object::SetPropertyValue;
        using Object::SetPropertyValueObject;
        using Object::SetPropertyValueRaw;
        using Object::Unbox;
        using Object::UnboxRaw;

        String(int32_t InLength);
        String(const char16_t* InString, int32_t InLength);
    };
}
