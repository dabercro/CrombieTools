/**
  @file   KinematicFunctions.h
  Header file containing various functions used to do quick kinematics cuts.
  @todo Optimize the functions here that use TLorentzVector. They are probably not needed.
  @author Daniel Abercrombie <dabercro@mit.edu>
  @author Zeynep Demiragli <zeynep.demiragli@cern.ch>
*/

#ifndef KINEMATICFUNCTIONS_H
#define KINEMATICFUNCTIONS_H

#include "TVector2.h"
#include "TLorentzVector.h"

namespace crombie {
  const double pi = 3.14159265358979323846;
}

using crombie::pi;

/// Make phi that are from [0, 2pi) to [-pi, pi)
double make_pm(double phi) {
  if (phi >= pi)
    phi -= 2*pi;
  return phi;
}

/// Make phi that are from [-pi, pi) to [0, 2pi)
double make_2pi(double phi) {
  if (phi < 0)
    phi += 2*pi;
  return phi;
}

/// Calculate deltaPhi between two particles
double deltaPhi(double phi1, double phi2) {
  // Make phi between +- pi, if -pi < phi < 3*pi
  return fabs(make_pm(fabs(make_pm(phi1) - make_pm(phi2))));
}

/// Calculate deltaR2 between two particles
double deltaR2(double eta1, double phi1, double eta2, double phi2) {
  auto d_eta = eta2 - eta1;
  auto d_phi = deltaPhi(phi1, phi2);
  return d_eta * d_eta + d_phi * d_phi;
}

/// Calculate deltaR between two particles
double deltaR(double eta1, double phi1, double eta2, double phi2) {
  return sqrt(deltaR2(eta1, phi1, eta2, phi2));
}

/// Get TVector2 for vector in the transverse plane
TVector2 vectorSum(double pt1, double phi1, double pt2, double phi2) {
  TVector2 vec1;
  TVector2 vec2;
  vec1.SetMagPhi(pt1, phi1);
  vec2.SetMagPhi(pt2, phi2);
  vec1 += vec2;
  return vec1;
}

/// Phi of a mother of two particles
double vectorSumPhi(double pt1, double phi1, double pt2, double phi2) {
  return vectorSum(pt1, phi1, pt2, phi2).Phi();
}

/// Pt of a mother of two particles
double vectorSumPt(double pt1, double phi1, double pt2, double phi2) {
  return vectorSum(pt1, phi1, pt2, phi2).Mod();
}

/// Pt of a mother of three particles
double vectorSum3Pt(double pt1, double phi1, double pt2, double phi2,double pt3, double phi3) {
  return sqrt( pow(pt1*cos(phi1) + pt2*cos(phi2) + pt3*cos(phi3),2) +
	       pow(pt1*sin(phi1) + pt2*sin(phi2) + pt3*sin(phi3),2) );
}

/// TLorentz Vector of two particles
TLorentzVector vectorSum(double pt1, double eta1, double phi1, double mass1, double pt2, double eta2, double phi2, double mass2) {
  TLorentzVector vec1;
  TLorentzVector vec2;
  vec1.SetPtEtaPhiM(pt1,eta1,phi1,mass1);
  vec2.SetPtEtaPhiM(pt2,eta2,phi2,mass2);
  return vec1 + vec2;
}

/// Mass of a mother of two massive particles
double vectorSumMass(double pt1, double eta1, double phi1, double mass1, double pt2, double eta2, double phi2, double mass2) {
  return vectorSum(pt1, eta1, phi1, mass1, pt2, eta2, phi2, mass2).M();
}

/// Mass of a mother of two massless particles
double vectorSumMass(double pt1, double eta1, double phi1, double pt2, double eta2, double phi2) {
  return vectorSumMass(pt1, eta1, phi1, 0.0, pt2, eta2, phi2, 0.0);
}

/// Eta of a mother of two particles
double vectorSumEta(double pt1, double eta1, double phi1, double mass1, double pt2, double eta2, double phi2, double mass2) {
  return vectorSum(pt1, eta1, phi1, mass1, pt2, eta2, phi2, mass2).Eta();
}

/// Calculate uPerp given reconstructed recoil and boson phi
double uPerp(double recoil, double recoilPhi, double zPhi) {
  TLorentzVector recoilvec;
  recoilvec.SetPtEtaPhiM(recoil,0,recoilPhi + TMath::Pi() - zPhi,0);
  return recoilvec.Py();
}

/// Calculate uPara given reconstructed recoil and boson phi
double uPara(double recoil, double recoilPhi, double zPhi) {
  TLorentzVector recoilvec;
  recoilvec.SetPtEtaPhiM(recoil,0,recoilPhi + TMath::Pi() - zPhi,0);
  return recoilvec.Px();
}

/// Calculate uPerp given reconstructed MET and boson pt and phi
double uPerp(double met, double metPhi, double zPt, double zPhi) {
  TLorentzVector metvec;
  metvec.SetPtEtaPhiM(vectorSumPt(met,metPhi,zPt,zPhi),0,vectorSumPhi(met,metPhi,zPt,zPhi)
                      + TMath::Pi() - zPhi,0);
  return metvec.Py();
}

/// Calculate uPara given reconstructed MET and boson pt and phi
double uPara(double met, double metPhi, double zPt, double zPhi) {
  TLorentzVector metvec;
  metvec.SetPtEtaPhiM(vectorSumPt(met,metPhi,zPt,zPhi),0,vectorSumPhi(met,metPhi,zPt,zPhi)
                   + TMath::Pi() - zPhi,0);
  return metvec.Px();
}

/// Calculates transverse mass given a lepton and MET
double transverseMass(double lepPt, double lepPhi, double met,  double metPhi) {
  double cosDPhi = cos(deltaPhi(lepPhi,metPhi));
  return sqrt(2*lepPt*met*(1-cosDPhi));
}

#endif
