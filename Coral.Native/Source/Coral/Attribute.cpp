#include "Coral/Attribute.hpp"
#include "Coral/Type.hpp"
#include "Coral/NativeString.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {

    Type& Attribute::GetType()
    {
        if (!m_Type)
        {
            Type type;
            s_ManagedFunctions.GetAttributeTypeFptr(m_Handle, &type.m_Id);
            m_Type = TypeCache::Get().CacheType(std::move(type));
        }

        return *m_Type;
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
