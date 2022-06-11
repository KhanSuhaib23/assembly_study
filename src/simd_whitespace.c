#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <immintrin.h>
#include <intrin.h>

void lex(const char* b) {
    const __m128i simd_space = _mm_set1_epi8(' ');
    const __m128i simd_tab = _mm_set1_epi8('\t');

    start:
    char ch = b[0];

    switch (ch) {
        case '\r':
            ++b;
            if (b[0] != '\n') goto start;
        case '\n':
            ++b;
            unsigned long len;
            uint8_t cont;
            do {
                __m128i v = _mm_loadu_epi8(b);
                int is_space = _mm_movemask_epi8(_mm_cmpeq_epi8(v, simd_space));
                int is_tab = _mm_movemask_epi8(_mm_cmpeq_epi8(v, simd_tab));

                int is_indent_char = is_space | is_tab;

                cont = _BitScanForward(&len, ~is_indent_char);

                b += len;
            } while (cont);
            break;
        case '\0':
            return;
        default:
            ++b;
    }
}

// taken from https://gist.github.com/pervognsen/12d358c8fdd531d21fbccdca251fc2cd
void lex1(const char* b) {
    const __m128i simd_space = _mm_set1_epi8(' ');
    start:
    b += b[0] == ' '; // single space separator between tokens is very common so handle that branchless
    char ch = b[0];

    // consume upto 16 after a newline character, if more than 16 or some other whitespace goto one at a time slow path
    switch (ch) {
        case '\r':
            ++b;
            if (b[0] != '\n') goto start;
        case '\n':
            ++b;
            unsigned long len;
            uint8_t cont;
            __m128i v = _mm_loadu_epi8(b);
            int is_space = _mm_movemask_epi8(_mm_cmpeq_epi8(v, simd_space));

            cont = _BitScanForward(&len, ~is_space);

            b += len;
            goto start;
        case '\0':
            return;
        default:
            ++b;
    }
}

typedef struct Lexer Lexer;
typedef struct Token Token;

typedef enum {
    Character_Class_Null,
    Character_Class_Symbol,
    Character_Class_Whitespace,
    Character_Class_Character,
    Character_Class_Digit,
    Character_Class_Invalid
} Character_Class;

typedef enum {
Token_None = 0xff
} Token_Tag;

struct Token {
    Token_Tag tag;
    union {
        int val;
    };
};

struct Lexer {
    const char* start;
    char* pos;
};

#define N_TOKEN 1024

#define nl (Character_Class_Null)
#define ws (Character_Class_Whitespace)
#define ch (Character_Class_Character)
#define di (Character_Class_Digit)
#define sm (Character_Class_Symbol)
#define iv (Character_Class_Invalid)

uint8_t character_class_map[] = {
//            0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
/* 0 */      nl,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   ws,   ws,   ws,   iv,   ws,   iv,   iv,
/* 1 */      iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,   iv,
/* 2 */      ws,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,   sm,
/* 3 */      di,   di,   di,   di,   di,   di,   di,   di,   di,   di,   sm,   sm,   sm,   sm,   sm,   sm,
/* 4 */      sm,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* 5 */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   sm,   sm,   sm,   sm,   ch,
/* 6 */      sm,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* 7 */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   sm,   sm,   sm,   sm,   sm,
/* 8 */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* 9 */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* a */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* b */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* c */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* d */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* e */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
/* f */      ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,   ch,
};

#define BITS_PER_STATE 5
#define state_encode(tt, ts, pi, nu, st) \
    (((ed) << (BITS_PER_STATE * 0)) | \
     ((tt) << (BITS_PER_STATE * 1)) | \
     ((ts) << (BITS_PER_STATE * 2)) | \
     ((pi) << (BITS_PER_STATE * 3)) | \
     ((nu) << (BITS_PER_STATE * 4)) | \
     ((st) << (BITS_PER_STATE * 5)))

typedef enum {
State_Terminal_End = 0 * BITS_PER_PIXEL,
State_Terminal_Digit_Iden = 1 * BITS_PER_PIXEL,
State_Terminal_Symbol = 2 * BITS_PER_PIXEL,
State_Terminal_Last = State_Terminal_Symbol,
State_Parsing_Iden = 3 * BITS_PER_PIXEL,
State_Parsing_Number = 4 * BITS_PER_PIXEL,
State_Start = 5 * BITS_PER_PIXEL
} State_Enum;

#define ed State_Terminal_End
#define tt State_Terminal_Digit_Iden
#define ts State_Terminal_Symbol
#define pi State_Parsing_Iden
#define nu State_Parsing_Number
#define st State_Start

uint32_t state_transition[] = {
//          tt  ts  pi  nu  st
/* nl */  state_encode(ed, ed, ed, ed, ed),
/* ws */  state_encode(st, st, st, st, st),
/* ch */  state_encode(pi, pi, pi, nu, pi),
/* di */  state_encode(nu, nu, pi, nu, nu),
/* sm */  state_encode(sm, sm, sm, sm, sm),
/* iv */  state_encode(ed, ed, ed, ed, ed)
};

#define error()

size_t lex_n_token(Token* tok_out, Lexer* lexer) {
    size_t token_idx = 0;
    char* b = lexer->pos;

    return 0;
}

int main(int argc, char** argv) {

}
