#ifndef UNSIGNED_NUMBER_H
#define UNSIGNED_NUMBER_H
#include "big_integer.h"

struct big_integer;

struct unsigned_number {
    friend class big_integer;
private:
    size_t size;
    uint32_t* data;
    void shrink_to_fit();
    void swap(unsigned_number& other);
public:
    unsigned_number() {}
    unsigned_number(int num);
    unsigned_number(size_t size, int num);
    unsigned_number(size_t size, const uint32_t* data, bool sign);
    unsigned_number(unsigned_number const& other);
    explicit unsigned_number(std::string const& str);
    ~unsigned_number();

    unsigned_number& operator-=(unsigned_number& rhs);

    unsigned_number& operator=(unsigned_number const& other);
    friend std::pair<unsigned_number, unsigned_number> div_mod(unsigned_number a, unsigned_number b);
    friend unsigned_number operator*(unsigned_number const& a, unsigned_number const& b);
    friend unsigned_number operator/(unsigned_number const& a, unsigned_number const& b);
    friend unsigned_number operator%(unsigned_number const& a, unsigned_number const& b);
    friend unsigned_number operator+(unsigned_number const& a, unsigned_number const& b);
    friend bool operator>(unsigned_number const& a, unsigned_number const& b);
    friend bool operator==(unsigned_number const& a, unsigned_number const& b);
    friend bool operator!=(unsigned_number const& a, unsigned_number const& b);
    friend std::string to_string(unsigned_number a);
};

#endif // UNSIGNED_NUMBER_H
