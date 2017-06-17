#include <iostream>
#include "big_integer.h"
#include <string>
#include <climits>

big_integer::big_integer() {
    size = 1;
    number.small_data[0] = 0;
}

big_integer::big_integer(int a) {
    size = 1;
    number.small_data[0] = static_cast<uint32_t> (a);
}

big_integer::big_integer(big_integer const& other) {
    size = other.size;
    if (other.size <= 2) {
        memcpy(number.small_data, other.number.small_data, size * sizeof(uint32_t));
    } else {
        other.number.big_data[-1]++;
        number.big_data = other.number.big_data;
    }
}

big_integer::big_integer(size_t other_size, uint32_t*& other_data) {
    uint32_t* data;
    size = other_size;
    if (size <= 2) {
        data = number.small_data;
    } else {
        number.big_data = static_cast<uint32_t*> (operator new((other_size + 1) * sizeof(uint32_t))) + 1;
        number.big_data[-1] = 1;
        data = number.big_data;
    }
    memcpy(data, other_data, other_size * sizeof(uint32_t));
}

big_integer::big_integer(unsigned_number const& other, bool sign) {
    size = other.size + 1;
    uint32_t* data;
    if (size <= 2) {
        data = number.small_data;
    } else {
        number.big_data = static_cast<uint32_t*> (operator new((size + 1) * sizeof(uint32_t))) + 1;
        number.big_data[-1] = 1;
        data = number.big_data;
    }
    if (sign) {
        bool add = true;
        for (size_t i = 0; i < size - 1; i++) {
            data[i] = (~other.data[i]);
            if (add) {
                data[i] += 1;
                if (data[i] != 0) {
                    add = false;
                }
            }
        }
        if (add) {
            data[size - 1] = 0;
        } else {
            data[size - 1] = ~0u;
        }
    } else {
        data[0] = other.data[0];
        memcpy(data, other.data, other.size * sizeof(uint32_t));
        data[size - 1] = 0;
    }
    shrink_to_fit();
}

big_integer::big_integer(std::string const& str) {
    if (str[0] == '-') {
        *this = big_integer(unsigned_number(str.substr(1)), true);
    } else {
        *this = big_integer(unsigned_number(str), false);
    }
}

big_integer::~big_integer() {
    if (size > 2) {
        if (number.big_data[-1] == 1) {
            operator delete (number.big_data - 1);
        } else {
            number.big_data[-1]--;
        }
    }
}

void big_integer::detach() {
    if (size <= 2 || number.big_data[-1] == 1) {
        return;
    }
    big_integer res(size, number.big_data);
    swap(res);
}

uint32_t* big_integer::get_data() {
    if (size <= 2) {
        return number.small_data;
    }
    return number.big_data;
}

uint32_t const* big_integer::get_data() const {
    if (size <= 2) {
        return number.small_data;
    }
    return number.big_data;
}

bool big_integer::get_sign() {
    return (bool) (get_data()[size - 1] >> 31);
}

bool big_integer::get_sign() const {
    return (bool) (get_data()[size - 1] >> 31);
}

big_integer operator/(big_integer a, big_integer const& b) {
    return a /= b;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    detach();
    upd_size(std::max(rhs.size, size) + 1);
    uint32_t rem = 0;
    uint32_t* data = get_data();
    const uint32_t* rhs_data = rhs.get_data();
    for (size_t i = 0; i < size; i++) {
        uint64_t k = (uint64_t) data[i] + (i < rhs.size ? rhs_data[i] : rhs.get_sign() ? ~0u : 0) + rem;
        data[i] = (uint32_t) k;
        rem = (uint32_t) (k > UINT_MAX);
    }
    shrink_to_fit();
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    return *this += -rhs;
}

big_integer operator%(big_integer a, big_integer const& b) {
    return a %= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
    return a *= b;
}

void big_integer::swap(big_integer& other) {
    std::swap(number, other.number);
    std::swap(size, other.size);
}

