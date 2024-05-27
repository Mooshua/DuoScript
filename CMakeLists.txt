cmake_minimum_required(VERSION 3.26)
project(DuoScript C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory(pkg)

function(create_zip output_file input_files working_dir)
    add_custom_command(
            COMMAND ${CMAKE_COMMAND} -E tar "cf" "${output_file}" --format=zip -- ${input_files}
            WORKING_DIRECTORY "${working_dir}"
            OUTPUT  "${output_file}"
            DEPENDS ${input_files}
            COMMENT "Zipping to ${output_file}."
    )
endfunction()

# =========
# Protobufs
# =========

if (LINUX)
    set(PROTOC_EXECUTABLE devtools/bin/linux/protoc)
elseif(WIN32)
    set(PROTOC_EXECUTABLE devtools/bin/protoc.exe)
endif()

add_custom_command(
        OUTPUT protobuf_output_stamp
        COMMAND ${PROTOC_EXECUTABLE} --proto_path=thirdparty/protobuf-3.21.8/src --proto_path=common --cpp_out=common common/network_connection.proto
        COMMENT "Generating protobuf file"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pkg/sdk
        VERBATIM
)

add_library(Protobufs STATIC
        pkg/sdk/common/network_connection.pb.cc)
set_target_properties(Protobufs PROPERTIES LINKER_LANGUAGE CXX)

# =========
# DuoScript
# =========

# Platform Defines
include("pkg/platform.cmake")

# Libuv, AMTL, Luau
include_directories(
        pkg/mimalloc/include

        pkg/uv/include

        pkg/luau/Config/include
        pkg/luau/CodeGen/include
        pkg/luau/VM/include
        pkg/luau/Compiler/include
        pkg/luau/Analysis/include
        pkg/luau/Ast/include

        pkg/amtl

        pkg/zip/src
)


include_directories(public src)

#   ===========================================
#   Duo Core
#       Module for core implementation (on CS2)
#   ===========================================

set (duo_core_sources
        src/Core/Script/ScriptVM.cpp
        src/Core/Script/ScriptCall.cpp
        src/Core/Script/ScriptHandles.cpp
        src/Core/Script/ScriptFiber.cpp
        src/Core/Script/ScriptObject.cpp
        src/Core/Script/ScriptControllers.cpp
        src/Core/Script/ScriptControllerEvents.cpp

        src/Core/Logging/Log.cpp

        src/Core/Modules/ControllerManagerSink.cpp

        src/Core/Loop/Loop.cpp
        src/Core/Loop/AsyncHandle.cpp
        src/Core/Loop/DelayHandle.cpp
        src/Core/Loop/WorkHandle.cpp

        src/Core/Plugins/PluginManager.cpp
        src/Core/Plugins/Plugin.cpp
        src/Core/Files/Files.cpp
        src/Core/Modules/ModuleManager.cpp
        src/Core/Modules/ModuleInstance.cpp
)

add_library(duo_core STATIC
    ${duo_core_sources}
)

#   Some code doesn't support running under both source-engine
#   and independently, so we build a separate lib with a compiler
#   define so they can sort their problems out.
add_library(duo_core_independent STATIC
        ${duo_core_sources}
)

target_include_directories(duo_core PRIVATE src/Core)
target_include_directories(duo_core_independent PRIVATE src/Core)

target_compile_definitions(duo_core PRIVATE DUO_DEBUG COMPILING_DUOSCRIPT)
target_compile_definitions(duo_core_independent PUBLIC DUO_DEBUG COMPILING_DUOSCRIPT DUO_INDEPENDENT)

target_link_libraries(duo_core uv_a mimalloc-static zip)
target_link_libraries(duo_core Luau.VM Luau.Compiler Luau.CodeGen)

target_link_libraries(duo_core_independent uv_a mimalloc-static zip)
target_link_libraries(duo_core_independent Luau.VM Luau.Compiler Luau.CodeGen)

target_link_libraries(duo_core AVX HOTPATCH MOARDEBUG)



#   ===
#   Duo Metamod
#

# Metamod
include_directories(
        pkg/metamod/core
        pkg/metamod/core/sourcehook
)

# HL2SDK
include_directories(
        pkg/sdk/common
        pkg/sdk/game/shared
        pkg/sdk/game/server
        pkg/sdk/public
        pkg/sdk/public/engine
        pkg/sdk/public/mathlib
        pkg/sdk/public/tier0
        pkg/sdk/public/tier1
        pkg/sdk/public/entity2
        pkg/sdk/public/game/server
        pkg/sdk/thirdparty/protobuf-3.21.8/src
)

add_library(duo SHARED
        src/Metamod/Duo.cpp
        src/Metamod/DuoMetamod.cpp
)

target_include_directories(duo PRIVATE src/Core)

target_link_libraries(duo Metamod Protobufs Sdk)
target_link_libraries(duo duo_core)

#   ==============================================
#   Duo Logic
#       Completely non-game/engineless controllers
#   ==============================================

set(duo_logic_sources
        src/Logic/Threading/FiberController.cpp

        src/Logic/Files/ZipController.cpp
        src/Logic/Files/FileSystemController.cpp

        src/Logic/Generic/PerformanceController.cpp
        src/Logic/Generic/LogController.cpp


)

add_library(duo_logic SHARED
        src/Logic/DuoModule.cpp

        ${duo_logic_sources}
)

target_include_directories(duo_logic PRIVATE src/Logic)

# TODO: Uncomment me when NO_HL2SDK is fixed!
# target_link_libraries(duo_logic MetamodAbstract)
target_link_libraries(duo_logic Protobufs Sdk Metamod)
target_link_libraries(duo_logic mimalloc-static uv_a zip)


#   =============================================
#   Duo Cstrike
#       Module for Counter-Strike 2 functionality
#   =============================================

add_library(duo_cstrike SHARED
    src/Cstrike/DuoModule.cpp
)

target_include_directories(duo_cstrike PRIVATE src/Cstrike)


target_link_libraries(duo_cstrike Protobufs Sdk Metamod)
target_link_libraries(duo_cstrike mimalloc-static)

#   =============================================
#   Duo Source 2
#       Module for general Source 2 functionality
#   =============================================

add_library(duo_source2 SHARED
        src/Engine/DuoModule.cpp
)

target_include_directories(duo_source2 PRIVATE src/Engine)


target_link_libraries(duo_source2 Protobufs Sdk Metamod)
target_link_libraries(duo_source2 mimalloc-static)

#   ==============================
#   Duo Sdk
#       Build tools & more for Duo
#   ==============================

add_executable(duokit
        src/Sdk/Main.cpp

        ${duo_logic_sources}
        src/Sdk/Controllers/LuauCompilerController.cpp
        src/Sdk/Controllers/LuauCompilerController.h
        src/Sdk/Controllers/LuauFrontend.cpp
        src/Sdk/Controllers/LuauFrontend.h
        src/Sdk/Controllers/LuauCheckResult.cpp
        src/Sdk/Controllers/LuauCheckResult.h
)


file(GLOB duokit_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/Sdk/Provider/*")
create_zip("${CMAKE_CURRENT_BINARY_DIR}/duokit.duo" "${duokit_sources}" "${CMAKE_CURRENT_SOURCE_DIR}/src/Sdk/Provider")
add_custom_target(duokit_provider ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/duokit.duo")

add_dependencies(duokit duokit_provider)

target_include_directories(duokit PRIVATE src/Core src/Logic)
target_compile_definitions(duokit PUBLIC DUO_INDEPENDENT)
target_link_libraries(duokit uv_a mimalloc-static zip)
target_link_libraries(duokit Luau.VM Luau.Compiler Luau.CodeGen Luau.Analysis)
target_link_libraries(duokit duo_core_independent)

#   ================================================
#   Duo All
#       Build target to create the entire repository
#   ================================================

add_custom_target(duo_all)
add_dependencies(duo_all duokit duo_core duo duo_cstrike duo_logic duo_source2)