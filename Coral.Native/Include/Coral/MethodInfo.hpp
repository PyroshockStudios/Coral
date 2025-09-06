#pragma once

#include "Core.hpp"
#include "String.hpp"

namespace Coral {

    class Type;
    class Attribute;
    class Object;
    class PropertyInfo;
    class MethodInfo
    {
    public:
        std::string GetName() const;

        Type& GetReturnType();
        const std::vector<Type*>& GetParameterTypes();

        TypeAccessibility GetAccessibility() const;
        bool IsStatic() const;

        bool HasAttribute(const Type& InAttributeType) const;
        std::vector<Attribute> GetAttributes() const;
        Attribute GetAttribute(const Type& InAttributeType) const;

        MetadataToken GetMetadataToken() const;

        constexpr inline operator bool() const { return m_Handle != -1; }

    private:
        ManagedHandle m_Handle = -1;
        Type* m_ReturnType = nullptr;
        std::vector<Type*> m_ParameterTypes;

        friend class Type;
        friend class Object;
        friend class PropertyInfo;
        friend class Assembly;
    };

}
