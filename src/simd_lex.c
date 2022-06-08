#include <stddef.h>
#include <stdio.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <intrin.h>
#include <pmmintrin.h>
#include <stdint.h>

typedef enum {
I1 = 0x01, // 0, 1 | 0..f
I2 = 0x02, // 7 | f
W1 = 0x04, // 0 | 9,a,d
W2 = 0x08, // 2 | 0
D1 = 0x10, // 3 | 0..9
C1 = 0x20, // 4,6 | 1..f
C2 = 0x40,  // 5,7 | 0..a,f
Q1 = 0x80   // 2 | 2, 7
} Character_Class_Bit_Mask;

typedef enum {
Symbol = 0x00, // symbols are a fallthrough class ie and has no common bits so = 0
Character1 = C1, // 4,6 | 1..f are exclusively characters so same as the mask C1
Character2 = C2, // 5,7 | 0..a,f are characters except for 7f which is invalid, if ch is 7f then I2 bit is also set,
                   // so the value 0x40 only happens is I2 is not set ie the value is not 7f, so the value is 5 | 0..a,f or 7 | 0..a
                   // which are all characters + _
Character3_1 = I1 | W2, // this is clever bit of coding so get UTF characters as a character class (rather than going through the fallthrough)
                     // the way this works is for values >= 80 the high mask always has I1, W2, C1 set
                     // and in the low mask value I1 is always set, and either W2 (0) or C1 (1..f) is set for all nibbles
                     // this means for UTF8 hi & lo will always have I1 set and either W2 or C1 set
                     // Character3_1 is I1 set W2 set
Character3_2 = I1 | C1, // Character3_2 is I1 set C1 set
Digit = D1,        // exclusively digits so no clever hacking
Quote = Q1,        // exclusively quotes so no clever hacking
Whitespace1 = W1 | I1,  // Whitespace shares some of its space with I1, W1 is fully included in I1, (I1 is superset)
                     // so if W1 is set I1 will be set so whitespace happens when W1 | I1 is the value
Whitespace2 = W2,  // exclusively whitespace so no clever hacking
Invalid1 = I1,     // if I1 is set and W1 is not set than means it is actually invalid (because I1 is superset of W1)
Invalid2 = I2 | C2,   // if I2 is set then C2 has to be set, since C2 is a super set
} Character_Class;

#define lut(i1, i2, w1, w2, d1, c1, c2, q1) \
    (char) ((i1 * I1) | (i2 * I2) | (w1 * W1) | (w2 * W2) | (d1 * D1) | (c1 * C1) | (c2 * C2) | (q1 * Q1))

int main(int argc, char** argv) {
    // b7 b6 b5 b4 b3 b2 b1 b0
    // c2 c1 d1 w2 w1 i3 i2 i1

    const __m128i low_lut = _mm_setr_epi8(
//              i1  i2  w1  w2  d1  c1  c2  q1
/* 0 */    lut(  1,  0,  0,  1,  1,  0,  1 , 0 ),
/* 1 */    lut(  1,  0,  0,  0,  1,  1,  1 , 0 ),
/* 2 */    lut(  1,  0,  0,  0,  1,  1,  1 , 1 ),
/* 3 */    lut(  1,  0,  0,  0,  1,  1,  1 , 0 ),
/* 4 */    lut(  1,  0,  0,  0,  1,  1,  1 , 0 ),
/* 5 */    lut(  1,  0,  0,  0,  1,  1,  1 , 0 ),
/* 6 */    lut(  1,  0,  0,  0,  1,  1,  1 , 0 ),
/* 7 */    lut(  1,  0,  0,  0,  1,  1,  1 , 1 ),
/* 8 */    lut(  1,  0,  0,  0,  1,  1,  1 , 0 ),
/* 9 */    lut(  1,  0,  1,  0,  1,  1,  1 , 0 ),
/* a */    lut(  1,  0,  1,  0,  0,  1,  1 , 0 ),
/* b */    lut(  1,  0,  0,  0,  0,  1,  0 , 0 ),
/* c */    lut(  1,  0,  0,  0,  0,  1,  0 , 0 ),
/* d */    lut(  1,  0,  1,  0,  0,  1,  0 , 0 ),
/* e */    lut(  1,  0,  0,  0,  0,  1,  0 , 0 ),
/* f */    lut(  1,  1,  0,  0,  0,  1,  1 , 0 )
    );

    const __m128i hi_lut = _mm_setr_epi8(
//              i1  i2  w1  w2  d1  c1  c2  q1
/* 0 */    lut(  1,  0,  1,  0,  0,  0,  0 , 0 ),
/* 1 */    lut(  1,  0,  0,  0,  0,  0,  0 , 0 ),
/* 2 */    lut(  0,  0,  0,  1,  0,  0,  0 , 1 ),
/* 3 */    lut(  0,  0,  0,  0,  1,  0,  0 , 0 ),
/* 4 */    lut(  0,  0,  0,  0,  0,  1,  0 , 0 ),
/* 5 */    lut(  0,  0,  0,  0,  0,  0,  1 , 0 ),
/* 6 */    lut(  0,  0,  0,  0,  0,  1,  0 , 0 ),
/* 7 */    lut(  0,  1,  0,  0,  0,  0,  1 , 0 ),
/* 8 */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 ),
/* 9 */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 ),
/* a */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 ),
/* b */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 ),
/* c */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 ),
/* d */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 ),
/* e */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 ),
/* f */    lut(  1,  0,  0,  0,  1,  0,  1 , 0 )
    );

    uint8_t arr[256];
    uint8_t cc[256];

    for (size_t i = 0; i < 256; ++i) {
        arr[i] = (uint8_t) i;
    }

    for (size_t i = 0; i < 256; i += 16) {
        const __m128i bytes = _mm_lddqu_si128((__m128i*)&arr[i]);
        __m128i low = _mm_and_si128(bytes, _mm_set1_epi8(0x0f));
        /* __m128i hi = _mm_set1_epi8(0); */
        __m128i hi = _mm_srli_epi16(bytes, 4);
        hi = _mm_and_si128(hi, _mm_set1_epi8(0xf));
        __m128i l = _mm_shuffle_epi8(low_lut, low);
        __m128i h = _mm_shuffle_epi8(hi_lut, hi);

        __m128i r = _mm_and_si128(l, h);

        _mm_storeu_si128((__m128i*) &cc[i], r);


    }

    printf("Hello World\n");
}
