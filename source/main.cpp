#include <XTree.hpp>

struct Controller {
  virtual void get_id() {}
};
struct C1:public Controller { void get_id() { std::cout << "ga" << std::endl; } };
struct C2 :public Controller { void get_id() { std::cout << "aea" << std::endl; } };

int main() {
  XTree<double, 14, 10> demo;
  std::cout << typeid(demo).name() << std::endl;
  std::shared_ptr<Controller> ptr = std::make_shared<C1>();
  ptr->get_id();
  std::cout << typeid(*ptr).name() << std::endl;
  std::cout << (typeid(*ptr) == typeid(C1)) << std::endl;
  ptr = std::make_shared<C2>();
  std::cout << typeid(*ptr).name() << std::endl;
  ptr->get_id();
  std::vector<std::shared_ptr<Controller>> vec(1);
  vec[0] = std::make_shared<C1>();
  //std::cout << typeid(*vec[0]).name() << std::endl;
  std::cout << (typeid(*vec[0]) == typeid(C1)) << std::endl;
  return 0;
}