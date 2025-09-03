#pragma once

#include "Core.hpp"
#include "Utility.hpp"
#include "String.hpp"

namespace Coral {

	class FieldInfo;
	class MethodInfo;
	class PropertyInfo;

	class ManagedAssembly;
	class Type;

	template <typename... TArgs>
	struct MethodParams
	{
		MethodParams() = default;
		MethodParams(const MethodParams&) = delete;
		MethodParams& operator=(const MethodParams&) = delete;
		MethodParams(MethodParams&&) = default;
		MethodParams& operator=(MethodParams&&) = default;

		MethodParams(const TArgs&... InParameters)
		{
			if constexpr (sizeof...(InParameters))
			{
				AddToArray<TArgs...>(parameterValues, parameterTypes, std::forward<TArgs>(InParameters)..., std::make_index_sequence<sizeof...(InParameters)> {});
			}
		}
		MethodParams(TArgs&&... InParameters)
		{
			if constexpr (sizeof...(InParameters))
			{
				AddToArray<TArgs...>(parameterValues, parameterTypes, std::forward<TArgs>(InParameters)..., std::make_index_sequence<sizeof...(InParameters)> {});
			}
		}

	private:
		friend class ManagedObject;
		const void* parameterValues[sizeof...(TArgs)];
		ManagedType parameterTypes[sizeof...(TArgs)];
	};
	class alignas(8) ManagedObject
	{
	public:
		ManagedObject() = default;
		ManagedObject(const ManagedObject& InOther);
		ManagedObject(ManagedObject&& InOther) noexcept;
		~ManagedObject();

		ManagedObject& operator=(const ManagedObject& InOther);
		ManagedObject& operator=(ManagedObject&& InOther) noexcept;

		template <typename TReturn, typename... TArgs>
		TReturn InvokeMethod(const MethodInfo& InMethod, MethodParams<TArgs...>&& InParameters = {}, ManagedObject* OutException = nullptr) const
		{
			constexpr size_t paramCount = sizeof(InParameters.parameterValues) / sizeof(*InParameters.parameterValues);
			TReturn result;
			InvokeMethodRetInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, paramCount, &result);
			return result;
		}

		template <typename... TArgs>
		void InvokeMethod(const MethodInfo& InMethod, MethodParams<TArgs...>&& InParameters = {}, ManagedObject* OutException = nullptr) const
		{
			constexpr size_t paramCount = sizeof(InParameters.parameterValues) / sizeof(*InParameters.parameterValues);
			InvokeMethodInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, paramCount);
		}

		template <typename TValue>
		void SetFieldValue(const FieldInfo& InField, TValue InValue) const
		{
			SetFieldValueRaw(InField, &InValue);
		}

		template <typename TReturn>
		TReturn GetFieldValue(const FieldInfo& InField) const
		{
			TReturn result;
			GetFieldValueRaw(InField, &result);
			return result;
		}

		template <typename TValue>
		void SetPropertyValue(const PropertyInfo& InProperty, TValue InValue) const
		{
			SetPropertyValueRaw(InProperty, &InValue);
		}

		template <typename TReturn>
		TReturn GetPropertyValue(const PropertyInfo& InProperty) const
		{
			TReturn result;
			GetPropertyValueRaw(InProperty, &result);
			return result;
		}

		void SetFieldValueRaw(const FieldInfo& InField, void* InValue) const;
		void GetFieldValueRaw(const FieldInfo& InField, void* OutValue) const;
		void SetPropertyValueRaw(const PropertyInfo& InProperty, void* InValue, ManagedObject* OutException = nullptr) const;
		void GetPropertyValueRaw(const PropertyInfo& InProperty, void* OutValue, ManagedObject* OutException = nullptr) const;

		const Type& GetType();

		void Destroy();

		bool IsValid() const { return m_Handle != nullptr && m_Type != nullptr; }

	private:
		void InvokeMethodInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
		void InvokeMethodRetInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, void* InResultStorage) const;

	public:
		alignas(8) void* m_Handle = nullptr;
		alignas(8) const Type* m_Type = nullptr;

	private:
		friend class ManagedAssembly;
		friend class Type;
	};

	static_assert(offsetof(ManagedObject, m_Handle) == 0);
	static_assert(offsetof(ManagedObject, m_Type) == 8);
	static_assert(sizeof(ManagedObject) == 16);

	template <>
	inline void ManagedObject::SetFieldValue(const FieldInfo& InField, std::string InValue) const
	{
		String s = String::New(InValue);
		SetFieldValueRaw(InField, &InValue);
		String::Free(s);
	}

	template <>
	inline void ManagedObject::SetFieldValue(const FieldInfo& InField, bool InValue) const
	{
		Bool32 s = InValue;
		SetFieldValueRaw(InField, &s);
	}

	template <>
	inline std::string ManagedObject::GetFieldValue(const FieldInfo& InField) const
	{
		String result;
		GetFieldValueRaw(InField, &result);
		auto s = result.Data() ? std::string(result) : "";
		String::Free(result);
		return s;
	}

	template <>
	inline bool ManagedObject::GetFieldValue(const FieldInfo& InField) const
	{
		Bool32 result;
		GetFieldValueRaw(InField, &result);
		return result;
	}

}
