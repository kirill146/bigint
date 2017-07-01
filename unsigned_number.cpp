#include "unsigned_number.h"
#include <iostream>

unsigned_number::unsigned_number(size_t size, int num) {
    this->size = size;
    data = static_cast<uint32_t*> (operator new(size * sizeof(uint32_t)));
    memset(data, num, size * sizeof(uint32_t));
}

unsigned_number::unsigned_number(int num) {
    size = 1;
    data = static_cast<uint32_t*> (operator new(sizeof(uint32_t)));
    data[0] = num;
}

unsigned_number::unsigned_number(size_t size, const uint32_t* data, bool sign) {
    this->size = size;
    this->data = static_cast<uint32_t*> (operator new(size * sizeof(uint32_t)));
    memcpy(this->data, data, size * sizeof(uint32_t));
    if (sign) {
        bool add = true;
        for (size_t i = 0; i < size; i++) {
            this->data[i] = (~this->data[i]);
            if (add) {
                this->data[i] += 1;
                if (this->data[i] != 0) {
                    add = false;
                }
            }
        }
    }
    shrink_to_fit();
}

unsigned_number::unsigned_number(unsigned_number const& other) {
    size = other.size;
    data = static_cast<uint32_t*> (operator new(size * sizeof(uint32_t)));
    memcpy(data, other.data, size * sizeof(uint32_t));
}

unsigned_number::unsigned_number(std::string const& str) {
    size = 1;
    data = static_cast<uint32_t*> (operator new(sizeof(uint32_t)));
    data[0] = 0;
    unsigned_number const TEN = 10;
    for (size_t i = 0; i < str.length(); i++) {
        *this = *this * TEN + (str[i] - '0');
    }
}

unsigned_number::~unsigned_number() {
    operator delete(data);
}

void unsigned_number::shrink_to_fit() {
    while (size > 1 && data[size - 1] == 0) {
        size--;
    }
}

void unsigned_number::swap(unsigned_number& other) {
    std::swap(size, other.size);
    std::swap(data, other.data);
}

bool operator==(unsigned_number const& a, unsigned_number const& b) {
    if (a.size != b.size) {
        return false;
    }
    for (size_t i = 0; i < a.size; i++) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(unsigned_number const& a, unsigned_number const& b) {
    return !(a == b);
}

std::string to_string(unsigned_number a) {
    std::string res;
    if (a == 0) {
        return "0";
    }
    unsigned_number const TEN = 10;
    while (a != 0) {
        res = std::to_string((a % TEN).data[0]) + res;
        a = a / TEN;
    }
    return res;
}

bool operator>(unsigned_number const& a, unsigned_number const& b) {
    if (a.size > b.size) {
        return true;
    }
    if (a.size < b.size) {
        return false;
    }
    for (int i = a.size; i--; ) {
        if (a.data[i] < b.data[i]) {
            return false;
        } else if (a.data[i] > b.data[i]) {
            return true;
        }
    }
    return false;
}

unsigned_number operator*(unsigned_number const& a, unsigned_number const& b) {
    unsigned_number ans(a.size + b.size, 0);
    for (size_t ib = 0; ib < b.size; ++ib) {
        uint64_t t = 0, tt = 0;
        for (size_t ic = 0; ic < a.size; ++ic) {
            tt = (uint64_t) b.data[ib] * a.data[ic] + (tt >> 32);
            t = (t >> 32) + ans.data[ib + ic] + (uint32_t)tt;
            ans.data[ib + ic] = (uint32_t) t;
        }
        ans.data[ib + a.size] = (tt >> 32) + (t >> 32);
    }
    ans.shrink_to_fit();
    return ans;
}

unsigned_number& unsigned_number::operator=(unsigned_number const& other) {
    unsigned_number copy(other);
    swap(copy);
    return *this;
}

unsigned_number operator+(unsigned_number const& a, unsigned_number const& b) {
    unsigned_number ans(std::max(a.size, b.size) + 1, 0);
    uint64_t t = 0;
    for (int i = 0; i < (int)ans.size; i++) {
        t = (uint64_t) (i < (int)a.size ? a.data[i] : 0) + (i < (int)b.size ? b.data[i] : 0) + (t >> 32);
        ans.data[i] = (uint32_t) t;
    }
    ans.shrink_to_fit();
    return ans;
}

unsigned_number& unsigned_number::operator-=(unsigned_number& rhs) {
    uint64_t t = 0;
    for (size_t i = 0; i < size; i++) {
        t = (uint64_t) data[i] - (i < rhs.size ? rhs.data[i] : 0) - (t >> 63);
        data[i] = (uint32_t) t;
    }
    shrink_to_fit();
    return *this;
}

std::pair<unsigned_number, unsigned_number> div_mod(unsigned_number a, unsigned_number b) {
    uint32_t norm = 0;
    while((b.data[b.size - 1] >> (31 - norm) & 1) == 0) {
        norm++;
    }
    a = a * (1 << norm);
    b = b * (1 << norm);
    unsigned_number ans((size_t) std::max(1, (int)a.size - (int)b.size + 1), 0);
    unsigned_number cur_dividend(b.size + 1, 0);
    memcpy(cur_dividend.data, a.data + a.size - b.size, b.size * sizeof(uint32_t));
    cur_dividend.data[cur_dividend.size - 1] = 0;
    for (int ia = a.size - b.size - 1; ia >= -1; ia--) {
        uint64_t t = (((uint64_t) cur_dividend.data[cur_dividend.size - 1]) << 32) +
                cur_dividend.data[cur_dividend.size - 2];
        uint32_t cur = (uint32_t) (t / b.data[b.size - 1]);
        unsigned_number subtrahend(b * cur);
        while (subtrahend > cur_dividend) {
            cur--;
            subtrahend -= b;
        }
        ans.data[ans.size + ia - a.size + b.size] = cur;
        uint32_t mem0 = (ia == -1 ? 0 : a.data[ia]);
        uint32_t rem = 0;
        for (int ic = 0; ic < (int)cur_dividend.size; ic++) {
            int64_t mem1 = (int64_t)cur_dividend.data[ic] - subtrahend.data[ic] - rem;
            rem = (uint32_t) (mem1 < 0);
            cur_dividend.data[ic] = mem0;
            mem0 = (uint32_t) mem1;
        }
    }
    for (size_t i = 0; i < cur_dividend.size - 1; i++) {
        cur_dividend.data[i] = cur_dividend.data[i + 1];
    }
    cur_dividend.size--;
    for (int i = 0; i < (int)cur_dividend.size; i++) {
        cur_dividend.data[i] = (norm ? (i == (int)cur_dividend.size - 1 ? 0 : cur_dividend.data[i + 1]) << (32 - norm) : 0) + (cur_dividend.data[i] >> norm);
    }
    cur_dividend.shrink_to_fit();
    ans.shrink_to_fit();
    return std::make_pair(ans, cur_dividend);
}

unsigned_number operator/(unsigned_number const& a, unsigned_number const& b) {
    return div_mod(a, b).first;
}

unsigned_number operator%(unsigned_number const& a, unsigned_number const& b) {
    return div_mod(a, b).second;
}
