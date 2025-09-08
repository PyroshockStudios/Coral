#pragma once

#include "Core.hpp"
#include "NativeString.hpp"

namespace Coral {

    class Type;
    class Attribute;
    class Object;
    class PropertyInfo;
    class MethodInfo
    {
    public:
        StdString GetName() const;

        Type& GetReturnType();
        const StdVector<Type>& GetParameterTypes();

        TypeAccessibility GetAccessibility() const;
        bool IsStatic() const;

        bool HasAttribute(const Type& InAttributeType) const;
        StdVector<Attribute> GetAttributes() const;
        Attribute GetAttribute(const Type& InAttributeType) const;

        MetadataToken GetMetadataToken() const;

        constexpr inline operator bool() const { return m_Handle != -1; }

    private:
        ManagedHandle m_Handle = -1;
        Type* m_ReturnType = nullptr;
        StdOptional<StdVector<Type>> m_ParameterTypes;

        friend class Type;
        friend class Object;
        friend class PropertyInfo;
        friend class Assembly;
    };

}
