#ifndef CROMBIE_STOREPARTICLES_H
#define CROMBIE_STOREPARTICLES_H 1

#include <initializer_list>
#include <utility>
#include <vector>
#include <functional>

#include <iostream>

template<typename E, typename T, typename F>
class ObjectStore {
 public:

  enum class order {
    eAsc,
    eDesc
  };

 ObjectStore(std::initializer_list<E> sorted_enums,
             std::function<F(T*)> compare,
             order which = order::eDesc)
   : compare(compare), which_order(which), total(sorted_enums.size()) {
    for (auto valid_enum : sorted_enums)
      store.push_back({valid_enum, nullptr, 0});
  }

  ~ObjectStore () {}

  struct Particle {
    const E branch;
    T* particle;
    int flag;
  };

  std::vector<Particle> store;

  void check (T& obj, int flag = 0) {
    T* addr = &obj;
    // Get the first empty iterator or the end
    auto it = store.begin() + num_filled;
    if (num_filled < total)
      num_filled++;
    // Insertion sort: find location
    for (;it != store.begin(); --it) {
      if ((compare(addr) < compare((it - 1)->particle)) == (which_order == order::eDesc))
        break;
    }
    // Insertion sort: move elements down
    { // Extra scope so temp_flag doesn't leak
      int temp_flag = 0;
      for(T* temp = nullptr; it != store.end() && addr; ++it) {
        temp = it->particle;
        it->particle = addr;
        addr = temp;

        temp_flag = it->flag;
        it->flag = flag;
        flag = temp_flag;
      }
    }
  }

 private:
  const std::function<F(T*)> compare;
  const order which_order;

  const unsigned total;
  unsigned num_filled = 0;
};

#endif