big_integer& big_integer::operator=(big_integer const& other) {
    big_integer copy(other);
    swap(copy);
    return *this;
}

void big_integer::shrink_to_fit() {
    uint32_t* data = get_data();
    if (get_sign()) {
        while (size > 1 && data[size - 1] == ~0u && (size - 2 >= 0 ? (bool)(data[size - 2] >> 31) : true)) {
            --size;
        }
    } else {
        while (size > 1 && data[size - 1] == 0 && (size - 2 >= 0 ? !(bool)(data[size - 2] >> 31) : true)) {
            --size;
        }
    }
    if (size <= 2 && data != number.small_data) {
        uint32_t* p = number.big_data - 1;
        number.small_data[0] = data[0];
        number.small_data[1] = data[1];
        delete p;
    }
}

void big_integer::upd_size(size_t new_size) {
    detach();
    bool sign = get_sign();
    if (size < new_size) {
        if (new_size == 2) {
            number.small_data[1] = (sign ? ~0u : 0);
            return;
        }
        if (size <= 2) {
            uint32_t data[2];
            memcpy(data, number.small_data, size * sizeof(uint32_t));
            number.big_data = static_cast<uint32_t*> (operator new((new_size + 1) * sizeof(uint32_t))) + 1;
            number.big_data[-1] = 1;
            memcpy(number.big_data, data, size * sizeof(uint32_t));
            memset(number.big_data + size, sign ? ~0u : 0, (new_size - size) * sizeof(uint32_t));
            size = new_size;
            return;
        }
        big_integer copy;
        copy.number.big_data = static_cast<uint32_t*> (operator new((new_size + 1) * sizeof(uint32_t))) + 1;
        copy.number.big_data[-1] = 1;
        memcpy(copy.number.big_data, get_data(), size * sizeof(uint32_t));
        memset(copy.number.big_data + size, sign ? ~0u : 0, (new_size - size) * sizeof(uint32_t));
        copy.size = new_size;
        swap(copy);
    }
}

big_integer big_integer::operator~() const {
    big_integer res(*this);
    res.detach();
    uint32_t* res_data = res.get_data();
    for (size_t i = 0; i < size; i++) {
        res_data[i] = ~res_data[i];
    }
    res.shrink_to_fit();
    return res;
}

