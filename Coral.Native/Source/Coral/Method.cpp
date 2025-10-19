#include "Coral/Method.hpp"
#include "Coral/Type.hpp"
#include "Coral/Attribute.hpp"
#include "Coral/StringHelper.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {

    StdString Method::GetName() const
    {
        NativeString str = s_ManagedFunctions.GetMethodInfoNameFptr(m_Handle);
        return StringHelper::ConsumeNativeString(str);
    }

    Type& Method::GetReturnType()
    {
        Type returnType;
        s_ManagedFunctions.GetMethodInfoReturnTypeFptr(m_Handle, &returnType.m_Id);
        return returnType;
    }

    const StdVector<Type>& Method::GetParameterTypes()
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
                types[i] = type;
            }
            m_ParameterTypes.emplace(std::move(types));
        }

        return *m_ParameterTypes;
    }

    TypeAccessibility Method::GetAccessibility() const
    {
        return s_ManagedFunctions.GetMethodInfoAccessibilityFptr(m_Handle);
    }

    bool Method::IsStatic() const
    {
        return s_ManagedFunctions.GetMethodInfoIsStaticFptr(m_Handle);
    }

    bool Method::HasAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        return GetAttribute(InAttributeType);
    }

    StdVector<Attribute> Method::GetAttributes() const
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

    Attribute Method::GetAttribute(const Type& InAttributeType) const
    {
        // TODO: make efficient
        auto list = GetAttributes();
        for (Attribute& attr : list)
        {
            if (attr.GetType() == InAttributeType) return attr;
        }
        return {};
    }

    MetadataToken Method::GetMetadataToken() const
    {
        return s_ManagedFunctions.GetMethodInfoTokenFptr(m_Handle);
    }
}
