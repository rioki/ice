# Ice Engine

The ice engine is a code first game engine.

## Building

The ice engine uses vcpkg, cmake and ninja.

You should set the variable VCPKG_ROOT the root of you vcpkg installation then run cmake:

```shell
cmake --preset preset
```

To build use cmake:

```shell
cmake --build --preset preset
```

The following presets exist:

| Preset  | Debug Symbols | Optmized |
| ------- | ------------- | -------- |
| debug   | yes           | no       |
| profile | yes           | yes      |
| release | no            | yes      |

The output will then be written to build/<preset>

## License

The ice engine is released under the MIT license, see LICENSE.txt.
