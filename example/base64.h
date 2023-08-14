#ifndef BASE64_H
#define BASE64_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include<iostream>
#include<cstdio>
void uint8_to_64(char* out, const uint8_t* in, int len);
size_t uint64_to_8(uint8_t* out, const char* in, size_t len);

#endif // BASE64_H
