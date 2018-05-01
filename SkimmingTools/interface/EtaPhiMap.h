#ifndef CROMBIE_ETAPHIMAP_H
#define CROMBIE_ETAPHIMAP_H 1

#include <vector>
#include <cmath>
#include <functional>
#include "TMath.h"
#include "TVector2.h"

#include "PlotTools/interface/KinematicFunctions.h"

template<typename T>
class EtaPhiMap {
 public:
  EtaPhiMap (double spacing, double maxeta = 5.0,
             std::function<double(const T*)> eta = [](const T* p){ return p->eta(); },
             std::function<double(const T*)> phi = [](const T* p){ return p->phi(); })
    : _spacing{spacing}, _maxeta{maxeta},
      n_etabins{2 * (static_cast<unsigned>(std::ceil(_maxeta/_spacing)) + 1)}, // Add one for over/underflow and times two for positive and negative
      n_phibins{static_cast<unsigned>(std::ceil(TMath::TwoPi()/_spacing))},    // Always transform phi into [0, 2pi) in bin
      geteta{eta}, getphi{phi} {

        particles.resize(n_etabins * n_phibins);

      }

  /// Add a collection of particles to the grid. Call once per event, because this gets cleared
  template<typename C> void AddParticles (C& collection);

  /// Get particles within dr of a given eta, phi
  std::vector<const T*> GetParticles(double eta, double phi, double dr);

 private:

  /// Stores all of the particles
  std::vector<std::vector<const T*>> particles;
  /// Spacing of each grid
  double _spacing;
  /// Max eta of grid (has overflow and underflow bins)
  double _maxeta;

  /// Number of eta bins
  unsigned n_etabins;
  /// Number of phi bins
  unsigned n_phibins;

  std::function<double(const T*)> geteta;  ///< Function for getting eta from pointers
  std::function<double(const T*)> getphi;  ///< Function for getting phi from pointers

  /// Reset the particles in each grid point
  void clear();
  /// Get the bin number
  unsigned bin(double eta, double phi);
  /// Get eta bin
  unsigned etabin(double eta);
  /// Get phi bin
  unsigned phibin(double phi);

};


template<typename T>
template<typename C>
void EtaPhiMap<T>::AddParticles (C& collection) {

  // Only call this function when adding a full collection
  clear();

  for (auto& cand : collection) {
    auto* ptr = &cand;
    particles[bin(geteta(ptr), getphi(ptr))].push_back(ptr);
  }

}


template<typename T>
std::vector<const T*> EtaPhiMap<T>::GetParticles(double eta, double phi, double dr) {
  double dr2 = std::pow(dr, 2);

  std::vector<const T*> output;

  auto max_eta = eta + dr;
  auto max_phi = phi + dr;

  auto running_eta = eta - dr;
  while (running_eta <= max_eta) {
    auto running_phi = phi - dr;
    while (running_phi <= max_phi) {  // This is the easiest way to not worry about phi wrapping
      for (auto* particle : particles[bin(running_eta, running_phi)]) {
        if (deltaR2(eta, phi, geteta(particle), getphi(particle)) < dr2)
          output.push_back(particle);
      }
      running_phi += _spacing;
    }
    running_eta += _spacing;
  }

  return output;

}


template<typename T>
unsigned EtaPhiMap<T>::bin(double eta, double phi) {
  return n_etabins * phibin(phi) + etabin(eta);
}

template<typename T>
unsigned EtaPhiMap<T>::etabin(double eta) {
  return std::max(0u, std::min(n_etabins - 1, n_etabins/2 + static_cast<unsigned>(eta/_spacing)));
}

template<typename T>
unsigned EtaPhiMap<T>::phibin(double phi) {
  return TVector2::Phi_0_2pi(phi)/_spacing;
}


template<typename T>
void EtaPhiMap<T>::clear() {
  for (auto& grid : particles)
    grid.clear();
}


#endif
