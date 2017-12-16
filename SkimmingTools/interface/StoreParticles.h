#include <initializer_list>
#include <utility>
#include <vector>
#include <functional>

#include <iostream>

template<typename E, typename T, typename F>
class ObjectStore {
 public:

  ObjectStore(std::initializer_list<E> sorted_enums, std::function<F(T*)> compare) : compare(compare) {
    for (auto valid_enum : sorted_enums)
      store.push_back({valid_enum, nullptr});
    total = store.size();
  }

  ~ObjectStore () {}

  std::vector<std::pair<E, T*>> store;

  void check (T& obj) {
    T* addr = &obj;
    // Get the first empty iterator or the end
    auto it = store.begin() + num_filled;
    if (num_filled < total)
      num_filled++;
    // Insertion sort: find location
    for (;it != store.begin(); --it) {
      if (compare(addr) < compare((it - 1)->second))
        break;
    }
    // Insertion sort: move elements down
    for(T* temp = nullptr; it != store.end() && addr; ++it) {
      temp = it->second;
      it->second = addr;
      addr = temp;
    }
  }

 private:
  std::function<F(T*)> compare;
  unsigned num_filled = 0;
  unsigned total;
};
