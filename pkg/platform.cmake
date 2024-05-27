
add_definitions(-DMETA_IS_SOURCE2)

if (LINUX)
    add_definitions(-D_LINUX -DPOSIX -DLINUX -DGNUC -DCOMPILER_GCC -DPLATFORM_64BITS)
elseif(WIN32)
    add_definitions(
            -DCOMPILER_MSVC -DCOMPILER_MSVC64 -D_WIN32 -D_WINDOWS -D_ALLOW_KEYWORD_MACROS -D__STDC_LIMIT_MACROS
            -D_CRT_SECURE_NO_WARNINGS=1 -D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NONSTDC_NO_DEPRECATE=1
            -DNOMINMAX
    )
endif()

add_library(AVX INTERFACE IMPORTED)
if(MSVC)
    target_compile_options(AVX INTERFACE /arch:AVX)
else()
    target_compile_options(AVX INTERFACE -mavx)
endif()

# Creates a hotpatchable image.
# This has little impact on performance, and is more about
# alignment & instruction size choice at the top of methods.
add_library(HOTPATCH INTERFACE IMPORTED)
if(MSVC)
#    target_compile_options(HOTPATCH INTERFACE /hotpatch)
    target_link_options(HOTPATCH INTERFACE /functionpadmin:16)
else()
endif()

# Moar debug!
add_library(MOARDEBUG INTERFACE IMPORTED)
if(MSVC)
    # target_compile_options(HOTPATCH INTERFACE /Zo /Zi)
else()
endif()
