#pragma once

#include "Core.hpp"
#include "String.hpp"

namespace Coral {

    class Type;

    class Attribute
    {
    public:
        Type& GetType();

        template <typename TReturn>
        TReturn GetFieldValue(StdStringView InFieldName)
        {
            TReturn result;
            GetFieldValueInternal(InFieldName, &result);
            return result;
        }

        constexpr inline operator bool()
        {
            return m_Handle != -1;
        }

    private:
        void GetFieldValueInternal(StdStringView InFieldName, void* OutValue) const;

    private:
        ManagedHandle m_Handle = -1;
        Type* m_Type = nullptr;

        friend class Type;
        friend class MethodInfo;
        friend class FieldInfo;
        friend class PropertyInfo;
    };

}
