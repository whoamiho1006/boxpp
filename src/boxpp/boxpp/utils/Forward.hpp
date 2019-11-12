#ifndef __BOXPP_UTILS_FORWARD_HPP__
#define __BOXPP_UTILS_FORWARD_HPP__

#ifndef __BOXPP_HPP__
/* Loads boxpp.hpp header if not loaded. */
#include <boxpp.hpp>
#endif

namespace boxpp
{
	namespace _ {
		template<typename _Type> struct IsLeftValueReferenceImpl { static constexpr bool Value = false; };
		template<typename _Type> struct IsLeftValueReferenceImpl<_Type&> { static constexpr bool Value = true; };

		template<typename _Type> struct IsRightValueReferenceImpl { static constexpr bool Value = false; };
		template<typename _Type> struct IsRightValueReferenceImpl<_Type&&> { static constexpr bool Value = true; };

		template<typename _Type> struct ToNonReferenceTypeImpl { using Type = _Type; };
		template<typename _Type> struct ToNonReferenceTypeImpl<_Type&> { using Type = _Type; };
		template<typename _Type> struct ToNonReferenceTypeImpl<_Type&&> { using Type = _Type; };
	}

	/* Convert given type to non-reference type. */
	template<typename Type>
	using ToNonReferenceType = typename _::ToNonReferenceTypeImpl<Type>::Type;

	/* Determines given type is LValue reference or not. (Easily, == Type&) */
	template<typename Type>
	constexpr bool IsTypeLeftValueReference = _::IsLeftValueReferenceImpl<Type>::Value;

	/* Determines given type is RValue reference or not. (Easily, == Type&&) */
	template<typename Type>
	constexpr bool IsTypeRightValueReference = _::IsRightValueReferenceImpl<Type>::Value;

	/* Cast given LValue reference to be RValue reference. */
	template<typename Type>
	constexpr Type&& Forward(ToNonReferenceType<Type>& Ref) noexcept {
		return static_cast<Type&&>(Ref); 
	}

	template<typename Type>
	constexpr Type&& Forward(ToNonReferenceType<Type>&& Ref) noexcept {
		static_assert(!IsTypeLeftValueReference<Type>,
			"Error: boxpp::Forward(REF) method requires LValue reference!");

		return static_cast<Type&&>(Ref);
	}

}

#endif // !__BOXPP_UTILS_FORWARD_HPP__