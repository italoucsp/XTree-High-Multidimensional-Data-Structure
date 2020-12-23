#include <algorithm>
#include <array>
#include <assert.h>
#include <iostream>
#include <limits>
#include <memory>
#include <stack>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

template<typename T, size_t N>
struct HyperPoint {
  HyperPoint();
  HyperPoint(std::array<T,N> m_val);
  HyperPoint& operator=(const HyperPoint<T, N>& other);
  T& operator[](size_t index);
  T operator[](size_t index) const;
  bool operator==(const HyperPoint<T, N>& other);
  bool operator<(const HyperPoint<T, N>& other);
  bool operator>(const HyperPoint<T, N>& other);
private:
  std::array<T, N> axises;
};

template<typename T, size_t N>
HyperPoint<T, N>::HyperPoint() {
  axises.fill(T(0));
}

template<typename T, size_t N>
HyperPoint<T, N>::HyperPoint(std::array<T, N> m_val) {
  axises.swap(m_val);
}

template<typename T, size_t N>
HyperPoint<T, N>& HyperPoint<T, N>::operator=(const HyperPoint<T, N>& other) {
  for (size_t axis(0); axis < N; ++axis)
    axises[axis] == other.axises[axis];
  return *this;
}

template<typename T, size_t N>
T& HyperPoint<T, N>::operator[](size_t index) {
  assert(index < N);
  return axises[index];
}

template<typename T, size_t N>
T HyperPoint<T, N>::operator[](size_t index) const {
  assert(index < N);
  return axises[index];
}

template<typename T, size_t N>
bool HyperPoint<T, N>::operator==(const HyperPoint<T, N>& other) {
  bool ok = true;
  for (size_t axis(0); axis < N; ++axis)
    ok = ok && (axises[axis] == other.axises[axis]);
  return ok;
}

template<typename T, size_t N>
bool HyperPoint<T, N>::operator<(const HyperPoint<T, N>& other) {
  bool ok = true;
  for (size_t axis(0); axis < N; ++axis)
    ok = ok && (axises[axis] < other.axises[axis]);
  return ok;
}

template<typename T, size_t N>
bool HyperPoint<T, N>::operator>(const HyperPoint<T, N>& other) {
  bool ok = true;
  for (size_t axis(0); axis < N; ++axis)
    ok = ok && (axises[axis] > other.axises[axis]);
  return ok;
}

template<typename T, size_t N>
struct HyperRectangle {
  HyperRectangle() {}
  HyperRectangle(HyperPoint<T, N>& A, HyperPoint<T, N>& B);
  HyperRectangle<T, N>& operator=(const HyperRectangle<T, N>& other);
  bool overlaps(const HyperRectangle<T, N>& other);
  bool contains(const HyperPoint<T, N>& point);
  double get_hyper_area();
  double get_hyper_margin();
  HyperPoint<T, N> get_min_bound();
  HyperPoint<T, N> get_max_bound();
  void expand(const HyperRectangle<T, N>& other);
private:
  HyperPoint<T, N> bottom_left, top_right;
};

template<typename T, size_t N>
HyperRectangle<T, N>::HyperRectangle(HyperPoint<T, N>& A, HyperPoint<T, N>& B) {
  for (size_t axis(0); axis < N; ++axis) {
    bottom_left[axis] = std::min(A[axis], B[axis]);
    top_right[axis] = std::max(A[axis], B[axis]);
  }
}

template<typename T, size_t N>
HyperRectangle<T, N>& HyperRectangle<T, N>::operator=(const HyperRectangle<T, N>& other) {
  bottom_left = other.bottom_left;
  top_right = other.top_right;
  return *this;
}

template<typename T, size_t N>
bool HyperRectangle<T, N>::overlaps(const HyperRectangle<T, N>& other) {
  for (size_t axis(0); axis < N; ++axis) {
    if (!(bottom_left[axis] <= other.top_right[axis] &&
      top_right[axis] >= other.bottom_left[axis])) {
      return false;
    }
  }
  return true;
}

template<typename T, size_t N>
bool HyperRectangle<T, N>::contains(const HyperPoint<T, N>& point) {
  for (size_t axis(0); axis < N; ++axis) {
    if (!(bottom_left[axis] <= point[axis] && point[axis] <= top_right[axis]))
      return false;
  }
  return true;
}

template<typename T, size_t N>
double HyperRectangle<T, N>::get_hyper_area() {
  double hyper_area = double(!(bottom_left == top_right));
  for (size_t axis(0); axis < N; ++axis) {
    hyper_area *= (top_right[axis] - bottom_left[axis]);
  }
  return hyper_area;
}

template<typename T, size_t N>
double HyperRectangle<T, N>::get_hyper_margin() {
  double hyper_margin = 0.0, dim_margin_m = (1 << (N - 1));
  for (size_t axis(0); axis < N; ++axis) {
    hyper_margin += (top_right[axis] - bottom_left[axis]);
  }
  return hyper_margin * dim_margin_m;
}

template<typename T, size_t N>
HyperPoint<T, N> HyperRectangle<T, N>::get_min_bound() {
  return bottom_left;
}

template<typename T, size_t N>
HyperPoint<T, N> HyperRectangle<T, N>::get_max_bound() {
  return top_right;
}

template<typename T, size_t N>
void HyperRectangle<T, N>::expand(const HyperRectangle<T, N>& other) {
  for (size_t axis(0); axis < N; ++axis) {
    bottom_left[axis] = std::min(bottom_left[axis], other.bottom_left[axis]);
    top_right[axis] = std::max(top_right[axis], other.top_right[axis]);
  }
}