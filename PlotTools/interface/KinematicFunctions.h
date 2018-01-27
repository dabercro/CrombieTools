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

/// Calculate deltaPhi between two particles
float deltaPhi(float phi1, float phi2) {
  const double pi = 3.14159265358979323846;
  // Make phi between +- pi, if -pi < phi < 3*pi
  auto make_pm = [pi](float phi) {
    if (phi > pi)
      phi -= 2*pi;
    return phi;
  };

  return fabs(make_pm(fabs(make_pm(phi1) - make_pm(phi2))));
}

/// Calculate deltaR2 between two particles
float deltaR2(float eta1, float phi1, float eta2, float phi2) {
  auto d_eta = eta2 - eta1;
  auto d_phi = deltaPhi(phi1, phi2);
  return d_eta * d_eta + d_phi * d_phi;
}

/// Calculate deltaR between two particles
float deltaR(float eta1, float phi1, float eta2, float phi2) {
  return sqrt(deltaR2(eta1, phi1, eta2, phi2));
}

/// Get TVector2 for vector in the transverse plane
TVector2 vectorSum(float pt1, float phi1, float pt2, float phi2) {
  TVector2 vec1;
  TVector2 vec2;
  vec1.SetMagPhi(pt1, phi1);
  vec2.SetMagPhi(pt2, phi2);
  vec1 += vec2;
  return vec1;
}

/// Phi of a mother of two particles
float vectorSumPhi(float pt1, float phi1, float pt2, float phi2) {
  return vectorSum(pt1, phi1, pt2, phi2).Phi();
}

/// Pt of a mother of two particles
float vectorSumPt(float pt1, float phi1, float pt2, float phi2) {
  return vectorSum(pt1, phi1, pt2, phi2).Mod();
}

/// Pt of a mother of three particles
float vectorSum3Pt(float pt1, float phi1, float pt2, float phi2,float pt3, float phi3) {
  return sqrt( pow(pt1*cos(phi1) + pt2*cos(phi2) + pt3*cos(phi3),2) +
	       pow(pt1*sin(phi1) + pt2*sin(phi2) + pt3*sin(phi3),2) );
}

/// Mass of a mother of two massive particles
float vectorSumMass(float pt1, float eta1, float phi1, float mass1, float pt2, float eta2, float phi2, float mass2) {
  TLorentzVector vec1;
  TLorentzVector vec2;
  TLorentzVector vec3;
  vec1.SetPtEtaPhiM(pt1,eta1,phi1,mass1);
  vec2.SetPtEtaPhiM(pt2,eta2,phi2,mass2);
  vec3 = vec1 + vec2;
  return vec3.M();
}

/// Mass of a mother of two massless particles
float vectorSumMass(float pt1, float eta1, float phi1, float pt2, float eta2, float phi2) {
  return vectorSumMass(pt1,eta1,phi1,0.0,pt2,eta2,phi2,0.0);
}

/// Eta of a mother of two particles
float vectorSumEta(float pt1, float eta1, float phi1, float pt2, float eta2, float phi2) {
  TLorentzVector vec1;
  TLorentzVector vec2;
  TLorentzVector vec3;
  vec1.SetPtEtaPhiM(pt1,eta1,phi1,0);
  vec2.SetPtEtaPhiM(pt2,eta2,phi2,0);
  vec3 = vec1 + vec2;
  return vec3.Eta();
}

/// Calculate uPerp given reconstructed recoil and boson phi
float uPerp(float recoil, float recoilPhi, float zPhi) {
  TLorentzVector recoilvec;
  recoilvec.SetPtEtaPhiM(recoil,0,recoilPhi + TMath::Pi() - zPhi,0);
  return recoilvec.Py();
}

/// Calculate uPara given reconstructed recoil and boson phi
float uPara(float recoil, float recoilPhi, float zPhi) {
  TLorentzVector recoilvec;
  recoilvec.SetPtEtaPhiM(recoil,0,recoilPhi + TMath::Pi() - zPhi,0);
  return recoilvec.Px();
}

/// Calculate uPerp given reconstructed MET and boson pt and phi
float uPerp(float met, float metPhi, float zPt, float zPhi) {
  TLorentzVector metvec;
  metvec.SetPtEtaPhiM(vectorSumPt(met,metPhi,zPt,zPhi),0,vectorSumPhi(met,metPhi,zPt,zPhi)
                      + TMath::Pi() - zPhi,0);
  return metvec.Py();
}

/// Calculate uPara given reconstructed MET and boson pt and phi
float uPara(float met, float metPhi, float zPt, float zPhi) {
  TLorentzVector metvec;
  metvec.SetPtEtaPhiM(vectorSumPt(met,metPhi,zPt,zPhi),0,vectorSumPhi(met,metPhi,zPt,zPhi)
                   + TMath::Pi() - zPhi,0);
  return metvec.Px();
}

/// Calculates transverse mass given a lepton and MET
float transverseMass(float lepPt, float lepPhi, float met,  float metPhi) {
  double cosDPhi = cos(deltaPhi(lepPhi,metPhi));
  return sqrt(2*lepPt*met*(1-cosDPhi));
}

#endif
