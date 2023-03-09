#pragma once

constexpr int INT24_MAX = (1 << 23) - 1;
constexpr int INT24_MIN = -INT24_MAX - 1;

class int24_t
{
protected:
	char byt[3] = {
		0,
		0,
		0
	};
public:
	int24_t() = default;

	int24_t(int val)
	{
		operator=(val);
	}

	int24_t& operator=(int val)
	{
		byt[0] = reinterpret_cast<char*>(&val)[0];
		byt[1] = reinterpret_cast<char*>(&val)[1];
		byt[2] = reinterpret_cast<char*>(&val)[2];
		return *this;
	}

	operator int() const
	{
		return (byt[0]) | (byt[1] << 8) | (byt[2] << 16) | (byt[2] & 0x80);
	}
};