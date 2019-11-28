#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/mathematics/Vector.hpp>

namespace boxpp
{
	template<typename NumberType = f32>
	class TRectangle
	{
	public:
		TRectangle() { }
		TRectangle(NumberType X1, NumberType Y1, NumberType X2, NumberType Y2)
			: LeftTop(Min(X1, X2), Min(Y1, Y2)), RightBottom(Max(X1, X2), Max(Y1, Y2))
		{
		}

		TRectangle(const TVector2<NumberType>& A, const TVector2<NumberType>& B)
			: TRectangle(A.X, A.Y, B.X, B.Y)
		{
		}

		/* Conversion between number-types. */
		template<typename OtherType>
		TRectangle(const TRectangle<OtherType>& Other) : TRectangle(Other.LeftTop, Other.RightBottom) { }

		/* Copy/Move use default. */
		TRectangle(const TRectangle<NumberType>&) = default;
		TRectangle(TRectangle<NumberType>&&) = default;

		TRectangle<NumberType>& operator =(const TRectangle<NumberType>&) = default;
		TRectangle<NumberType>& operator =(TRectangle<NumberType>&&) = default;

	public:
		TVector2<NumberType> LeftTop;
		TVector2<NumberType> RightBottom;

	public:
		FASTINLINE bool operator !=(const TRectangle<NumberType>& Other) const { return LeftTop != Other.LeftTop || RightBottom != Other.RightBottom; }
		FASTINLINE bool operator ==(const TRectangle<NumberType>& Other) const { return LeftTop == Other.LeftTop && RightBottom == Other.RightBottom; }

	public:
		FASTINLINE TVector2<NumberType> Center() const { return (LeftTop + RightBottom) * 0.5f; }
		FASTINLINE TVector2<NumberType> LeftBottom() const { return TVector2<NumberType>(LeftTop.X, RightBottom.Y); }
		FASTINLINE TVector2<NumberType> RightTop() const { return TVector2<NumberType>(RightBottom.X, LeftTop.Y); }

	public:
		FASTINLINE bool Intersects(const TVector2<NumberType>& Point) const {
			return LeftTop.X <= Point.X && LeftTop.Y <= Point.Y &&
				RightBottom.X >= Point.X && RightBottom.Y >= Point.Y;
		}

		FASTINLINE bool Intersects(const TRectangle<NumberType>& Other) const {
			return	Intersects(Other.LeftTop) || Intersects(Other.RightBottom) ||
					Intersects(Other.Center()) || Intersects(Other.LeftBottom()) ||
					Intersects(Other.RightTop());
		}
	};

	typedef TRectangle<f32> FRectangle;
}