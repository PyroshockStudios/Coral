#pragma once

#include "Core.hpp"

namespace Coral {
    namespace Internal {
        struct NativeString_LayoutTest;
    }
    class NativeString
    {
    public:
        static NativeString New(const char* InString);
        static NativeString New(StdStringView InString);
        static void Free(NativeString& InString);

        void Assign(StdStringView InString);

        operator StdString() const;

        bool operator==(const NativeString& InOther) const;
        bool operator==(StdStringView InOther) const;

        UCChar* Data() { return m_String; }
        const UCChar* Data() const { return m_String; }
    private:
        UCChar* m_String = nullptr;
        [[maybe_unused]] Bool32 m_IsDisposed = false; // NOTE(Peter): Required for the layout to match the C# NativeString struct, unused in C++
        friend struct Internal::NativeString_LayoutTest;
    };
    namespace Internal {
        struct NativeString_LayoutTest
        {
            static_assert(offsetof(NativeString, m_String) == 0);
            static_assert(offsetof(NativeString, m_IsDisposed) == 8);
            static_assert(sizeof(NativeString) == 16);
        };
    }

    struct ScopedNativeString
    {
        ScopedNativeString(NativeString InString)
            : m_String(InString) {}
        ScopedNativeString(const char* InString)
            : m_String(NativeString::New(InString)) {}
        ScopedNativeString(StdStringView InString)
            : m_String(NativeString::New(InString)) {}

        ScopedNativeString& operator=(NativeString InOther)
        {
            NativeString::Free(m_String);
            m_String = InOther;
            return *this;
        }

        ScopedNativeString& operator=(const ScopedNativeString& InOther)
        {
            NativeString::Free(m_String);
            m_String = InOther.m_String;
            return *this;
        }

        ~ScopedNativeString()
        {
            NativeString::Free(m_String);
        }

        operator StdString() const { return m_String; }
        operator NativeString() const { return m_String; }

        bool operator==(const ScopedNativeString& InOther) const
        {
            return m_String == InOther.m_String;
        }

        bool operator==(StdStringView InOther) const
        {
            return m_String == InOther;
        }

    private:
        NativeString m_String;
    };

}
