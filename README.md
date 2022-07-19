# 3DRenderer-Alt

## Dependencies
1. SDL2 (2.0.22 or higher)
2. SDL2_image (2.6.0 or higher)
3. Imgui (1.88 or higher)

## Build Instructions

### Windows
- Keep SDL2 and SDL2_image include files under extern/includes
- Keep SDL2 and SDL2_image lib files under extern/lib/x64
- Get required dll files for SDL2 and SDL2_image

> g++ -o done Source.cpp src/*.cpp extern/src/*.cpp -Iextern/includes -Iincludes -Lextern/lib/x64 -lSDL2main -lSDL2 -lSDL2_image -Wno-return-type

### Linux
> g++ -o done Source.cpp src/*.cpp extern/src/*.cpp -Iextern/includes -Iincludes -lSDL2 -lSDL2_image -Wno-return-type -DSDL_VIDEO_DRIVER_X11 -DSDL_PROTOTYPES_ONLY
