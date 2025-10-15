#include "Coral/Attribute.hpp"
#include "Coral/Type.hpp"
#include "Coral/NativeString.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {

    Type Attribute::GetType()
    {
        Type type;
        s_ManagedFunctions.GetAttributeTypeFptr(m_Handle, &type.m_Id);
        return type;
    }

    template <>
    StdString Attribute::GetFieldValue(StdStringView InFieldName)
    {
        NativeString result;
        GetFieldValueInternal(InFieldName, &result);
        return StdString(result);
    }

    template <>
    bool Attribute::GetFieldValue(StdStringView InFieldName)
    {
        Bool32 result;
        GetFieldValueInternal(InFieldName, &result);
        return result;
    }

    void Attribute::GetFieldValueInternal(StdStringView InFieldName, void* OutValue) const
    {
        auto fieldName = NativeString::New(InFieldName);
        s_ManagedFunctions.GetAttributeFieldValueFptr(m_Handle, fieldName, OutValue);
        NativeString::Free(fieldName);
    }

}
