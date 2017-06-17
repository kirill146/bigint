#ifndef BIG_INTEGER_BIG_INTEGER_H
#define BIG_INTEGER_BIG_INTEGER_H

#include <string>
#include <vector>
#include <cstring>
#include "unsigned_number.h"

struct unsigned_number;
struct big_integer {
    friend class unsuged_number;
private:
    size_t size = 1;
    union {
        uint32_t small_data[2];
        uint32_t* big_data; //inv: big_data[-1] == ref_cnt
    } number;
    void detach();
    void shrink_to_fit();
    void upd_size(size_t new_capacity);
    uint32_t const* get_data() const;
    uint32_t* get_data();
public:
    big_integer();
    big_integer(int a);
    big_integer(big_integer const& other);
    big_integer(size_t size, uint32_t*& data);
    big_integer(unsigned_number const& other, bool sign);
    explicit big_integer(std::string const& str);
    ~big_integer();
    void swap(big_integer& other);
    bool get_sign() const;
    bool get_sign();
    big_integer& operator=(big_integer const& other);
    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);
    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);
    big_integer& operator<<=(int const& rhs);
    big_integer& operator>>=(int const& rhs);
    big_integer operator~() const;
    big_integer operator-() const;
    big_integer operator+() const;
    big_integer& operator++();
    big_integer operator++(int);
    big_integer& operator--();
    big_integer operator--(int);

    friend big_integer operator+(big_integer a, big_integer const& b);
    friend big_integer operator-(big_integer a, big_integer const& b);
    friend big_integer operator/(big_integer a, big_integer const& b);
    friend big_integer operator%(big_integer a, big_integer const& b);
    friend big_integer operator*(big_integer a, big_integer const& b);
    friend big_integer operator&(big_integer a, big_integer const& b);
    friend big_integer operator|(big_integer a, big_integer const& b);
    friend big_integer operator^(big_integer a, big_integer const& b);
    friend big_integer operator<<(big_integer a, int const& b);
    friend big_integer operator>>(big_integer a, int const& b);
    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);
};

std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif //BIG_INTEGER_BIG_INTEGER_H