big_integer big_integer::operator-() const {
    return ~*this + 1;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer &big_integer::operator&=(big_integer const& rhs) {
    upd_size(std::max(size, rhs.size));
    const uint32_t* rhs_data = rhs.get_data();
    uint32_t* data = get_data();
    for (size_t i = 0; i < size; i++) {
        data[i] &= rhs_data[i];
    }
    shrink_to_fit();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const& rhs) {
    upd_size(std::max(size, rhs.size));
    const uint32_t* rhs_data = rhs.get_data();
    uint32_t* data = get_data();
    for (size_t i = 0; i < size; i++) {
        data[i] |= rhs_data[i];
    }
    shrink_to_fit();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const& rhs) {
    upd_size(std::max(size, rhs.size));
    const uint32_t* rhs_data = rhs.get_data();
    uint32_t* data = get_data();
    for (size_t i = 0; i < size; i++) {
        data[i] ^= rhs_data[i];
    }
    shrink_to_fit();
    return *this;
}

big_integer &big_integer::operator<<=(int const& rhs) {
    upd_size(size + (rhs >> 5) + 1);
    uint32_t* data = get_data();
    for (int i = size - 1; i >= (rhs >> 5); i--) {
        data[i] = (data[i - (rhs >> 5)] << (rhs % 32)) +
                ((i - (rhs >> 5)) && rhs % 32 != 0 ? (data[i - (rhs >> 5) - 1] >> (32 - rhs % 32)) : 0);
    }
    for (int i = 0; i < (rhs >> 5); i++) {
        data[i] = 0;
    }
    shrink_to_fit();
    return *this;
}

big_integer &big_integer::operator>>=(int const& rhs) {
    detach();
    bool sign = get_sign();
    uint32_t* data = get_data();
    if (rhs % 32 == 0) {
        for (int i = 0; i < (int)size - (rhs >> 5); i++) {
            data[i] = data[i + (rhs >> 5)];
        }
        for (int i = std::max((int)size - (rhs >> 5), 0); i < (int)size; i++) {
            data[i] = (sign ? ~0u : 0);
        }
    } else {
        for (int i = 0; i < (int) size - (rhs >> 5) - 1; i++) {
            data[i] = (data[i + (rhs >> 5) + 1] << (32 - rhs % 32)) +
                      (data[i + (rhs >> 5)] >> rhs % 32);
        }
        if (sign) {
            data[size - (rhs >> 5) - 1] = (~0u << (32 - rhs % 32)) + (data[size - 1] >> (rhs % 32));
        } else {
            data[size - (rhs >> 5) - 1] = (data[size - 1] >> (rhs % 32));
        }
        for (int i = std::max((int) size - (rhs >> 5), 0); i < (int)size; i++) {
            data[i] = (sign ? ~0u : 0);
        }
    }
    shrink_to_fit();
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    return *this = big_integer(
                    unsigned_number(size, get_data(), get_sign()) *
                    unsigned_number(rhs.size, rhs.get_data(), rhs.get_sign()),
                    get_sign() != rhs.get_sign());
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    return *this = big_integer(
                    div_mod(unsigned_number(size, get_data(), get_sign()),
                            unsigned_number(rhs.size, rhs.get_data(), rhs.get_sign()))
                    .first, get_sign() != rhs.get_sign());
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this = big_integer(
                    div_mod(unsigned_number(size, get_data(), get_sign()),
                            unsigned_number(rhs.size, rhs.get_data(), rhs.get_sign()))
                    .second, get_sign());
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer res(*this);
    ++*this;
    return res;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer res(*this);
    --*this;
    return res;
}

big_integer operator+(big_integer a, big_integer const& b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
    return a -= b;
}

bool operator==(big_integer const& a, big_integer const& b) {
    if (a.size != b.size) {
        return false;
    }
    const uint32_t* a_data = a.get_data();
    const uint32_t* b_data = b.get_data();
    for (size_t i = 0; i < a.size; i++) {
        if (a_data[i] != b_data[i]) {
            return false;
        }
    }
    return true;
}

bool operator<(big_integer const& a, big_integer const& b) {
    bool sign_a = a.get_sign();
    bool sign_b = b.get_sign();
    const uint32_t* a_data = a.get_data();
    const uint32_t* b_data = b.get_data();
    if (sign_a == 1 && sign_b == 0) {
        return true;
    }
    if (sign_a == 0 && sign_b == 1) {
        return false;
    }
    if (sign_a == 0 && sign_b == 0) {
        if (a.size < b.size) {
            return true;
        }
        if (a.size > b.size) {
            return false;
        }
        for (int i = a.size - 1; i >= 0; i--) {
            if (a_data[i] < b_data[i]) {
                return true;
            }
            if (a_data[i] > b_data[i]) {
                return false;
            }
        }
        return false;
    }
    if (a.size < b.size) {
        return false;
    }
    if (a.size > b.size) {
        return true;
    }
    for (int i = a.size - 1; i >= 0; i--) {
        if (a_data[i] < b_data[i]) {
            return true;
        }
        if (a_data[i] > b_data[i]) {
            return false;
        }
    }
    return false;
}

bool operator>(big_integer const& a, big_integer const& b) {
    return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b) {
    return (a < b) || (a == b);
}

bool operator>=(big_integer const& a, big_integer const& b) {
    return (b <= a);
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return !(a == b);
}

big_integer operator&(big_integer a, big_integer const& b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)  {
    return a ^= b;
}

big_integer operator<<(big_integer a, int const& b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int const& b) {
    return a >>= b;
}

std::string to_string(big_integer const& a) {
    return (a.get_sign() ? "-" : "") + to_string(unsigned_number(a.size, a.get_data(), a.get_sign()));
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    s << to_string(a);
    return s;
}
