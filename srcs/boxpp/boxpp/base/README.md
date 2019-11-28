# Base library for entire framework.
### DO NOT make any reference to outside on BASE library.
Base-library can be uniquely included by other pure-utilities also.

	Library Dependencies:
		1. Windows: Nothing yet.
		2. Linux: link with -lpthread -ldl.

## 1. BaseMacros.hpp
This header stabilize the preprocessor environment for framework codes and, 
provides preprocessor branches for multi-platforming.

### For identifying operating system.
	* PLATFORM_WINDOWS (0/1)
	* PLATFORM_LINUX (0/1)
	* PLATFORM_UNIX (0/1)
	* PLATFORM_APPLE (0/1)
	* PLATFORM_POSIX (0/1)
	* PLATFORM_ANDROID (0/1)
	
### For identifying processor types.
	* PLATFORM_BITNESS (32/64)
	* PLATFORM_LITTLE_ENDIAN (0/1)
	* PLATFORM_BIG_ENDIAN (0/1)

	** NOTE: Currently, Intel X86, X86-64, AMD64 only supported.
	
### For wide-charactors.
	* PLATFORM_UNICODE (0/1) - Automatically intergrated if on MSVC. (WANNA_UNICODE required for others)
	* PLATFORM_NATIVE_WCHAR (0/1) - about native wchar_t support.

### For identifying IDE and debug-mode build.
	* BOX_NOT_COMPILED - this will indicate under IDE or not. (like intellisense on MSVC)
	* BOX_DEBUG - Controlled by _DEBUG preprocessor that defined by command-line.

### Exposure of API.
	* BOXEXPORT - __declspec(dllexport) for windows, empty for others.
	* BOXIMPORT - __declspec(dllimport) for windows, empty for others.

### boxpp.dll(.so) only APIs.
	* BOX_COMPILE_BODY (0/1)
	* BOXPP - BOXEXPORT for inner, BOXIMPORT for outers.

### Output type specs.
	* BOX_COMPILE_EXECUTABLE (0/1)
	* BOX_COMPILE_SHARED_LIB (0/1) - requires __BOX_SHARED__ under non-MSVC.
	* BOX_COMPILE_STATIC_LIB (0/1) - requires __BOX_STATIC__ under non-MSVC.

### Alignment controls
	* NO_PADDING(...) - e.g. NO_PADDING(struct Something { ... }); for alignment=1 byte.
	* BOXALIGN(x) - alignas(x).

### Calling conventions (only for windows)
	* BOX_STDCALL - __stdcall or __attribute__ ((stdcall))

### TCHAR like:
	* BOXTEXT(...) - PLATFORM_UNICODE == 1 ? L"blahblah" : "blahblah"

### File extensions:
	* PLATFORM_EXT_SHARED - dll for windows, so for unix kinds, dylib for apple.
	* PLATFORM_EXT_STATIC - lib for windows, a for others.

### etc...
	* FASTINLINE - __forceinline (MSVC) and __inline__ (GCC)
	* BOXEXTERN	- extern keyword.
	* NO_MANGLED - extern "C"

## 2. BaseTypes.hpp
### Primitive types.
	* signed: s8, s16, s32, s64
	* unsigned: u8, u16, u32, u64
	* floating point: f32, f64
	* size-types: size_t, ssize_t (for offset)
	* char-types: ansi_t (for MBS), wide_t (for WCS)

### Numeric limits.
type_db::TNumberLimits<...>::{Min, Max, Tolerant, Epsilon}. 
(note: this will be moved to TLimit)

### Size to suitable integer.
type_db::TSizedInt<Size>::{Signed, Unsigned}.
(note: this will be moved to TSizedInt)

### Endian type (for template-specialization purpose)
```
	enum class EEndianType {
		Unknown = 0,/* Unknown, undefined. */
		Little,		/* Little endian. */
		Big,		/* Big endian. */

		/* Compile-time system endian. */
		System = (PLATFORM_BIG_ENDIAN ? Big :
				 (PLATFORM_LITTLE_ENDIAN ? Little : Unknown))
	};
```

### Platform type (for template-specialization purpose)
```
	enum class EPlatformType {
		Undefined = 0,/* Undefined system. */
		Windows,	/* Windows. */
		Linux,		/* Linux. (android also linux but, this is not for android) */
		Android,	/* Android. */
		Apple,		/* Apple kind. (iPhone, iMac, ...) */
		Unix,		/* Unix kind. (commonly known unix like FreeBSD, ...) */

		/* Compile-time system platform. */
		System = (PLATFORM_WINDOWS ? Windows : (PLATFORM_ANDROID ? Android :
				(PLATFORM_LINUX ? Linux : (PLATFORM_APPLE ? Apple :
				(PLATFORM_UNIX ? Unix : Undefined)))))
	};
```

# 3. Template library (tpls directory)
	* tpls/BaseTraits.hpp : Base-types for type trait.
	* tpls/traits/*.hpp : Type-trait related templates.
	* tpls/shareds/*.hpp : Smart-pointer.
	* tpls/strings/*.hpp : String templates.
	* tpls/containers/*.hpp : Container templates.

# 4. Opacity wrappers for windows and posix.
	* opacities/posix.hpp : POSIX API autocompletion support for MSVC.
	* opacities/windows.hpp : Windows API Opacity declarations.
	* opacities/vsprintf.hpp : vsprintf series template specializations.

# 5. Boilerplate.
	* boilerplates/Memory.hpp : Memory boilerplate. (for unifying the heap where memory is allocated)

# 6. Debugging support and Minidump, barrior (mutex like), tls, event object, threaded.
	* systems/Debugger{.hpp, .cpp}:
		- BOX_ASSERT(Expr, Msg): Test expression and abort execution.
		- BOX_ENSURE(Expr): Test expression and break debugger.
		- BOX_BREAK(): Break debugger.

	* systems/Minidump.cpp: (currently works only for windows) Creates a DMP file when exception occurred.
	* systems/Barrior.hpp
	* systems/RawEvent.hpp
	* systems/RawTls.hpp
	* systems/Threaded.hpp

# 7. Utilities.
	* FDateTime, FTimeSpan - standard wrapper.
	* FName

