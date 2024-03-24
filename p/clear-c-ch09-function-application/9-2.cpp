#include <iostream>


template <class type>
type reMax(const type x, const type y) {
    return x > y ? x : y;
}


int main() {
    const int x1 = 2, y1 = 3;
    std::cout << reMax(x1, y1) << std::endl;

    const double x2 = 4.5, y2 = 3.8;
    std::cout << reMax(x2, y2) << std::endl;
}