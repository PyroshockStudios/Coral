#pragma once

#include "Core.hpp"
#include "Utility.hpp"
#include "String.hpp"

namespace Coral {

    class FieldInfo;
    class MethodInfo;
    class PropertyInfo;

    class Assembly;
    class Type;

    class alignas(8) Object
    {
    public:
        Object() = default;
        Object(const Object& InOther);
        Object(Object&& InOther) noexcept;
        ~Object();

        Object& operator=(const Object& InOther);
        Object& operator=(Object&& InOther) noexcept;

        template <typename TReturn = void, typename... TArgs>
        auto InvokeMethod(const MethodInfo& InMethod, MethodParams<TArgs...>&& InParameters = {}, Object* OutException = nullptr) const
        {
            constexpr size_t paramCount = sizeof...(TArgs);

            if constexpr (std::is_void_v<TReturn>)
            {
                if (InParameters.paramCount > 0)
                {
                    InvokeMethodInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount);
                }
                else
                {
                    InvokeMethodInternal(OutException, InMethod, nullptr, nullptr, 0);
                }
            }
            else
            {
                TReturn result {};
                if (InParameters.paramCount > 0)
                {
                    InvokeMethodRetInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, &result);
                }
                else
                {
                    InvokeMethodRetInternal(OutException, InMethod, nullptr, nullptr, 0, &result);
                }
                return result;
            }
        }

        template <typename TReturn = void, typename... TArgs>
        auto InvokeDelegate(MethodParams<TArgs...>&& InParameters = {}, Object* OutException = nullptr) const
        {
            constexpr size_t paramCount = sizeof...(TArgs);

            if constexpr (std::is_void_v<TReturn>)
            {
                if (InParameters.paramCount > 0)
                {
                    InvokeDelegateInternal(OutException, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount);
                }
                else
                {
                    InvokeDelegateInternal(OutException, nullptr, nullptr, 0);
                }
            }
            else
            {
                TReturn result {};
                if (InParameters.paramCount > 0)
                {
                    InvokeDelegateRetInternal(OutException, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, &result);
                }
                else
                {
                    InvokeDelegateRetInternal(OutException, nullptr, nullptr, 0, &result);
                }
                return result;
            }
        }

        template <typename TValue>
        void SetFieldValue(const FieldInfo& InField, TValue InValue) const
        {
            SetFieldValueRaw(InField, &InValue);
        }

        template <typename TReturn>
        TReturn GetFieldValue(const FieldInfo& InField) const
        {
            TReturn result;
            GetFieldValueRaw(InField, &result);
            return result;
        }

        template <typename TValue>
        void SetPropertyValue(const PropertyInfo& InProperty, TValue InValue) const
        {
            SetPropertyValueRaw(InProperty, &InValue);
        }

        template <typename TReturn>
        TReturn GetPropertyValue(const PropertyInfo& InProperty) const
        {
            TReturn result;
            GetPropertyValueRaw(InProperty, &result);
            return result;
        }

        void SetFieldValueRaw(const FieldInfo& InField, void* InValue) const;
        void GetFieldValueRaw(const FieldInfo& InField, void* OutValue) const;
        void SetPropertyValueRaw(const PropertyInfo& InProperty, void* InValue, Object* OutException = nullptr) const;
        void GetPropertyValueRaw(const PropertyInfo& InProperty, void* OutValue, Object* OutException = nullptr) const;

        const Type& GetType();

        void Destroy();

        bool IsValid() const { return m_Handle != nullptr && m_Type != nullptr; }

    private:
        void InvokeMethodInternal(Object* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
        void InvokeMethodRetInternal(Object* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, void* InResultStorage) const;

        void InvokeDelegateInternal(Object* OutException, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
        void InvokeDelegateRetInternal(Object* OutException, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, void* InResultStorage) const;

    public:
        alignas(8) void* m_Handle = nullptr;
        alignas(8) const Type* m_Type = nullptr;

    private:
        friend class Assembly;
        friend class Type;
    };

    static_assert(offsetof(Object, m_Handle) == 0);
    static_assert(offsetof(Object, m_Type) == 8);
    static_assert(sizeof(Object) == 16);

    template <>
    inline void Object::SetFieldValue(const FieldInfo& InField, std::string InValue) const
    {
        String s = String::New(InValue);
        SetFieldValueRaw(InField, &s);
        String::Free(s);
    }

    template <>
    inline void Object::SetFieldValue(const FieldInfo& InField, bool InValue) const
    {
        Bool32 s = InValue;
        SetFieldValueRaw(InField, &s);
    }

    template <>
    inline std::string Object::GetFieldValue(const FieldInfo& InField) const
    {
        String result;
        GetFieldValueRaw(InField, &result);
        auto s = result.Data() ? std::string(result) : "";
        String::Free(result);
        return s;
    }

    template <>
    inline bool Object::GetFieldValue(const FieldInfo& InField) const
    {
        Bool32 result;
        GetFieldValueRaw(InField, &result);
        return result;
    }

}
