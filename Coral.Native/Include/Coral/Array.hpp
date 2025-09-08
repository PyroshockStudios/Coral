#pragma once

#include "Object.hpp"
#include "NativeArray.hpp"

namespace Coral {
    class Type;
    class alignas(8) Array : public Object
    {
    public:
        Array() = default;
        static Array CreateEmptyArray(int32_t InLength, const Type& type);

        void SetElementRaw(const void* InData, int32_t InIndex);
        void GetElementRaw(void* OutData, int32_t InIndex);
        void SetElementObject(const Coral::Object& InObject, int32_t InIndex);
        Coral::Object GetElementObject(int32_t InIndex);

        template <typename T>
        void SetElement(const T& InData, int32_t InIndex)
        {
            SetElementRaw(&InData, InIndex);
        }
        template <typename T>
        T GetElement(int32_t InIndex)
        {
            T result = {};
            GetElementRaw(&result, InIndex);
            return result;
        }

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

        Array(int32_t length, const Type& type);
    };

    
    template <>
    inline void Array::SetElement(const Coral::Object& InValue, int32_t InIndex)
    {
        SetElementObject(InValue, InIndex);
    }
    template <>
    inline Coral::Object Array::GetElement(int32_t InIndex)
    {
        return GetElementObject(InIndex);
    }
}
