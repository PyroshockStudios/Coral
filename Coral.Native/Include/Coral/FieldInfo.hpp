#pragma once

#include "Core.hpp"
#include "String.hpp"

namespace Coral {

    class Type;
    class Object;
    class Attribute;

    class FieldInfo
    {
    public:
        std::string GetName() const;
        Type& GetType();

        TypeAccessibility GetAccessibility() const;
        bool IsStatic() const;
        bool IsLiteral() const;

        std::vector<Attribute> GetAttributes() const;

        constexpr inline operator bool()
        {
            return m_Handle != -1;
        }

        MetadataToken GetMetadataToken() const
        {
            return static_cast<MetadataToken>(m_Handle);
        }

    private:
        ManagedHandle m_Handle = -1;
        Type* m_Type = nullptr;

        friend class Type;
        friend class Object;
    };

}
