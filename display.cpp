#include <iostream>
#include <cstdint>
using namespace std;

string buildString(uint8_t *bytes, uint8_t size){
    string character = string("");
    for(int i=0; i<size; i++, ++bytes){
        uint8_t nibble = (*bytes & 0xf0) >> 4;
        for(int j = 3; j >= 0; j--) character += " *"[(nibble >> j) & 0x01]; character += "\n";
    }
    return character;
}

int main(){
    uint8_t characters[10][5] = {
    {0xF0, 0x90, 0x90, 0x90, 0xF0},
    {0x20, 0x60, 0x20, 0x20, 0x70},
    {0xF0, 0x10, 0xF0, 0x80, 0xF0},
    {0xF0, 0x10, 0xF0, 0x10, 0xF0},
    {0x90, 0x90, 0xF0, 0x10, 0x10},
    {0xF0, 0x80, 0xF0, 0x10, 0xF0},
    {0xF0, 0x80, 0xF0, 0x90, 0xF0},
    {0xF0, 0x10, 0x20, 0x40, 0x40},
    {0xF0, 0x90, 0xF0, 0x90, 0xF0},
    {0xF0, 0x90, 0xF0, 0x10, 0xF0}};
    cout << buildString(characters[0], sizeof(characters[0])) << endl;
    cout << buildString(characters[1], sizeof(characters[1])) << endl;
    cout << buildString(characters[2], sizeof(characters[2])) << endl;
    cout << buildString(characters[3], sizeof(characters[3])) << endl;
    return 0;
}
