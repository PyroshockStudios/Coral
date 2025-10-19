#pragma once

#include "Core.hpp"
#include "NativeString.hpp"
#include "Object.hpp"
#include "Method.hpp"
#include "Field.hpp"
#include "Property.hpp"

namespace Coral {
    class TypeImpl;
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

        Type GetBaseType() const;
        StdVector<Type> GetInterfaceTypes() const;

        int32_t GetSize() const;

        bool IsSubclassOf(const Type& InOther) const;
        bool IsAssignableTo(const Type& InOther) const;
        bool IsAssignableFrom(const Type& InOther) const;

        StdVector<Method> GetMethods() const;
        StdVector<Field> GetFields() const;
        StdVector<Property> GetProperties() const;

        Method GetMethod(StdStringView MethodName, bool InStatic = false) const;
        Method GetMethod(StdStringView MethodName, int32_t InParamCount, bool InStatic = false) const;
        Method GetMethodByParamTypes(StdStringView MethodName, const StdVector<Type>& InParamTypes, bool InStatic = false) const;
        Field GetField(StdStringView FieldName, bool InStatic = false) const;
        Property GetProperty(StdStringView PropertyName, bool InStatic = false) const;

        bool HasAttribute(const Type& InAttributeType) const;
        StdVector<Attribute> GetAttributes() const;
        Attribute GetAttribute(const Type& InAttributeType) const;

        ManagedType GetManagedType() const;

        Type GetGenericArgument(int32_t InArgIndex) const;
        Type GetGenericTypeDefinition() const;

        bool IsSZArray() const;
        bool IsArray() const;
        bool IsClass() const;
        bool IsInterface() const;
        bool IsAbstract() const;
        bool IsSealed() const;
        bool IsValueType() const;

        Type GetElementType() const;

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
        auto InvokeStaticMethod(const Method& InMethod, MethodParams<TArgs...>&& InParameters = {}, Object* OutException = nullptr) const
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
        void InvokeStaticMethodRaw(const Method& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException = nullptr) const;
        void InvokeStaticMethodRetRaw(const Method& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage, Object* OutException = nullptr) const;

    private:
        TypeId m_Id = -1;

        friend class HostInstance;
        friend class Assembly;
        friend class AssemblyLoadContext;
        friend class Method;
        friend class Field;
        friend class Property;
        friend class Attribute;
        friend class ReflectionType;
        friend class Object;
        friend class TypeImpl;
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
