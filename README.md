# CHIP8 Emulator
CHIP8 is an interpreted programming language and a virtual machine for RCA 1802 based machines, designed for writing simple video games.
It has a total of 35 instructions.
For more information (contains the specifications and virtual machine description):
[Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

## Screenshots
- Space Invaders
  ![image](https://github.com/user-attachments/assets/3fec2251-4aba-4e92-9b32-a7e3c81c7fc6)

- Pong
  ![image](https://github.com/user-attachments/assets/9e2b22f0-48c4-4ab1-b763-1f46ec24cd75)

- Blinky
  ![image](https://github.com/user-attachments/assets/a692cec6-b9e2-4a5d-899c-9489c0de271b)

   

## Installation (*nix)

Pre-requisites: make (for compilation) and SDL2 (for graphics)
```
$ sudo apt-get update
$ sudo apt-get install make libsdl2-dev
```

clone this repo, then build the project using:
```
$ make /path/to/chip8
```

You can start using the emulator:
```
# a few ROMS have been added with the installation.
$ ./chip8 /path/to/rom

# to know more
$ ./chip8 -help
```

## References
1. [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
2. [Chip8 Emulator by sarbajitsaha](https://github.com/sarbajitsaha/Chip-8-Emulator)
