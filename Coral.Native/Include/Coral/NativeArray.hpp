#pragma once

#include "Memory.hpp"

namespace Coral {
    namespace Internal {
        struct NativeArray_LayoutTest;
    }
    class Array;
    template <typename TValue>
    class alignas(8) NativeArray
    {
    public:
        static NativeArray New(size_t InLength)
        {
            NativeArray<TValue> result;
            if (InLength > 0)
            {
                result.m_Ptr = static_cast<TValue*>(Memory::AllocHGlobal(InLength * sizeof(TValue)));
                result.m_Length = static_cast<int32_t>(InLength);
            }
            return result;
        }

        static NativeArray New(const StdVector<TValue>& InValues)
        {
            NativeArray<TValue> result;

            if (!InValues.empty())
            {
                result.m_Ptr = static_cast<TValue*>(Memory::AllocHGlobal(InValues.size() * sizeof(TValue)));
                result.m_Length = static_cast<int32_t>(InValues.size());
                memcpy(result.m_Ptr, InValues.data(), InValues.size() * sizeof(TValue));
            }

            return result;
        }

        static NativeArray New(const void* buffer, size_t size)
        {
            NativeArray<TValue> result;

            if (buffer)
            {
                result.m_Ptr = static_cast<TValue*>(Memory::AllocHGlobal(size));
                result.m_Length = static_cast<int32_t>(size);
                memcpy(result.m_Ptr, buffer, size);
            }

            return result;
        }

        static NativeArray New(StdInitializerList<TValue> InValues)
        {
            NativeArray result;

            if (InValues.size() > 0)
            {
                result.m_Ptr = static_cast<TValue*>(Memory::AllocHGlobal(InValues.size() * sizeof(TValue)));
                result.m_Length = static_cast<int32_t>(InValues.size());
                memcpy(result.m_Ptr, InValues.begin(), InValues.size() * sizeof(TValue));
            }

            return result;
        }

        static void Free(NativeArray InArray)
        {
            if (!InArray.m_Ptr || InArray.m_Length == 0)
                return;

            Memory::FreeHGlobal(InArray.m_Ptr);
            InArray.m_Ptr = nullptr;
            InArray.m_Length = 0;
        }

        void Assign(const NativeArray& InOther)
        {
            memcpy(m_Ptr, InOther.m_Ptr, InOther.m_Length * sizeof(TValue));
        }

        bool IsEmpty() const { return m_Length == 0 || m_Ptr == nullptr; }

        TValue& operator[](size_t InIndex) { return m_Ptr[InIndex]; }
        const TValue& operator[](size_t InIndex) const { return m_Ptr[InIndex]; }

        size_t Length() const { return m_Length; }
        size_t ByteLength() const { return m_Length * sizeof(TValue); }

        TValue* Data() { return m_Ptr; }
        const TValue* Data() const { return m_Ptr; }

        TValue* begin() { return m_Ptr; }
        TValue* end() { return m_Ptr + m_Length; }

        const TValue* begin() const { return m_Ptr; }
        const TValue* end() const { return m_Ptr + m_Length; }

        const TValue* cbegin() const { return m_Ptr; }
        const TValue* cend() const { return m_Ptr + m_Length; }

    private:
        alignas(8) TValue* m_Ptr = nullptr;
        alignas(8) TValue* m_ArrayHandle = nullptr;
        alignas(8) int32_t m_Length = 0;
        alignas(8) Bool32 m_IsDisposed = false;

        friend struct Internal::NativeArray_LayoutTest;
        friend class Array;
    };
    namespace Internal {
        struct NativeArray_LayoutTest
        {
            static_assert(offsetof(NativeArray<char>, m_Ptr) == 0);
            static_assert(offsetof(NativeArray<char>, m_ArrayHandle) == 8);
            static_assert(offsetof(NativeArray<char>, m_Length) == 16);
            static_assert(offsetof(NativeArray<char>, m_IsDisposed) == 24);
            static_assert(sizeof(NativeArray<char>) == 32);
        };
    }
}
