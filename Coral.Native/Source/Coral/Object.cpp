#include "Coral/Object.hpp"
#include "Coral/Assembly.hpp"
#include "Coral/NativeString.hpp"
#include "Coral/StringHelper.hpp"
#include "Coral/Type.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"

#include <cassert>

namespace Coral {

    Object::Object(const Object& InOther)
    {
        if (InOther.m_Handle)
        {
            m_Handle = s_ManagedFunctions.CopyObjectFptr(InOther.m_Handle);
            //m_Type = InOther.m_Type;
        }
    }

    Object::Object(Object&& InOther) noexcept : m_Handle(InOther.m_Handle) //, m_Type(InOther.m_Type)
    {
        InOther.m_Handle = nullptr;
        //InOther.m_Type = nullptr;
    }

    Object::~Object()
    {
        Destroy();
    }

    Object& Object::operator=(Object&& InOther) noexcept
    {
        if (this != &InOther)
        {
            m_Handle = InOther.m_Handle;
            //m_Type = InOther.m_Type;
            InOther.m_Handle = nullptr;
            //InOther.m_Type = nullptr;
        }

        return *this;
    }

    Object& Object::operator=(const Object& InOther)
    {
        if (this != &InOther)
        {
            Destroy();
            if (InOther.IsValid())
            {
                m_Handle = s_ManagedFunctions.CopyObjectFptr(InOther.m_Handle);
            }
        }

        return *this;
    }

    Object Object::BoxRaw(const void* InValue, int32_t InSize, const Type& InType)
    {
        Object result = {};
        result.m_Handle = s_ManagedFunctions.GetObjectBoxedValueFptr(InValue, InSize, InType.m_Id);
        return result;
    }

    void Object::UnboxRaw(void* OutValue) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        s_ManagedFunctions.GetObjectUnboxedValueFptr(m_Handle, OutValue);
    }

    void Object::InvokeMethodRaw(const Method& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        // NOTE(Peter): If you get an exception in this function it's most likely because you're using a Native only debugger type in Visual Studio
        //				and it's catching a C# exception even though it shouldn't. I recommend switching the debugger type to Mixed (.NET Core)
        //				which should be the default for Hazelnut, or simply press "Continue" until it works.
        //				This is a problem with the Visual Studio debugger and nothing we can change.
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeMethodFptr(m_Handle, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Object::InvokeMethodRetRaw(const Method& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage, Object* OutException) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeMethodRetFptr(m_Handle, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), InResultStorage, InRetIsObject, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Object::InvokeDelegateRaw(const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, Object* OutException) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeDelegateFptr(m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Object::InvokeDelegateRetRaw(const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, bool InRetIsObject, void* InResultStorage, Object* OutException) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        void* exceptionResult = nullptr;
        s_ManagedFunctions.InvokeDelegateRetFptr(m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), InResultStorage, InRetIsObject, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Object::SetFieldValueRaw(const Field& InField, const void* InValue)
    {
        assert(IsValid() && "Do not use an invalid object!");
        s_ManagedFunctions.SetFieldValueFptr(m_Handle, InField.m_Handle, InValue, false);
    }

    void Object::GetFieldValueRaw(const Field& InField, void* OutValue) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        s_ManagedFunctions.GetFieldValueFptr(m_Handle, InField.m_Handle, OutValue, false);
    }

    void Object::SetFieldValueObject(const Field& InField, const Object& InObject)
    {
        assert(IsValid() && "Do not use an invalid object!");
        s_ManagedFunctions.SetFieldValueFptr(m_Handle, InField.m_Handle, &InObject.m_Handle, true);
    }

    Object Object::GetFieldValueObject(const Field& InField) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        Object result = {};
        s_ManagedFunctions.GetFieldValueFptr(m_Handle, InField.m_Handle, &result.m_Handle, true);
        return result;
    }

    void Object::SetPropertyValueRaw(const Property& InProperty, const void* InValue, Object* OutException)
    {
        assert(IsValid() && "Do not use an invalid object!");
        void* exceptionResult = nullptr;
        s_ManagedFunctions.SetPropertyValueFptr(m_Handle, InProperty.m_Handle, InValue, false, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Object::GetPropertyValueRaw(const Property& InProperty, void* OutValue, Object* OutException) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        void* exceptionResult = nullptr;
        s_ManagedFunctions.GetPropertyValueFptr(m_Handle, InProperty.m_Handle, OutValue, false, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    void Object::SetPropertyValueObject(const Property& InProperty, const Object& InObject, Object* OutException)
    {
        assert(IsValid() && "Do not use an invalid object!");
        void* exceptionResult = nullptr;
        s_ManagedFunctions.SetPropertyValueFptr(m_Handle, InProperty.m_Handle, &InObject.m_Handle, true, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
    }

    Object Object::GetPropertyValueObject(const Property& InProperty, Object* OutException) const
    {
        assert(IsValid() && "Do not use an invalid object!");
        Object result = {};
        void* exceptionResult = nullptr;
        s_ManagedFunctions.GetPropertyValueFptr(m_Handle, InProperty.m_Handle, &result.m_Handle, true, OutException ? &exceptionResult : nullptr);
        if (OutException)
        {
            *OutException = Object();
            OutException->m_Handle = exceptionResult;
        }
        return result;
    }

    Type Object::GetType() const
    {
        assert(IsValid() && "Do not use an invalid object!");
        Type type;
        s_ManagedFunctions.GetObjectTypeIdFptr(m_Handle, &type.m_Id);
        return type;
    }

    void Object::Destroy()
    {
        if (!IsValid())
            return;

        s_ManagedFunctions.DestroyObjectFptr(m_Handle);
        m_Handle = nullptr;
    }

}
