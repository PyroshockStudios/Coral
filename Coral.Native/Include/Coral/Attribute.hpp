#pragma once

#include "Core.hpp"
#include "NativeString.hpp"

namespace Coral {

    class Type;

    class Attribute
    {
    public:
        Type GetType() const;

        template <typename TReturn>
        TReturn GetFieldValue(StdStringView InFieldName) const
        {
            TReturn result;
            GetFieldValueInternal(InFieldName, &result);
            return result;
        }

        constexpr inline operator bool() const
        {
            return m_Handle != -1;
        }

    private:
        void GetFieldValueInternal(StdStringView InFieldName, void* OutValue) const;

    private:
        ManagedHandle m_Handle = -1;

        friend class Type;
        friend class Method;
        friend class Field;
        friend class Property;
    };

    template <>
    StdString Attribute::GetFieldValue(StdStringView InFieldName) const;

    template <>
    bool Attribute::GetFieldValue(StdStringView InFieldName) const;

}
