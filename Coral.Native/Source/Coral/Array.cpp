#include "Coral/Array.hpp"
#include "Coral/Type.hpp"
#include "CoralManagedFunctions.hpp"

#include <cassert>

namespace Coral {
    Array::Array(int32_t length, const Type& type)
    {
        m_Handle = s_ManagedFunctions.CreateNewManagedArrayFptr(length, type.GetTypeId());
    }
    Array Array::CreateEmptyArray(int32_t InLength, const Type& type)
    {
        return Array(InLength, type);
    }

    void Array::SetElementRaw(const void* InData, int32_t InIndex)
    {
        assert(IsValid() && "Do not use an invalid array!");
        s_ManagedFunctions.SetArrayElementFptr(m_Handle, InData, false, InIndex);
    }

    void Array::GetElementRaw(void* OutData, int32_t InIndex)
    {
        assert(IsValid() && "Do not use an invalid array!");
        s_ManagedFunctions.GetArrayElementFptr(m_Handle, OutData, false, InIndex);
    }

    void Array::SetElementObject(const Coral::Object& InObject, int32_t InIndex)
    {
        assert(IsValid() && "Do not use an invalid array!");
        s_ManagedFunctions.SetArrayElementFptr(m_Handle, &InObject, true, InIndex);
    }

    Coral::Object Array::GetElementObject(int32_t InIndex)
    {
        assert(IsValid() && "Do not use an invalid array!");
        Object result = {};
        s_ManagedFunctions.GetArrayElementFptr(m_Handle, &result, true, InIndex);
        return result;
    }

}
