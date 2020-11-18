#pragma once

#include "def.h"
#include "__/dtoa_milo.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <utility>

namespace fast {

// double to ascii string
inline int dtoa(double v, char* buf) {
    return dtoa_milo(v, buf);
}

// integer to hex string  (255 -> "0xff". eg.)
int u32toh(uint32 v, char* buf);
int u64toh(uint64 v, char* buf);

// integer to ascii string
int u32toa(uint32 v, char* buf);
int u64toa(uint64 v, char* buf);

inline int i32toa(int32 v, char* buf) {
    if (v >= 0) return u32toa((uint32)v, buf);
    *buf = '-';
    return u32toa((uint32)(-v), buf + 1) + 1;
}

inline int i64toa(int64 v, char* buf) {
    if (v >= 0) return u64toa((uint64)v, buf);
    *buf = '-';
    return u64toa((uint64)(-v), buf + 1) + 1;
}

class stream {
  public:
    constexpr stream() noexcept : _cap(0), _size(0), _p(0) {}
    
    explicit stream(size_t cap)
        : _cap(cap), _size(0), _p((char*) malloc(cap)) {
    }

    ~stream() {
        if (_p) free(_p);
    }

    stream(const stream&) = delete;
    void operator=(const stream&) = delete;

    stream(stream&& s) noexcept
        : _cap(s._cap), _size(s._size), _p(s._p) {
        s._p = 0;
        s._cap = s._size = 0;
    }

    stream& operator=(stream&& s) noexcept {
        if (&s != this) {
            if (_p) free(_p);
            _cap = s._cap;
            _size = s._size;
            _p = s._p;
            s._p = 0;
            s._cap = s._size = 0;
        }
        return *this;
    }

    inline const char* data() const {
        return _p;
    }

    inline size_t size() const {
        return _size;
    }

    inline bool empty() const {
        return _size == 0;
    }

    inline size_t capacity() const {
        return _cap;
    }

    inline void clear() {
        _size = 0;
    }

    // !! newly allocated memory is not initialized
    inline void resize(size_t n) {
        this->reserve(n);
        _size = n;
    }

    inline void reserve(size_t n) {
        if (_cap < n) {
            _p = (char*) realloc(_p, n);
            assert(_p);
            _cap = n;
        }
    }

    inline const char* c_str() const {
        ((stream*)this)->reserve(_size + 1);
        if (_p[_size] != '\0') _p[_size] = '\0';
        return _p;
    }

    inline char& back() const {
        return _p[_size - 1];
    }

    inline char& front() const {
        return _p[0];
    }

    inline char& operator[](size_t i) const {
        return _p[i];
    }

    // inline const char& back() const {
    //     return _p[_size - 1];
    // }

    // inline const char& front() const {
    //     return _p[0];
    // }

    // inline const char& operator[](size_t i) const {
    //     return _p[i];
    // }

    inline void swap(stream& fs) noexcept {
        std::swap(fs._cap, _cap);
        std::swap(fs._size, _size);
        std::swap(fs._p, _p);
    }

    inline void swap(stream&& fs) noexcept {
        fs.swap(*this);
    }

    inline stream& append(const std::string& s) {
        return this->_Append(s.data(), s.size());
    }

    inline stream& append(size_t n, char c) {
        this->_Ensure(n);
        memset(_p + _size, c, n);
        _size += n;
        return *this;
    }

    inline stream& append(char c) {
        this->_Ensure(1);
        _p[_size++] = c;
        return *this;
    }

    inline stream& append(uint8 v) {
        static_assert(sizeof(uint8) == sizeof(char), "");
        return this->append((char)v);
    }

    inline stream& append(int16 v) {
        return this->_Append(&v, sizeof(v));
    }

    inline stream& append(uint16 v) {
        return this->_Append(&v, sizeof(v));
    }

    inline stream& append(int32 v) {
        return this->_Append(&v, sizeof(v));
    }

    inline stream& append(uint32 v) {
        return this->_Append(&v, sizeof(v));
    }

    inline stream& append(int64 v) {
        return this->_Append(&v, sizeof(v));
    }

    inline stream& append(uint64 v) {
        return this->_Append(&v, sizeof(v));
    }

    inline stream& append_hex(uint32 v) {
        this->_Ensure(24);
        _size += fast::u32toa(v, _p + _size);
        return *this;
    }

    inline stream& append_hex(uint64 v) {
        this->_Ensure(24);
        _size += fast::u64toa(v, _p + _size);
        return *this;
    }
    inline stream& append(const void* p, size_t n){
        return _Append(p, n);
    }

    // inline stream& append_hex(const void* p, size_t n, bool ){
    //     return _Append(chunk, chunk_size);
    // }



    inline stream& operator<<(bool v) {
        return v ? this->_Append("true", 4) : this->_Append("false", 5);
    }

    inline stream& operator<<(char v) {
        return this->append(v);
    }

    inline stream& operator<<(unsigned char v) {
        this->_Ensure(4);
        _size += fast::u32toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(short v) {
        this->_Ensure(8);
        _size += fast::i32toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(unsigned short v) {
        this->_Ensure(8);
        _size += fast::u32toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(int v) {
        this->_Ensure(12);
        _size += fast::i32toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(unsigned int v) {
        this->_Ensure(12);
        _size += fast::u32toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(long v) {
        this->_Ensure(sizeof(v) * 3);
        _size += fast::i64toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(unsigned long v) {
        this->_Ensure(sizeof(v) * 3);
        _size += fast::u64toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(long long v) {
        this->_Ensure(24);
        _size += fast::i64toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(unsigned long long v) {
        this->_Ensure(24);
        _size += fast::u64toa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(const char* v) {
        return this->_Append(v, strlen(v));
    }

    inline stream& operator<<(const std::string& v) {
        return this->_Append(v.data(), v.size());
    }

    inline stream& operator<<(const void* v) {
        this->_Ensure(20);
        _size += fast::u64toh((uint64)v, _p + _size);
        return *this;
    }

    inline stream& operator<<(float v) {
        this->_Ensure(24);
        _size += fast::dtoa(v, _p + _size);
        return *this;
    }

    inline stream& operator<<(double v) {
        this->_Ensure(24);
        _size += fast::dtoa(v, _p + _size);
        return *this;
    }


  protected:
    inline void _Ensure(size_t n) {
        if (_cap < _size + n) this->reserve((_cap * 3 >> 1) + n);
    }

    inline stream& _Append(const void* p, size_t n) {
        this->_Ensure(n);
        memcpy(_p + _size, p, n);
        _size += n;
        return *this;
    }
    
    size_t _cap;
    size_t _size;
    char* _p;
};

} // namespace fast
