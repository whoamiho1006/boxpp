#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>

namespace boxpp
{
	/* Determines Src-Type can be converted to Dest-Type. */
	template<typename DestType, typename SrcType>
	struct TIsConvertibleType : TTraitBoolConstant<__is_convertible_to(SrcType, DestType)> { };

	/* Determines Src-Type can be converted to Dest-Type. */
	template<typename DestType, typename SrcType>
	constexpr bool IsConvertibleType = TIsConvertibleType<DestType, SrcType>::Value;
}