#include <stddef.h>
#include <stdio.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <intrin.h>
#include <pmmintrin.h>
#include <stdint.h>

typedef enum {
Symbol1 = 0x81, // 1000 0001 2 | 1,3..6,8..f
Symbol2 = 0x82, // 1000 0010 3 | a..f
Symbol3 = 0x84, // 1000 0100 4,6 | 0
Symbol4 = 0x88, // 1000 1000 5 | b,d..e
Symbol5 = 0x90, // 1001 0000 5,7 | b..e
Single_Quote = 0x90, // 1001 0000 2 | 7
Double_Quote = 0xa0, // 1010 0000 2 | 2
Whitespace1 = 0x41, // 0100 0001 0 | 9,a,d
Whitespace2 = 0x42, // 0100 0010 2 | 0
Character1 = 0x21, // 0010 0001 4, 6 | 1..f
Character2 = 0x22, // 0010 0010 5, 7 | 0..a
Character3 = 0x24, // 0010 0100 8..f | 0..f
Underscore = 0x24, // 0010 0100 5 | f
Null = 0x11, // 0001 0001 0 | 0
Escape = 0x12, // 0001 0010 5 | c
Digit = 0x09, // 0000 1001 3 | 0..9
} Character_Class;

#define lut(s1, s2, s3, s4, s5, sq, dq, w1, w2, c1, c2, c3, us, n0, es, di) \
                (char) ((s1*Symbol1) | \
                (s2*Symbol2) | \
                (s3*Symbol3) | \
                (s4*Symbol4) | \
                (s5*Symbol5) | \
                (sq*Single_Quote) | \
                (dq*Double_Quote) | \
                (w1*Whitespace1) | \
                (w2*Whitespace2) | \
                (c1*Character1) | \
                (c2*Character2) | \
                (c3*Character3) | \
                (us*Underscore) | \
                (n0*Null) | \
                (es*Escape) | \
                (di*Digit))



int main(int argc, char** argv) {
    // b7 b6 b5 b4 b3 b2 b1 b0
    // c2 c1 d1 w2 w1 i3 i2 i1

    const __m128i low_lut = _mm_setr_epi8(
//          s1, s2, s3, s4, s5, sq, dq, w1, w2, c1, c2, c3, us, n0, es, di
/* 0 */ lut( 0,  0,  1,  0,  0,  0,  0,  0,  1,  0,  1,  1,  0,  1,  0,  1 ),
/* 1 */ lut( 1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 2 */ lut( 0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 3 */ lut( 1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 4 */ lut( 1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 5 */ lut( 1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 6 */ lut( 1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 7 */ lut( 0,  0,  0,  0,  0,  1,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 8 */ lut( 1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* 9 */ lut( 1,  0,  0,  0,  0,  0,  0,  1,  0,  1,  1,  1,  0,  0,  0,  1 ),
/* a */ lut( 1,  1,  0,  0,  0,  0,  0,  1,  0,  1,  1,  1,  0,  0,  0,  0 ),
/* b */ lut( 1,  1,  0,  1,  1,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0 ),
/* c */ lut( 1,  1,  0,  0,  1,  0,  0,  0,  0,  1,  0,  1,  0,  0,  1,  0 ),
/* d */ lut( 1,  1,  0,  1,  1,  0,  0,  1,  0,  1,  0,  1,  0,  0,  0,  0 ),
/* e */ lut( 1,  1,  0,  1,  1,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0 ),
/* f */ lut( 1,  1,  0,  0,  1,  0,  0,  0,  0,  1,  0,  1,  1,  0,  0,  0 )
    );

    const __m128i hi_lut = _mm_setr_epi8(
//          s1, s2, s3, s4, s5, sq, dq, w1, w2, c1, c2, c3, us, n0, es, di
/* 0 */ lut( 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  1,  0,  0 ),
/* 1 */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 ),
/* 2 */ lut( 1,  0,  0,  0,  0,  1,  1,  0,  1,  0,  0,  0,  0,  0,  0,  0 ),
/* 3 */ lut( 0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1 ),
/* 4 */ lut( 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0 ),
/* 5 */ lut( 0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  1,  0,  1,  0,  1,  0 ),
/* 6 */ lut( 0,  0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0 ),
/* 7 */ lut( 0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0 ),
/* 8 */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 ),
/* 9 */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 ),
/* a */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 ),
/* b */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 ),
/* c */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 ),
/* d */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 ),
/* e */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 ),
/* f */ lut( 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 )
    );

    uint8_t arr[256];
    uint8_t cc[256];
    Character_Class cc1[256];

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

        for (size_t j = i; j < i + 16; ++j) {
            cc1[j] = (Character_Class) cc[j];
        }

        printf("Hello World\n");

    }

    printf("Hello World\n");
}

// TODO(suhaibnk): verify each value goes as expected
// TODO(suhaibnk): null value is now being catogerized as invalid
