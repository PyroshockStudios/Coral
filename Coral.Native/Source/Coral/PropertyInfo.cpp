#include "Coral/PropertyInfo.hpp"
#include "Coral/MethodInfo.hpp"
#include "Coral/Type.hpp"
#include "Coral/Attribute.hpp"
#include "Coral/StringHelper.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {

    StdString PropertyInfo::GetName() const
    {
        NativeString str = s_ManagedFunctions.GetPropertyInfoNameFptr(m_Handle);
        return StringHelper::ConsumeNativeString(str);
    }

    Type& PropertyInfo::GetType()
    {
        if (!m_Type)
        {
            Type propertyType;
            s_ManagedFunctions.GetPropertyInfoTypeFptr(m_Handle, &propertyType.m_Id);
            m_Type = TypeCache::Get().CacheType(std::move(propertyType));
        }

        return *m_Type;
    }

    MethodInfo PropertyInfo::GetGetMethod() const
    {
        MethodInfo info;
        info.m_Handle = s_ManagedFunctions.GetPropertyInfoGetMethodFptr(m_Handle);
        return info;
    }

    MethodInfo PropertyInfo::GetSetMethod() const
    {
        MethodInfo info;
        info.m_Handle = s_ManagedFunctions.GetPropertyInfoSetMethodFptr(m_Handle);
        return info;
    }

    bool PropertyInfo::HasAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        return GetAttribute(InAttributeType);
    }

    StdVector<Attribute> PropertyInfo::GetAttributes() const
    {
        int32_t attributeCount;
        s_ManagedFunctions.GetPropertyInfoAttributesFptr(m_Handle, nullptr, &attributeCount);

        StdVector<ManagedHandle> attributeHandles(static_cast<size_t>(attributeCount));
        s_ManagedFunctions.GetPropertyInfoAttributesFptr(m_Handle, attributeHandles.data(), &attributeCount);

        StdVector<Attribute> result(attributeHandles.size());
        for (size_t i = 0; i < attributeHandles.size(); i++)
            result[i].m_Handle = attributeHandles[i];

        return result;
    }

    Attribute PropertyInfo::GetAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        auto list = GetAttributes();
        for (Attribute& attr : list) {
            if (attr.GetType() == InAttributeType) return attr;
        }
        return {};
    }

}
