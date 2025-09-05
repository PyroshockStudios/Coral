#pragma once

#include "Coral/Core.hpp"
#include "Coral/String.hpp"

namespace Coral {

    struct UnmanagedArray;
    enum class AssemblyLoadStatus;
    class Object;
    enum class GCCollectionMode;
    enum class ManagedType : uint32_t;
    class ManagedField;

    using SetInternalCallsFn = void (*)(int32_t, void*, int32_t);
    using CreateAssemblyLoadContextFn = int32_t(*)(String, String);
    using UnloadAssemblyLoadContextFn = void (*)(int32_t);
    using LoadAssemblyFn = int32_t(*)(int32_t, String);
    using LoadAssemblyFromMemoryFn = int32_t(*)(int32_t, const std::byte*, int64_t);
    using GetLastLoadStatusFn = AssemblyLoadStatus(*)();
    using GetAssemblyNameFn = String(*)(int32_t, int32_t);
    using GetSystemAssemblyFn = int32_t(*)(int32_t);

#pragma region DotnetServices
    using RunMSBuildFn = void (*)(String, Bool32, Bool32*);
#pragma endregion DotnetServices

#pragma region TypeInterface

    using GetAssemblyTypesFn = void (*)(int32_t, int32_t, TypeId*, int32_t*);
    using GetTypeIdFn = void (*)(String, TypeId*);
    using GetFullTypeNameFn = String(*)(TypeId);
    using GetTypeNameFn = String(*)(TypeId);
    using GetTypeNamespaceFn = String(*)(TypeId);
    using GetAssemblyQualifiedNameFn = String(*)(TypeId);
    using GetBaseTypeFn = void (*)(TypeId, TypeId*);
    using GetInterfaceTypeCountFn = void (*)(TypeId, int32_t*);
    using GetInterfaceTypesFn = void (*)(TypeId, TypeId*);
    using GetTypeSizeFn = int32_t(*)(TypeId);
    using IsTypeSubclassOfFn = Bool32(*)(TypeId, TypeId);
    using IsTypeAssignableToFn = Bool32(*)(TypeId, TypeId);
    using IsTypeAssignableFromFn = Bool32(*)(TypeId, TypeId);
    using IsTypeSZArrayFn = Bool32(*)(TypeId);
    using GetElementTypeFn = void (*)(TypeId, TypeId*);
    using GetTypeMethodsFn = void (*)(TypeId, ManagedHandle*, int32_t*);
    using GetTypeFieldsFn = void (*)(TypeId, ManagedHandle*, int32_t*);
    using GetTypePropertiesFn = void (*)(TypeId, ManagedHandle*, int32_t*);
    using HasTypeAttributeFn = Bool32(*)(TypeId, TypeId);
    using GetTypeAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
    using GetTypeManagedTypeFn = ManagedType(*)(TypeId);
    using GetTypeGenericArgumentFn = TypeId(*)(TypeId, int32_t);
    using GetTypeGenericTypeDefinitionFn = TypeId(*)(TypeId);

#pragma endregion

#pragma region MethodInfo
    using GetMethodInfoByNameFn = ManagedHandle(*)(TypeId, String, BindingFlags);
    using GetMethodInfoByNameParamCountFn = ManagedHandle(*)(TypeId, String, int32_t, BindingFlags);
    using GetMethodInfoByNameParamTypesFn = ManagedHandle(*)(TypeId, String, int32_t, const TypeId*, BindingFlags);
    using GetMethodInfoNameFn = String(*)(ManagedHandle);
    using GetMethodInfoReturnTypeFn = void (*)(ManagedHandle, TypeId*);
    using GetMethodInfoParameterTypesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
    using GetMethodInfoAccessibilityFn = TypeAccessibility(*)(ManagedHandle);
    using GetMethodInfoIsStaticFn = Bool32(*)(ManagedHandle);
    using GetMethodInfoAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
#pragma endregion

#pragma region FieldInfo
    using GetFieldInfoByNameFn = ManagedHandle(*)(TypeId, String, BindingFlags);
    using GetFieldInfoNameFn = String(*)(ManagedHandle);
    using GetFieldInfoTypeFn = void (*)(ManagedHandle, TypeId*);
    using GetFieldInfoAccessibilityFn = TypeAccessibility(*)(ManagedHandle);
    using GetFieldInfoIsStaticFn = Bool32(*)(ManagedHandle);
    using GetFieldInfoIsLiteralFn = Bool32(*)(ManagedHandle);
    using GetFieldInfoAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
#pragma endregion

#pragma region PropertyInfo
    using GetPropertyInfoByNameFn = ManagedHandle(*)(TypeId, String, BindingFlags);
    using GetPropertyInfoNameFn = String(*)(ManagedHandle);
    using GetPropertyInfoTypeFn = void (*)(ManagedHandle, TypeId*);
    using GetPropertyInfoGetMethodFn = ManagedHandle(*)(ManagedHandle);
    using GetPropertyInfoSetMethodFn = ManagedHandle(*)(ManagedHandle);
    using GetPropertyInfoAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
#pragma endregion

#pragma region Attribute
    using GetAttributeFieldValueFn = void (*)(ManagedHandle, String, void*);
    using GetAttributeTypeFn = void (*)(ManagedHandle, TypeId*);
#pragma endregion

