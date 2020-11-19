#include "co/fast.h"
#include <string.h>
#include <stdarg.h>

namespace fast {

static void init_itoh_table(uint16* p) {
    for (int i = 0; i < 256; ++i) {
        char* b = (char*)(p + i);
        b[0] = "0123456789abcdef"[i >> 4];
        b[1] = "0123456789abcdef"[i & 0x0f];
    }    
}

static void init_itohex_table(uint16* p) {
    for (int i = 0; i < 256; ++i) {
        char* b = (char*)(p + i);
        b[0] = "0123456789ABCDEF"[i >> 4];
        b[1] = "0123456789ABCDEF"[i & 0x0f];
    }    
}

static void init_itoa_table(uint32* p) {
    for (int i = 0; i < 10000; ++i) {
        char* b = (char*)(p + i);
        b[3] = (char)(i % 10 + '0');
        b[2] = (char)(i % 100 / 10 + '0');
        b[1] = (char)(i % 1000 / 100 + '0');
        b[0] = (char)(i / 1000);

        // digits of i: (b[0] >> 4) + 1 
        if (i > 999) {
            b[0] |= (3 << 4); // 0x30
        } else if (i > 99) {
            b[0] |= (2 << 4); // 0x20
        } else if (i > 9) {
            b[0] |= (1 << 4); // 0x10
        }
    }    
}

static inline uint16* create_itoh_table() {
    static uint16 itoh_table[256];
    init_itoh_table(itoh_table);
    return itoh_table;
}

static inline uint16* create_itohex_table() {
    static uint16 itohex_table[256];
    init_itohex_table(itohex_table);
    return itohex_table;
}

static inline uint32* create_itoa_table() {
    static uint32 itoa_table[10000];
    init_itoa_table(itoa_table);
    return itoa_table;
}

static inline uint16* get_itoh_table() {
    static uint16* itoh_table = create_itoh_table();
    return itoh_table;
}

static inline uint16* get_itohex_table() {
    static uint16* itohex_table = create_itohex_table();
    return itohex_table;
}

static inline uint32* get_itoa_table() {
    static uint32* itoa_table = create_itoa_table();
    return itoa_table;
}

#define TOH(width, table) \
static uint16* itoh_table = table();\
uint16 b[width], *p = b + width;\
do {\
    *--p = itoh_table[v & 0xff];\
    v >>= 8;\
} while (v > 0);\
int len = (int) ((char*)(b + width) - (char*)p - (*(char*)p == '0'));\
memcpy(buf , (char*)(b + width) - len, (size_t)len);\
return len 

#define TOHH(width, table) \
static uint16* itoh_table = table();\
uint16 b[width], *p = b + width;\
do {\
    *--p = itoh_table[v & 0xff];\
    v >>= 8;\
} while (v > 0);\
buf[0] = '0';\
buf[1] = 'x';\
int len = (int) ((char*)(b + width) - (char*)p - (*(char*)p == '0'));\
memcpy(buf +2, (char*)(b + width) - len, (size_t)len);\
return len +2

int u32toh(uint32 v, char* buf) {
    TOH(4, get_itoh_table);
}

int u64toh(uint64 v, char* buf) {
    TOH(8, get_itoh_table);
    
}

int u32tohex(uint32 v, char* buf) {
    TOH(4, get_itohex_table);
}

int u64tohex(uint64 v, char* buf) {
    TOH(8, get_itohex_table);
}

int u32toh_with_prefix(uint32 v, char* buf) {
    TOHH(4, get_itoh_table);
}

int u64toh_with_prefix(uint64 v, char* buf) {
    TOHH(8, get_itoh_table);
}

int u32tohex_with_prefix(uint32 v, char* buf) {
    TOHH(4, get_itohex_table);
}

int u64tohex_with_prefix(uint64 v, char* buf) {
    TOHH(8, get_itohex_table);
}




int u32toa(uint32 v, char* buf) {
    static uint32* itoa_table = get_itoa_table();
    uint32 b[3], *p = b + 2;

    if (v > 9999) {
        b[1] = v / 10000;
        b[2] = itoa_table[v - b[1] * 10000] | 0x30303030;
        --p;
    } else {
        b[2] = itoa_table[v];
        goto u32toa_end;
    }

    if (b[1] > 9999) {
        b[0] = b[1] / 10000;
        b[1] = itoa_table[b[1] - b[0] * 10000] | 0x30303030;
        b[0] = itoa_table[b[0]];
        --p;
    } else {
        b[1] = itoa_table[b[1]];
        goto u32toa_end;
    }

  u32toa_end:
    int len = (int) (((char*)b) + 9 + ((*(char*)p) >> 4) - ((char*)p));
    memcpy(buf, ((char*)b) + 12 - len, (size_t)len);
    return len;
}

int u64toa(uint64 v, char* buf) {
    static uint32* itoa_table = get_itoa_table();
    uint32 b[5], *p = b + 4;
    uint64 x;

    if (v > 9999) {
        x = v / 10000;
        b[4] = itoa_table[v - x * 10000] | 0x30303030;
        --p;
    } else {
        b[4] = itoa_table[v];
        goto u64toa_end;
    }

    if (x > 9999) {
        v = x / 10000;
        b[3] = itoa_table[x - v * 10000] | 0x30303030;
        --p;
    } else {
        b[3] = itoa_table[x];
        goto u64toa_end;
    }

    if (v > 9999) {
        x = v / 10000;
        b[2] = itoa_table[v - x * 10000] | 0x30303030;
        --p;
    } else {
        b[2] = itoa_table[v];
        goto u64toa_end;
    }

    if (x > 9999) {
        b[0] = (uint32) (x / 10000);
        b[1] = itoa_table[x - b[0] * 10000] | 0x30303030;
        b[0] = itoa_table[b[0]];
        --p;
    } else {
        b[1] = itoa_table[x];
        goto u64toa_end;
    }

  u64toa_end:
    int len = (int) (((char*)b) + 17 + ((*(char*)p) >> 4) - ((char*)p));
    memcpy(buf, ((char*)b) + 20 - len, (size_t)len);
    return len;
}

} // namespace fast

