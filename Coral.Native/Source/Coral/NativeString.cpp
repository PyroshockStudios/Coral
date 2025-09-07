#include "Coral/NativeString.hpp"
#include "Coral/StringHelper.hpp"
#include "Coral/Memory.hpp"

#include "Verify.hpp"

namespace Coral {

    NativeString NativeString::New(const char* InString)
    {
        NativeString result;
        result.Assign(InString);
        return result;
    }

    NativeString NativeString::New(StdStringView InString)
    {
        NativeString result;
        result.Assign(InString);
        return result;
    }

    void NativeString::Free(NativeString& InString)
    {
        if (InString.m_String == nullptr)
            return;

        Memory::FreeCoTaskMem(InString.m_String);
        InString.m_String = nullptr;
    }

    void NativeString::Assign(StdStringView InString)
    {
        if (m_String != nullptr)
            Memory::FreeCoTaskMem(m_String);

        m_String = Memory::StringToCoTaskMemAuto(StringHelper::ConvertUtf8ToUC(InString));
    }

    NativeString::operator StdString() const
    {
        UCStringView string(m_String);

        return StringHelper::ConvertUCToUtf8(string);
    }

    bool NativeString::operator==(const NativeString& InOther) const
    {
        if (m_String == InOther.m_String)
        {
            return true;
        }

        if (m_String == nullptr || InOther.m_String == nullptr)
        {
            return false;
        }

#ifdef CORAL_WIDE_CHARS
        return wcscmp(m_String, InOther.m_String) == 0;
#else
        return strcmp(m_String, InOther.m_String) == 0;
#endif
    }

    bool NativeString::operator==(StdStringView InOther) const
    {
        if (m_String == nullptr && InOther.empty())
        {
            return true;
        }

        if (m_String == nullptr)
            return false;

#ifdef CORAL_WIDE_CHARS
        auto str = StringHelper::ConvertUtf8ToUC(InOther);
        return wcscmp(m_String, str.data()) == 0;
#else
        return strcmp(m_String, InOther.data()) == 0;
#endif
    }

}
