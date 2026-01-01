#pragma once

#ifndef CORAL_NATIVE_COMPILE_SRC
// already included by the PCH
#include "Config.hpp"
#endif

#include <cstdint>
#include <cwchar>

#define CORAL_DEPRECATE_MSG_P(s, x) s ". See `" x "`"

#define CORAL_GLOBAL_ALC_MSG "Global type cache has been superseded by Assembly/ALC-local type APIs"
#define CORAL_GLOBAL_ALC_MSG_P(x) CORAL_DEPRECATE_MSG_P(CORAL_GLOBAL_ALC_MSG, #x)

#define CORAL_LEAK_UC_TYPES_MSG "Global namespace string type abstraction will be removed"
#define CORAL_LEAK_UC_TYPES_MSG_P(x) CORAL_DEPRECATE_MSG_P(CORAL_LEAK_UC_TYPES_MSG, #x)

#ifdef _WIN32
    #define CORAL_WINDOWS
#elif defined(__APPLE__)
    #define CORAL_APPLE
#endif

#ifdef CORAL_WINDOWS
    #define CORAL_CALLTYPE __cdecl
    #define CORAL_HOSTFXR_NAME "hostfxr.dll"

    // TODO(Emily): On Windows shouldn't this use the `UNICODE` macro?
    #ifdef _WCHAR_T_DEFINED
        #define CORAL_WIDE_CHARS
    #endif
#else
    #define CORAL_CALLTYPE

    #ifdef CORAL_APPLE
        #define CORAL_HOSTFXR_NAME "libhostfxr.dylib"
    #else
        #define CORAL_HOSTFXR_NAME "libhostfxr.so"
    #endif
#endif

#ifdef CORAL_WIDE_CHARS
    #define CORAL_STR(s) L##s

namespace Coral {
    using UCChar = wchar_t;
    using UCStringView = StdWStringView;
    using UCString = StdWString;
}

#else
    #define CORAL_STR(s) s

namespace Coral {
    using UCChar = char;
    using UCStringView = StdStringView;
    using UCString = StdString;
}
#endif

#define CORAL_UNMANAGED_CALLERS_ONLY ((const UCChar*)(~0ULL))

namespace Coral {
    using Bool32 = uint32_t;
    using Char = char16_t; // C# strings are UTF16
    static_assert(sizeof(Bool32) == 4);
    static_assert(sizeof(Char) == 2);

    enum class TypeAccessibility
    {
        Public,
        Private,
        Protected,
        Internal,
        ProtectedPublic,
        PrivateProtected
    };

    enum class BindingFlags : uint32_t
    {
        Default = 0,
        IgnoreCase = 1 << 0,
        DeclaredOnly = 1 << 1,
        Instance = 1 << 2,
        Static = 1 << 3,
        Public = 1 << 4,
        NonPublic = 1 << 5,
        FlattenHierarchy = 1 << 6,
        InvokeMethod = 1 << 8,
        CreateInstance = 1 << 9,
        GetField = 1 << 10,
        SetField = 1 << 11,
        GetProperty = 1 << 12,
        SetProperty = 1 << 13,
        PutDispProperty = 1 << 14,
        PutRefDispProperty = 1 << 15,
        ExactBinding = 1 << 16,
        SuppressChangeType = 1 << 17,
        OptionalParamBinding = 1 << 18,
        IgnoreReturn = 1 << 24,
        DoNotWrapExceptions = 1 << 25
    };

    constexpr inline BindingFlags operator|(BindingFlags a, BindingFlags b)
    {
        return static_cast<BindingFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr inline BindingFlags operator&(BindingFlags a, BindingFlags b)
    {
        return static_cast<BindingFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr inline BindingFlags operator^(BindingFlags a, BindingFlags b)
    {
        return static_cast<BindingFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    constexpr inline BindingFlags operator~(BindingFlags a)
    {
        return static_cast<BindingFlags>(~static_cast<uint32_t>(a));
    }
    constexpr inline BindingFlags& operator|=(BindingFlags& a, BindingFlags b)
    {
        a = a | b;
        return a;
    }

    constexpr inline BindingFlags& operator&=(BindingFlags& a, BindingFlags b)
    {
        a = a & b;
        return a;
    }

    constexpr inline BindingFlags& operator^=(BindingFlags& a, BindingFlags b)
    {
        a = a ^ b;
        return a;
    }

    using TypeId = int32_t;
    using ManagedHandle = int32_t;
    using MetadataToken = int32_t;

    struct InternalCall
    {
        // TODO(Emily): Review all `UCChar*` refs to see if they could be `UCStringView`.
        const UCChar* Name;
        void* NativeFunctionPtr;
    };

}
