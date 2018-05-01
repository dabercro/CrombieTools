#ifndef CROMBIE_ETAPHIMAP_H
#defind CROMBIE_ETAPHIMAP_H 1

#include <vector>
#include <cmath>
#include <functional>
#include "TMath.h"
#include "TVector2.h"

#include "PlotTools/include/KinematicFunctions.h"

template<typename T>
class EtaPhiMap {
 public:
  EtaPhiMap (double spacing, double maxeta = 5.0,
             std::function<double(const T*)> eta = [](const T* p){ return p.eta(); },
             std::function<double(const T*)> phi = [](const T* p){ return p.phi(); });

  /// Add a collection of particles to the grid. Call once per event, because this gets cleared
  template<C> AddParticles (C& collection);

  /// Get particles within dr of a given eta, phi
  std::vector<const T*> GetParticles(double eta, double phi, double dr);

 private:

  /// Stores all of the particles
  std::vector<std::vector<const T*>> _particles;
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
  /// Get the bin number
  unsigned bin(unsigned etab, unsigned phib);
  /// Get eta bin
  unsigned etabin(double eta);
  /// Get phi bin
  unsigned phibin(double phi);

};


EtaPhiMap::EtaPhiMap(double spacing, double maxeta)
: _spacing{spacing}, _maxeta{maxeta},
  n_etabins{2 * (std::ceil(_maxeta/_spacing) + 1)}, // Add one for over/underflow and times two for positive and negative
  n_phibins{std::ceil(TMath::TwoPi()/_spacing)},    // Always transform phi into [0, 2pi) in bin
  particles(n_etabins * n_phibins) { }


template<C> void EtaPhiMap::AddParticles (C& collection) {

  // Only call this function when adding a full collection
  clear();

  for (auto& cand : collection) {
    auto* ptr = &cand;
    particles[bin(geteta(ptr), getphi(ptr))].push_back(ptr);
  }

}


std::vector<T*> EtaPhiMap::GetParticles(double eta, double phi, double dr) {
  double dr2 = std::pow(dr, 2);

  std::vector<T*> output;

  auto max_eta = eta + dr;
  auto max_phi = phi + dr;

  auto running_eta = eta - dr;
  while (running_eta <= max_eta) {
    auto running_phi = phi - dr;
    while (running_phi <= max_phi) {
      for (auto* particle : particles[bin(running_eta, running_phi)]) {
        if (deltaR2(eta, phi, geteta(particle), getphi(particle)) < dr2)
          output.push_back(particle);
      }
    }
  }

  return output;

}


unsigned EtaPhiMap::bin(double eta, double phi) {
  return bin(etabin(eta), phibin(phi));
}


unsigned EtaPhiMap::bin(unsigned etab, unsigned phib) {
  return n_etabins * phib + etab;
}


unsigned EtaPhiMap::etabin(double eta) {
  return std::max(0u, std::min(n_etabins - 1, n_etabins/2 + static_cast<unsigned>(eta/_spacing)));
}


unsigned EtaPhiMap::phibin(double phi) {
  return TVector2::Phi_0_2pi(phi)/_spacing;
}


void EtaPhiMap::clear() {
  for (auto& grid : particles)
    grid.clear();
}


#endif
