#ifndef __BOXPP_UTILS_COMPARATOR_HPP__
#define __BOXPP_UTILS_COMPARATOR_HPP__

#ifndef __BOXPP_UTILS_FUNCTIONALITY_HPP__
/* Loads Functionality.hpp header if not loaded. */
#include <boxpp/utils/Functionality.hpp>
#endif

#include <string.h>

namespace boxpp
{
	namespace _ {
		/* Equal operation (forward impl) */
		template< typename Type,
			bool CanEqual = HasExpectedOperatorInType<ops::Equal<>, bool, Type, Type>,
			bool CanNotEqual = HasExpectedOperatorInType<ops::NotEqual<>, bool, Type, Type>
		> struct EqualsImpl {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return Left == Right;
			}
		};

		/* Equal operation (alternative impl) */
		template< typename Type> struct EqualsImpl<Type, false, true> {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return !(Left != Right);
			}
		};

		/* Equal operation (fail-over impl) */
		template< typename Type> struct EqualsImpl<Type, false, false> {
			static constexpr bool UseMemcmp = true;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return !::memcmp(&Left, &Right, sizeof(Type));
			}
		};

		/* Not equal operation (forward impl) */
		template< typename Type,
			bool CanNotEqual = HasExpectedOperatorInType<ops::NotEqual<>, bool, Type, Type>,
			bool CanEqual = HasExpectedOperatorInType<ops::Equal<>, bool, Type, Type>
		> struct NotEqualsImpl {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return Left != Right;
			}
		};

		/* Not equal operation (alternative impl) */
		template< typename Type> struct NotEqualsImpl<Type, false, true> {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return !(Left == Right);
			}
		};

		/* Not equal operation (fail-over impl) */
		template< typename Type> struct NotEqualsImpl<Type, false, false> {
			static constexpr bool UseMemcmp = true;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return ::memcmp(&Left, &Right, sizeof(Type));
			}
		};

		/* Greater operation (forward impl) */
		template< typename Type,
			bool CanGreater = HasExpectedOperatorInType<ops::Greater<>, bool, Type, Type>,
			bool CanLessEqaul = HasExpectedOperatorInType<ops::LessEqual<>, bool, Type, Type>
		> struct GreaterImpl {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return Left > Right;
			}
		};

		/* Greater operation (alternative impl) */
		template< typename Type> struct GreaterImpl<Type, false, true> {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return !(Left <= Right);
			}
		};

		/* Greater operation (fail-over impl) */
		template< typename Type> struct GreaterImpl<Type, false, false> {
			static constexpr bool UseMemcmp = true;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return ::memcmp(&Left, &Right, sizeof(Type)) > 0;
			}
		};

		/* Greater-equal operation (forward impl) */
		template< typename Type,
			bool CanGreaterEqual = HasExpectedOperatorInType<ops::GreaterEqual<>, bool, Type, Type>,
			bool CanLess = HasExpectedOperatorInType<ops::Less<>, bool, Type, Type>
		> struct GreaterEqualImpl {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return Left >= Right;
			}
		};

		/* Greater-equal operation (alternative impl) */
		template< typename Type> struct GreaterEqualImpl<Type, false, true> {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return !(Left < Right);
			}
		};

		/* Greater-equal operation (fail-over impl) */
		template< typename Type> struct GreaterEqualImpl<Type, false, false> {
			static constexpr bool UseMemcmp = true;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return ::memcmp(&Left, &Right, sizeof(Type)) >= 0;
			}
		};

		/* Less operation (forward impl) */
		template< typename Type,
			bool CanLess = HasExpectedOperatorInType<ops::Less<>, bool, Type, Type>,
			bool CanGreaterEqaul = HasExpectedOperatorInType<ops::GreaterEqual<>, bool, Type, Type>
		> struct LessImpl {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return Left < Right;
			}
		};

		/* Less operation (alternative impl) */
		template< typename Type> struct LessImpl<Type, false, true> {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return !(Left >= Right);
			}
		};

		/* Less operation (fail-over impl) */
		template< typename Type> struct LessImpl<Type, false, false> {
			static constexpr bool UseMemcmp = true;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return ::memcmp(&Left, &Right, sizeof(Type)) < 0;
			}
		};

		/* Less-equal operation (forward impl) */
		template< typename Type,
			bool CanLessEqual = HasExpectedOperatorInType<ops::LessEqual<>, bool, Type, Type>,
			bool CanGreater = HasExpectedOperatorInType<ops::Greater<>, bool, Type, Type>
		> struct LessEqualImpl {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return Left <= Right;
			}
		};

		/* Less-equal operation (alternative impl) */
		template< typename Type> struct LessEqualImpl<Type, false, true> {
			static constexpr bool UseMemcmp = false;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return !(Left > Right);
			}
		};

		/* Less-equal operation (fail-over impl) */
		template< typename Type> struct LessEqualImpl<Type, false, false> {
			static constexpr bool UseMemcmp = true;
			FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
				return ::memcmp(&Left, &Right, sizeof(Type)) <= 0;
			}
		};

		template<typename Type, bool _UseMemcmp = EqualsImpl<Type>::UseMemcmp || GreaterImpl<Type>::UseMemcmp>
		struct OperatorOrMemcmp
		{
			static constexpr bool UseMemcmp = _UseMemcmp;
			FASTINLINE static s32 CStyleCompare(const Type& Left, const Type& Right) {
				return ::memcmp(&Left, &Right, sizeof(Type));
			}
		};

		template<typename Type>
		struct OperatorOrMemcmp<Type, false>
		{
			static constexpr bool UseMemcmp = false;
			FASTINLINE static s32 CStyleCompare(const Type& Left, const Type& Right) {
				return
					(EqualsImpl<Type>::Compare(Left, Right) ? 0 :
					(GreaterImpl<Type>::Compare(Left, Right) ? 1 : -1));
			}
		};
	}

	/* Compare two values and returns memcmp style output. */
	template<typename Type>
	FASTINLINE s32 Compare(const Type& Left, const Type& Right)
	{
		return _::OperatorOrMemcmp<Type>::CStyleCompare(Left, Right);
	}

	template<typename Type>
	struct TEquals
	{
		FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
			return !boxpp::Compare(Left, Right);
		}
	};

	template<typename Type>
	struct TSmaller
	{
		FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
			return boxpp::Compare(Left, Right) < 0;
		}
	};

	template<typename Type>
	struct TBigger
	{
		FASTINLINE static bool Compare(const Type& Left, const Type& Right) {
			return boxpp::Compare(Left, Right) > 0;
		}
	};

	/*
		Determines an item is ranged between left and right.
		NOTE: Left == item and Right == item is not ranged.
	*/
	template<typename T>
	struct TRanged
	{
		FASTINLINE static bool Ranged(const T& Item, const T& Left, const T& Right) {
			return TBigger<T>::Compare(Item, Left) && TSmaller<T>::Compare(Item, Right);
		}
	};

	/*
		Determines an item is ranged between left and right.
		NOTE: Left == item and Right == item is not ranged.
	*/
	template<typename T>
	struct TInvertedRange
	{
		FASTINLINE static bool Ranged(const T& Item, const T& Left, const T& Right) {
			return TSmaller<T>::Compare(Item, Left) && TBigger<T>::Compare(Item, Right);
		}
	};
}

#endif // !__BOXPP_UTILS_COMPARATOR_HPP__
