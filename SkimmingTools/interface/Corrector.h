/**
   @file   Corrector.h

   Header file for the Corrector class.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_CORRECTOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_CORRECTOR_H

#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TH1.h"

/**
   @ingroup skimminggroup
   @class Corrector
   @brief Can be created using the CrombieTools.SkimmingTools.Corrector module.

   A Corrector reads from a tree and histogram and returns a correction factor.
   The CorrectorApplicator facilitates this application by providing the trees 
   and writes the result to a branch.
*/

class Corrector
{
 public:
  /// Constructor setting the name of a branch it would like to write to.
  Corrector( TString name = "correction" );
  virtual ~Corrector();

  /// Copy this Corrector for parallelization
  Corrector* Copy                         ();

  /// Set the file containing the correction histogram by name.
  virtual   void        SetCorrectionFile ( TString fileName );
  /// Set the correction histogram name within the correction file.
  void                  SetCorrectionHist ( TString histName );
  /// Set two histograms to divide in order to obtain the correction histogram.
  void                  SetCorrectionHist ( TString hist1, TString hist2 );

  /// Evaluate the TTree pointer fInTree at its current entry.
  Float_t               Evaluate          ();

  /// Get the name of the branch that this Corrector would like to write to.
  TString               GetName           () const              { return fName;                                            }

  /**
     Add an expression to read from the histogram.
     In most normal uses (reading from a TH1) this function needs to only be called once.
     If reading from a 2D histogram, call it twice. This gets the bin contents in the order
     that the expressions were added. Up to 3D histograms is supported.
  */
  void                  AddInExpression   ( TString expres )    { fInExpressions.push_back(expres); ++fNumDims;            }
  /// Set a cut for the corrector.
  void                  SetInCut          ( TString cut )       { fInCut = cut;                                            }
  /// Set the pointer to the TTree this Corrector is reading.
  void                  SetInTree         ( TTree* tree )       { fInTree = tree; InitializeTree();                        } 

 protected:

  TString               fName;                        ///< Name of branch to write to
  
  TTree*                fInTree = NULL;               ///< Pointer to tree being read
  void                  InitializeTree    ();         ///< Function to initialize TTreeFormula on the tree

  TFile*                fCorrectionFile = NULL;       ///< Name of file containing correction histogram
  TH1*                  fCorrectionHist = NULL;       ///< Name of correction histogram
  void                  SetMinMax         ();         ///< Set the mininum and maximum values for each histogram axis

 private:
  Int_t                       fNumDims = 0;           ///< Number of dimensions in the correction histogram
  TString                     fInCut = "1";           ///< Corrector cut
  TTreeFormula*               fCutFormula = NULL;     ///< Formula for cut
  std::vector<TString>        fInExpressions;         ///< Expressions to read from tree
  std::vector<TTreeFormula*>  fFormulas;              ///< Formulae of fInExpressions
  std::vector<Double_t>       fMins;                  ///< Minimum possible values of each axis
  std::vector<Double_t>       fMaxs;                  ///< Maximum possible values of each axis
  Bool_t                      fIsCopy = false;        ///< Track if instance is a copy
  /// Evaluate one of the formulae
  Double_t                    GetFormulaResult            ( Int_t index );

  ClassDef(Corrector,1)
};

#endif
