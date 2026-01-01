#include "Coral/Array.hpp"
#include "Coral/Type.hpp"
#include "CoralManagedFunctions.hpp"

#include <cassert>

namespace Coral {
    Array::Array(const Type& type,int32_t length)
    {
        m_Handle = s_ManagedFunctions.CreateNewManagedArrayFptr(length, type.GetTypeId());
    }
    Array Array::CreateEmptyArray(const Type& type, int32_t InLength)
    {
        return Array(type, InLength);
    }

    void Array::SetElementRaw(int32_t InIndex, const void* InData)
    {
        assert(IsValid() && "Do not use an invalid array!");
        s_ManagedFunctions.SetArrayElementFptr(m_Handle, InData, false, InIndex);
    }

    void Array::GetElementRaw(int32_t InIndex, void* OutData) const
    {
        assert(IsValid() && "Do not use an invalid array!");
        s_ManagedFunctions.GetArrayElementFptr(m_Handle, OutData, false, InIndex);
    }

    void Array::SetElementObject(int32_t InIndex, const Coral::Object& InObject)
    {
        assert(IsValid() && "Do not use an invalid array!");
        s_ManagedFunctions.SetArrayElementFptr(m_Handle, &InObject, true, InIndex);
    }

    Coral::Object Array::GetElementObject(int32_t InIndex) const
    {
        assert(IsValid() && "Do not use an invalid array!");
        Object result = {};
        s_ManagedFunctions.GetArrayElementFptr(m_Handle, &result, true, InIndex);
        return result;
    }

}
