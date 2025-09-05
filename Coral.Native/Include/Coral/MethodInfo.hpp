#pragma once

#include "Core.hpp"
#include "String.hpp"

namespace Coral {

	class Type;
	class Attribute;
	class Object;

	class MethodInfo
	{
	public:
		String GetName() const;

		Type& GetReturnType();
		const std::vector<Type*>& GetParameterTypes();

		TypeAccessibility GetAccessibility() const;

		std::vector<Attribute> GetAttributes() const;

		constexpr inline operator bool()
		{
			return m_Handle != -1;
		}

		MetadataToken GetMetadataToken() const
		{
			return static_cast<MetadataToken>(m_Handle);
		}
	private:
		ManagedHandle m_Handle = -1;
		Type* m_ReturnType = nullptr;
		std::vector<Type*> m_ParameterTypes;

		friend class Type;
		friend class Object;
	};

}
