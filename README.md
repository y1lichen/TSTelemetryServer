This fork is based on [OrkenWhite/TSTelemetryServer](https://github.com/OrkenWhite/TSTelemetryServer) and is primarily aimed at building the telemetry server for macOS.
---

## macos build
Since ETS2/ATS runs on Mac via Rosetta, this project must be built on the x86 architecture.

```
mkdir build && cd build
arch -x86_64 cmake -DCMAKE_BUILD_TYPE=Release ..
arch -x86_64 make -j8
```

---

## windows build

```
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
cmake --build . --config Release
```

---
# TSTelemetryServer

This is a plugin for the games *Euro Truck Simulator 2* and *American Truck Simulator* which provides a TCP server on port **3101** and sends telemetry data in JSON to every client.

This plugin works under the **native Linux** builds of the game, **Proton/Wine** and **Windows** as well (macOS unknown).

**This plugin is intended for application developers only!**

## Usage

Put the so/dll file in the bin/*platform*_*arch*/plugins directory of the game. The plugin automatically starts the TCP server on port 3101.

**The plugin only works with ETS2/ATS 1.46 or newer!**

An example telemetry frame can be found in the *example_frame.json* file, you can also consult the *include/telemetry_\** header files for the structure of the JSON output.

Detailed documentation may come later.

## Build

**Remember to recursively clone this repository to get all dependencies!**

The build process has been tested with GCC, Clang and MSVC(under Wine on Linux), the results with other compilers are not known.

### Linux/MinGW
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j8
```

## License

This library is available under the GNU Lesser General Public License, version 3. See the *COPYING* and *COPYING.LESSER* files for details.

## Third-party libraries

The plugin uses SCS Software's telemetry SDK, which is available under the MIT License (see *include/scs_sdk/LICENSE*).

The plugin uses Niels Lohmann's [json library](https://github.com/nlohmann/json), available under the MIT license.
