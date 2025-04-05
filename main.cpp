/*

    The main driver program for the chip8 emulator

    Takes "program.ch8" and runs it in the game loop

*/

#include <iostream>
#include <string>
#include <cstring>
#include "chip8.h"
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_events.h>
#include <SDL.h>

using namespace std;

CHIP8 chip8_instance;

enum MACHINESTATE {ON, READY, RUN, STOP, OFF, UNDEF};

void parse_commands(int argc, char* argv[]){
    if(argc < 2) {
        cout<<"usage: ./chip8 <rom>.ch8 <-options[hvac]>";
        exit(0);
    }

    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "help") == 0) {
        cout<<"usage: ./chip8 <rom>.ch8 <options>";
        cout<<"options:";
        cout<<"\t-h : shows this message.";
        cout<<"\t-v : verbose mode, shows interal trace.";
        cout<<"\t-a : disables audio.";
        cout<<"\t-c : displays controls.";
        exit(0);
    }

    if(argv[2][0] == '-'){
        if(argv[2].contains("h")){
            cout<<"usage: ./chip8 <rom>.ch8 <options>";
            cout<<"options:";
            cout<<"\t-h : shows this message.";
            cout<<"\t-v : verbose mode, shows interal trace.";
            cout<<"\t-a : disables audio.";
            cout<<"\t-c : displays controls.";
        }
    }   
}

int setup_rom(char *rom) {
    //for now call load_rom directly, add fancy path checkers later
    chip8_instance.load_rom(rom);
}

int main(int argc, char *argv[]) {

    parse_commands(argc,argv);
    
    if(setup_rom(argv[1]) == -1) {
        cerr<<"could not open ROM file.";
        exit(1);
    }

    return 0;
}
