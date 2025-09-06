#include "Coral/FieldInfo.hpp"
#include "Coral/Type.hpp"
#include "Coral/Attribute.hpp"
#include "Coral/StringHelper.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {

    StdString FieldInfo::GetName() const
    {
        String str = s_ManagedFunctions.GetFieldInfoNameFptr(m_Handle);
        return StringHelper::ConsumeNativeString(str);
    }

    Type& FieldInfo::GetType()
    {
        if (!m_Type)
        {
            Type fieldType;
            s_ManagedFunctions.GetFieldInfoTypeFptr(m_Handle, &fieldType.m_Id);
            m_Type = TypeCache::Get().CacheType(std::move(fieldType));
        }

        return *m_Type;
    }

    TypeAccessibility FieldInfo::GetAccessibility() const
    {
        return s_ManagedFunctions.GetFieldInfoAccessibilityFptr(m_Handle);
    }

    bool FieldInfo::IsStatic() const
    {
        return s_ManagedFunctions.GetFieldInfoIsStaticFptr(m_Handle);
    }

    bool FieldInfo::IsLiteral() const
    {
        return s_ManagedFunctions.GetFieldInfoIsLiteralFptr(m_Handle);
    }

    bool FieldInfo::HasAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        return GetAttribute(InAttributeType);
    }

    StdVector<Attribute> FieldInfo::GetAttributes() const
    {
        int32_t attributeCount;
        s_ManagedFunctions.GetFieldInfoAttributesFptr(m_Handle, nullptr, &attributeCount);
        StdVector<ManagedHandle> attributeHandles(static_cast<size_t>(attributeCount));
        s_ManagedFunctions.GetFieldInfoAttributesFptr(m_Handle, attributeHandles.data(), &attributeCount);

        StdVector<Attribute> result(attributeHandles.size());
        for (size_t i = 0; i < attributeHandles.size(); i++)
            result[i].m_Handle = attributeHandles[i];

        return result;
    }

    Attribute FieldInfo::GetAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        auto list = GetAttributes();
        for (Attribute& attr : list) {
            if (attr.GetType() == InAttributeType) return attr;
        }
        return {};
    }

    MetadataToken FieldInfo::GetMetadataToken() const
    {
        return s_ManagedFunctions.GetFieldInfoTokenFptr(m_Handle);
    }
}
