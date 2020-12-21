#include <algorithm>
#include <array>
#include <assert.h>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <typeinfo>
#include <vector>

template<typename T, size_t N>
struct HyperPoint {
  HyperPoint();
  HyperPoint(std::array<T,N> m_val);
  HyperPoint& operator=(const HyperPoint<T, N>& other);
  T& operator[](size_t index);
  T operator[](size_t index) const;
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
  for (size_t i(0); i < N; ++i)
    axises[i] == other.axises[i];
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
struct HyperRectangle {
  HyperRectangle() {}
  HyperRectangle(HyperPoint<T, N>& A, HyperPoint<T, N>& B);
  HyperRectangle<T, N>& operator=(const HyperRectangle<T, N>& other);
  bool overlaps(const HyperRectangle<T, N>& other);
  bool contains(const HyperPoint<T, N>& point);
private:
  HyperPoint<T, N> bottom_left, top_right;
};

template<typename T, size_t N>
HyperRectangle<T, N>::HyperRectangle(HyperPoint<T, N>& A, HyperPoint<T, N>& B) {
  for (size_t i(0); i < N; ++i) {
    bottom_left[i] = std::min(A[i], B[i]);
    top_right[i] = std::max(A[i], B[i]);
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
  for (size_t i(0); i < N; ++i) {
    if (!(bottom_left[i] <= other.top_right[i] &&
      top_right[i] >= other.bottom_left[i])) {
      return false;
    }
  }
  return true;
}

template<typename T, size_t N>
bool contains(const HyperPoint<T, N>& point) {
  for (size_t i(0); i < N; ++i) {
    if (!(bottom_left[i] <= point[i] && point[i] <= top_right[i]))
      return false;
  }
  return true;
}