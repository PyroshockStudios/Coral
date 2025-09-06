#include "Coral/Type.hpp"
#include "Coral/Attribute.hpp"
#include "Coral/StringHelper.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

namespace Coral {

    Type& Type::VoidType() { return *TypeCache::Get().m_VoidType; }
    Type& Type::ByteType() { return *TypeCache::Get().m_ByteType; }
    Type& Type::SByteType() { return *TypeCache::Get().m_SByteType; }
    Type& Type::ShortType() { return *TypeCache::Get().m_ShortType; }
    Type& Type::UShortType() { return *TypeCache::Get().m_UShortType; }
    Type& Type::IntType() { return *TypeCache::Get().m_IntType; }
    Type& Type::UIntType() { return *TypeCache::Get().m_UIntType; }
    Type& Type::LongType() { return *TypeCache::Get().m_LongType; }
    Type& Type::ULongType() { return *TypeCache::Get().m_ULongType; }
    Type& Type::FloatType() { return *TypeCache::Get().m_FloatType; }
    Type& Type::DoubleType() { return *TypeCache::Get().m_DoubleType; }
    Type& Type::BoolType() { return *TypeCache::Get().m_BoolType; }
    Type& Type::CharType() { return *TypeCache::Get().m_CharType; }
    Type& Type::StringType() { return *TypeCache::Get().m_StringType; }
    Type& Type::ObjectType() { return *TypeCache::Get().m_ObjectType; }
    Type& Type::IntPtrType() { return *TypeCache::Get().m_IntPtrType; }
    Type& Type::UIntPtrType() { return *TypeCache::Get().m_UIntPtrType; }
    Type& Type::DecimalType() { return *TypeCache::Get().m_DecimalType; }
    Type& Type::DateTimeType() { return *TypeCache::Get().m_DateTimeType; }
    Type& Type::ExceptionType() { return *TypeCache::Get().m_ExceptionType; }
    Type& Type::ArrayType() { return *TypeCache::Get().m_ArrayType; }

