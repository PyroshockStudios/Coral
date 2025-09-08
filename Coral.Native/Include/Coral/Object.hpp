#pragma once

#include "Core.hpp"
#include "Utility.hpp"
#include "NativeString.hpp"

namespace Coral {
    namespace Internal {
        struct Object_LayoutTest;
    }
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
            if constexpr (std::is_void_v<TReturn>)
            {
                if (InParameters.paramCount > 0)
                {
                    InvokeMethodRaw(InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount);
                }
                else
                {
                    InvokeMethodRaw(InMethod, nullptr, nullptr, 0);
                }
            }
            else
            {
                TReturn result {};
                if (InParameters.paramCount > 0)
                {
                    InvokeMethodRetRaw(InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, std::derived_from<TReturn, Object>, &result, OutException);
                }
                else
                {
                    InvokeMethodRetRaw(InMethod, nullptr, nullptr, 0, std::derived_from<TReturn, Object>, &result, OutException);
                }
                return result;
            }
        }

        template <typename TReturn = void, typename... TArgs>
        auto InvokeDelegate(MethodParams<TArgs...>&& InParameters = {}, Object* OutException = nullptr) const
        {
            if constexpr (std::is_void_v<TReturn>)
            {
                if (InParameters.paramCount > 0)
                {
                    InvokeDelegateRaw(InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, OutException);
                }
                else
                {
                    InvokeDelegateRaw(nullptr, nullptr, 0);
                }
            }
            else
            {
                TReturn result {};
                if (InParameters.paramCount > 0)
                {
                    InvokeDelegateRetRaw(InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, std::derived_from<TReturn, Object>, &result, OutException);
                }
                else
                {
                    InvokeDelegateRetRaw(nullptr, nullptr, 0, std::derived_from<TReturn, Object>, &result, OutException);
                }
                return result;
            }
        }

        template <typename TValue>
        void SetFieldValue(const FieldInfo& InField, const TValue& InValue)
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
        void SetPropertyValue(const PropertyInfo& InProperty, TValue InValue)
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

        void SetFieldValueRaw(const FieldInfo& InField, const void* InValue);
        void GetFieldValueRaw(const FieldInfo& InField, void* OutValue) const;
        void SetFieldValueObject(const FieldInfo& InField, const Object& InObject);
        Object GetFieldValueObject(const FieldInfo& InField) const;

        void SetPropertyValueRaw(const PropertyInfo& InProperty, const void* InValue, Object* OutException = nullptr);
        void GetPropertyValueRaw(const PropertyInfo& InProperty, void* OutValue, Object* OutException = nullptr) const;
        void SetPropertyValueObject(const PropertyInfo& InProperty, const Object& InObject, Object* OutException = nullptr);
        Object GetPropertyValueObject(const PropertyInfo& InProperty, Object* OutException = nullptr) const;

        const Type& GetType() const;

        void Destroy();

        bool IsValid() const { return m_Handle != nullptr; }

        inline operator bool() const { return IsValid(); }

        template <typename T>
        static Object Box(const T& value, const Type& type)
        {
            constexpr ManagedType t = GetManagedType<T>();
            static_assert(IsManagedTypeValueType(t) || t == ManagedType::Unknown /*could be struct*/, "Only value types can be boxed/unboxed!");
            return BoxRaw(&value, sizeof(T), type);
        }
        template <typename T>
        T Unbox()
        {
            constexpr ManagedType t = GetManagedType<T>();
            static_assert(IsManagedTypeValueType(t) || t == ManagedType::Unknown /*could be struct*/, "Only value types can be boxed/unboxed!");
            T v;
            UnboxRaw(&v);
            return v;
        }

        static Object BoxRaw(const void* InValue, int32_t InSize, const Type& InType);
        void UnboxRaw(void* OutValue) const;

        void InvokeMethodRaw(const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException = nullptr) const;
        void InvokeMethodRetRaw(const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage, Object* OutException = nullptr) const;

        void InvokeDelegateRaw(const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException = nullptr) const;
        void InvokeDelegateRetRaw(const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage, Object* OutException = nullptr) const;

    protected:
        alignas(8) void* m_Handle = nullptr;
        alignas(8) mutable const Type* m_Type = nullptr;

    private:
        friend struct Internal::Object_LayoutTest;
        friend class Assembly;
        friend class Type;
    };

    namespace Internal {
        struct Object_LayoutTest
        {
            static_assert(offsetof(Object, m_Handle) == 0);
            static_assert(offsetof(Object, m_Type) == 8);
            static_assert(sizeof(Object) == 16);
        };
    }

    template <>
    inline void Object::SetFieldValue(const FieldInfo& InField, const Coral::Object& InValue)
    {
        SetFieldValueObject(InField, InValue);
    }

    template <>
    inline void Object::SetFieldValue(const FieldInfo& InField, const StdString& InValue)
    {
        NativeString s = NativeString::New(InValue);
        SetFieldValueRaw(InField, &s);
        NativeString::Free(s);
    }

    template <>
    inline void Object::SetFieldValue(const FieldInfo& InField, const bool& InValue)
    {
        Bool32 s = InValue;
        SetFieldValueRaw(InField, &s);
    }

    template <>
    inline Object Object::GetFieldValue(const FieldInfo& InField) const
    {
        return GetFieldValueObject(InField);
    }

    template <>
    inline StdString Object::GetFieldValue(const FieldInfo& InField) const
    {
        NativeString result;
        GetFieldValueRaw(InField, &result);
        auto s = result.Data() ? StdString(result) : "";
        NativeString::Free(result);
        return s;
    }

    template <>
    inline bool Object::GetFieldValue(const FieldInfo& InField) const
    {
        Bool32 result;
        GetFieldValueRaw(InField, &result);
        return result;
    }

    template <>
    inline void Object::SetPropertyValue(const PropertyInfo& InProperty, const Coral::Object& InValue)
    {
        SetPropertyValueObject(InProperty, InValue);
    }

    template <>
    inline void Object::SetPropertyValue(const PropertyInfo& InProperty, const StdString& InValue)
    {
        NativeString s = NativeString::New(InValue);
        SetPropertyValueRaw(InProperty, &s);
        NativeString::Free(s);
    }

    template <>
    inline void Object::SetPropertyValue(const PropertyInfo& InProperty, bool InValue)
    {
        Bool32 s = InValue;
        SetPropertyValueRaw(InProperty, &s);
    }

    template <>
    inline Object Object::GetPropertyValue(const PropertyInfo& InProperty) const
    {
        return GetPropertyValueObject(InProperty);
    }

    template <>
    inline StdString Object::GetPropertyValue(const PropertyInfo& InProperty) const
    {
        NativeString result;
        GetPropertyValueRaw(InProperty, &result);
        auto s = result.Data() ? StdString(result) : "";
        NativeString::Free(result);
        return s;
    }

    template <>
    inline bool Object::GetPropertyValue(const PropertyInfo& InProperty) const
    {
        Bool32 result;
        GetPropertyValueRaw(InProperty, &result);
        return result;
    }
}
