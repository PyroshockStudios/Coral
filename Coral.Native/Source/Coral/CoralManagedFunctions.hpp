#pragma once

#include "Coral/Core.hpp"
#include "Coral/NativeString.hpp"

namespace Coral {

    struct UnmanagedArray;
    enum class AssemblyLoadStatus;
    class Object;
    enum class GCCollectionMode;
    enum class ManagedType : uint32_t;
    class ManagedField;

    using SetInternalCallsFn = void (*)(int32_t, void*, int32_t);
    using CreateAssemblyLoadContextFn = int32_t (*)(NativeString, NativeString);
    using UnloadAssemblyLoadContextFn = void (*)(int32_t);
    using LoadAssemblyFn = int32_t (*)(int32_t, NativeString);
    using LoadAssemblyFromMemoryFn = int32_t (*)(int32_t, const std::byte*, int64_t);
    using GetLastLoadStatusFn = AssemblyLoadStatus (*)();
    using GetAssemblyNameFn = NativeString (*)(int32_t, int32_t);
    using GetSystemAssemblyFn = int32_t (*)(int32_t);
    using GetMethodInfoFromTokenFn = ManagedHandle (*)(int32_t, int32_t, MetadataToken);
    using GetFieldInfoFromTokenFn = ManagedHandle (*)(int32_t, int32_t, MetadataToken);

#pragma region DotnetServices
    using RunMSBuildFn = void (*)(const void*, NativeString, Bool32, void*, void*, void*, Bool32*, Object*);
#pragma endregion DotnetServices

#pragma region TypeInterface

    using GetAssemblyTypesFn = void (*)(int32_t, int32_t, TypeId*, int32_t*);
    using GetTypeIdFn = void (*)(NativeString, TypeId*);
    using GetFullTypeNameFn = NativeString (*)(TypeId);
    using GetTypeNameFn = NativeString (*)(TypeId);
    using GetTypeNamespaceFn = NativeString (*)(TypeId);
    using GetAssemblyQualifiedNameFn = NativeString (*)(TypeId);
    using GetBaseTypeFn = void (*)(TypeId, TypeId*);
    using GetInterfaceTypeCountFn = void (*)(TypeId, int32_t*);
    using GetInterfaceTypesFn = void (*)(TypeId, TypeId*);
    using GetTypeSizeFn = int32_t (*)(TypeId);
    using IsTypeSubclassOfFn = Bool32 (*)(TypeId, TypeId);
    using IsTypeAssignableToFn = Bool32 (*)(TypeId, TypeId);
    using IsTypeAssignableFromFn = Bool32 (*)(TypeId, TypeId);
    using IsTypeSZArrayFn = Bool32 (*)(TypeId);
    using IsTypeArrayFn = Bool32 (*)(TypeId);
    using IsTypeClassFn = Bool32 (*)(TypeId);
    using IsTypeInterfaceFn = Bool32 (*)(TypeId);
    using IsTypeAbstractFn = Bool32 (*)(TypeId);
    using IsTypeSealedFn = Bool32 (*)(TypeId);
    using IsTypeValueTypeFn = Bool32 (*)(TypeId);

