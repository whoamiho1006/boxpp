#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/core/Barrior.hpp>
#include <boxpp/containers/Array.hpp>

namespace boxpp
{
	enum class ELog
	{
		Note,
		Warning,
		Error,
		Fatal
	};

	class ILogging;
	class BOXPP FLogging
	{
	private:
		FLogging() { }
		~FLogging() { }

	public:
		static FLogging& Get();

	private:
		FBarrior Barrior;
		char_t Buffer[16384];
		TArray<ILogging*> Loggers;
		bool bUnderCLI;

	public:
		void WriteFormat(ELog LogLevel,
			const char_t* File, s32 Line,
			const char_t* Category, 
			const char_t* FormatString,
			...);

		void Write(ELog LogLevel,
			const char_t* File, s32 Line,
			const char_t* Category,
			const char_t* Message);

	public:
		FASTINLINE bool IsUnderCLI() const { return PLATFORM_WINDOWS && bUnderCLI; }
		FASTINLINE void Register(ILogging* Logger)
		{
			FBarriorScope Guard(Barrior);
			Loggers.Add(Logger);
		}

		FASTINLINE void Unregister(ILogging* Logger)
		{
			FBarriorScope Guard(Barrior);
			Loggers.Remove(Logger);
		}

	public:
		/* Internal use- only!*/
		FASTINLINE void SetUnderCLI(bool Value) { bUnderCLI = Value; }
	};

	/* Log category for CORE. */
	constexpr const char_t* LogCore = BOXTEXT("Core");

	/* Log category for Engine. */
	constexpr const char_t* LogEngine = BOXTEXT("Engine");

	namespace _ {
		template<typename CharType>
		struct TBypass
		{
			FASTINLINE static const CharType* Select(const ansi_t* A, const wide_t* B)
			{
				return nullptr;
			}
		};

		template<>
		struct TBypass<ansi_t>
		{
			FASTINLINE static const ansi_t* Select(const ansi_t* A, const wide_t* B)
			{
				return A;
			}
		};

		template<>
		struct TBypass<wide_t>
		{
			FASTINLINE static const wide_t* Select(const ansi_t* A, const wide_t* B)
			{
				return B;
			}
		};
	}

#define BX_LOG(Verbosity, Category, Format, ...) \
	boxpp::FLogging::Get().WriteFormat(boxpp::ELog::Verbosity, \
		boxpp::_::TBypass<boxpp::char_t>::Select(__FILE__, __FILEW__), \
		 __LINE__, Category, Format, __VA_ARGS__)

	class ILogging
	{
	public:
		virtual ~ILogging() { }

	public:
		virtual void Write(ELog LogLevel,
			const char_t* Category, const char_t* Message) = 0;
	};

}