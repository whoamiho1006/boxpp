#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <ctime>
#include <string.h>

#include <boxpp/base/utilities/TimeSpan.hpp>

namespace boxpp
{
	class FDateTime
	{
	private:
		struct NowType { };
		struct TodayType { };

	public:
		FASTINLINE static FDateTime Now() {
			return FDateTime(NowType());
		}

		FASTINLINE static FDateTime Today() {
			return FDateTime(TodayType());
		}

		FASTINLINE static FDateTime Make(s32 Hour, s32 Minutes, s32 Seconds, s32 Day, s32 Month, s32 Year)
		{
			FDateTime DateTime;

			DateTime.Time.tm_hour = Hour;
			DateTime.Time.tm_min = Minutes;
			DateTime.Time.tm_sec = Seconds;
			DateTime.Time.tm_yday = Day;
			DateTime.Time.tm_mon = Month;
			DateTime.Time.tm_year = (Year - 1900) < 0 ? 0 : Year - 1900;

			return DateTime;
		}

	public:
		FASTINLINE FDateTime()
		{
			::memset(&Time, 0, sizeof(Time));
		}

		FASTINLINE FDateTime(time_t Timestamp)
		{
			Time = *localtime(&Timestamp);
		}
		
		/* use default operations for. */
		FDateTime(const FDateTime&) = default;
		FDateTime(FDateTime&&) = default;

	private:
		FASTINLINE FDateTime(NowType)
		{
			time_t Timestamp = time(0);
			Time = *localtime(&Timestamp);
		}

		FASTINLINE FDateTime(TodayType)
		{
			time_t Timestamp = time(0);
			Time = *localtime(&Timestamp);

			// Set HMS to zero for today.
			Time.tm_hour = Time.tm_min = Time.tm_sec = 0;
		}

	public:
		/* use default operations for. */
		FDateTime& operator =(const FDateTime&) = default;
		FDateTime& operator =(FDateTime&&) = default;

	public:
		FASTINLINE bool operator ==(const FDateTime& Right) const { return GetRawTime() - Right.GetRawTime() == 0; }
		FASTINLINE bool operator !=(const FDateTime& Right) const { return GetRawTime() - Right.GetRawTime() != 0; }
		FASTINLINE bool operator >=(const FDateTime& Right) const { return GetRawTime() - Right.GetRawTime() >= 0; }
		FASTINLINE bool operator > (const FDateTime& Right) const { return GetRawTime() - Right.GetRawTime() >  0; }
		FASTINLINE bool operator <=(const FDateTime& Right) const { return GetRawTime() - Right.GetRawTime() <= 0; }
		FASTINLINE bool operator < (const FDateTime& Right) const { return GetRawTime() - Right.GetRawTime() <  0; }

	public:
		FASTINLINE FDateTime operator +(const FTimeSpan& Span) const {
			FDateTime Time = *this;

			struct tm& Left = Time.Time;
			struct tm& Right = *Span.GetRaw();

			Left.tm_hour += Right.tm_hour;
			Left.tm_min += Right.tm_min;
			Left.tm_sec += Right.tm_sec;
			Left.tm_yday += Right.tm_yday;
			Left.tm_mon += Right.tm_mon;
			Left.tm_year += Right.tm_year;

			Time.SetYear(Left.tm_year);
			return Time;
		}

		FASTINLINE FDateTime operator -(const FTimeSpan& Span) const {
			FDateTime Time = *this;

			struct tm& Left = Time.Time;
			struct tm& Right = *Span.GetRaw();

			Left.tm_hour -= Right.tm_hour;
			Left.tm_min -= Right.tm_min;
			Left.tm_sec -= Right.tm_sec;
			Left.tm_yday -= Right.tm_yday;
			Left.tm_mon -= Right.tm_mon;
			Left.tm_year -= Right.tm_year;

			Time.SetYear(Left.tm_year);
			return Time;
		}

		FASTINLINE FTimeSpan operator -(const FDateTime& Right) const {
			FTimeSpan Span;

			Span.SetYears(GetYear() - Right.GetYear());
			Span.SetMonths(GetMonth() - Right.GetMonth());
			Span.SetDays(GetDay() - Right.GetDay());
			Span.SetHours(GetHour() - Right.GetHour());
			Span.SetMinutes(GetMinutes() - Right.GetMinutes());
			Span.SetSeconds(GetSeconds() - Right.GetSeconds());

			return Span;
		}

		FASTINLINE FDateTime& operator +=(const FTimeSpan& Span) {
			return (*this = (*this + Span));
		}

		FASTINLINE FDateTime& operator -=(const FTimeSpan& Span) {
			return (*this = (*this - Span));
		}

	private:
		mutable struct tm Time;

	public:
		FASTINLINE struct tm* GetRaw() const { return &Time; }
		FASTINLINE time_t GetRawTime() const { return mktime(&Time); }

		/* Year, 1900 - ... */
		FASTINLINE s32 GetYear() const { return Time.tm_year + 1900; }

		/* Month, 1 - 12. */
		FASTINLINE s32 GetMonth() const { return Time.tm_mon + 1; }

		/* Month day, 1 - 31. */
		FASTINLINE s32 GetDay() const { return Time.tm_mday; }

		/* Week day, 0 - 6. */
		FASTINLINE s32 GetWeekDay() const { return Time.tm_wday; }

		/* Year day, 0 (jan 1st) - ... */
		FASTINLINE s32 GetYearDay() const { return Time.tm_yday; }

		/* Hour, 0 - 24. */
		FASTINLINE s32 GetHour() const { return Time.tm_hour; }

		/* Minutes, 0 - 59. */
		FASTINLINE s32 GetMinutes() const { return Time.tm_min; }

		/* Seconds, 0 - 61. */
		FASTINLINE s32 GetSeconds() const { return Time.tm_sec; }

		/* Year, 1900 - ... */
		FASTINLINE FDateTime& SetYear(s32 Year)
		{
			time_t Timestamp;
			
			Time.tm_year = (Year - 1900) < 0 ? 0 : Year - 1900;
			Timestamp = mktime(&Time);

			return (*this = Timestamp);
		}

		/* Month, 1 - 12. */
		FASTINLINE FDateTime& SetMonth(s32 Month)
		{
			time_t Timestamp;

			Time.tm_mon = Month;
			Timestamp = mktime(&Time);

			return (*this = Timestamp);
		}

		/* Month day, 1 - 31. */
		FASTINLINE FDateTime& SetDay(s32 Day)
		{
			time_t Timestamp;

			Time.tm_mday = Day;
			Timestamp = mktime(&Time);

			return (*this = Timestamp);
		}

		/* Hour, 0 - 24. */
		FASTINLINE FDateTime& SetHour(s32 Hour)
		{
			time_t Timestamp;

			Time.tm_hour = Hour;
			Timestamp = mktime(&Time);

			return (*this = Timestamp);
		}

		/* Minutes, 0 - 59. */
		FASTINLINE FDateTime& SetMinutes(s32 Minutes)
		{
			time_t Timestamp;

			Time.tm_min = Minutes;
			Timestamp = mktime(&Time);

			return (*this = Timestamp);
		}

		/* Seconds, 0 - 61. */
		FASTINLINE FDateTime& SetSeconds(s32 Seconds)
		{
			time_t Timestamp;

			Time.tm_sec = Seconds;
			Timestamp = mktime(&Time);

			return (*this = Timestamp);
		}
	};

}