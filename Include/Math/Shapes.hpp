#pragma once
#include "Vector.hpp"

namespace Edvar::Math {
struct Rectanglei {
    Rectanglei(const int left, const int right, const int top, const int bottom) : positions(left, right, top, bottom) {}

private:
    Vector4<int> positions; // x, y, top, bottom
};

struct Boxi {
    Boxi(const int inLeft, const int inTop, const int inFront, const int inRight, const int inBottom, const int inBack)
        : left(inLeft), top(inTop), front(inFront), right(inRight), bottom(inBottom), back(inBack) {}

private:
    int left;
    int top;
    int front;
    int right;
    int bottom;
    int back;
};

} // namespace Edvar::Math