    using CreateObjectFn = void* (*)(TypeId, Bool32, const void**, const ManagedType*, int32_t, void**);
    using CopyObjectFn = void* (*)(void*);
    using InvokeMethodFn = void (*)(void*, ManagedHandle, const void**, const ManagedType*, int32_t, void**);
    using InvokeMethodRetFn = void (*)(void*, ManagedHandle, const void**, const ManagedType*, int32_t, void*, void**);
    using InvokeDelegateFn = void (*)(void*, const void**, const ManagedType*, int32_t, void**);
    using InvokeDelegateRetFn = void (*)(void*, const void**, const ManagedType*, int32_t, void*, void**);
    using InvokeStaticMethodFn = void (*)(TypeId, ManagedHandle, const void**, const ManagedType*, int32_t, void**);
    using InvokeStaticMethodRetFn = void (*)(TypeId, ManagedHandle, const void**, const ManagedType*, int32_t, void*, void**);
    using SetFieldValueFn = void (*)(void*, ManagedHandle, void*);
    using GetFieldValueFn = void (*)(void*, ManagedHandle, void*);
    using SetPropertyValueFn = void (*)(void*, ManagedHandle, void*, void**);
    using GetPropertyValueFn = void (*)(void*, ManagedHandle, void*, void**);
    using DestroyObjectFn = void (*)(void*);
    using GetObjectTypeIdFn = void (*)(void*, int32_t*);

    using CollectGarbageFn = void (*)(int32_t, GCCollectionMode, Bool32, Bool32);
    using WaitForPendingFinalizersFn = void (*)();

    struct ManagedFunctions
    {
        SetInternalCallsFn SetInternalCallsFptr = nullptr;
        LoadAssemblyFn LoadAssemblyFptr = nullptr;
        LoadAssemblyFromMemoryFn LoadAssemblyFromMemoryFptr = nullptr;
        UnloadAssemblyLoadContextFn UnloadAssemblyLoadContextFptr = nullptr;
        GetLastLoadStatusFn GetLastLoadStatusFptr = nullptr;
        GetAssemblyNameFn GetAssemblyNameFptr = nullptr;
        GetSystemAssemblyFn GetSystemAssemblyFptr = nullptr;

#pragma region DotnetServices
        RunMSBuildFn RunMSBuildFptr = nullptr;
#pragma endregion DotnetServices

#pragma region TypeInterface

