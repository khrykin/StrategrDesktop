# Strategr
### *No-Fuss Time Management*

Stategr helps you maximize your productivity, giving you the quickest and most effective way to time-box your day with *strategies*.

### More on the [website](https://khrykin.github.io/strategr/)

<img src="https://khrykin.github.io/strategr/resources/mac_main_original.png" alt="Screenshot" width="250">  <img src="https://khrykin.github.io/strategr/resources/mac_black_main.png" alt="Screenshot" width="250"> <img src="https://khrykin.github.io/strategr/resources/win_main.png" alt="Screenshot" width="250">



## Tech
Strategr's data model is built with vanilla C++ 17, UI layer is Qt 5.

## Platforms
- [x] macOS
- [x] Windows
- [ ] iOS
- [ ] Linux
- [ ] Android

## Build Requirements
 - C++17-compatible C++ compliler
 - CMake >=3.14
 - Boost >= 1.7
 - Qt >= 5.14
 - [utf8proc](https://github.com/JuliaStrings/utf8proc) >= v2.4.0 - Note that this must be built as a static library
 - [Sparkle](https://sparkle-project.org/) (macOS only)
 - [WinSparkle](https://winsparkle.org/) (Windows only)
 
## Building 
Given that current directory is the root of this repository, and all dependencies are avaliable for CMake, you can build Strategr like this:
```
mkdir build && cd build
cmake ../
cmake --build . --target Strategr
```

## Contributing
PR's or any help and suggestions are appreciated!
