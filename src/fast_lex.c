#include <stdio.h>
#include <stdint.h>

typedef enum {
    Character_Class_Null = 0x00,
    Character_Class_Letter = 0x01,
    Character_Class_Digit = 0x02,
    Character_Class_Invalid = 0x03,
} Character_Class;

#define nl Character_Class_Null
#define lt Character_Class_Letter
#define di Character_Class_Digit
#define iv Character_Class_Invalid

typedef enum {
    State_Ident_Complete = 0,
    State_Got_Letter = 3,
    State_Start = 6,
} State;

#define ic State_Ident_Complete
#define gl State_Got_Letter
#define st State_Start

const uint8_t character_class[] = {
//            0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
/* 0 */      nl,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* 1 */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* 2 */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* 3 */      di,  di,  di,  di,  di,  di,  di,  di,  di,  di,  iv,  iv,  iv,  iv,  iv,  iv,
/* 4 */      iv,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,
/* 5 */      lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  iv,  iv,  iv,  iv,  iv,
/* 6 */      iv,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,
/* 7 */      lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  lt,  iv,  iv,  iv,  iv,  iv,
/* 8 */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* 9 */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* a */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* b */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* c */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* d */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* e */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
/* f */      iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,  iv,
};


#define BITS_PER_STATE 3

#define lut(s0, s1, s2) \
    (uint16_t) (((s0) << (BITS_PER_STATE * 0)) |\
                ((s1) << (BITS_PER_STATE * 1)) |\
                ((s2) << (BITS_PER_STATE * 2)))
const uint16_t dfa[] = {
//                ic   gl   st
/* nl */    lut(  st,  ic,  st ),
/* lt */    lut(  gl,  gl,  gl ),
/* di */    lut(  st,  gl,  st ),
/* iv */    lut(  st,  ic,  st ),
};


#undef nl
#undef lt
#undef di
#undef iv

#undef ic
#undef gl
#undef st


int main(int argc, char** argv) {

}
