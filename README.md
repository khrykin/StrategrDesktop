# Strategr
### *No-Fuss Time Management*

Stategr helps you maximize your productivity, giving you the quickest and most effective way to time-box your day with *strategies*.

### More on the [website](https://khrykin.github.io/strategr/)

<img src="https://khrykin.github.io/StrategrDesktop/assets/resources/mac_main_original.png" alt="Screenshot" width="30%">  <img src="https://khrykin.github.io/StrategrDesktop/assets/resources/mac_black_main.png" alt="Screenshot" width="30%"> <img src="https://khrykin.github.io/StrategrDesktop/assets/resources/win_main.png" alt="Screenshot" width="30%">

## Tech

Strategr's data model is built with vanilla C++ 17, UI layer is Qt 5.

## Platforms

- [x] macOS
- [x] Windows
- [ ] iOS
- [ ] Linux
- [ ] Android

## Building

Things are being reworked at the moment, and building is currently only supported on macOS. It may
build on Windows & Linux with a fix or two though.

Dependencies are managed via [vcpkg](https://vcpkg.io/en/). Run once:

```shell
./scripts/setup_vcpkg.sh
```

Build the app target:
```
cd desktop
cmake --preset macos
cmake --build --preset default -t Strategr
```

## Contributing

PR's or any help and suggestions are appreciated!
