#include "Coral/MethodInfo.hpp"
#include "Coral/Type.hpp"
#include "Coral/Attribute.hpp"
#include "Coral/StringHelper.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {

    StdString MethodInfo::GetName() const
    {
        NativeString str = s_ManagedFunctions.GetMethodInfoNameFptr(m_Handle);
        return StringHelper::ConsumeNativeString(str);
    }

    Type& MethodInfo::GetReturnType()
    {
        if (!m_ReturnType)
        {
            Type returnType;
            s_ManagedFunctions.GetMethodInfoReturnTypeFptr(m_Handle, &returnType.m_Id);
            m_ReturnType = TypeCache::Get().CacheType(std::move(returnType));
        }

        return *m_ReturnType;
    }

    const StdVector<Type>& MethodInfo::GetParameterTypes()
    {
        if (!m_ParameterTypes)
        {
            int32_t parameterCount;
            s_ManagedFunctions.GetMethodInfoParameterTypesFptr(m_Handle, nullptr, &parameterCount);

            StdVector<TypeId> parameterTypes(static_cast<size_t>(parameterCount));
            s_ManagedFunctions.GetMethodInfoParameterTypesFptr(m_Handle, parameterTypes.data(), &parameterCount);

            StdVector<Type> types {};
            
            types.resize(parameterTypes.size());

            for (size_t i = 0; i < parameterTypes.size(); i++)
            {
                Type type;
                type.m_Id = parameterTypes[i];
                types[i] = *TypeCache::Get().CacheType(std::move(type));
            }
            m_ParameterTypes.emplace(std::move(types));
        }

        return *m_ParameterTypes;
    }

    TypeAccessibility MethodInfo::GetAccessibility() const
    {
        return s_ManagedFunctions.GetMethodInfoAccessibilityFptr(m_Handle);
    }

    bool MethodInfo::IsStatic() const
    {
        return s_ManagedFunctions.GetMethodInfoIsStaticFptr(m_Handle);
    }
    
    bool MethodInfo::HasAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        return GetAttribute(InAttributeType);
    }

    StdVector<Attribute> MethodInfo::GetAttributes() const
    {
        int32_t attributeCount;
        s_ManagedFunctions.GetMethodInfoAttributesFptr(m_Handle, nullptr, &attributeCount);

        StdVector<ManagedHandle> attributeHandles(static_cast<size_t>(attributeCount));
        s_ManagedFunctions.GetMethodInfoAttributesFptr(m_Handle, attributeHandles.data(), &attributeCount);

        StdVector<Attribute> result(attributeHandles.size());
        for (size_t i = 0; i < attributeHandles.size(); i++)
            result[i].m_Handle = attributeHandles[i];

        return result;
    }

    Attribute MethodInfo::GetAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        auto list = GetAttributes();
        for (Attribute& attr : list) {
            if (attr.GetType() == InAttributeType) return attr;
        }
        return {};
    }

    MetadataToken MethodInfo::GetMetadataToken() const
    {
        return s_ManagedFunctions.GetMethodInfoTokenFptr(m_Handle);
    }
}
