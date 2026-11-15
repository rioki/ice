# Agent Guide

## Project overview

Ice Engine is a code-first game engine written in C++23. It currently contains
a small shared library, a GoogleTest suite, and a sample application.

## Repository layout

- `ice/`: engine headers and implementation; `ice/ice.h` is the public umbrella header.
- `ice/config.h`: shared-library import/export definitions (`ICE_API`).
- `ice-test/`: GoogleTest tests, named `test-<component>.cpp`.
- `ice-lab/`: sample application for exercising the engine.
- `CMakeLists.txt`: library, test, and sample targets; source files are listed explicitly.
- `CMakePresets.json`: Ninja-based configure, build, and test presets.
- `vcpkg.json`: dependency manifest (currently GoogleTest).
- `build/`: generated, git-ignored build output; do not edit or commit it.

## Build and test

Requirements: CMake 4.2 or newer, Ninja, a C++23-capable compiler, and vcpkg.
Assume vcpkg is installed at `../vcpkg` relative to the repository root.
Set `VCPKG_ROOT` to that installation. The presets use
`$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake` as the toolchain file.
Run commands from the repository root:

```sh
export VCPKG_ROOT="$(cd ../vcpkg && pwd)"
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

The `profile` (RelWithDebInfo) and `release` presets are also available for all
three commands. Outputs are placed under `build/<preset>/`.

To run a focused test after building:

```sh
ctest --preset debug -R 'Engine\.'
```

## Code conventions

- Match surrounding C++ style: four-space indentation and braces on their own lines.
- Keep engine code in namespace `ice`; use PascalCase for types and snake_case
  for functions and variables.
- Use `.h` and `.cpp` files, with `#pragma once` in headers.
- Mark public shared-library classes/functions with `ICE_API`; preserve the
  Windows and non-Windows visibility handling in `ice/config.h`.
- Expose new public headers through `ice/ice.h` when appropriate.
- Preserve existing MIT license headers and follow their format for new source files.
- Add new source and test files to the appropriate target in `CMakeLists.txt`.
- Declare additional external dependencies in `vcpkg.json` and wire them into
  CMake explicitly.

## Validation and scope

- Add or update GoogleTest coverage in `ice-test/` for behavior changes, following
  the existing `TEST(Engine, init)` pattern.
- Build and run relevant tests before reporting a code change as validated.
  If tooling or dependencies prevent validation, state that clearly.
- Keep changes focused; avoid unrelated refactoring or generated-file changes.
