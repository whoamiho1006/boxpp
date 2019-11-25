#pragma once

#define BOXPP_VERSION_MAJOR		1
#define BOXPP_VERSION_MINOR		0
#define BOXPP_VERSION_BUILD		0

#define BOXPP_VERSION_STRING \
	BOX_TOSTR(BOXPP_VERSION_MAJOR) "." \
	BOX_TOSTR(BOXPP_VERSION_MINOR)

#define BOXPP_VERSION_FULL_STRING \
	BOX_TOSTR(BOXPP_VERSION_MAJOR) "." \
	BOX_TOSTR(BOXPP_VERSION_MINOR) "." \
	BOX_TOSTR(BOXPP_VERSION_BUILD)

constexpr const char* pp = BOXPP_VERSION_STRING;

namespace boxpp
{
	/* Version information. */
	struct SVersion {

		/* constructor. */
		constexpr SVersion(u8 Major = 0, u8 Minor = 0, u16 Build = 0)
			: Major(Major), Minor(Minor), Build(Build) { }

		u8 Major;
		u8 Minor;
		u16 Build;

		/* Pack version to single u32. */
		constexpr u32 Serialize(bool bWithBuild = false) const {
			return bWithBuild ?
				(u32(Major) << 24) | (u32(Minor) << 16) | (u32(Build) << 0) :
				(u32(Major) << 24) | (u32(Minor) << 16);
		}

		/* Unpack version from single u32. */
		static constexpr SVersion Deserialize(const u32 Version) {
			return SVersion(
				(Version >> 24) & 0xff,
				(Version >> 16) & 0xff,
				 Version & 0xff);
		}

		/*
			Compare two version.
			< 0 : This < Other.
			= 0 : This = Other.
			> 0 : This > Other.
		*/
		constexpr int Compare(const SVersion& Other, bool bWithBuild = false) const {
			return Serialize(bWithBuild) - Other.Serialize(bWithBuild);
		}

		/* comparators (only major, minor) */
		constexpr bool operator ==(const SVersion& Other) const { return !Compare(Other); }
		constexpr bool operator !=(const SVersion& Other) const { return Compare(Other); }
		constexpr bool operator <=(const SVersion& Other) const { return Compare(Other) <= 0; }
		constexpr bool operator >=(const SVersion& Other) const { return Compare(Other) >= 0; }
		constexpr bool operator < (const SVersion& Other) const { return Compare(Other) < 0; }
		constexpr bool operator > (const SVersion& Other) const { return Compare(Other) > 0; }
	};
}