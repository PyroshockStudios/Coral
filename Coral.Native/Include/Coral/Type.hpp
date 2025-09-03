#pragma once

#include "Core.hpp"
#include "String.hpp"
#include "ManagedObject.hpp"
#include "MethodInfo.hpp"
#include "FieldInfo.hpp"
#include "PropertyInfo.hpp"

#include <optional>

namespace Coral {

	class Type
	{
	public:
		String GetFullName() const;
		String GetAssemblyQualifiedName() const;

		Type& GetBaseType();
		std::vector<Type*>& GetInterfaceTypes();

		int32_t GetSize() const;

		bool IsSubclassOf(const Type& InOther) const;
		bool IsAssignableTo(const Type& InOther) const;
		bool IsAssignableFrom(const Type& InOther) const;

		std::vector<MethodInfo> GetMethods() const;
		std::vector<FieldInfo> GetFields() const;
		std::vector<PropertyInfo> GetProperties() const;

		MethodInfo GetMethodByName(std::string_view MethodName, int32_t InParamCount, BindingFlags InBindingFlags = BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::Static) const;
		FieldInfo GetFieldByName(std::string_view FieldName, BindingFlags InBindingFlags = BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::Static) const;
		PropertyInfo GetPropertyByName(std::string_view PropertyName, BindingFlags InBindingFlags = BindingFlags::Public | BindingFlags::NonPublic | BindingFlags::Static) const;

		bool HasAttribute(const Type& InAttributeType) const;
		std::vector<Attribute> GetAttributes() const;

		ManagedType GetManagedType() const;

		bool IsSZArray() const;
		Type& GetElementType();

		bool operator==(const Type& InOther) const;

		operator bool() const { return m_Id != -1; }

		TypeId GetTypeId() const { return m_Id; }

	public:
		template <typename... TArgs>
		ManagedObject CreateInstance(MethodParams<TArgs...>&& InParameters = {}, ManagedObject* OutException = nullptr) const
		{
			constexpr size_t paramCount = sizeof(InParameters.parameterValues) / sizeof(*InParameters.parameterValues);
			ManagedObject result;
			result = CreateInstanceInternal(OutException, InParameters.parameterValues, InParameters.parameterTypes, paramCount);
			return result;
		}

		template <typename TReturn, typename... TArgs>
		TReturn InvokeStaticMethod(const MethodInfo& InMethod, MethodParams<TArgs...>&& InParameters = {}, ManagedObject* OutException = nullptr) const
		{
			TReturn result = {};
			constexpr size_t paramCount = sizeof(InParameters.parameterValues) / sizeof(*InParameters.parameterValues);
			InvokeStaticMethodRetInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, paramCount, &result);
			return result;
		}

		template <typename... TArgs>
		void InvokeStaticMethod(const MethodInfo& InMethod, MethodParams<TArgs...>&& InParameters = {}, ManagedObject* OutException = nullptr) const
		{
			constexpr size_t parameterCount = sizeof...(InParameters);
			if constexpr (parameterCount > 0)
			{
				constexpr size_t paramCount = sizeof(InParameters.parameterValues) / sizeof(*InParameters.parameterValues);
				InvokeStaticMethodInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, paramCount);
			}
			else
			{
				InvokeStaticMethodInternal(OutException, InMethod, nullptr, nullptr, 0);
			}
		}

	private:
		ManagedObject CreateInstanceInternal(ManagedObject* OutException, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
		void InvokeStaticMethodInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
		void InvokeStaticMethodRetInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, void* InResultStorage) const;

	private:
		TypeId m_Id = -1;
		Type* m_BaseType = nullptr;
		std::optional<std::vector<Type*>> m_InterfaceTypes = std::nullopt;
		Type* m_ElementType = nullptr;

		friend class HostInstance;
		friend class ManagedAssembly;
		friend class AssemblyLoadContext;
		friend class MethodInfo;
		friend class FieldInfo;
		friend class PropertyInfo;
		friend class Attribute;
		friend class ReflectionType;
		friend class ManagedObject;
	};

	class ReflectionType
	{
	public:
		operator Type&() const;

	public:
		TypeId m_TypeID;
	};

	static_assert(offsetof(ReflectionType, m_TypeID) == 0);
	static_assert(sizeof(ReflectionType) == 4);
}