// #include <iostream>
void vsprintf(fast::stream& oss, const char* fmt, va_list vlist)
{
    if(!fmt) return;

    do{
        const char* p = strchr(fmt,'%');
        if(!p){
            oss<<fmt;
            break;
        }
        if(p!=fmt)
            oss.append(fmt, p-fmt);
        fmt=++p;
        char a= *fmt;
        switch(a){
            case '\0':{
                oss.append('%');
                ++fmt;
            }break;
            case '%':{
                oss.append('%');
                ++fmt;
            }break;
            case 'b':{
                bool arg = va_arg(vlist, int);
                oss<<arg;
                ++fmt;
            }break;
            case 'B':{
                bool arg = va_arg(vlist, int);
                arg?oss.append("TRUE", 4):oss.append("FALSE", 5);
                ++fmt;
            }break;
            case 'c':{
                char arg = va_arg(vlist, int);
                oss.append(arg);
                ++fmt;
            }break;
            case 'd':{
                int arg = va_arg(vlist, int);
                oss<<arg;
                ++fmt;
            }break;
            case 'u':{
                uint32 arg = va_arg(vlist, uint32);
                oss<<arg;
                ++fmt;
            }break;
            case 'f':{
                double arg = va_arg(vlist, double);
                oss<<arg;
                ++fmt;
            }break;
            case 'x':{
                uint32 arg = va_arg(vlist, uint32);
                oss.append_hex(arg);
                ++fmt;
            }break;
            case 'X':{
                uint32 arg = va_arg(vlist, uint32);
                oss.append_hex_upper(arg);
                ++fmt;
            }break;
            case 'p':{
                void* arg = va_arg(vlist, void*);
                oss<<arg;
                ++fmt;
            }break;
            case 'P':{
                void* arg = va_arg(vlist, void*);
                oss.append_hex_upper_with_prefix((uint64_t)arg);
                ++fmt;
            }break;
            case 'l':{
                char b = *++fmt;
                switch(b){
                    case 'd':{
                        int64 arg = va_arg(vlist, int64);
                        oss<<arg;
                        ++fmt;
                    }break;
                    case 'u':{
                        uint64 arg = va_arg(vlist, uint64);
                        oss<<arg;
                        ++fmt;
                    }break;
                    // case 'f':{
                    //     long double arg = va_arg(vlist, long double);
                    //     oss<<arg;
                    //     ++fmt;
                    // }break; 
                    case 'x':{
                        uint64 arg = va_arg(vlist, uint64);
                        oss.append_hex(arg);
                        ++fmt;
                    }break;
                    case 'X':{
                        uint64 arg = va_arg(vlist, uint64);
                        oss.append_hex_upper(arg);
                        ++fmt;
                    }break;
                    default:{
                        oss.append(fmt-1, 3);
                        ++fmt;
                    }break;
                }

            }break;
            case 's':{
                const char* arg = va_arg(vlist, const char*);
                oss<<arg;
                ++fmt;
            }break;
            case '.':{
                if(strncmp(fmt+1, "*s", 2) == 0){
                    int arg1 = va_arg(vlist, int);
                    const char* arg2 = va_arg(vlist, const char*);
                    oss.append(arg2, (size_t)arg1);
                    fmt+=3;
                }
                else{
                    oss.append(fmt-1,2);
                    ++fmt;
                }
                
            }break;
            default:{
                oss.append(fmt-1,2);
                ++fmt;
            }break;
            
        }
    }while(*fmt);
}
void sprintf(fast::stream& oss, const char* fmt, ...)
{
    if(!fmt) return;

    va_list arg_list;
	va_start( arg_list,fmt );
    vsprintf(oss, fmt, arg_list);
    va_end(arg_list);
}