    std::string Type::GetFullName() const
    {
        String str = s_ManagedFunctions.GetFullTypeNameFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    std::string Type::GetName() const
    {
        String str = s_ManagedFunctions.GetTypeNameFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    std::string Type::GetNamespace() const
    {
        String str = s_ManagedFunctions.GetTypeNamespaceFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    std::string Type::GetAssemblyQualifiedName() const
    {
        String str = s_ManagedFunctions.GetAssemblyQualifiedNameFptr(m_Id);
        return StringHelper::ConsumeNativeString(str);
    }

    Type& Type::GetBaseType()
    {
        if (!m_BaseType)
        {
            Type baseType;
            s_ManagedFunctions.GetBaseTypeFptr(m_Id, &baseType.m_Id);
            m_BaseType = TypeCache::Get().CacheType(std::move(baseType));
        }

        return *m_BaseType;
    }

    std::vector<Type*>& Type::GetInterfaceTypes()
    {
        if (!m_InterfaceTypes)
        {
            int count;
            s_ManagedFunctions.GetInterfaceTypeCountFptr(m_Id, &count);

            std::vector<TypeId> typeIds;
            typeIds.resize(static_cast<size_t>(count));
            s_ManagedFunctions.GetInterfaceTypesFptr(m_Id, typeIds.data());

            m_InterfaceTypes = std::vector<Type*>();
            m_InterfaceTypes->reserve(static_cast<size_t>(count));

            for (auto id : typeIds)
            {
                Type type;
                type.m_Id = id;
                m_InterfaceTypes->emplace_back(TypeCache::Get().CacheType(std::move(type)));
            }
        }

        return *m_InterfaceTypes;
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

    std::vector<MethodInfo> Type::GetMethods() const
    {
        int32_t methodCount = 0;
        s_ManagedFunctions.GetTypeMethodsFptr(m_Id, nullptr, &methodCount);
        std::vector<ManagedHandle> handles(static_cast<size_t>(methodCount));
        s_ManagedFunctions.GetTypeMethodsFptr(m_Id, handles.data(), &methodCount);

        std::vector<MethodInfo> methods(handles.size());
        for (size_t i = 0; i < handles.size(); i++)
            methods[i].m_Handle = handles[i];

        return methods;
    }

    std::vector<FieldInfo> Type::GetFields() const
    {
        int32_t fieldCount = 0;
        s_ManagedFunctions.GetTypeFieldsFptr(m_Id, nullptr, &fieldCount);
        std::vector<ManagedHandle> handles(static_cast<size_t>(fieldCount));
        s_ManagedFunctions.GetTypeFieldsFptr(m_Id, handles.data(), &fieldCount);

        std::vector<FieldInfo> fields(handles.size());
        for (size_t i = 0; i < handles.size(); i++)
            fields[i].m_Handle = handles[i];

        return fields;
    }

    std::vector<PropertyInfo> Type::GetProperties() const
    {
        int32_t propertyCount = 0;
        s_ManagedFunctions.GetTypePropertiesFptr(m_Id, nullptr, &propertyCount);
        std::vector<ManagedHandle> handles(static_cast<size_t>(propertyCount));
        s_ManagedFunctions.GetTypePropertiesFptr(m_Id, handles.data(), &propertyCount);

        std::vector<PropertyInfo> properties(handles.size());
        for (size_t i = 0; i < handles.size(); i++)
            properties[i].m_Handle = handles[i];

        return properties;
    }

    MethodInfo Type::GetMethod(std::string_view MethodName, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedString string = ScopedString(MethodName);
        MethodInfo method{};
        method.m_Handle = s_ManagedFunctions.GetMethodInfoByNameFptr(m_Id, string, flags);
        return method;
    }
    MethodInfo Type::GetMethod(std::string_view MethodName, int32_t InParamCount, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedString string = ScopedString(MethodName);
        MethodInfo method{};
        method.m_Handle = s_ManagedFunctions.GetMethodInfoByNameParamCountFptr(m_Id, string, InParamCount, flags);
        return method;
    }
    MethodInfo Type::GetMethodByParamTypes(std::string_view MethodName, const std::vector<Type*>& InParamTypes, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedString string = ScopedString(MethodName);
        MethodInfo method{};
        std::vector<TypeId> typeIds = {};
        typeIds.reserve(InParamTypes.size());
        for (const Type* type : InParamTypes)
        {
            typeIds.push_back(type->GetTypeId());
        }
        method.m_Handle = s_ManagedFunctions.GetMethodInfoByNameParamTypesFptr(m_Id, string, static_cast<int32_t>(typeIds.size()), typeIds.data(), flags);
        return method;
    }

    FieldInfo Type::GetField(std::string_view FieldName, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedString string = ScopedString(FieldName);
        FieldInfo field{};
        field.m_Handle = s_ManagedFunctions.GetFieldInfoByNameFptr(m_Id, string, flags);
        return field;
    }

    PropertyInfo Type::GetProperty(std::string_view PropertyName, bool InStatic) const
    {
        BindingFlags flags = BindingFlags::Public | BindingFlags::NonPublic;
        flags |= InStatic ? BindingFlags::Static : BindingFlags::Instance;
        ScopedString string = ScopedString(PropertyName);
        PropertyInfo property{};
        property.m_Handle = s_ManagedFunctions.GetPropertyInfoByNameFptr(m_Id, string, flags);
        return property;
    }

    bool Type::HasAttribute(const Type& InAttributeType) const
    {
        return s_ManagedFunctions.HasTypeAttributeFptr(m_Id, InAttributeType.m_Id);
    }

    std::vector<Attribute> Type::GetAttributes() const
    {
        int32_t attributeCount;
        s_ManagedFunctions.GetTypeAttributesFptr(m_Id, nullptr, &attributeCount);
        std::vector<ManagedHandle> attributeHandles(static_cast<size_t>(attributeCount));
        s_ManagedFunctions.GetTypeAttributesFptr(m_Id, attributeHandles.data(), &attributeCount);

        std::vector<Attribute> result(attributeHandles.size());
        for (size_t i = 0; i < attributeHandles.size(); i++)
            result[i].m_Handle = attributeHandles[i];

        return result;
    }

    Attribute Type::GetAttribute(const Type& InAttributeType) const
    {
        auto list = GetAttributes();
        for (Attribute& attr : list) {
            if (attr.GetType() == InAttributeType) return attr;
        }
        return {};
    }

    ManagedType Type::GetManagedType() const
    {
        return s_ManagedFunctions.GetTypeManagedTypeFptr(m_Id);
    }

    Type& Type::GetGenericArgument(int32_t InArgIndex) const
    {
        TypeId id = s_ManagedFunctions.GetTypeGenericArgumentFptr(m_Id, InArgIndex);
        return *TypeCache::Get().GetTypeByID(id);
    }

    Type& Type::GetGenericTypeDefinition() const
    {
        TypeId id = s_ManagedFunctions.GetTypeGenericTypeDefinitionFptr(m_Id);
        return *TypeCache::Get().GetTypeByID(id);
    }

    bool Type::IsSZArray() const
    {
        return s_ManagedFunctions.IsTypeSZArrayFptr(m_Id);
    }

    Type& Type::GetElementType()
    {
        if (!m_ElementType)
        {
            Type elementType;
            s_ManagedFunctions.GetElementTypeFptr(m_Id, &elementType.m_Id);
            m_ElementType = TypeCache::Get().CacheType(std::move(elementType));
        }

        return *m_ElementType;
    }

    bool Type::operator==(const Type& InOther) const
    {
        return m_Id == InOther.m_Id;
    }

    Object Type::CreateInstanceInternal(Object* OutException, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const
    {
        void* exceptionResult = nullptr;
        Object result;
        result.m_Handle = s_ManagedFunctions.CreateObjectFptr(m_Id, false, InParameters, InParameterTypes, static_cast<int32_t>(InLength), OutException ? &exceptionResult : nullptr);
        result.m_Type = this;
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
        return result;
    }

    void Type::InvokeStaticMethodInternal(Object* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const
    {
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeStaticMethodFptr(m_Id, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Type::InvokeStaticMethodRetInternal(Object* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage) const
    {
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeStaticMethodRetFptr(m_Id, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), InResultStorage, InRetIsObject, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    ReflectionType::operator Type& () const
    {
        static Type s_NullType;

        auto* result = TypeCache::Get().GetTypeByID(m_TypeID);

        if (result == nullptr)
        {
            Type type;
            type.m_Id = m_TypeID;
            result = TypeCache::Get().CacheType(std::move(type));
        }

        return result != nullptr ? *result : s_NullType;
    }

}
