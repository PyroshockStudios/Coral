#pragma once

#include "Core.hpp"
#include "Utility.hpp"
#include "String.hpp"
#include "MethodInfo.hpp"

namespace Coral {

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

		MethodParams(TArgs&&... InParameters)
		{
			if constexpr (sizeof...(InParameters))
			{
				AddToArray<TArgs...>(parameterValues, parameterTypes, std::forward<TArgs>(InParameters)..., std::make_index_sequence<parameterCount> {});
			}
		}

	private:
		friend class ManagedObject;
		const void* parameterValues[sizeof...(InParameters)];
		ManagedType parameterTypes[sizeof...(InParameters)];
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
		TReturn InvokeMethod(const MethodInfo& InMethod, MethodParams<TArgs&&...>&& InParameters, ManagedObject* Exception = nullptr) const
		{
			constexpr size_t paramCount = sizeof(InParameters.parameterValues) / sizeof(*InParameters.parameterValues);
			TReturn result;
			InvokeMethodRetInternal(Exception, InMethod.m_Handle, InParameters.parameterValues, InParameters.parameterTypes, paramCount, &result);
			return result;
		}

		template <typename... TArgs>
		void InvokeMethod(const MethodInfo& InMethod, MethodParams<TArgs&&...>&& InParameters, ManagedObject* Exception = nullptr) const
		{
			constexpr size_t paramCount = sizeof(InParameters.parameterValues) / sizeof(*InParameters.parameterValues);
			InvokeMethodInternal(Exception, InMethod.m_Handle, InParameters.parameterValues, InParameters.parameterTypes, paramCount);
		}

		template <typename TValue>
		void SetFieldValue(std::string_view InFieldName, TValue InValue) const
		{
			SetFieldValueRaw(InFieldName, &InValue);
		}

		template <typename TReturn>
		TReturn GetFieldValue(std::string_view InFieldName) const
		{
			TReturn result;
			GetFieldValueRaw(InFieldName, &result);
			return result;
		}

		template <typename TValue>
		void SetPropertyValue(std::string_view InPropertyName, TValue InValue) const
		{
			SetPropertyValueRaw(InPropertyName, &InValue);
		}

		template <typename TReturn>
		TReturn GetPropertyValue(std::string_view InPropertyName) const
		{
			TReturn result;
			GetPropertyValueRaw(InPropertyName, &result);
			return result;
		}

		void SetFieldValueRaw(std::string_view InFieldName, void* InValue) const;
		void GetFieldValueRaw(std::string_view InFieldName, void* OutValue) const;
		void SetPropertyValueRaw(std::string_view InPropertyName, void* InValue, ManagedObject* Exception = nullptr) const;
		void GetPropertyValueRaw(std::string_view InPropertyName, void* OutValue, ManagedObject* Exception = nullptr) const;

		const Type& GetType();

		void Destroy();

		bool IsValid() const { return m_Handle != nullptr && m_Type != nullptr; }

	private:
		void InvokeMethodInternal(ManagedObject* Exception, ManagedHandle InMethodHandle, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
		void InvokeMethodRetInternal(ManagedObject* Exception, ManagedHandle InMethodHandle, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, void* InResultStorage) const;

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
	inline void ManagedObject::SetFieldValue(std::string_view InFieldName, std::string InValue) const
	{
		String s = String::New(InValue);
		SetFieldValueRaw(InFieldName, &InValue);
		String::Free(s);
	}

	template <>
	inline void ManagedObject::SetFieldValue(std::string_view InFieldName, bool InValue) const
	{
		Bool32 s = InValue;
		SetFieldValueRaw(InFieldName, &s);
	}

	template <>
	inline std::string ManagedObject::GetFieldValue(std::string_view InFieldName) const
	{
		String result;
		GetFieldValueRaw(InFieldName, &result);
		auto s = result.Data() ? std::string(result) : "";
		String::Free(result);
		return s;
	}

	template <>
	inline bool ManagedObject::GetFieldValue(std::string_view InFieldName) const
	{
		Bool32 result;
		GetFieldValueRaw(InFieldName, &result);
		return result;
	}

}
