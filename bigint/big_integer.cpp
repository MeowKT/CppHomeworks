#include "big_integer.h"

typedef unsigned __int128 uint128_t;

static big_integer abs(big_integer const&);

big_integer::big_integer() : sign(0) {}

big_integer::big_integer(int x) : sign(0)
{
    if (x)
    {
        dig.push_back(std::abs((long long)x));
        if (x < 0)
        {
            *this = -*this;
        }
    }
}

big_integer::big_integer(std::string s)
{
    for (char c : s)
    {
        if (c == '-')
        {
            continue;
        }
        *this *= 10;
        *this += (c - '0');
    }
    if (s[0] == '-')
        *this *= -1;
}

std::string to_string(big_integer x)
{
    bool sign = x.sign;
    x = abs(x);
    std::string digits;
    while (x > 0)
    {
        big_integer r = x % big_integer(10);
        uint32_t c = (r == 0 ? 0 : r.dig.back());
        digits.push_back('0' + c);
        x /= 10;
    }
    if (sign)
    {
        digits.push_back('-');
    }
    std::reverse(digits.begin(), digits.end());
    return (digits.empty() ? "0" : digits);
}

big_integer big_integer::operator~() const
{
    big_integer x(*this);
    for (unsigned int &d : x.dig)
    {
        d ^= UINT32_MAX;
    }
    x.sign ^= (__uint8_t)1;
    x.erase_insignificant_zeros();
    return x;
}

