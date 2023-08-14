#include <stdint.h>
#include "params.h"
#include "Algebra.h"

using namespace NTL;

const ZZX phi = Cyclo();

void bits_to_uint8(uint8_t* out, const long* in, int len){
    int len_0 = len / 8;
    for(int i = 0; i < len_0; i++){
        uint8_t tmp = 0;
        for(int j = 0; j < 8; j++){
            tmp = tmp << 1;
            tmp = tmp | (uint8_t) in[i*8+j];
        }
        out[i] = tmp;
    }
}
void uint8_to_bits(long* out, const uint8_t* in, int len){
    int len_0 = len / 8;
    for(int i = 0; i < len_0; i++){
        for(int j = 0; j < 8; j++){
            out[i*8+j] = (in[i]&(1<<(8-j-1)))>>(8-j-1);
        }
    }
}
 void long_to_uint8(uint8_t* out, const long* in, int len){
     for(int i = 0; i < len; i++){
         long tmp = in[i];
         uint8_t* p = (uint8_t*) &tmp;
         out[i*4+0] = p[0];
         out[i*4+1] = p[1];
         out[i*4+2] = p[2];
         out[i*4+3] = p[3];
     }
 }
//void long_to_uint8(uint8_t* out, const long* in, int len){
//    for(int i = 0; i < len; i++){
//        long tmp = in[i];
//        uint8_t* p = (uint8_t*) &tmp;
//        out[i*8+0] = p[0];
//        out[i*8+1] = p[1];
//        out[i*8+2] = p[2];
//        out[i*8+3] = p[3];
//        out[i*8+4] = p[4];
//        out[i*8+5] = p[5];
//        out[i*8+6] = p[6];
//        out[i*8+7] = p[7];
//    }
//}
 void uint8_to_long(long* out, const uint8_t* in, int len){
     for(int i = 0; i < len; i++){
         long tmp;
         uint8_t* p = (uint8_t*) &tmp;
         p[0] = in[i*4+0];
         p[1] = in[i*4+1];
         p[2] = in[i*4+2];
         p[3] = in[i*4+3];
         out[i] = tmp;
     }
 }
//void uint8_to_long(long* out, const uint8_t* in, int len){
//    for(int i = 0; i < len; i++){
//        long tmp;
//        uint8_t* p = (uint8_t*) &tmp;
//        p[0] = in[i*8+0];
//        p[1] = in[i*8+1];
//        p[2] = in[i*8+2];
//        p[3] = in[i*8+3];
//        p[4] = in[i*8+3];
//        p[5] = in[i*8+3];
//        p[6] = in[i*8+3];
//        p[7] = in[i*8+3];
//        out[i] = tmp;
//    }
//}
void ZZX_to_uint8(uint8_t* out, const ZZX in, int len){
    for(int i = 0; i < len; i++){
        int tmp;
        conv(tmp, in[i]);
        uint8_t* p = (uint8_t*) &tmp;
        out[i*4+0] = p[0];
        out[i*4+1] = p[1];
        out[i*4+2] = p[2];
        out[i*4+3] = p[3]; 
    }
}
void uint8_to_ZZX(ZZX& out, const uint8_t* in, int len){
    out.SetLength(len);
    for(int i = 0; i < len; i++){
        int tmp;
        uint8_t* p = (uint8_t*) &tmp;
        p[0] = in[i*4+0];
        p[1] = in[i*4+1];
        p[2] = in[i*4+2];
        p[3] = in[i*4+3];
        conv(out[i], tmp);
    }
}
void ZZpX_to_uint8(uint8_t* out, const ZZ_pX in, int len){
    for(int i = 0; i < len; i++){
        int tmp;
        conv(tmp, in[i]);
        // std::cout<<tmp<<" "<<in[i]<<std::endl;
        uint8_t* p = (uint8_t*) &tmp;
        out[i*4+0] = p[0];
        out[i*4+1] = p[1];
        out[i*4+2] = p[2];
        out[i*4+3] = p[3];
    }
}
void uint8_to_ZZpX(ZZ_pX& out, const uint8_t* in, int len){
    out.SetLength(len);
    ZZ_p::init(q1);
    for(int i = 0; i < len; i++){
        int tmp;
        uint8_t* p = (uint8_t*) &tmp;
        p[0] = in[i*4+0];
        p[1] = in[i*4+1];
        p[2] = in[i*4+2];
        p[3] = in[i*4+3];
        conv(out[i], tmp);
    }
}
char con[] = "0123456789ABCDEF";
void uint8_to_char(char* out, const uint8_t* in, int len)
{
    for(int i = 0; i < len; i++)
    {
        out[i*2+0] = con[in[i]>>4];
        out[i*2+1] = con[in[i]&15];
    }
}
static uint8_t cal(char x){
    if(x<='9') return x-'0';
    return x-'A'+10;
}
void char_to_uint8(uint8_t* out, const char* in, int len){
    for(int i = 0; i < len; i++){
        uint8_t tmp = 0;
        tmp = (tmp<<4)|cal(in[i*2+0]);
        tmp = (tmp<<4)|cal(in[i*2+1]);
        out[i] = tmp;
    }
}
