/*

    The main driver program for the chip8 emulator

    Takes "program.ch8" and runs it in the game loop

*/

#include <iostream>
#include <string>
#include <cstring>
#include "chip8.h"

#include<SDL2/SDL.h>

using namespace std;

#define MODE_VRB 000000001
#define MODE_SND 000000010

CHIP8 chip8_instance;

// enum MACHINESTATE {ON, READY, RUN, STOP, OFF, UNDEF};

void parse_commands(int argc, char* argv[], uint8_t *MODE){
    if(argc < 2) {
        cout<<"usage: ./chip8 <rom>.ch8 <-options[hvac]>";
        exit(0);
    }

    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "help") == 0) {
        cout<<"usage: ./chip8 <rom>.ch8 <options>"<<endl;
        cout<<"options:"<<endl;
        cout<<"\t-h : shows this message."<<endl;
        cout<<"\t-v : verbose mode, shows interal trace."<<endl;
        cout<<"\t-a : disables audio."<<endl;
        cout<<"\t-c : displays controls."<<endl;
        exit(0);
    }

    if(argc >= 3 && argv[2][0] == '-'){
        string options = argv[2];

        if(options.find("h") != string::npos){
            cout<<"usage: ./chip8 <rom>.ch8 <options>"<<endl;
            cout<<"options:"<<endl;
            cout<<"\t-h : shows this message."<<endl;
            cout<<"\t-v : verbose mode, shows interal trace."<<endl;
            cout<<"\t-a : disables audio."<<endl;
            cout<<"\t-c : displays controls."<<endl;
        }

        if(options.find("v") != string::npos) {
            cout<<"VERBOSE(trace) mode is ON."<<endl;
            *MODE |= MODE_VRB;
        }
    
        if(options.find("a") != string::npos) {
            cout<<"AUDIO is OFF."<<endl;
            *MODE |= MODE_SND;
        }
    
        if(options.find("c") != string::npos) {
            cout << "CONTROLS:";
        }
    }
    
    

}

int setup_rom(char *rom, uint8_t MODE) {
    //for now call load_rom directly, add fancy path checkers later
    bool sound = false;
    bool verbose = false;

    if(MODE & MODE_SND) {
        sound = true;
    }
    if(MODE & MODE_VRB) {
        verbose = true;
    }

    return chip8_instance.load_rom(rom, sound, verbose);
}

int main(int argc, char *argv[]) {

    /*
        A byte which determines the various modes (see options)

        Default                                 (00000000).
        Verbose ON : right-most bit     ON      (00000001).
        Sound   OFF: 2nd from right bit ON.     (00000010).
    */
    uint8_t MODE = 0;

    parse_commands(argc,argv, &MODE);
    
    if(setup_rom(argv[1], MODE) == -1) {
        cerr<<std::endl<<"could not open ROM file.";
        exit(1);
    }

    /*
        SETUP WINDOW        
        SDL window,surface Pointers
        Code from https://thenumb.at/cpp-course/sdl2/01/01.html
    */
    SDL_Surface* winSurface = NULL;
    SDL_Window* window = NULL;

	// Initialize SDL. SDL_Init will return -1 if it fails.
	if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
		cerr << "Error initializing SDL: " << SDL_GetError() << endl;
		system("pause");
		return 1;
	} 

	// Create  window
	window = SDL_CreateWindow( "Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN );

	// check window
	if ( !window ) {
		cout << "Error creating window: " << SDL_GetError()  << endl;
		system("pause");
		return 1;
	}

	// Get the surface from the window
	winSurface = SDL_GetWindowSurface( window );

	// check surface
	if ( !winSurface ) {
		cout << "Error getting surface: " << SDL_GetError() << endl;
		system("pause");
		return 1;
	}

    SDL_FillRect( winSurface, NULL, SDL_MapRGB( winSurface->format, 255, 255, 255 ) );

	// Update the window display
	SDL_UpdateWindowSurface( window );

	// Wait
	system("pause");

	// Destroy the window. This will also destroy the surface
	SDL_DestroyWindow( window );

	// Quit SDL
	SDL_Quit();


    return 0;
}
