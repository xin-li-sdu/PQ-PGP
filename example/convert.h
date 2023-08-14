#include <stdint.h>
#include "params.h"

void bits_to_uint8(uint8_t* out, const long* in, int len);
void uint8_to_bits(long* out, const uint8_t* in, int len);
void long_to_uint8(uint8_t* out, const long* in, int len);
void uint8_to_long(long* out, const uint8_t* in, int len);
void ZZX_to_uint8(uint8_t* out, const ZZX in, int len);
void uint8_to_ZZX(ZZX& out, const uint8_t* in, int len);
void ZZpX_to_uint8(uint8_t* out, const ZZ_pX in, int len);
void uint8_to_ZZpX(ZZ_pX& out, const uint8_t* in, int len);
void uint8_to_char(char* out, const uint8_t* in, int len);
void char_to_uint8(uint8_t* out, const char* in, int len);