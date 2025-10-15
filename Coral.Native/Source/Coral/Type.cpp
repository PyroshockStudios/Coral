#include "Coral/Type.hpp"
#include "Coral/Attribute.hpp"
#include "Coral/StringHelper.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {
    static Type s_NullType;

    const Type& Type::VoidType() { return TypeCache::Get().m_VoidType; }
    const Type& Type::ByteType() { return TypeCache::Get().m_ByteType; }
    const Type& Type::SByteType() { return TypeCache::Get().m_SByteType; }
    const Type& Type::ShortType() { return TypeCache::Get().m_ShortType; }
    const Type& Type::UShortType() { return TypeCache::Get().m_UShortType; }
    const Type& Type::IntType() { return TypeCache::Get().m_IntType; }
    const Type& Type::UIntType() { return TypeCache::Get().m_UIntType; }
    const Type& Type::LongType() { return TypeCache::Get().m_LongType; }
    const Type& Type::ULongType() { return TypeCache::Get().m_ULongType; }
    const Type& Type::FloatType() { return TypeCache::Get().m_FloatType; }
    const Type& Type::DoubleType() { return TypeCache::Get().m_DoubleType; }
    const Type& Type::BoolType() { return TypeCache::Get().m_BoolType; }
    const Type& Type::CharType() { return TypeCache::Get().m_CharType; }
    const Type& Type::StringType() { return TypeCache::Get().m_StringType; }
    const Type& Type::ObjectType() { return TypeCache::Get().m_ObjectType; }
    const Type& Type::IntPtrType() { return TypeCache::Get().m_IntPtrType; }
    const Type& Type::UIntPtrType() { return TypeCache::Get().m_UIntPtrType; }
    const Type& Type::DecimalType() { return TypeCache::Get().m_DecimalType; }
    const Type& Type::DateTimeType() { return TypeCache::Get().m_DateTimeType; }
    const Type& Type::ExceptionType() { return TypeCache::Get().m_ExceptionType; }
    const Type& Type::ArrayType() { return TypeCache::Get().m_ArrayType; }

    StdString Type::GetFullName() const
    {
        NativeString str = s_ManagedFunctions.GetFullTypeNameFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    StdString Type::GetName() const
    {
        NativeString str = s_ManagedFunctions.GetTypeNameFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    StdString Type::GetNamespace() const
    {
        NativeString str = s_ManagedFunctions.GetTypeNamespaceFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    StdString Type::GetAssemblyQualifiedName() const
    {
        NativeString str = s_ManagedFunctions.GetAssemblyQualifiedNameFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    Type Type::GetBaseType() const
    {
        TypeId baseTypeId = -1;
        s_ManagedFunctions.GetBaseTypeFptr(m_Id, &baseTypeId);
        if (baseTypeId <= 0) return s_NullType;
        Type result;
        result.m_Id = baseTypeId;
        return result;
    }

    StdVector<Type> Type::GetInterfaceTypes() const
    {
        StdVector<Type> interfaceTypes;
        int32_t count = -1;
        s_ManagedFunctions.GetInterfaceTypeCountFptr(m_Id, &count);
        if (count > 0)
        {
            StdVector<TypeId> typeIds(static_cast<size_t>(count), TypeId(-1));
            s_ManagedFunctions.GetInterfaceTypesFptr(m_Id, typeIds.data());
            interfaceTypes.clear();
            interfaceTypes.reserve(static_cast<size_t>(count));

            for (TypeId id : typeIds)
            {
                Type t;
                t.m_Id = id;
                interfaceTypes.emplace_back(t);
            }
        }
        return interfaceTypes;
    }

    int32_t Type::GetSize() const
    {
        return s_ManagedFunctions.GetTypeSizeFptr(m_Id);
    }

    bool Type::IsSubclassOf(const Type& InOther) const
    {
        return s_ManagedFunctions.IsTypeSubclassOfFptr(m_Id, InOther.m_Id);
    }

    bool Type::IsAssignableTo(const Type& InOther) const
    {
        return s_ManagedFunctions.IsTypeAssignableToFptr(m_Id, InOther.m_Id);
    }

    bool Type::IsAssignableFrom(const Type& InOther) const
    {
        return s_ManagedFunctions.IsTypeAssignableFromFptr(m_Id, InOther.m_Id);
    }

    StdVector<MethodInfo> Type::GetMethods() const
    {
        int32_t methodCount = 0;
        s_ManagedFunctions.GetTypeMethodsFptr(m_Id, nullptr, &methodCount);
        StdVector<ManagedHandle> handles(static_cast<size_t>(methodCount));
        s_ManagedFunctions.GetTypeMethodsFptr(m_Id, handles.data(), &methodCount);

        StdVector<MethodInfo> methods(handles.size());
        for (size_t i = 0; i < handles.size(); i++)
            methods[i].m_Handle = handles[i];

        return methods;
    }

    StdVector<FieldInfo> Type::GetFields() const
    {
        int32_t fieldCount = 0;
        s_ManagedFunctions.GetTypeFieldsFptr(m_Id, nullptr, &fieldCount);
        StdVector<ManagedHandle> handles(static_cast<size_t>(fieldCount));
        s_ManagedFunctions.GetTypeFieldsFptr(m_Id, handles.data(), &fieldCount);

        StdVector<FieldInfo> fields(handles.size());
        for (size_t i = 0; i < handles.size(); i++)
            fields[i].m_Handle = handles[i];

        return fields;
    }

    StdVector<PropertyInfo> Type::GetProperties() const
    {
        int32_t propertyCount = 0;
        s_ManagedFunctions.GetTypePropertiesFptr(m_Id, nullptr, &propertyCount);
        StdVector<ManagedHandle> handles(static_cast<size_t>(propertyCount));
        s_ManagedFunctions.GetTypePropertiesFptr(m_Id, handles.data(), &propertyCount);

        StdVector<PropertyInfo> properties(handles.size());
        for (size_t i = 0; i < handles.size(); i++)
            properties[i].m_Handle = handles[i];

        return properties;
    }

    MethodInfo Type::GetMethod(StdStringView MethodName, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedNativeString string = ScopedNativeString(MethodName);
        MethodInfo method {};
        method.m_Handle = s_ManagedFunctions.GetMethodInfoByNameFptr(m_Id, string, flags);
        return method;
    }
    MethodInfo Type::GetMethod(StdStringView MethodName, int32_t InParamCount, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedNativeString string = ScopedNativeString(MethodName);
        MethodInfo method {};
        method.m_Handle = s_ManagedFunctions.GetMethodInfoByNameParamCountFptr(m_Id, string, InParamCount, flags);
        return method;
    }
    MethodInfo Type::GetMethodByParamTypes(StdStringView MethodName, const StdVector<Type>& InParamTypes, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedNativeString string = ScopedNativeString(MethodName);
        MethodInfo method {};
        StdVector<TypeId> typeIds = {};
        typeIds.reserve(InParamTypes.size());
        for (const Type& type : InParamTypes)
        {
            typeIds.push_back(type.GetTypeId());
        }
        method.m_Handle = s_ManagedFunctions.GetMethodInfoByNameParamTypesFptr(m_Id, string, static_cast<int32_t>(typeIds.size()), typeIds.data(), flags);
        return method;
    }

    FieldInfo Type::GetField(StdStringView FieldName, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedNativeString string = ScopedNativeString(FieldName);
        FieldInfo field {};
        field.m_Handle = s_ManagedFunctions.GetFieldInfoByNameFptr(m_Id, string, flags);
        return field;
    }

    PropertyInfo Type::GetProperty(StdStringView PropertyName, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedNativeString string = ScopedNativeString(PropertyName);
        PropertyInfo property {};
        property.m_Handle = s_ManagedFunctions.GetPropertyInfoByNameFptr(m_Id, string, flags);
        return property;
    }

    bool Type::HasAttribute(const Type& InAttributeType) const
    {
        return s_ManagedFunctions.HasTypeAttributeFptr(m_Id, InAttributeType.m_Id);
    }

    StdVector<Attribute> Type::GetAttributes() const
    {
        int32_t attributeCount;
        s_ManagedFunctions.GetTypeAttributesFptr(m_Id, nullptr, &attributeCount);
        StdVector<ManagedHandle> attributeHandles(static_cast<size_t>(attributeCount));
        s_ManagedFunctions.GetTypeAttributesFptr(m_Id, attributeHandles.data(), &attributeCount);

        StdVector<Attribute> result(attributeHandles.size());
        for (size_t i = 0; i < attributeHandles.size(); i++)
            result[i].m_Handle = attributeHandles[i];

        return result;
    }

    Attribute Type::GetAttribute(const Type& InAttributeType) const
    {
        auto list = GetAttributes();
        for (Attribute& attr : list)
        {
            if (attr.GetType() == InAttributeType) return attr;
        }
        return {};
    }

    ManagedType Type::GetManagedType() const
    {
        return s_ManagedFunctions.GetTypeManagedTypeFptr(m_Id);
    }

    Type Type::GetGenericArgument(int32_t InArgIndex) const
    {
        Type ret;
        ret.m_Id = s_ManagedFunctions.GetTypeGenericArgumentFptr(m_Id, InArgIndex);
        return ret;
    }

    Type Type::GetGenericTypeDefinition() const
    {
        Type ret;
        ret.m_Id = s_ManagedFunctions.GetTypeGenericTypeDefinitionFptr(m_Id);
        return ret;
    }

    bool Type::IsSZArray() const
    {
        return s_ManagedFunctions.IsTypeSZArrayFptr(m_Id);
    }

    bool Type::IsArray() const
    {
        return s_ManagedFunctions.IsTypeArrayFptr(m_Id);
    }

    bool Type::IsClass() const
    {
        return s_ManagedFunctions.IsTypeClassFptr(m_Id);
    }

    bool Type::IsInterface() const
    {
        return s_ManagedFunctions.IsTypeInterfaceFptr(m_Id);
    }

    bool Type::IsAbstract() const
    {
        return s_ManagedFunctions.IsTypeAbstractFptr(m_Id);
    }

    bool Type::IsSealed() const
    {
        return s_ManagedFunctions.IsTypeSealedFptr(m_Id);
    }

    bool Type::IsValueType() const
    {
        return s_ManagedFunctions.IsTypeValueTypeFptr(m_Id);
    }

    Type Type::GetElementType() const
    {
        TypeId elementTypeId = -1;
        s_ManagedFunctions.GetElementTypeFptr(m_Id, &elementTypeId);
        if (elementTypeId <= 0) return s_NullType;
        Type result;
        result.m_Id = elementTypeId;
        return result;
    }

    bool Type::operator==(const Type& InOther) const
    {
        return m_Id == InOther.m_Id;
    }

    Type::operator ReflectionType() const
    {
        ReflectionType ret;
        ret.m_TypeID = m_Id;
        return ret;
    }

    Object Type::CreateInstanceRaw(const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException) const
    {
        void* exceptionResult = nullptr;
        Object result;
        result.m_Handle = s_ManagedFunctions.CreateObjectFptr(m_Id, false, InParameters, InParameterTypes, static_cast<int32_t>(InLength), OutException ? &exceptionResult : nullptr);
        //result.m_Type = this;
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
        return result;
    }

    void Type::InvokeStaticMethodRaw(const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException) const
    {
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeStaticMethodFptr(m_Id, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Type::InvokeStaticMethodRetRaw(const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage, Object* OutException) const
    {
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeStaticMethodRetFptr(m_Id, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), InResultStorage, InRetIsObject, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    ReflectionType::operator const Type&() const
    {
        if (m_TypeID <= 0) return s_NullType;
        Type type;
        type.m_Id = m_TypeID;
        return type;
    }

}
