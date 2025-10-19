#pragma once

#include "Core.hpp"
#include "NativeString.hpp"

namespace Coral {

    class Type;
    class Object;
    class Attribute;
    class Method;

    class Property
    {
    public:
        StdString GetName() const;
        Type& GetType();

        Method GetGetMethod() const;
        Method GetSetMethod() const;

        bool HasAttribute(const Type& InAttributeType) const;
        StdVector<Attribute> GetAttributes() const;
        Attribute GetAttribute(const Type& InAttributeType) const;

        constexpr inline operator bool() const { return m_Handle != -1; }
    private:
        ManagedHandle m_Handle = -1;
        Type* m_Type = nullptr;

        friend class Type;
        friend class Object;
    };

}
