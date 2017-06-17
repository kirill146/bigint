#include <iostream>
#include <big_integer.h>
using namespace std;



int main() {
    big_integer a( "10000000000000000000000000000000000000000000000000000");
    big_integer c("-10000000000000000000000000000000000000000000000000000");
    cout << -a << endl << -c << endl << -a << endl << -c << endl;
    return 0;
}
