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
        void GetElementRaw(int32_t InIndex, void* OutData) const;
        void SetElementObject(int32_t InIndex, const Coral::Object& InObject);
        Coral::Object GetElementObject(int32_t InIndex) const;

        template <typename T>
        void SetElement(int32_t InIndex, const T& InData)
        {
            SetElementRaw(InIndex, &InData);
        }
        template <typename T>
        T GetElement(int32_t InIndex) const
        {
            T result = {};
            GetElementRaw(InIndex, &result);
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
    inline Coral::Object Array::GetElement(int32_t InIndex) const
    {
        return GetElementObject(InIndex);
    }
}
