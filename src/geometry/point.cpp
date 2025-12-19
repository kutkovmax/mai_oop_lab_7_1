#include "../../include/geometry/point.h"

Point::Point() : x(0), y(0) {}

Point::Point(int x, int y) : x(x), y(y) {}

int Point::get_x() const { return x; }

int Point::get_y() const { return y; }

void Point::set_x(int x) { this->x = x; }

void Point::set_y(int y) { this->y = y; }

double Point::distance_to(const Point& other) const { return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2)); }
