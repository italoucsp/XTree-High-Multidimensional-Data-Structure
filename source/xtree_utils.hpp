#include <algorithm>
#include <assert.h>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>


template<typename T, size_t N>
struct HyperPoint {
  HyperPoint();
};

template<typename T, size_t N>
struct HyperRectangle {
  HyperRectangle();
private:
  HyperPoint<T, N> bottom_left, top_right;
};