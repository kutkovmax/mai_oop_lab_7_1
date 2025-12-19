#pragma once

#include <iostream>
#include <cmath>

class Point {
private:
    int x;
    int y;

public:
    Point();

    Point(int x, int y);

    void set_x(int x);

    void set_y(int y);

    int get_x() const;

    int get_y() const;

    double distance_to(const Point& other) const;
};
