#include <iostream>
union Opcode{
    uint16_t inst;
    union{
        uint16_t nnn: 12;
        struct {
            uint8_t n: 4;
            uint8_t y: 4;
            uint8_t x: 4;
            uint8_t u: 4;
        };
        uint8_t kk;
    };
};
