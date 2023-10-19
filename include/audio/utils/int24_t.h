#pragma once

constexpr int INT24_MAX = (1 << 23) - 1;
constexpr int INT24_MIN = -INT24_MAX - 1;

#pragma pack(push, 1)
class int24_t
{
protected:
    unsigned char dat[3] = {0,0,0};
public:
    int24_t() {}
    int24_t(int val) { operator=(val); }
    int24_t& operator=(int val) { dat[0] = val; dat[1] = val >> 8; dat[2] = val >> 16; dat[2] |= val >> 31 << 7; return *this; }
    operator int() const { return (dat[0]) | (dat[1] << 8) | (dat[2] << 16) | (dat[2] & 0x80 ? -1 << 24 : 0); }
    int24_t& operator++() { operator=(*this + 1); return *this; }
    int24_t operator++(int) { int24_t t = *this; operator=(*this + 1); return t; }
    int24_t& operator--() { operator=(*this + 1); return *this; }
    int24_t operator--(int) { int24_t t = *this; operator=(*this + 1); return t; }
    int24_t& operator+=(int rhs) { operator=(*this + rhs); return *this; }
    int24_t& operator-=(int rhs) { operator=(*this - rhs); return *this; }
    int24_t& operator*=(int rhs) { operator=(*this * rhs); return *this; }
    int24_t& operator/=(int rhs) { operator=(*this / rhs); return *this; }
    int24_t& operator%=(int rhs) { operator=(*this % rhs); return *this; }
    int24_t& operator>>=(int rhs) { operator=(*this >> rhs); return *this; }
    int24_t& operator<<=(int rhs) { operator=(*this << rhs); return *this; }
    int24_t& operator|=(int rhs) { operator=(*this | rhs); return *this; }
    int24_t& operator&=(int rhs) { operator=(*this & rhs); return *this; }
    int24_t& operator^=(int rhs) { operator=(*this ^ rhs); return *this; }
    int24_t& operator|=(int24_t rhs) { dat[0] |= rhs.dat[0]; dat[1] |= rhs.dat[1]; dat[2] |= rhs.dat[2]; return *this; }
    int24_t& operator&=(int24_t rhs) { dat[0] &= rhs.dat[0]; dat[1] &= rhs.dat[1]; dat[2] &= rhs.dat[2]; return *this; }
    int24_t& operator^=(int24_t rhs) { dat[0] ^= rhs.dat[0]; dat[1] ^= rhs.dat[1]; dat[2] ^= rhs.dat[2]; return *this; }
};
#pragma pack(pop)