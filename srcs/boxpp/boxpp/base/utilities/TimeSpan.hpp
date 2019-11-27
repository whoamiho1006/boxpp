#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <ctime>
#include <string.h>

namespace boxpp
{
	class FTimeSpan
	{
	public:
		FASTINLINE FTimeSpan(s32 Hour = 0, s32 Minutes = 0,
			s32 Seconds = 0, s32 Day = 0, s32 Month = 0, s32 Year = 0)
		{
			::memset(&Time, 0, sizeof(Time));

			Time.tm_hour = Hour;
			Time.tm_min = Minutes;
			Time.tm_sec = Seconds;
			Time.tm_yday = Day;
			Time.tm_mon = Month;
			Time.tm_year = Year;
		}

		/* use default operations for. */
		FTimeSpan(const FTimeSpan&) = default;
		FTimeSpan(FTimeSpan&&) = default;

	public:
		/* use default operations for. */
		FTimeSpan& operator =(const FTimeSpan&) = default;
		FTimeSpan& operator =(FTimeSpan&&) = default;

	public:
		FASTINLINE FTimeSpan operator +(const FTimeSpan& Other) const {
			return FTimeSpan(
				GetHours() + Other.GetHours(),
				GetMinutes() + Other.GetMinutes(),
				GetSeconds() + Other.GetSeconds(),
				GetDays() + Other.GetDays(),
				GetMonths() + Other.GetMonths(),
				GetYears() + Other.GetYears()
			);
		}

		FASTINLINE FTimeSpan operator -(const FTimeSpan& Other) const {
			return FTimeSpan(
				GetHours() - Other.GetHours(),
				GetMinutes() - Other.GetMinutes(),
				GetSeconds() - Other.GetSeconds(),
				GetDays() - Other.GetDays(),
				GetMonths() - Other.GetMonths(),
				GetYears() - Other.GetYears()
			);
		}

		FASTINLINE FTimeSpan& operator +=(const FTimeSpan& Other) {
			return (*this = (*this + Other));
		}

		FASTINLINE FTimeSpan& operator -=(const FTimeSpan& Other) {
			return (*this = (*this - Other));
		}

	private:
		mutable struct tm Time;

	public:
		FASTINLINE struct tm* GetRaw() const { return &Time; }

		FASTINLINE s32 GetYears() const { return Time.tm_year; }
		FASTINLINE s32 GetMonths() const { return Time.tm_mon; }
		FASTINLINE s32 GetDays() const { return Time.tm_mday; }
		FASTINLINE s32 GetHours() const { return Time.tm_hour; }
		FASTINLINE s32 GetMinutes() const { return Time.tm_min; }
		FASTINLINE s32 GetSeconds() const { return Time.tm_sec; }

		FASTINLINE FTimeSpan& SetYears(s32 Years) { Time.tm_year = Years; return *this; }
		FASTINLINE FTimeSpan& SetMonths(s32 Months) { Time.tm_mon = Months; return *this; }
		FASTINLINE FTimeSpan& SetDays(s32 Days) { Time.tm_mday = Days; return *this; }
		FASTINLINE FTimeSpan& SetHours(s32 Hours) { Time.tm_hour = Hours; return *this; }
		FASTINLINE FTimeSpan& SetMinutes(s32 Minutes) { Time.tm_min = Minutes; return *this; }
		FASTINLINE FTimeSpan& SetSeconds(s32 Seconds) { Time.tm_sec = Seconds; return *this; }
	};
}