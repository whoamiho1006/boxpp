#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/mathematics/BaseMath.hpp>

namespace boxpp
{
	template<typename NumberType>
	class TVector2;

	/*
		3-Dimension vector.
	*/
	template<typename NumberType = f32>
	class TVector3
	{
	public:
		typedef typename type_db::TSizedFloat<sizeof(NumberType)>::Type FloatType;

	public:
		struct UpType { };
		struct DownType { };
		struct ForwardType { };
		struct BackwardType { };
		struct LeftType { };
		struct RightType { };

	public:
		static constexpr UpType Up = UpType();
		static constexpr DownType Down = DownType();
		static constexpr ForwardType Forward = ForwardType();
		static constexpr BackwardType Backward = BackwardType();
		static constexpr LeftType Left = LeftType();
		static constexpr RightType Right = RightType();

	public:
		TVector3(NumberType X = NumberType())
			: X(X), Y(X), Z(X)
		{
		}

		TVector3(const TVector2<NumberType>& Vec2, NumberType Z);
		
		TVector3(NumberType X, NumberType Y, NumberType Z)
			: X(X), Y(Y), Z(Z)
		{
		}

		TVector3(UpType) : X(0), Y(0), Z(1) { }
		TVector3(DownType) : X(0), Y(0), Z(-1) { }

		TVector3(ForwardType) : X(0), Y(1), Z(0) { }
		TVector3(BackwardType) : X(0), Y(-1), Z(0) { }

		TVector3(LeftType) : X(-1), Y(0), Z(0) { }
		TVector3(RightType) : X(1), Y(0), Z(0) { }

		/* Conversion between number-types. */
		template<typename OtherType>
		TVector3(const TVector3<OtherType>& Other) : X((NumberType)Other.X), Y((NumberType)Other.Y), Z((NumberType)Other.Z) { }
		
		/* Copy/Move use default. */
		TVector3(const TVector3<NumberType>&) = default;
		TVector3(TVector3<NumberType>&&) = default;

		TVector3<NumberType>& operator =(const TVector3<NumberType>&) = default;
		TVector3<NumberType>& operator =(TVector3<NumberType>&&) = default;

	public:
		NumberType X, Y, Z;

	public:
		FASTINLINE bool operator !=(const TVector3<NumberType>& Other) const { return !(*this == Other); }
		FASTINLINE bool operator ==(const TVector3<NumberType>& Other) const {
			return	IsZero(Max(X, Other.X) - Min(X, Other.X)) &&
					IsZero(Max(Y, Other.Y) - Min(Y, Other.Y)) &&
					IsZero(Max(Z, Other.Z) - Min(Z, Other.Z));
		}

		FASTINLINE bool operator <=(const TVector3<NumberType>& Other) const { return GetLengthSquad() <= Other.GetLengthSquad(); }
		FASTINLINE bool operator < (const TVector3<NumberType>& Other) const { return GetLengthSquad() <  Other.GetLengthSquad(); }
		FASTINLINE bool operator >=(const TVector3<NumberType>& Other) const { return GetLengthSquad() >= Other.GetLengthSquad(); }
		FASTINLINE bool operator > (const TVector3<NumberType>& Other) const { return GetLengthSquad() >  Other.GetLengthSquad(); }

		FASTINLINE TVector3<NumberType>& operator +=(const TVector3<NumberType>& Other) { return (*this = (*this + Other)); }
		FASTINLINE TVector3<NumberType> operator +(const TVector3<NumberType>& Other) const {
			return TVector3<NumberType>(X + Other.X, Y + Other.Y, Z + Other.Z);
		}

		FASTINLINE TVector3<NumberType>& operator -=(const TVector3<NumberType>& Other) { return (*this = (*this - Other)); }
		FASTINLINE TVector3<NumberType> operator -(const TVector3<NumberType>& Other) const {
			return TVector3<NumberType>(X - Other.X, Y - Other.Y, Z - Other.Z);
		}

		FASTINLINE TVector3<NumberType>& operator *=(FloatType Scalar) { return (*this = (*this * Scalar)); }
		FASTINLINE TVector3<NumberType> operator *(FloatType Scalar) const {
			return TVector3<NumberType>((NumberType)(X * Scalar), (NumberType)(Y * Scalar), (NumberType)(Z * Scalar));
		}

		FASTINLINE TVector3<NumberType>& operator /=(FloatType Scalar) { return (*this = (*this / Scalar)); }
		FASTINLINE TVector3<NumberType> operator /(FloatType Scalar) const {
			BOX_ASSERT(IsZero(Scalar), "Tried to divide by zero!");
			return TVector3<NumberType>((NumberType)(X / Scalar), (NumberType)(Y / Scalar), (NumberType)(Z / Scalar));
		}

	public:
		FASTINLINE FloatType GetLength() const { return Sqrt(X * X + Y * Y + Z * Z); }
		FASTINLINE FloatType GetLengthSquad() const { return X * X + Y * Y + Z * Z; }

	public:
		FASTINLINE TVector3<NumberType> Normalize() const {
			FloatType Length = GetLength();

			if (IsZero(Length)) {
				return TVector3(NumberType(), NumberType(), NumberType());
			}

			return TVector3(
				(NumberType)(X / Length),
				(NumberType)(Y / Length),
				(NumberType)(Z / Length));
		}

		FASTINLINE TVector3<NumberType> Absolute() const {
			return TVector3<NumberType>(Abs(X), Abs(Y), Abs(Z));
		}

		FASTINLINE FloatType Dot(const TVector3<NumberType>& RightType) const {
			return X * RightType.X + Y * RightType.Y + Z * RightType.Z;
		}

