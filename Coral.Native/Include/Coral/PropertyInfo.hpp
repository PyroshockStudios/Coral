#pragma once

#include "Core.hpp"
#include "String.hpp"

namespace Coral {

    class Type;
    class Object;
    class Attribute;
    class MethodInfo;

    class PropertyInfo
    {
    public:
        std::string GetName() const;
        Type& GetType();

        MethodInfo GetGetMethod() const;
        MethodInfo GetSetMethod() const;

        std::vector<Attribute> GetAttributes() const;
        Attribute GetAttribute(const Type& InAttributeType) const;

        constexpr inline operator bool()
        {
            return m_Handle != -1;
        }
    private:
        ManagedHandle m_Handle = -1;
        Type* m_Type = nullptr;

        friend class Type;
        friend class Object;
    };

}
