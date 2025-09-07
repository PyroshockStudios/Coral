#pragma once

#include "Core.hpp"
#include "NativeString.hpp"
#include "Object.hpp"
#include "MethodInfo.hpp"
#include "FieldInfo.hpp"
#include "PropertyInfo.hpp"

#include <optional>

namespace Coral {
    class ReflectionType;
    class Type
    {
    public:
        static const Type& VoidType();
        static const Type& ByteType();
        static const Type& SByteType();
        static const Type& ShortType();
        static const Type& UShortType();
        static const Type& IntType();
        static const Type& UIntType();
        static const Type& LongType();
        static const Type& ULongType();
        static const Type& FloatType();
        static const Type& DoubleType();
        static const Type& BoolType();
        static const Type& CharType();
        static const Type& StringType();
        static const Type& ObjectType();
        static const Type& IntPtrType();
        static const Type& UIntPtrType();
        static const Type& DecimalType();
        static const Type& DateTimeType();
        static const Type& ExceptionType();
        static const Type& ArrayType();

    public:
        StdString GetFullName() const;
        StdString GetName() const;
        StdString GetNamespace() const;
        StdString GetAssemblyQualifiedName() const;

        const Type& GetBaseType() const;
        const StdVector<Type>& GetInterfaceTypes() const;

        int32_t GetSize() const;

        bool IsSubclassOf(const Type& InOther) const;
        bool IsAssignableTo(const Type& InOther) const;
        bool IsAssignableFrom(const Type& InOther) const;

        StdVector<MethodInfo> GetMethods() const;
        StdVector<FieldInfo> GetFields() const;
        StdVector<PropertyInfo> GetProperties() const;

        MethodInfo GetMethod(StdStringView MethodName, bool InStatic = false) const;
        MethodInfo GetMethod(StdStringView MethodName, int32_t InParamCount, bool InStatic = false) const;
        MethodInfo GetMethodByParamTypes(StdStringView MethodName, const StdVector<Type>& InParamTypes, bool InStatic = false) const;
        FieldInfo GetField(StdStringView FieldName, bool InStatic = false) const;
        PropertyInfo GetProperty(StdStringView PropertyName, bool InStatic = false) const;

        bool HasAttribute(const Type& InAttributeType) const;
        StdVector<Attribute> GetAttributes() const;
        Attribute GetAttribute(const Type& InAttributeType) const;

        ManagedType GetManagedType() const;

        const Type& GetGenericArgument(int32_t InArgIndex) const;
        const Type& GetGenericTypeDefinition() const;

        bool IsSZArray() const;
        const Type& GetElementType() const;

        bool operator==(const Type& InOther) const;

        operator bool() const { return m_Id != -1; }

        TypeId GetTypeId() const { return m_Id; }
        operator ReflectionType() const;

    public:
        template <typename... TArgs>
        Object CreateInstance(MethodParams<TArgs...>&& InParameters = {}, Object* OutException = nullptr) const
        {
            if (InParameters.paramCount > 0)
            {
                return CreateInstanceRaw(InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, OutException);
            }
            else
            {
                return CreateInstanceRaw(nullptr, nullptr, 0, OutException);
            }
        }

        template <typename TReturn = void, typename... TArgs>
        auto InvokeStaticMethod(const MethodInfo& InMethod, MethodParams<TArgs...>&& InParameters = {}, Object* OutException = nullptr) const
        {
            if constexpr (std::is_void_v<TReturn>)
            {
                if (InParameters.paramCount > 0)
                {
                    InvokeStaticMethodRaw(InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, OutException);
                }
                else
                {
                    InvokeStaticMethodRaw(InMethod, nullptr, nullptr, 0, OutException);
                }
            }
            else
            {
                TReturn result {};
                if (InParameters.paramCount > 0)
                {
                    InvokeStaticMethodRetRaw(InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, std::derived_from<TReturn, Object>, &result, OutException);
                }
                else
                {
                    InvokeStaticMethodRetRaw(InMethod, nullptr, nullptr, 0, std::derived_from<TReturn, Object>, &result, OutException);
                }
                return result;
            }
        }

        Object CreateInstanceRaw(const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException = nullptr) const;
        void InvokeStaticMethodRaw(const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException = nullptr) const;
        void InvokeStaticMethodRetRaw(const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage, Object* OutException = nullptr) const;

    private:
        TypeId m_Id = -1;
        mutable Type* m_BaseType = nullptr;
        mutable StdOptional<StdVector<Type>> m_InterfaceTypes = StdNullOpt;
        mutable Type* m_ElementType = nullptr;

        friend class HostInstance;
        friend class Assembly;
        friend class AssemblyLoadContext;
        friend class MethodInfo;
        friend class FieldInfo;
        friend class PropertyInfo;
        friend class Attribute;
        friend class ReflectionType;
        friend class Object;
    };

    class ReflectionType
    {
    public:
        operator const Type&() const;

    public:
        TypeId m_TypeID;
    };

    static_assert(offsetof(ReflectionType, m_TypeID) == 0);
    static_assert(sizeof(ReflectionType) == 4);
}
