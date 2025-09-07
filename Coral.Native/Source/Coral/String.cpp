#include "Coral/String.hpp"
#include "Coral/StringHelper.hpp"
#include "CoralManagedFunctions.hpp"
#include <stack>
namespace Coral {
    String::String(int32_t InLength)
    {
        m_Handle = s_ManagedFunctions.CreateNewManagedStringFptr(nullptr, InLength);
    }
    String::String(const char16_t* InString, int32_t InLength)
    {
        m_Handle = s_ManagedFunctions.CreateNewManagedStringFptr(InString, InLength);
    }
    String String::CreateEmptyString(int32_t InLength)
    {
        return String(InLength);
    }
    String String::CreateStringUtf8(StdStringView InString)
    {
        return CreateStringUtf16(StringHelper::ConvertUtf8ToWide(InString));
    }
    String String::CreateStringUtf16(StdWStringView InString)
    {
        return String(reinterpret_cast<const char16_t*>(InString.data()), InString.length());
    }

    StdString String::GetStringUtf8() const
    {
        return StringHelper::ConvertWideToUtf8(GetStringUtf16());
    }
    StdWString String::GetStringUtf16() const
    {
        int32_t length = GetLength();
        StdWString result = {};
        result.resize(static_cast<size_t>(length));
        s_ManagedFunctions.GetStringContentsFptr(m_Handle, reinterpret_cast<char16_t*>(result.data()));
        return result;
    }

    int32_t String::GetLength() const
    {
        return s_ManagedFunctions.GetStringContentsFptr(m_Handle, nullptr);
    }
}
