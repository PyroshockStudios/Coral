#pragma once

#include "Object.hpp"
#include "NativeArray.hpp"

namespace Coral {
    class Type;
    class alignas(8) Array : private Object
    {
    public:
        Array() = default;
        static Array CreateEmptyArray(const Type& type, int32_t InLength);

        void SetElementRaw(int32_t InIndex, const void* InData);
        void GetElementRaw(int32_t InIndex, void* OutData);
        void SetElementObject(int32_t InIndex, const Coral::Object& InObject);
        Coral::Object GetElementObject(int32_t InIndex);

        template <typename T>
        void SetElement(int32_t InIndex, const T& InData)
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
        using Object::GetType;
        using Object::Destroy;
        using Object::IsValid;
        using Object::operator bool;
        using Object::operator=;

    private:
        Array(const Type& type, int32_t InLength);
    };

    template <>
    inline void Array::SetElement(int32_t InIndex, const Coral::Object& InValue)
    {
        SetElementObject(InIndex, InValue);
    }
    template <>
    inline Coral::Object Array::GetElement(int32_t InIndex)
    {
        return GetElementObject(InIndex);
    }
}
