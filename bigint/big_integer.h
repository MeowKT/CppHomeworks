#pragma once
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

struct big_integer {
private:
    std::vector<uint32_t> dig;
    __uint8_t sign;
public:
    big_integer();
    big_integer(const big_integer& x) = default;
    big_integer(int x);
    explicit big_integer(std::string s);
    big_integer& operator=(big_integer const& other) = default;

    friend std::string to_string(big_integer);

    big_integer operator~() const;
    big_integer operator-() const;
    big_integer operator+() const;
    big_integer& operator++();
    big_integer const operator++(int) ;
    big_integer& operator--();
    big_integer const operator--(int);

    friend big_integer operator+(big_integer, big_integer const&);
    friend big_integer operator*(big_integer const&, big_integer const&);
    friend big_integer operator/(big_integer const&, big_integer const&);

    friend big_integer operator>>(big_integer const&, int);
    friend big_integer operator<<(big_integer, int);

    friend big_integer operator|(big_integer const&, big_integer const&);
    friend big_integer operator&(big_integer const&, big_integer const&);
    friend big_integer operator^(big_integer const&, big_integer const&);

    big_integer& operator+=(big_integer);
    big_integer& operator-=(big_integer const&);
    big_integer& operator*=(big_integer const&);
    big_integer& operator/=(big_integer const&);
    big_integer& operator%=(big_integer const&);
    big_integer& operator|=(big_integer const&);
    big_integer& operator&=(big_integer const&);
    big_integer& operator^=(big_integer const&);
    big_integer& operator >>=(int);
    big_integer& operator <<=(int);

    friend bool operator==(big_integer const&, big_integer const&);
    friend bool operator<(big_integer const&, big_integer const&);
    friend bool operator>(big_integer const&, big_integer const&);
    friend bool operator<=(big_integer const&, big_integer const&);
    friend bool operator>=(big_integer const&, big_integer const&);

private:
    void resize(size_t);
    void erase_insignificant_zeros();
    friend bool less(big_integer const&, big_integer const&, size_t);
    friend void difference(big_integer &a, big_integer const &b, size_t ind);
    friend big_integer div_short(big_integer const&, uint32_t);
    friend big_integer bit_op(big_integer, big_integer, const std::function<uint32_t(uint32_t, uint32_t)>&);
};

big_integer operator-(big_integer const&, big_integer const&);
big_integer operator%(big_integer const&, big_integer const&);
bool operator!=(big_integer const&, big_integer const&);