#ifndef CROMBIE_STOREPARTICLES_H
#define CROMBIE_STOREPARTICLES_H 1

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

 ObjectStore(const std::vector<E> sorted_enums,
             const std::function<F(T*)> compare,
             const order which = order::eDesc)
   : compare(compare), which_order(which), total(sorted_enums.size()) {
    for (auto valid_enum : sorted_enums)
      store.push_back({valid_enum, nullptr, {}, {}});
  }

  ~ObjectStore () {}

  struct Particle {
    const E branch;
    T* particle;
    S extra;
    F result;
  };

  std::vector<Particle> store;

  void check (T& obj, S extra = {}) {
    T* particle = &obj;
    F result = compare(particle);
    // Get the first empty iterator or the end
    auto it = store.begin() + num_filled;
    if (num_filled < total)
      num_filled++;
    // Insertion sort: find location
    for (;it != store.begin(); --it) {
      if ((result < (it - 1)->result) == (which_order == order::eDesc))
        break;
    }
    // Insertion sort: move elements down
    for(; it != store.end() && particle; ++it) {
      std::swap(it->particle, particle);
      std::swap(it->result, result);
      std::swap(it->extra, extra);
    }
  }

 private:
  const std::function<F(T*)> compare;
  const order which_order;

  const unsigned total;
  unsigned num_filled = 0;
};

#endif
