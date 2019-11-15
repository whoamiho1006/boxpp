#ifndef __BOXPP_UTFCOMMON_HPP__
#define __BOXPP_UTFCOMMON_HPP__

#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/utils/TypeCompares.hpp>

namespace boxpp
{
	/*
		utf8_t, utf16le_t, utf32le_t meta structures.
		these are defined for ploting character types
		between COMPILER supported one and CONCEPTIONAL one.
	*/
	NO_PADDING(struct utf8_t { c8 _; using NumType = c8; });
	NO_PADDING(struct utf16_t { c16 _; using NumType = c16; });
	NO_PADDING(struct utf32_t { c32 _; using NumType = c32; });


	/* Defines compatibility operators between utf*_t and c* series... */
#define BOXPP_UTF_COMPAT_OPERATORS(utfn_t, cn_t) \
	constexpr bool operator ==(cn_t _, const utfn_t& v) { return _ == v._; } \
	constexpr bool operator ==(const utfn_t& v, cn_t _) { return _ == v._; } \
	constexpr bool operator ==(const utfn_t& v1, const utfn_t& v2) { return v1._ == v2._; } \
	constexpr bool operator !=(cn_t _, const utfn_t& v) { return _ != v._; } \
	constexpr bool operator !=(const utfn_t& v, cn_t _) { return _ != v._; } \
	constexpr bool operator !=(const utfn_t& v1, const utfn_t& v2) { return v1._ != v2._; } \
	constexpr bool operator <=(cn_t _, const utfn_t& v) { return _ <= v._; } \
	constexpr bool operator <=(const utfn_t& v, cn_t _) { return v._ <= _; } \
	constexpr bool operator <=(const utfn_t& v1, const utfn_t& v2) { return v1._ <= v2._; } \
	constexpr bool operator >=(cn_t _, const utfn_t& v) { return _ >= v._; } \
	constexpr bool operator >=(const utfn_t& v, cn_t _) { return v._ >= _; } \
	constexpr bool operator >=(const utfn_t& v1, const utfn_t& v2) { return v1._ >= v2._; } \
	constexpr bool operator < (cn_t _, const utfn_t& v) { return _ < v._; } \
	constexpr bool operator < (const utfn_t& v, cn_t _) { return v._ < _; } \
	constexpr bool operator < (const utfn_t& v1, const utfn_t& v2) { return v1._ < v2._; } \
	constexpr bool operator > (cn_t _, const utfn_t& v) { return _ > v._; } \
	constexpr bool operator > (const utfn_t& v, cn_t _) { return v._ > _; } \
	constexpr bool operator > (const utfn_t& v1, const utfn_t& v2) { return v1._ > v2._; } \
	constexpr utfn_t operator -(const utfn_t& Left, const utfn_t& Right) { return { (cn_t)(Left._ - Right._) }; } \
	constexpr utfn_t operator +(const utfn_t& Left, const utfn_t& Right) { return { (cn_t)(Left._ + Right._) }; } \
	constexpr utfn_t operator -(const utfn_t& Left, const cn_t Right) { return { (cn_t)(Left._ - Right) }; } \
	constexpr utfn_t operator +(const utfn_t& Left, const cn_t Right) { return { (cn_t)(Left._ + Right) }; }
	
	BOXPP_UTF_COMPAT_OPERATORS(utf8_t, c8);
	BOXPP_UTF_COMPAT_OPERATORS(utf16_t, c16);
	BOXPP_UTF_COMPAT_OPERATORS(utf32_t, c32);

#undef BOXPP_UTF_COMPAT_OPERATORS

	template<typename Type>
	constexpr bool IsUtfType = IsSameType<Type, utf8_t> || IsSameType<Type, utf16_t> || IsSameType<Type, utf32_t>;

}

#endif // !__BOXPP_UTFCOMMON_HPP__