    using GetElementTypeFn = void (*)(TypeId, TypeId*);
    using GetTypeMethodsFn = void (*)(TypeId, ManagedHandle*, int32_t*);
    using GetTypeFieldsFn = void (*)(TypeId, ManagedHandle*, int32_t*);
    using GetTypePropertiesFn = void (*)(TypeId, ManagedHandle*, int32_t*);
    using HasTypeAttributeFn = Bool32 (*)(TypeId, TypeId);
    using GetTypeAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
    using GetTypeManagedTypeFn = ManagedType (*)(TypeId);
    using GetTypeGenericArgumentFn = TypeId (*)(TypeId, int32_t);
    using GetTypeGenericTypeDefinitionFn = TypeId (*)(TypeId);

#pragma endregion

#pragma region Method
    using GetMethodInfoByNameFn = ManagedHandle (*)(TypeId, NativeString, BindingFlags);
    using GetMethodInfoByNameParamCountFn = ManagedHandle (*)(TypeId, NativeString, int32_t, BindingFlags);
    using GetMethodInfoByNameParamTypesFn = ManagedHandle (*)(TypeId, NativeString, int32_t, const TypeId*, BindingFlags);
    using GetMethodInfoNameFn = NativeString (*)(ManagedHandle);
    using GetMethodInfoReturnTypeFn = void (*)(ManagedHandle, TypeId*);
    using GetMethodInfoParameterTypesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
    using GetMethodInfoAccessibilityFn = TypeAccessibility (*)(ManagedHandle);
    using GetMethodInfoIsStaticFn = Bool32 (*)(ManagedHandle);
    using GetMethodInfoAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
    using GetMethodInfoTokenFn = MetadataToken (*)(ManagedHandle);
#pragma endregion

#pragma region Field
    using GetFieldInfoByNameFn = ManagedHandle (*)(TypeId, NativeString, BindingFlags);
    using GetFieldInfoNameFn = NativeString (*)(ManagedHandle);
    using GetFieldInfoTypeFn = void (*)(ManagedHandle, TypeId*);
    using GetFieldInfoAccessibilityFn = TypeAccessibility (*)(ManagedHandle);
    using GetFieldInfoIsStaticFn = Bool32 (*)(ManagedHandle);
    using GetFieldInfoIsLiteralFn = Bool32 (*)(ManagedHandle);
    using GetFieldInfoAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
    using GetFieldInfoTokenFn = MetadataToken (*)(ManagedHandle);
#pragma endregion

#pragma region Property
    using GetPropertyInfoByNameFn = ManagedHandle (*)(TypeId, NativeString, BindingFlags);
    using GetPropertyInfoNameFn = NativeString (*)(ManagedHandle);
    using GetPropertyInfoTypeFn = void (*)(ManagedHandle, TypeId*);
    using GetPropertyInfoGetMethodFn = ManagedHandle (*)(ManagedHandle);
    using GetPropertyInfoSetMethodFn = ManagedHandle (*)(ManagedHandle);
    using GetPropertyInfoAttributesFn = void (*)(ManagedHandle, TypeId*, int32_t*);
#pragma endregion

#pragma region Attribute
    using GetAttributeFieldValueFn = void (*)(ManagedHandle, NativeString, void*);
    using GetAttributeTypeFn = void (*)(ManagedHandle, TypeId*);
#pragma endregion

    using CreateObjectFn = void* (*)(TypeId, Bool32, const void**, const ManagedType*, int32_t, void**);
    using CopyObjectFn = void* (*)(void*);
    using InvokeMethodFn = void (*)(void*, ManagedHandle, const void**, const ManagedType*, int32_t, void**);
    using InvokeMethodRetFn = void (*)(void*, ManagedHandle, const void**, const ManagedType*, int32_t, void*, Bool32, void**);
    using InvokeDelegateFn = void (*)(void*, const void**, const ManagedType*, int32_t, void**);
    using InvokeDelegateRetFn = void (*)(void*, const void**, const ManagedType*, int32_t, void*, Bool32, void**);
    using InvokeStaticMethodFn = void (*)(TypeId, ManagedHandle, const void**, const ManagedType*, int32_t, void**);
    using InvokeStaticMethodRetFn = void (*)(TypeId, ManagedHandle, const void**, const ManagedType*, int32_t, void*, Bool32, void**);
    using SetFieldValueFn = void (*)(void*, ManagedHandle, const void*, Bool32);
    using GetFieldValueFn = void (*)(void*, ManagedHandle, void*, Bool32);
    using SetPropertyValueFn = void (*)(void*, ManagedHandle, const void*, Bool32, void**);
    using GetPropertyValueFn = void (*)(void*, ManagedHandle, void*, Bool32, void**);
    using DestroyObjectFn = void (*)(void*);
    using GetObjectTypeIdFn = void (*)(void*, int32_t*);
    using GetObjectBoxedValueFn = void* (*)(const void*, int32_t, TypeId);
    using GetObjectUnboxedValueFn = void (*)(void*, void*);

