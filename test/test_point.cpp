#include "../include/geometry/point.h"
#include <gtest/gtest.h>

TEST(PointTest, Test1) {
    Point p;
    EXPECT_EQ(p.get_x(), 0);
    EXPECT_EQ(p.get_y(), 0);
}

TEST(PointTest, Test2) {
    Point p(10, 20);
    EXPECT_EQ(p.get_x(), 10);
    EXPECT_EQ(p.get_y(), 20);
    
    Point p2(-5, -10);
    EXPECT_EQ(p2.get_x(), -5);
    EXPECT_EQ(p2.get_y(), -10);
}

TEST(PointTest, Test3) {
    Point p1(3, 4);
    EXPECT_EQ(p1.get_x(), 3);
    EXPECT_EQ(p1.get_y(), 4);
    
    Point p2(-2, 5);
    EXPECT_EQ(p2.get_x(), -2);
    EXPECT_EQ(p2.get_y(), 5);
}


TEST(PointTest, Test4) {
    Point p;
    
    p.set_x(15);
    EXPECT_EQ(p.get_x(), 15);
    EXPECT_EQ(p.get_y(), 0); 
    
    p.set_y(25);
    EXPECT_EQ(p.get_x(), 15);
    EXPECT_EQ(p.get_y(), 25);
    
    p.set_x(-10);
    p.set_y(-20);
    EXPECT_EQ(p.get_x(), -10);
    EXPECT_EQ(p.get_y(), -20);
}

TEST(PointTest, Test5) {
    Point p1(5, 5);
    EXPECT_DOUBLE_EQ(p1.distance_to(p1), 0.0);
    
    Point p2(-3, 7);
    EXPECT_DOUBLE_EQ(p2.distance_to(p2), 0.0);
}

TEST(PointTest, Test6) {
    Point origin(0, 0);
    Point p1(3, 4);
    EXPECT_DOUBLE_EQ(origin.distance_to(p1), 5.0);
    
    Point p2(-2, 5);
    EXPECT_DOUBLE_EQ(origin.distance_to(p2), std::sqrt(29.0));
    
    Point p3(0, 5);
    EXPECT_DOUBLE_EQ(origin.distance_to(p3), 5.0);
}

TEST(PointTest, Test7) {
    Point p1(3, 4);
    Point p2(7, -1);
    double expected = std::sqrt(std::pow(3-7, 2) + std::pow(4-(-1), 2));
    EXPECT_DOUBLE_EQ(p1.distance_to(p2), expected);
    
    Point p3(1, 1);
    Point p4(4, 5);
    expected = std::sqrt(std::pow(1-4, 2) + std::pow(1-5, 2));
    EXPECT_DOUBLE_EQ(p3.distance_to(p4), expected);
}

TEST(PointTest, Test8) {
    Point p1(-1, -1);
    Point p2(-4, -5);
    double expected = std::sqrt(std::pow(-1 - (-4), 2) + std::pow(-1 - (-5), 2));
    EXPECT_DOUBLE_EQ(p1.distance_to(p2), expected);
    
    Point p3(2, 3);
    Point p4(-2, -3);
    expected = std::sqrt(std::pow(2 - (-2), 2) + std::pow(3 - (-3), 2));
    EXPECT_DOUBLE_EQ(p3.distance_to(p4), expected);
}

TEST(PointTest, Test9) {
    Point p1(3, 4);
    Point p2(7, 1);
    EXPECT_DOUBLE_EQ(p1.distance_to(p2), p2.distance_to(p1));
    
    Point p3(-2, 5);
    Point p4(3, -1);
    EXPECT_DOUBLE_EQ(p3.distance_to(p4), p4.distance_to(p3));
}

TEST(PointTest, Test10) {
    Point original(5, 5);
    Point other(2, 2);
    
    double distance = original.distance_to(other);
    
    EXPECT_EQ(original.get_x(), 5);
    EXPECT_EQ(original.get_y(), 5);
    EXPECT_EQ(other.get_x(), 2);
    EXPECT_EQ(other.get_y(), 2);
}

TEST(PointTest, Test11) {
    Point p1(1000, 1000);
    Point p2(1000, 1000);
    EXPECT_DOUBLE_EQ(p1.distance_to(p2), 0.0);
    
    Point p3(0, 0);
    Point p4(1000, 0);
    EXPECT_DOUBLE_EQ(p3.distance_to(p4), 1000.0);
}

TEST(PointTest, Test12) {
    Point p1(0, 0);
    Point p2(0, 0);
    EXPECT_DOUBLE_EQ(p1.distance_to(p2), 0.0);
    
    Point p3(0, 10);
    Point p4(0, 5);
    EXPECT_DOUBLE_EQ(p3.distance_to(p4), 5.0);
}

TEST(PointTest, Test13) {
    Point p;
    
    p.set_x(1);
    p.set_y(2);
    EXPECT_EQ(p.get_x(), 1);
    EXPECT_EQ(p.get_y(), 2);
    
    p.set_x(10);
    EXPECT_EQ(p.get_x(), 10);
    EXPECT_EQ(p.get_y(), 2);
    
    p.set_y(20);
    EXPECT_EQ(p.get_x(), 10);
    EXPECT_EQ(p.get_y(), 20);
}