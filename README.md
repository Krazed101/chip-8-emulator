## CHIP-8 Emulator

This is my rendition of a Chip 8 emulator written in C. For graphics I'm using SDL2 with the True Types Font Libraries included.

| File     | Description                                             |
|----------|---------------------------------------------------------|
| .vscode  | The vscode files for debuging and building the program  |
| build    | Both x86 and eventually x64 builds                      |
| external | The external dependencies for SDL2                      |
| Scripts  | Scripts used in the build process                       |
| chip8.c  | The main chip8 emulator code                            |
| chip8.h  | The header file for the chip8 emulator                  |
| main.c   | The main loop and SDL2 graphics drawing                 |


### TODO:
- [ ] Add more debuging features
    - [ ] Show registers and values
    - [ ] Allowing steping through process on key press
    - [ ] On\Off key for debug mode when running

- [ ] Add a way to load a .ch8 at runtime
- [ ] Add a way to change the font at runtime
- [ ] Add a way to change the two colors of the screen at runtime
- [ ] Add a way to adjust the overall clock speed at runtime
