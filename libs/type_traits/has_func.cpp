#include <lulib/type_traits/has_func.hpp>



class Foo {
public:
  void hoge() {}
  void hoge(int) {}
};
 
class Bar : public Foo {
};
 
class Moo {
};


LULIB_GEN_HAS_FUNC(hoge)
LULIB_GEN_HAS_FUNC(hoge, int)

int main() {
}