big_integer big_integer::operator-() const
{
    big_integer x(*this);
    x = (~x) + 1;
    x.erase_insignificant_zeros();
    return x;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer operator+(big_integer l, big_integer const &r)
{
    return l += r;
}
big_integer operator-(big_integer const& a, big_integer const& b)
{
    return a + (-b);
}
big_integer operator%(big_integer const& a, big_integer const& b)
{
    return a - a / b * b;
}


big_integer operator*(big_integer const& l, big_integer const& r)
{
    if (l == 0 || r == 0)
    {
        return 0;
    }
    big_integer a = abs(l);
    big_integer b = abs(r);
    big_integer ans;
    ans.resize(a.dig.size() + b.dig.size());
    for (size_t i = 0; i < a.dig.size(); i++)
    {
        uint32_t c = 0;
        for (size_t j = 0; j < b.dig.size(); j++)
        {
            uint64_t tmp = (uint64_t) a.dig[i] * b.dig[j] + ans.dig[i + j] + c;
            ans.dig[i + j] = tmp;
            c = tmp >> 32;
        }
        ans.dig[i + b.dig.size()] += c;
    }
    if (l.sign != r.sign)
    {
        ans = -ans;
    }
    ans.erase_insignificant_zeros();
    return ans;
}

big_integer operator/(big_integer const& l, big_integer const& r)
{
    big_integer a = abs(l);
    big_integer b = abs(r);
    big_integer ans;
    big_integer ml;
    if (a < b)
    {
        return 0;
    }
    if (b.dig.size() == 1)
    {
        ans = div_short(a, b.dig.back());
    } else
    {
        a.dig.push_back(0);
        size_t m = b.dig.size() + 1;
        size_t n = a.dig.size();
        ans.dig.resize(n - m + 1);
        uint32_t quot = 0;
        for (size_t i = m, j = ans.dig.size() - 1; i <= n; i++, j--)
        {
            uint128_t x = ((uint128_t) a.dig[a.dig.size() - 1] << 64)
                        | ((uint128_t) a.dig[a.dig.size() - 2] << 32)
                        | ((uint128_t) a.dig[a.dig.size() - 3]);
            uint128_t y = ((uint128_t) b.dig[b.dig.size() - 1] << 32)
                        | ((uint128_t) b.dig[b.dig.size() - 2]);
            quot = (uint32_t) (x / y);
            ml = b * big_integer(std::to_string(quot));
            if (!less(a, abs(ml), m)) {
                quot--;
                ml -= b;
            }
            ans.dig[j] = quot;
            difference(a, abs(ml), m);
            if (!a.dig.back())
            {
                a.dig.pop_back();
            }
        }
    }
    if (l.sign != r.sign)
    {
        ans = -ans;
    }
    ans.erase_insignificant_zeros();
    return ans;
}

big_integer operator&(big_integer const& a, big_integer const& b)
{
    return bit_op(a, b, [](int32_t a, int32_t b) {return a & b;});
}

big_integer operator|(big_integer const& a, big_integer const& b)
{
    return bit_op(a, b, [](int32_t a, int32_t b) {return a | b;});
}

big_integer operator^(big_integer const& a, big_integer const& b)
{
    return bit_op(a, b, [](int32_t a, int32_t b) {return a ^ b;});
}

big_integer operator>>(big_integer const& _a, int b) {
    big_integer a = _a;
    int k = b / 32;
    b %= 32;
    for (size_t i = k; i < a.dig.size(); i++)
        a.dig[i - k] = a.dig[i];
    a.dig.resize(a.dig.size() - k);
    a.erase_insignificant_zeros();
    big_integer _b = (b == 31 ? big_integer("2147483648") : (1 << b));
    a /= _b;
    if (a < 0 && a * _b != _a) {
        a--;
    }
    return a;
}

big_integer operator<<(big_integer a, int b)
{
    int k = b / 32;
    b %= 32;
    a.resize(a.dig.size() + k);
    for (int i = (int)a.dig.size() - 1; i >= k; i--)
        a.dig[i] = a.dig[i - k];
    for (size_t i = 0; i < k; i++)
        a.dig[i] = 0;
    a.erase_insignificant_zeros();
    a *= (b == 31 ? big_integer("2147483648") : (1 << b));
    return a;
}

big_integer& big_integer::operator++()
{
    return *this += 1;
}

big_integer const big_integer::operator++(int)
{
    big_integer x(*this);
    *this += 1;

    return x;
}

big_integer& big_integer::operator--()
{
    return *this -= 1;
}

big_integer const big_integer::operator--(int)
{
    big_integer x(*this);
    *this -= 1;
    return x;
}

big_integer& big_integer::operator+=(big_integer b)
{
    big_integer& a = *this;
    a.resize(std::max(a.dig.size(), b.dig.size()));
    b.resize(std::max(a.dig.size(), b.dig.size()));
    bool c = false;
    for (size_t i = 0; i < a.dig.size(); i++)
    {
        a.dig[i] += b.dig[i] + c;
        c = a.dig[i] < b.dig[i] + c || (b.dig[i] == UINT32_MAX && c);
    }
    if (a.sign && b.sign)
    {
        if (!c)
        {
            a.dig.push_back(UINT32_MAX - 1);
        }
    } else if (c)
    {
        if (a.sign || b.sign)
        {
            a.sign = false;
        } else
        {
            a.dig.push_back(1);
        }
    } else
    {
        a.sign |= b.sign;
    }
    a.erase_insignificant_zeros();
    return a;
}

big_integer& big_integer::operator-=(const big_integer & r)
{
    return *this += -r;
}

big_integer& big_integer::operator*=(const big_integer & r)
{
    return *this = *this * r;
}

big_integer & big_integer::operator/=(const big_integer & r)
{
    return *this = *this / r;
}

big_integer & big_integer::operator%=(const big_integer & r)
{
    return *this = *this % r;
}

big_integer & big_integer::operator&=(const big_integer & r)
{
    return *this = *this & r;
}

big_integer & big_integer::operator|=(const big_integer & r)
{
    return *this = *this | r;
}

big_integer & big_integer::operator^=(const big_integer & r)
{
    return *this = *this ^ r;
}

big_integer & big_integer::operator>>=(int x)
{
    return *this = *this >> x;
}

big_integer & big_integer::operator<<=(int x)
{
    return *this = *this << x;
}

bool operator==(big_integer const& a, big_integer const& b)
{
    big_integer c = a + (-b);
    return c.dig.empty() && (!c.sign);
}

bool operator<(big_integer const& a, big_integer const& b)
{
    big_integer c = a + (-b);
    return c.sign;
}

bool operator>(big_integer const& a, big_integer const& b)
{
    return (!(a < b)) && (!(a == b));
}

bool operator<=(big_integer const& a, big_integer const& b)
{
    return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b)
{
    return !(a < b);
}

bool operator!=(big_integer const& a, big_integer const& b)
{
    return !(a == b);
}

void big_integer::resize(size_t len)
{
    while (dig.size() < len)
    {
        dig.push_back(sign ? UINT32_MAX : 0);
    }
}

void big_integer::erase_insignificant_zeros()
{
    while (!dig.empty() && dig.back() == (sign ? UINT32_MAX : 0))
    {
        dig.pop_back();
    }
}

big_integer div_short(big_integer const &a, uint32_t d)
{
    big_integer ans;
    uint64_t c = 0;
    for (auto it = a.dig.rbegin(); it != a.dig.rend(); it++)
    {
        uint64_t cur = *it;
        ans.dig.push_back(((c << 32) | cur) / d);
        c = ((c << 32) | cur) % d;
    }
    reverse(ans.dig.begin(), ans.dig.end());
    ans.erase_insignificant_zeros();
    return ans;
}

big_integer bit_op(big_integer a, big_integer b, const std::function<uint32_t(uint32_t, uint32_t)>& f)
{
    a.resize(std::max(a.dig.size(), b.dig.size()));
    b.resize(std::max(a.dig.size(), b.dig.size()));
    big_integer res;
    for (int i = 0; i < a.dig.size(); i++)
    {
        res.dig.push_back(f(a.dig[i], b.dig[i]));
    }
    std::reverse(res.dig.begin(), res.dig.end());
    res.erase_insignificant_zeros();
    res.sign = f(a.sign, b.sign);
    return res;
}

void difference(big_integer &a, big_integer const &b, size_t ind)
{
    size_t start = a.dig.size() - ind;
    bool c = false;
    for (size_t i = 0; i < ind; i++)
    {
        uint32_t x = a.dig[start + i];
        uint32_t y = (i < b.dig.size() ? b.dig[i] : 0);
        uint64_t res = (int64_t) x - y - c;
        c = (y + c > x);
        a.dig[start + i] = (uint32_t)res;
    }
}

bool less(big_integer const &a, big_integer const &b, size_t ind)
{
    for (size_t i = 1; i <= a.dig.size(); i++)
    {
        if (a.dig[a.dig.size() - i] != (ind - i < b.dig.size() ? b.dig[ind - i] : 0))
        {
            return a.dig[a.dig.size() - i] > (ind - i < b.dig.size() ? b.dig[ind - i] : 0);
        }
    }
    return true;
}

static big_integer abs(big_integer const& x)
{
    return x < 0 ? -x : x;
}