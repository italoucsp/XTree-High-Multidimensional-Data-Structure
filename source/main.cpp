#include <XTree.hpp>

int main() {
  XTree<double, 14, 10> demo;
  std::cout << typeid(demo).name() << std::endl;
  return 0;
}