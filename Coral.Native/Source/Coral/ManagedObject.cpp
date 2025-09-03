#include "Coral/ManagedObject.hpp"
#include "Coral/Assembly.hpp"
#include "Coral/String.hpp"
#include "Coral/StringHelper.hpp"
#include "Coral/Type.hpp"
#include "Coral/TypeCache.hpp"

#include "CoralManagedFunctions.hpp"

namespace Coral {

	ManagedObject::ManagedObject(const ManagedObject& InOther)
	{
		if (InOther.m_Handle)
		{
			m_Handle = s_ManagedFunctions.CopyObjectFptr(InOther.m_Handle);
			m_Type = InOther.m_Type;
		}
	}

	ManagedObject::ManagedObject(ManagedObject&& InOther) noexcept : m_Handle(InOther.m_Handle), m_Type(InOther.m_Type)
	{
		InOther.m_Handle = nullptr;
		InOther.m_Type = nullptr;
	}

	ManagedObject::~ManagedObject()
	{
		Destroy();
	}

	ManagedObject& ManagedObject::operator=(ManagedObject&& InOther) noexcept
	{
		if (this != &InOther)
		{
			m_Handle = InOther.m_Handle;
			m_Type = InOther.m_Type;
			InOther.m_Handle = nullptr;
			InOther.m_Type = nullptr;
		}

		return *this;
	}

	ManagedObject& ManagedObject::operator=(const ManagedObject& InOther)
	{
		if (this != &InOther)
		{
			Destroy();
			if (InOther.m_Handle)
			{
				m_Handle = s_ManagedFunctions.CopyObjectFptr(InOther.m_Handle);
				m_Type = InOther.m_Type;
			}
		}

		return *this;
	}

	void ManagedObject::InvokeMethodInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const
	{
		// NOTE(Peter): If you get an exception in this function it's most likely because you're using a Native only debugger type in Visual Studio
		//				and it's catching a C# exception even though it shouldn't. I recommend switching the debugger type to Mixed (.NET Core)
		//				which should be the default for Hazelnut, or simply press "Continue" until it works.
		//				This is a problem with the Visual Studio debugger and nothing we can change.
		void* exceptionResult = nullptr;
		s_ManagedFunctions.InvokeMethodFptr(m_Handle, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), OutException ? &exceptionResult : nullptr);
		if (OutException)
		{
			*OutException = ManagedObject();
			OutException->m_Handle = exceptionResult;
		}
	}

	void ManagedObject::InvokeMethodRetInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, void* InResultStorage) const
	{
		void* exceptionResult = nullptr;
		s_ManagedFunctions.InvokeMethodRetFptr(m_Handle, InMethod.m_Handle, InParameters, InParameterTypes, static_cast<int32_t>(InLength), InResultStorage, OutException ? &exceptionResult : nullptr);
		if (OutException)
		{
			*OutException = ManagedObject();
			OutException->m_Handle = exceptionResult;
		}
	}

	void ManagedObject::SetFieldValueRaw(const FieldInfo& InField , void* InValue) const
	{
		s_ManagedFunctions.SetFieldValueFptr(m_Handle, InField.m_Handle, InValue);
	}

	void ManagedObject::GetFieldValueRaw(const FieldInfo& InField, void* OutValue) const
	{
		s_ManagedFunctions.GetFieldValueFptr(m_Handle, InField.m_Handle, OutValue);
	}

	void ManagedObject::SetPropertyValueRaw(const PropertyInfo& InProperty, void* InValue, ManagedObject* OutException) const
	{
		void* exceptionResult = nullptr;
		s_ManagedFunctions.SetPropertyValueFptr(m_Handle, InProperty.m_Handle, InValue, OutException ? &exceptionResult : nullptr);
		if (OutException)
		{
			*OutException = ManagedObject();
			OutException->m_Handle = exceptionResult;
		}
	}

	void ManagedObject::GetPropertyValueRaw(const PropertyInfo& InProperty, void* OutValue, ManagedObject* OutException) const
	{
		void* exceptionResult = nullptr;
		s_ManagedFunctions.GetPropertyValueFptr(m_Handle, InProperty.m_Handle, OutValue, OutException ? &exceptionResult : nullptr);
		if (OutException)
		{
			*OutException = ManagedObject();
			OutException->m_Handle = exceptionResult;
		}
	}

	const Type& ManagedObject::GetType()
	{
		if (!m_Type)
		{
			Type type;
			s_ManagedFunctions.GetObjectTypeIdFptr(m_Handle, &type.m_Id);
			m_Type = TypeCache::Get().CacheType(std::move(type));
		}

		return *m_Type;
	}

	void ManagedObject::Destroy()
	{
		if (!m_Handle)
			return;

		s_ManagedFunctions.DestroyObjectFptr(m_Handle);
		m_Handle = nullptr;
		m_Type = nullptr;
	}

}