        GetAssemblyTypesFn GetAssemblyTypesFptr = nullptr;
        GetFullTypeNameFn GetFullTypeNameFptr = nullptr;
        GetTypeNameFn GetTypeNameFptr = nullptr;
        GetTypeNamespaceFn GetTypeNamespaceFptr = nullptr;
        GetAssemblyQualifiedNameFn GetAssemblyQualifiedNameFptr = nullptr;
        GetBaseTypeFn GetBaseTypeFptr = nullptr;
        GetInterfaceTypeCountFn GetInterfaceTypeCountFptr = nullptr;
        GetInterfaceTypesFn GetInterfaceTypesFptr = nullptr;
        GetTypeSizeFn GetTypeSizeFptr = nullptr;
        IsTypeSubclassOfFn IsTypeSubclassOfFptr = nullptr;
        IsTypeAssignableToFn IsTypeAssignableToFptr = nullptr;
        IsTypeAssignableFromFn IsTypeAssignableFromFptr = nullptr;
        IsTypeSZArrayFn IsTypeSZArrayFptr = nullptr;
        GetElementTypeFn GetElementTypeFptr = nullptr;
        GetTypeMethodsFn GetTypeMethodsFptr = nullptr;
        GetTypeFieldsFn GetTypeFieldsFptr = nullptr;
        GetTypePropertiesFn GetTypePropertiesFptr = nullptr;
        HasTypeAttributeFn HasTypeAttributeFptr = nullptr;
        GetTypeAttributesFn GetTypeAttributesFptr = nullptr;
        GetTypeManagedTypeFn GetTypeManagedTypeFptr = nullptr;
        GetTypeGenericArgumentFn GetTypeGenericArgumentFptr = nullptr;
        GetTypeGenericTypeDefinitionFn GetTypeGenericTypeDefinitionFptr = nullptr;

#pragma endregion

#pragma region MethodInfo
        GetMethodInfoByNameFn GetMethodInfoByNameFptr = nullptr;
        GetMethodInfoByNameParamCountFn GetMethodInfoByNameParamCountFptr = nullptr;
        GetMethodInfoByNameParamTypesFn GetMethodInfoByNameParamTypesFptr = nullptr;
        GetMethodInfoNameFn GetMethodInfoNameFptr = nullptr;
        GetMethodInfoReturnTypeFn GetMethodInfoReturnTypeFptr = nullptr;
        GetMethodInfoParameterTypesFn GetMethodInfoParameterTypesFptr = nullptr;
        GetMethodInfoAccessibilityFn GetMethodInfoAccessibilityFptr = nullptr;
        GetMethodInfoIsStaticFn GetMethodInfoIsStaticFptr = nullptr;
        GetMethodInfoAttributesFn GetMethodInfoAttributesFptr = nullptr;
#pragma endregion

#pragma region FieldInfo
        GetFieldInfoByNameFn GetFieldInfoByNameFptr = nullptr;
        GetFieldInfoNameFn GetFieldInfoNameFptr = nullptr;
        GetFieldInfoTypeFn GetFieldInfoTypeFptr = nullptr;
        GetFieldInfoAccessibilityFn GetFieldInfoAccessibilityFptr = nullptr;
        GetFieldInfoIsStaticFn GetFieldInfoIsStaticFptr = nullptr;
        GetFieldInfoIsLiteralFn GetFieldInfoIsLiteralFptr = nullptr;
        GetFieldInfoAttributesFn GetFieldInfoAttributesFptr = nullptr;
#pragma endregion

#pragma region PropertyInfo
        GetPropertyInfoByNameFn GetPropertyInfoByNameFptr = nullptr;
        GetPropertyInfoNameFn GetPropertyInfoNameFptr = nullptr;
        GetPropertyInfoTypeFn GetPropertyInfoTypeFptr = nullptr;
        GetPropertyInfoGetMethodFn GetPropertyInfoGetMethodFptr = nullptr;
        GetPropertyInfoSetMethodFn GetPropertyInfoSetMethodFptr = nullptr;
        GetPropertyInfoAttributesFn GetPropertyInfoAttributesFptr = nullptr;
#pragma endregion

#pragma region Attribute
        GetAttributeFieldValueFn GetAttributeFieldValueFptr = nullptr;
        GetAttributeTypeFn GetAttributeTypeFptr = nullptr;
#pragma endregion

        CreateObjectFn CreateObjectFptr = nullptr;
        CopyObjectFn CopyObjectFptr = nullptr;
        CreateAssemblyLoadContextFn CreateAssemblyLoadContextFptr = nullptr;
        InvokeMethodFn InvokeMethodFptr = nullptr;
        InvokeMethodRetFn InvokeMethodRetFptr = nullptr;
        InvokeDelegateFn InvokeDelegateFptr = nullptr;
        InvokeDelegateRetFn InvokeDelegateRetFptr = nullptr;
        InvokeStaticMethodFn InvokeStaticMethodFptr = nullptr;
        InvokeStaticMethodRetFn InvokeStaticMethodRetFptr = nullptr;
        SetFieldValueFn SetFieldValueFptr = nullptr;
        GetFieldValueFn GetFieldValueFptr = nullptr;
        SetPropertyValueFn SetPropertyValueFptr = nullptr;
        GetPropertyValueFn GetPropertyValueFptr = nullptr;
        DestroyObjectFn DestroyObjectFptr = nullptr;
        GetObjectTypeIdFn GetObjectTypeIdFptr = nullptr;

        CollectGarbageFn CollectGarbageFptr = nullptr;
        WaitForPendingFinalizersFn WaitForPendingFinalizersFptr = nullptr;
    };

    inline ManagedFunctions s_ManagedFunctions;

}
