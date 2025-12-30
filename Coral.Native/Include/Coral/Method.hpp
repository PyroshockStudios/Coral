#pragma once

#include "Core.hpp"
#include "NativeString.hpp"

namespace Coral {

    class Type;
    class Attribute;
    class Object;
    class Property;
    class Method
    {
    public:
        StdString GetName() const;

        Type GetReturnType() const;
        StdVector<Type> GetParameterTypes() const;

        TypeAccessibility GetAccessibility() const;
        bool IsStatic() const;

        bool HasAttribute(const Type& InAttributeType) const;
        StdVector<Attribute> GetAttributes() const;
        Attribute GetAttribute(const Type& InAttributeType) const;

        MetadataToken GetMetadataToken() const;

        constexpr inline operator bool() const { return m_Handle != -1; }

    private:
        ManagedHandle m_Handle = -1;

        friend class Type;
        friend class Object;
        friend class Property;
        friend class Assembly;
    };

}
