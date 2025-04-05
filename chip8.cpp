/*

    CHIP8 

*/

#include "chip8.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;

/*

    Initialize CHIP8 

*/
CHIP8::CHIP8() {
 
    /*
        
        CPU Data

    */
    memset(V, 0, sizeof(V));
    
    PC   = PC_STARTADR;
    I    = 0;
    DT   = 0;
    ST   = 0;
    
    /*
        
        MEM Data
        
    */
    memset(STACK, 0, sizeof(STACK));
   
    SP   = 0;

    /*
        
        I/O Data
        
    */
    memset(DISP, 0, sizeof(DISP));
    memset(KEYP, 0, sizeof(KEYP));

    draw_flag = false;

    /* initialise font set */
    uint8_t font_set[MAX_FONTCOUNT] {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    /* load font set in memory from 0x050 to 0x09F */
    for(int i=0; i < 80; i++) {
        MEM[i + 0x50] = font_set[i];
    }

}

CHIP8::~CHIP8() {
    cout<<"CHIP8 instance stopped.";
}

int CHIP8::load_rom(char* path) {
    ifstream rom_file(path, ios::binary);

    if(!rom_file.is_open()){
        cerr<< "file does not exist.";
        return -1;
    }

    char data;
    for(int i=PC_STARTADR; rom_file.get(data); i++){
        if(i >= MAX_MEMSIZE){
            cerr<< "file size too large";
            return -1;
        }
        MEM[i] = (uint8_t)data;
    }
    return 0;
}

int CHIP8::cycle() {

    /*
        Each insturction is 16-bits, big-endian.
        so we get the first byte and LEFTSHIFT it by 8,
        then get the next byte and OR it.
        then we send this instruction to the exec unit,
        which decodes it and executes it
    */

    if(PC >= MAX_MEMSIZE) {
        cerr << "memory overflow";
        return -1;
    }

    uint16_t instruction = ( ( MEM[PC] << 8 ) ||  MEM[PC+1] );
    /* go to next address +2 bytes */
    PC += 2;

    if(instr_exec(instruction) == -1) {
        cerr<<"error executing instruction: <print instruction here>";
        return -1;
    }

    if(DT > 0) {
        DT--;
    } else {

    }

    if(ST > 0) {
        ST--;
    } else {

    }

    return 0;
}

/*

    Large number of switch cases to group instructions with the same nibble.
    first we need to get all the nibbles
    
*/
int CHIP8::instr_exec(uint16_t instruction) {

    

}