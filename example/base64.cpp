#include"base64.h"
void uint8_to_64(char* out, const uint8_t* in, int len)
{
//    std::cout<<"old:"<<len<<std::endl;
//     std::cout<<"old:"<<in<<std::endl;
    static const char b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int i, j;
        for (i = 0, j = 0; i < len; i += 3, j += 4) {
            uint32_t tmp = in[i] << 16 | (i + 1 < len ? in[i + 1] << 8 : 0) | (i + 2 < len ? in[i + 2] : 0);
            out[j] = b64_chars[(tmp >> 18) & 0x3f];
            out[j + 1] = b64_chars[(tmp >> 12) & 0x3f];
            out[j + 2] = i + 1 < len ? b64_chars[(tmp >> 6) & 0x3f] : '=';
            out[j + 3] = i + 2 < len ? b64_chars[tmp & 0x3f] : '=';
        }
        out[j] = '\0';
 }
    size_t uint64_to_8(uint8_t* out, const char* in, size_t len) //its out_len is useless.
    {
        static const uint8_t b64_chars[] = {
            0x3e, 0xff, 0xff, 0xff, 0x3f, 0x34, 0x35, 0x36,
            0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01,
            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
            0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
            0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1a, 0x1b,
            0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
            0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
            0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };

        if (len % 4 != 0) {
            // Base64字符串长度必须为4的倍数
            return -1;
        }
        size_t real_out_len = (len / 4) * 3;
//        printf("real_out_len0 = %d\n",real_out_len);
        if (in[len - 1] == '=') {
            real_out_len--;
            if (in[len - 2] == '=') {
                real_out_len--;
            }
        }
//        printf("real_out_len1 = %d\n",real_out_len);
//        if (real_out_len > out_len) {
//            // 输出缓存不足
//            return -1;
//        }

        int i, j;
        for (i = 0, j = 0; i < len; i += 4, j += 3) {
            uint32_t tmp = 0;
            int k;
            for (k = 0; k < 4; k++) {
                uint8_t ch = in[i + k];
                if (ch >= 'A' && ch <= 'Z') {
                    ch -= 'A';
                }
                else if (ch >= 'a' && ch <= 'z') {
                    ch -= 'a' - 26;
                }
                else if (ch >= '0' && ch <= '9') {
                    ch -= '0' - 52;
                }
                else if (ch == '+') {
                    ch = 62;
                }
                else if (ch == '/') {
                    ch = 63;
                }
                else if (ch == '=') {
                    // Base64填充字符直接忽略
                    continue;
                }
                else {
                    return -1; // 非法的Base64字符
                }
                tmp |= ch << (6 * (3 - k));
            }
            out[j] = (tmp >> 16) & 0xff;
            if (i + 2 < len && in[i + 2] != '=') {
                out[j + 1] = (tmp >> 8) & 0xff;
            }
            if (i + 3 < len && in[i + 3] != '=') {
                out[j + 2] = tmp & 0xff;
            }
        }
//        std::cout<<"new:"<<real_out_len<<std::endl;
//        std::cout<<"new:"<<out<<std::endl;
//        out[real_out_len]='\0';
        return real_out_len;
    }
