#include "co/fastring.h"
#include <cstring>

fastring& fastring::operator=(const fastring& s) {
    if (&s == this) return *this;

    _size = s.size();
    if (_size == 0) return *this;

    this->reserve(_size + 1);
    memcpy(_p, s.data(), _size);
    return *this;
}

fastring& fastring::operator=(const std::string& s) {
    _size = s.size();
    if (_size == 0) return *this;

    this->reserve(_size + 1);
    memcpy(_p, s.data(), _size);
    return *this;
}

fastring& fastring::operator=(const char* s) {
    if (!*s) { this->clear(); return *this; }

    if (!this->_Inside(s)) {
        _size = strlen(s);
        this->reserve(_size + 1);
        memcpy(_p, s, _size + 1);
    } else if (s != _p) {
        _size -= (s - _p);
        memmove(_p, s, _size + 1);
    }

    return *this;
}

// fastring& fastring::append(const void* x, size_t n) {
//     const char* p = (const char*) x;

//     if (!this->_Inside(p)) {
//         return (fastring&) this->_Append(p, n);
//     } else {
//         assert(p + n <= _p + _size);
//         size_t pos = p - _p;
//         this->_Ensure(n);
//         memcpy(_p + _size, _p + pos, n);
//         _size += n;
//         return *this;
//     }
// }

fastring& fastring::append(const fastring& s) {
    if (&s != this) {
        if (s.empty()) return *this;
        return (fastring&) this->_Append(s.data(), s.size());
    } else { /* append itself */
        if (_p) {
            this->reserve(_size << 1);
            memcpy(_p + _size, _p, _size);
            _size <<= 1;
        }
        return *this;
    }
}

size_t fastring::rfind(const char* sub) const {
    size_t m = strlen(sub);
    if (m == 1) return this->rfind(*sub);

    size_t n = this->size();
    if (n < m) return npos;

    const unsigned char* s = (const unsigned char*) _p;
    const unsigned char* p = (const unsigned char*) sub;

    size_t tbl[256] = { 0 };
    for (size_t i = m; i > 0; --i) tbl[p[i - 1]] = i;

    for (size_t j = n - m;;) {
        if (memcmp(p, s + j, m) == 0) return j;
        if (j == 0) return npos;

        size_t x = tbl[s[j - 1]];
        if (x == 0) x = m + 1;
        if (j < x) return npos;
        j -= x;
    }

    return npos;
}

size_t fastring::find_last_of(const char* s) const {
    if (this->empty()) return npos;

    typedef unsigned char u8;
    char bs[256] = { 0 };
    while (*s) bs[(const u8) (*s++)] = 1;

    for (size_t i = _size; i > 0;) {
        if (bs[(u8) (_p[--i])]) return i;
    }

    return npos;
}

size_t fastring::find_last_not_of(const char* s) const {
    if (this->empty()) return npos;

    typedef unsigned char u8;
    char bs[256] = { 0 };
    while (*s) bs[(const u8) (*s++)] = 1;

    for (size_t i = _size; i > 0;) {
        if (!bs[(u8) (_p[--i])]) return i;
    }

    return npos;
}

size_t fastring::find_last_not_of(char c) const {
    if (this->empty()) return npos;
    for (size_t i = _size; i > 0;) {
        if (_p[--i] != c) return i;
    }
    return npos;
}

void fastring::replace(const char* sub, const char* to, size_t maxreplace) {
    if (this->empty()) return;

    const char* from = this->c_str();
    const char* p = strstr(from, sub);
    if (!p) return;

    size_t n = strlen(sub);
    size_t m = strlen(to);

    fastring s(_size);

    do {
        s.append(from, p - from).append(to, m);
        from = p + n;
        if (maxreplace && --maxreplace == 0) break;
    } while ((p = strstr(from, sub)));

    if (from < _p + _size) s.append(from);

    this->swap(s);
}

