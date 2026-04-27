cmake_minimum_required(VERSION 3.20)

# Bare wasm32 in the browser: clang + wasm-ld, before project() so CMake picks the right compiler.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR wasm32)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET wasm32-unknown-unknown)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(c_wasm_button C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_EXECUTABLE_SUFFIX .wasm)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")

add_library(
  aulib_wasm STATIC
  aulib-wasm/src/format.c)
target_include_directories(
  aulib_wasm
  PUBLIC "${CMAKE_SOURCE_DIR}/aulib-wasm/include")
target_compile_options(
  aulib_wasm
  PRIVATE -O2
          -std=c11
          -ffreestanding
          -nostdlib
          -fvisibility=hidden
          -fno-builtin)

add_executable(
  app
  src/main.c
  src/counter.c
  src/range.c
  src/ball.c
  src/snake.c)
target_link_libraries(app PRIVATE aulib_wasm)

target_compile_options(
  app
  PRIVATE -O2
          -std=c11
          -ffreestanding
          -nostdlib
          -fvisibility=hidden
          -fno-builtin)

target_link_options(
  app
  PRIVATE
  -nostdlib
  -Wl,--no-entry
  -Wl,--export-dynamic
  -Wl,--export=init
  -Wl,--export-table
  -Wl,--allow-undefined
  -Wl,--initial-memory=131072
  -Wl,--stack-first)

# Only run when `app` is actually linked — avoids wiping dist/ on no-op builds.
add_custom_command(
  TARGET app
  PRE_LINK
  COMMAND "${CMAKE_COMMAND}" -E rm -rf "${CMAKE_SOURCE_DIR}/dist"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/dist"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/public"
          "${CMAKE_SOURCE_DIR}/dist"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_SOURCE_DIR}/runtime/wasm_loader.js"
          "${CMAKE_SOURCE_DIR}/dist/wasm_loader.js"
  COMMENT "Preparing dist/ (clean + public + wasm_loader.js)")

add_custom_command(
  TARGET app
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${CMAKE_SOURCE_DIR}/compile_commands.json"
  COMMENT "compile_commands.json -> project root (clangd / C++ extension)")

install(
  TARGETS aulib_wasm
  ARCHIVE DESTINATION lib
  INCLUDES
  DESTINATION include)
install(
  DIRECTORY "${CMAKE_SOURCE_DIR}/aulib-wasm/include/"
  DESTINATION include)
