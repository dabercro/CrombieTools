#ifndef CROMBIE_STOREPARTICLES_H
#define CROMBIE_STOREPARTICLES_H 1

#include <initializer_list>
#include <utility>
#include <vector>
#include <functional>

#include <iostream>

/**
   @ingroup skimminggroup
   @class ObjectStore
   @brief A generic class for storing a short vector of related particles,
   including information about the eventual output branche and extra sidecar info.

   @param E is the enum class used to enumerate branches in the eventual output tree (see crombie maketree) for details.
   @param T is the type of object pointer to store. This object pointer must also be the input type for the comparison function.
   @param S is the type of the sidecar information to store. Can be a complicated struct, if you'd like.
   @param F is the return type of the comparison function.
*/
template<typename E, typename T, typename S = void*, typename F = float>
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
      store.push_back({valid_enum, nullptr, {}});
  }

  ~ObjectStore () {}

  struct Particle {
    const E branch;
    T* particle;
    S extra;
  };

  std::vector<Particle> store;

 void check (T& obj, S extra = {}) {
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
    { // Extra scope so temp_extra doesn't leak
      S temp_extra {};
      for(T* temp = nullptr; it != store.end() && addr; ++it) {
        temp = it->particle;
        it->particle = addr;
        addr = temp;

        temp_extra = it->extra;
        it->extra = extra;
        extra = temp_extra;
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