void fastring::strip(const char* s, char d) {
    if (this->empty()) return;

    typedef unsigned char u8;
    char bs[256] = { 0 };
    while (*s) bs[(const u8)(*s++)] = 1;

    if (d == 'l' || d == 'L') {
        size_t b = 0;
        while (b < _size && bs[(u8)(_p[b])]) ++b;
        if (b == 0 || (_size -= b) == 0) return;
        memmove(_p, _p + b, _size);

    } else if (d == 'r' || d == 'R') {
        size_t e = _size;
        while (e > 0 && bs[(u8)(_p[e - 1])]) --e;
        if (e != _size) _size = e;

    } else {
        size_t e = _size;
        while (e > 0 && bs[(u8)(_p[e - 1])]) --e;
        if (e != _size) _size = e;
        if (e == 0) return;

        size_t b = 0;
        while (b < _size && bs[(u8)(_p[b])]) ++b;
        if (b == 0 || (_size -= b) == 0) return;
        memmove(_p, _p + b, _size);
    }
}

static bool _Match(const char* e, const char* p) {
    if (*p == '*' && !p[1]) return true;

    for (; *p && *e;) {
        if (*p == '*') return _Match(e, p + 1) || _Match(e + 1, p);
        if (*p != '?' && *p != *e) return false;
        ++p, ++e;
    }

    return (*p == '*' && !p[1]) || (!*p && !*e);
}

bool fastring::match(const char* p) const {
    return _Match(this->c_str(), p);
}

fastring& fastring::toupper() {
    for (size_t i = 0; i < _size; ++i) {
        char& c = _p[i];
        if ('a' <= c && c <= 'z') c ^= 32;
    }
    return *this;
}

fastring& fastring::tolower() {
    for (size_t i = 0; i < _size; ++i) {
        char& c = _p[i];
        if ('A' <= c && c <= 'Z') c ^= 32;
    }
    return *this;
}

void favsprintf(fast::stream& oss, const char* fmt, va_list vlist)
{
    if(!fmt) return;
#if 1
    do{
        const char* p = strchr(fmt,'%');
        if(!p){
            oss<<fmt;
            break;
        }
        if(p!=fmt)
            oss.append(fmt, p-fmt);
        fmt=++p;
        switch(*fmt){
            case '\0':{
                oss.append('%');
                ++fmt;
            }break;
            case '%':{
                oss.append('%');
                ++fmt;
            }break;
            case 'b':{
                bool arg = va_arg(vlist, bool);
                oss<<arg;
                ++fmt;
            }break;
            case 'c':{
                char arg = va_arg(vlist, char);
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
                float arg = va_arg(vlist, float);
                oss<<arg;
                ++fmt;
            }break;
            case 'x':{
                uint32 arg = va_arg(vlist, uint32);
                oss.append_hex(arg);
                ++fmt;
            }break;
            case 'p':{
                void* arg = va_arg(vlist, void*);
                oss<<arg;
                ++fmt;
            }break;
            case 'l':{
                ++fmt;
                switch(*fmt){
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
                    case 'f':{
                        double arg = va_arg(vlist, double);
                        oss<<arg;
                        ++fmt;
                    }break;
                    case 'x':{
                        uint64 arg = va_arg(vlist, uint64);
                        oss.append_hex(arg);
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

#else
    while(*fmt != '\0'){
        if(*fmt != '%'){
            oss.append(*fmt);
            continue;
        }
        ++fmt;
        switch(*fmt){
            case '\0':{
                oss.append('%');
                ++fmt;
            }break;
            case '%':{
                oss.append('%');
                ++fmt;
            }break;
            case 'b':{
                bool arg = va_arg(vlist, bool);
                oss<<arg;
                ++fmt;
            }break;
            case 'c':{
                char arg = va_arg(vlist, char);
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
                float arg = va_arg(vlist, float);
                oss<<arg;
                ++fmt;
            }break;
            case 'x':{
                uint32 arg = va_arg(vlist, uint32);
                oss.append_hex(arg);
                ++fmt;
            }break;
            case 'p':{
                void* arg = va_arg(vlist, void*);
                oss<<arg;
                ++fmt;
            }break;
            case 'l':{
                ++fmt;
                switch(*fmt){
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
                    case 'f':{
                        double arg = va_arg(vlist, double);
                        oss<<arg;
                        ++fmt;
                    }break;
                    case 'x':{
                        uint64 arg = va_arg(vlist, uint64);
                        oss.append_hex(arg);
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
        
    }
#endif
}
void fasprintf(fast::stream& oss, const char* fmt, ...)
{
    if(!fmt) return;

    va_list arg_list;
	va_start( arg_list,fmt );
    favsprintf(oss, fmt, arg_list);
    va_end(arg_list);
}
