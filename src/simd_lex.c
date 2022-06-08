#include <stdio.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <intrin.h>

typedef enum {
I1 = 0x01, // 0, 1 | 0..f
I2 = 0x02, // 7 | f
W1 = 0x04, // 0 | 9,a,d
W2 = 0x08, // 2 | 0
D1 = 0x10, // 3 | 0..9
C1 = 0x20, // 4,6 | 1..f
C2 = 0x40,  // 5,7 | 0..a,f
Q1 = 0x80   // 2 | 2, 7
} Character_Class;

#define lut(i1, i2, w1, w2, d1, c1, c2, q1) \
    (char) ((i1 * I1) | (i2 * I2) | (w1 * W1) | (w2 * W2) | (d1 * D1) | (c1 * C1) | (c2 * C2) | (q1 * Q1))

int main(int argc, char** argv) {
    // b7 b6 b5 b4 b3 b2 b1 b0
    // c2 c1 d1 w2 w1 i3 i2 i1

    const __m128i low_lut = _mm_setr_epi8(
//              i1  i2  w1  w2  d1  c1  c2
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
//              i1  i2  i3  w1  w2  d1  c1  c2
/* 0 */    lut(  1,  0,  1,  0,  0,  0,  0 , 0 ),
/* 1 */    lut(  1,  0,  0,  0,  0,  0,  0 , 0 ),
/* 2 */    lut(  0,  0,  0,  1,  0,  0,  0 , 1 ),
/* 3 */    lut(  0,  0,  0,  0,  1,  0,  0 , 0 ),
/* 4 */    lut(  0,  0,  0,  0,  0,  1,  0 , 0 ),
/* 5 */    lut(  0,  0,  0,  0,  0,  0,  1 , 0 ),
/* 6 */    lut(  0,  0,  0,  0,  0,  1,  0 , 0 ),
/* 7 */    lut(  0,  1,  0,  0,  0,  0,  1 , 0 ),
/* 8 */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 ),
/* 9 */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 ),
/* a */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 ),
/* b */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 ),
/* c */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 ),
/* d */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 ),
/* e */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 ),
/* f */    lut(  0,  0,  0,  0,  0,  0,  0 , 0 )
    );

    const __m128i bytes = _mm_setr_epi8('a', 'z', 'B', '_', 0, ' ', '4', '\n', '\r', '"', '}', 0x13, 0x7f, '`', '(', '-');
    __m128i low = _mm_and_si128(bytes, _mm_set1_epi8(0x0f));
    /* __m128i hi = _mm_set1_epi8(0); */
    __m128i hi = _mm_srli_epi16(bytes, 4);
    hi = _mm_and_si128(hi, _mm_set1_epi8(0xf));
     __m128i l = _mm_shuffle_epi8(low_lut, low);
    __m128i h = _mm_shuffle_epi8(hi_lut, hi);

    __m128i r = _mm_and_si128(l, h);

    printf("Hello World\n");
}
