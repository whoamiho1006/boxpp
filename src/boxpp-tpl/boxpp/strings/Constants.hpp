#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/strings/UtfTypes.hpp>

namespace boxpp
{
	namespace strings {

		/* All C* Series have same code position for bellow characters. */
		template<typename CharType, bool IsUtfSeries = IsUtfType<CharType>>
		struct TConstants {
			static constexpr const CharType Null = '\0';

			static constexpr const CharType Space = ' ';
			static constexpr const CharType Tab = '\t';
			static constexpr const CharType Backspace = '\b';
			static constexpr const CharType LineFeed = '\n';
			static constexpr const CharType CarrageReturn = '\r';
			static constexpr const CharType Asteroid = '*';
			static constexpr const CharType Slash = '/';
			static constexpr const CharType Backslash = '\\';

			static constexpr const CharType Hipen = '-';
			static constexpr const CharType Underbar = '_';
			static constexpr const CharType At = '@';
			static constexpr const CharType Sharp = '#';
			static constexpr const CharType Question = '?';
			static constexpr const CharType Less = '<';
			static constexpr const CharType Greater = '>';

			static constexpr const CharType Collon = ':';
			static constexpr const CharType Semicollon = ';';

			static constexpr const CharType Alphabet(size_t Offset) { return "abcdefghijklmnopqrstuvwxyz"[Offset]; }
			static constexpr const CharType AlphabetUpper(size_t Offset) { return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[Offset]; }
			static constexpr const CharType Number(size_t Offset) { return "0123456789"[Offset]; }
		};

		template<typename CharType>
		struct TConstants<CharType, true> {
			static constexpr const CharType Null = { '\0' };

			static constexpr const CharType Space = { ' ' };
			static constexpr const CharType Tab = { '\t' };
			static constexpr const CharType Backspace = { '\b' };
			static constexpr const CharType LineFeed = { '\n' };
			static constexpr const CharType CarrageReturn = { '\r' };
			static constexpr const CharType Asteroid = { '*' };
			static constexpr const CharType Slash = { '/' };
			static constexpr const CharType Backslash = { '\\' };

			static constexpr const CharType Hipen = { '-' };
			static constexpr const CharType Underbar = { '_' };
			static constexpr const CharType At = { '@' };
			static constexpr const CharType Sharp = { '#' };
			static constexpr const CharType Question = { '?' };
			static constexpr const CharType Less = { '<' };
			static constexpr const CharType Greater = { '>' };

			static constexpr const CharType Collon = { ':' };
			static constexpr const CharType Semicollon = { ';' };

			static constexpr const CharType Alphabet(size_t Offset) { return { "abcdefghijklmnopqrstuvwxyz"[Offset] }; }
			static constexpr const CharType AlphabetUpper(size_t Offset) { return { "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[Offset] }; }
			static constexpr const CharType Number(size_t Offset) { return { "0123456789"[Offset] }; }
		};
	}
}