#pragma once
#include <boxpp/Normalize.hpp>
#include <boxpp/BaseTypes.hpp>

#define BOX_CONCAT_2ND(Front, Back)	Front ## Back
#define BOX_CONCAT(Front, Back)		BOX_CONCAT_2ND(Front, Back)

// Version header. (pre-include for getting version information only)
#define __BOXPP_VERSION_MACRO_ONLY
#include <boxpp/Version.hpp>

#include <new>