    using CreateNewManagedStringFn = void* (*)(const char16_t*, int32_t);
    using GetStringContentsFn = int (*)(void*, char16_t*);
    using WriteStringContentsUtf16Fn = void (*)(void*, const char16_t*, int32_t);
    using CreateNewManagedArrayFn = void* (*)(int32_t, TypeId);
    using SetArrayElementFn = void (*)(void*, const void*, Bool32, int32_t);
    using GetArrayElementFn = void (*)(void*, void*, Bool32, int32_t);

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
        GetMethodInfoFromTokenFn GetMethodInfoFromTokenFptr = nullptr;
        GetFieldInfoFromTokenFn GetFieldInfoFromTokenFptr = nullptr;

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
        IsTypeArrayFn IsTypeArrayFptr = nullptr;
        IsTypeClassFn IsTypeClassFptr = nullptr;
        IsTypeInterfaceFn IsTypeInterfaceFptr = nullptr;
        IsTypeAbstractFn IsTypeAbstractFptr = nullptr;
        IsTypeSealedFn IsTypeSealedFptr = nullptr;
        IsTypeValueTypeFn IsTypeValueTypeFptr = nullptr;

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

#pragma region Method
        GetMethodInfoByNameFn GetMethodInfoByNameFptr = nullptr;
        GetMethodInfoByNameParamCountFn GetMethodInfoByNameParamCountFptr = nullptr;
        GetMethodInfoByNameParamTypesFn GetMethodInfoByNameParamTypesFptr = nullptr;
        GetMethodInfoNameFn GetMethodInfoNameFptr = nullptr;
        GetMethodInfoReturnTypeFn GetMethodInfoReturnTypeFptr = nullptr;
        GetMethodInfoParameterTypesFn GetMethodInfoParameterTypesFptr = nullptr;
        GetMethodInfoAccessibilityFn GetMethodInfoAccessibilityFptr = nullptr;
        GetMethodInfoIsStaticFn GetMethodInfoIsStaticFptr = nullptr;
        GetMethodInfoAttributesFn GetMethodInfoAttributesFptr = nullptr;
        GetMethodInfoTokenFn GetMethodInfoTokenFptr = nullptr;
#pragma endregion

#pragma region Field
        GetFieldInfoByNameFn GetFieldInfoByNameFptr = nullptr;
        GetFieldInfoNameFn GetFieldInfoNameFptr = nullptr;
        GetFieldInfoTypeFn GetFieldInfoTypeFptr = nullptr;
        GetFieldInfoAccessibilityFn GetFieldInfoAccessibilityFptr = nullptr;
        GetFieldInfoIsStaticFn GetFieldInfoIsStaticFptr = nullptr;
        GetFieldInfoIsLiteralFn GetFieldInfoIsLiteralFptr = nullptr;
        GetFieldInfoAttributesFn GetFieldInfoAttributesFptr = nullptr;
        GetFieldInfoTokenFn GetFieldInfoTokenFptr = nullptr;
#pragma endregion

#pragma region Property
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
        GetObjectBoxedValueFn GetObjectBoxedValueFptr = nullptr;
        GetObjectUnboxedValueFn GetObjectUnboxedValueFptr = nullptr;

        CreateNewManagedStringFn CreateNewManagedStringFptr = nullptr;

        GetStringContentsFn GetStringContentsFptr = nullptr;
        CreateNewManagedArrayFn CreateNewManagedArrayFptr = nullptr;
        SetArrayElementFn SetArrayElementFptr = nullptr;
        GetArrayElementFn GetArrayElementFptr = nullptr;

        CollectGarbageFn CollectGarbageFptr = nullptr;
        WaitForPendingFinalizersFn WaitForPendingFinalizersFptr = nullptr;
    };

    inline ManagedFunctions s_ManagedFunctions;

}
