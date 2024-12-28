#include <iostream>

#include "Object.h"


class Foo {
public:
    bool IsInside(double x, double y) const {
        return false;
    }
    void Print() const {
        std::cout << "I am Foo" << std::endl;
    }
};

class Goo {
public:
    bool IsInside(double x, double y) const {
        return true;
    }
    void Print() const {
        std::cout << "I am Goo" << std::endl;
    }
};

int main() {
    Foo foo;
    mock::Object obj1(&foo);
    mock::Object obj2(Goo{});

    obj1.Print();
    obj2.Print();
    return 0;
}