		FASTINLINE TVector3<NumberType> Cross(const TVector3<NumberType>& RightType) const {
			return TVector3<NumberType>(
				(NumberType)(((FloatType)Y) * RightType.Z - ((FloatType)Z) * RightType.Y),
				(NumberType)(((FloatType)Z) * RightType.X - ((FloatType)X) * RightType.Z),
				(NumberType)(((FloatType)X) * RightType.Y - ((FloatType)Y) * RightType.X)
			);
		}
	};

	/*
		2-Dimension Vector.
	*/
	template<typename NumberType = f32>
	class TVector2
	{
	public:
		typedef typename type_db::TSizedFloat<sizeof(NumberType)>::Type FloatType;

	public:
		struct ForwardType { };
		struct BackwardType { };
		struct LeftType { };
		struct RightType { };

	public:
		static constexpr ForwardType Forward = ForwardType();
		static constexpr BackwardType Backward = BackwardType();
		static constexpr LeftType Left = LeftType();
		static constexpr RightType Right = RightType();

	public:
		TVector2(NumberType X = NumberType())
			: X(X), Y(X)
		{
		}
		
		TVector2(NumberType X, NumberType Y)
			: X(X), Y(Y)
		{
		}

		TVector2(ForwardType) : X(0), Y(1) { }
		TVector2(BackwardType) : X(0), Y(-1) { }

		TVector2(LeftType) : X(-1), Y(0) { }
		TVector2(RightType) : X(1), Y(0) { }

		/* Conversion between number-types. */
		template<typename OtherType>
		TVector2(const TVector2<OtherType>& Other) : X((NumberType)Other.X), Y((NumberType)Other.Y) { }

		/* Copy/Move use default. */
		TVector2(const TVector2<NumberType>&) = default;
		TVector2(TVector2<NumberType>&&) = default;

		TVector2<NumberType>& operator =(const TVector2<NumberType>&) = default;
		TVector2<NumberType>& operator =(TVector2<NumberType>&&) = default;

	public:
		NumberType X, Y;

	public:
		FASTINLINE bool operator !=(const TVector2<NumberType>& Other) const { return !(*this == Other); }
		FASTINLINE bool operator ==(const TVector2<NumberType>& Other) const {
			return	IsZero(Max(X, Other.X) - Min(X, Other.X)) &&
					IsZero(Max(Y, Other.Y) - Min(Y, Other.Y));
		}

		FASTINLINE bool operator <=(const TVector2<NumberType>& Other) const { return GetLengthSquad() <= Other.GetLengthSquad(); }
		FASTINLINE bool operator < (const TVector2<NumberType>& Other) const { return GetLengthSquad() < Other.GetLengthSquad(); }
		FASTINLINE bool operator >=(const TVector2<NumberType>& Other) const { return GetLengthSquad() >= Other.GetLengthSquad(); }
		FASTINLINE bool operator > (const TVector2<NumberType>& Other) const { return GetLengthSquad() > Other.GetLengthSquad(); }

		FASTINLINE TVector2<NumberType>& operator +=(const TVector2<NumberType>& Other) { return (*this = (*this + Other)); }
		FASTINLINE TVector2<NumberType> operator +(const TVector2<NumberType>& Other) const {
			return TVector2<NumberType>(X + Other.X, Y + Other.Y);
		}

		FASTINLINE TVector2<NumberType>& operator -=(const TVector2<NumberType>& Other) { return (*this = (*this - Other)); }
		FASTINLINE TVector2<NumberType> operator -(const TVector2<NumberType>& Other) const {
			return TVector2<NumberType>(X - Other.X, Y - Other.Y);
		}

		FASTINLINE TVector2<NumberType>& operator *=(FloatType Scalar) { return (*this = (*this * Scalar)); }
		FASTINLINE TVector2<NumberType> operator *(FloatType Scalar) const {
			return TVector2<NumberType>((NumberType)(X * Scalar), (NumberType)(Y * Scalar));
		}

		FASTINLINE TVector2<NumberType>& operator /=(FloatType Scalar) { return (*this = (*this / Scalar)); }
		FASTINLINE TVector2<NumberType> operator /(FloatType Scalar) const {
			BOX_ASSERT(IsZero(Scalar), "Tried to divide by zero!");
			return TVector2<NumberType>((NumberType)(X / Scalar), (NumberType)(Y / Scalar));
		}

	public:
		FASTINLINE FloatType GetLength() const { return Sqrt(X * X + Y * Y); }
		FASTINLINE FloatType GetLengthSquad() const { return X * X + Y * Y; }

	public:
		FASTINLINE TVector2<NumberType> Normalize() const {
			FloatType Length = GetLength();

			if (IsZero(Length)) {
				return TVector2(NumberType(), NumberType());
			}

			return TVector2(
				(NumberType)(X / Length),
				(NumberType)(Y / Length));
		}

		FASTINLINE TVector2<NumberType> Absolute() const {
			return TVector2<NumberType>(Abs(X), Abs(Y));
		}

		FASTINLINE FloatType Dot(const TVector2<NumberType>& RightType) const {
			return X * RightType.X + Y * RightType.Y;
		}
	};

	template<typename NumberType>
	inline TVector3<NumberType>::TVector3(const TVector2<NumberType>& Vec2, NumberType Z)
		: X(Vec2.X), Y(Vec2.Y), Z(Z)
	{
	}

	typedef TVector2<f32> FVector2;
	typedef TVector3<f32> FVector3;

	typedef TVector2<s32> FPoint2;
	typedef TVector2<u32> FSize2;

}