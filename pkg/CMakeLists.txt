
# libuv
# NOTE: statically linked uv is called uv_a!
# "uv" is shared version. >:[
set(LIBUV_BUILD_SHARED OFF)
add_subdirectory(uv)

# Zip
add_subdirectory(zip)

# mimalloc
add_subdirectory(mimalloc)

# luau
add_subdirectory(luau)
# use mimalloc allocator
target_link_libraries(Luau.VM PUBLIC mimalloc-static)

# HL2SDK

# Platform Defines
include("platform.cmake")

include_directories(
        sdk/common
        sdk/game/shared
        sdk/game/server
        sdk/public
        sdk/public/engine
        sdk/public/mathlib
        sdk/public/tier0
        sdk/public/tier1
        sdk/public/entity2
        sdk/public/game/server
        sdk/thirdparty/protobuf-3.21.8/src
)

add_library(Sdk STATIC
        sdk/tier1/convar.cpp
        sdk/tier1/generichash.cpp
        sdk/tier1/keyvalues3.cpp
        sdk/entity2/entityidentity.cpp
        sdk/entity2/entitysystem.cpp
        sdk/entity2/entitykeyvalues.cpp
        )

# Metamod
include_directories(
        amtl
        metamod/public
        metamod/core
        metamod/core/sourcehook
)

add_library(Metamod STATIC

        metamod/core/sourcehook/sourcehook.cpp
        metamod/core/sourcehook/sourcehook_impl_chookidman.cpp
        metamod/core/sourcehook/sourcehook_impl_chookmaninfo.cpp
        metamod/core/sourcehook/sourcehook_impl_cvfnptr.cpp
        metamod/core/sourcehook/sourcehook_impl_cproto.cpp
    )



if (LINUX)

    add_library(SdkTier0 SHARED IMPORTED)
    set_target_properties(SdkTier0 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/linux64/libtier0.so)

    add_library(SdkTier1 STATIC IMPORTED)
    set_target_properties(SdkTier1 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/linux64/tier1.a)

    add_library(SdkInterfaces STATIC IMPORTED)
    set_target_properties(SdkInterfaces PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/linux64/interfaces.a)

    add_library(SdkMath STATIC IMPORTED)
    set_target_properties(SdkMath PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/linux64/mathlib.a)

endif()

if (WIN32)

    # Grrr >:[

    add_library(SdkTier0 SHARED IMPORTED)
    set_target_properties(SdkTier0 PROPERTIES IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/tier0.lib)
    set_target_properties(SdkTier0 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/tier0.lib)

    add_library(SdkTier1 STATIC IMPORTED)
    set_target_properties(SdkTier1 PROPERTIES IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/tier1.lib)
    set_target_properties(SdkTier1 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/tier1.lib)

    add_library(SdkInterfaces STATIC IMPORTED)
    set_target_properties(SdkInterfaces PROPERTIES IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/interfaces.lib)
    set_target_properties(SdkInterfaces PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/interfaces.lib)

    add_library(SdkMath STATIC IMPORTED)
    set_target_properties(SdkMath PROPERTIES IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/mathlib.lib)
    set_target_properties(SdkMath PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/public/win64/mathlib.lib)


endif()


target_link_libraries(Sdk
        SdkTier0
        SdkTier1
        SdkInterfaces
        SdkMath
)