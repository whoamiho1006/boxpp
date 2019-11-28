#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/IsCallableType.hpp>
#include <boxpp/base/tpls/traits/AbstractedOperators.hpp>

#include <math.h>

namespace boxpp
{
	namespace _ {
		template<typename Type>
		struct TTriangles
		{
			FASTINLINE static Type Sqrt(const Type& Value) {
				return (Type) ::sqrtf((f32)Value);
			}

			FASTINLINE static Type Sin(const Type& Value) {
				return (Type) ::sinf((f32)Value);
			}

			FASTINLINE static Type Cos(const Type& Value) {
				return (Type) ::cosf((f32)Value);
			}

			FASTINLINE static Type Tan(const Type& Value) {
				return (Type) ::tanf((f32)Value);
			}

			FASTINLINE static Type ArcSin(const Type& Value) {
				return (Type) ::asinf((f32)Value);
			}

			FASTINLINE static Type ArcCos(const Type& Value) {
				return (Type) ::acosf((f32)Value);
			}

			FASTINLINE static Type ArcTan(const Type& Value) {
				return (Type) ::atanf((f32)Value);
			}

			FASTINLINE static Type ArcTan2(const Type& Y, const Type& X) {
				return (Type) ::atan2f((f32)Y, (f32)X);
			}
		};

		template<>
		struct TTriangles<f64>
		{
			FASTINLINE static f64 Sqrt(const f64& Value) {
				return ::sqrt(Value);
			}

			FASTINLINE static f64 Sin(const f64& Value) {
				return ::sin(Value);
			}

			FASTINLINE static f64 Cos(const f64& Value) {
				return ::cos(Value);
			}

			FASTINLINE static f64 Tan(const f64& Value) {
				return ::tan(Value);
			}

			FASTINLINE static f64 ArcSin(const f64& Value) {
				return ::asin(Value);
			}

			FASTINLINE static f64 ArcCos(const f64& Value) {
				return ::acos(Value);
			}

			FASTINLINE static f64 ArcTan(const f64& Value) {
				return ::atan(Value);
			}

			FASTINLINE static f64 ArcTan2(const f64& Y, const f64& X) {
				return ::atan2(Y, X);
			}
		};
	}

	template<typename Type>
	FASTINLINE const Type& Min(const Type& LeftType, const Type& Right) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return LeftType < Right ? LeftType : Right;
	}

	template<typename Type>
	FASTINLINE const Type& Max(const Type& LeftType, const Type& Right) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return LeftType > Right ? LeftType : Right;
	}

	template<typename Type>
	FASTINLINE const Type& Clamp(const Type& Value, const Type& MaxVal, const Type& MinVal) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return Min(Max(MinVal, Value), MaxVal);
	}

	template<typename Type>
	FASTINLINE bool IsZero(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return Abs(Value) <= type_db::TNumberLimits<Type>::Tolerant;
	}

	template<typename Type>
	FASTINLINE Type Abs(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return Value < 0 ? -Value : Value;
	}

	template<typename Type>
	FASTINLINE Type Sqrt(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::Sqrt(Value);
	}

	template<typename Type>
	FASTINLINE Type Sin(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::Sin(Value);
	}

	template<typename Type>
	FASTINLINE Type Cos(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::Cos(Value);
	}

	template<typename Type>
	FASTINLINE Type Tan(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::Tan(Value);
	}


	template<typename Type>
	FASTINLINE Type ArcSin(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::ArcSin(Value);
	}

	template<typename Type>
	FASTINLINE Type ArcCos(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::ArcCos(Value);
	}

	template<typename Type>
	FASTINLINE Type ArcTan(const Type& Value) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::ArcTan(Value);
	}

	template<typename Type>
	FASTINLINE Type ArcTan(const Type& Y, const Type& X) {
		static_assert(IsNumericType<Type>, "Given type should be numeric type!");
		return _::TTriangles<Type>::ArcTan2(Y, X);
	}
}