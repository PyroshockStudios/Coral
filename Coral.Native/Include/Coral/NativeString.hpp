#pragma once

#include "Core.hpp"

namespace Coral {
    namespace Internal {
        struct NativeString_LayoutTest;
    }
    // TODO(Emily): Could this benefit from retaining a var for size?
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

    struct ScopedString
    {
        ScopedString(NativeString InString)
            : m_String(InString) {}
        ScopedString(const char* InString)
            : m_String(NativeString::New(InString)) {}
        ScopedString(StdStringView InString)
            : m_String(NativeString::New(InString)) {}

        ScopedString& operator=(NativeString InOther)
        {
            NativeString::Free(m_String);
            m_String = InOther;
            return *this;
        }

        ScopedString& operator=(const ScopedString& InOther)
        {
            NativeString::Free(m_String);
            m_String = InOther.m_String;
            return *this;
        }

        ~ScopedString()
        {
            NativeString::Free(m_String);
        }

        operator StdString() const { return m_String; }
        operator NativeString() const { return m_String; }

        bool operator==(const ScopedString& InOther) const
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
