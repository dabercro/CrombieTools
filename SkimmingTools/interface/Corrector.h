/**
   @file   Corrector.h

   Header file for the Corrector class.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_CORRECTOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_CORRECTOR_H

#include <utility>
#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TH1.h"

#include "Debug.h"

/**
   @ingroup skimminggroup
   @class Corrector
   @brief Can be created using the CrombieTools.SkimmingTools.Corrector module.

   A Corrector reads from a tree and histogram and returns a correction factor.
   The CorrectorApplicator facilitates this application by providing the trees
   and writes the result to a branch.
*/

class Corrector : virtual public Debug
{
 public:
  /// Constructor setting the name of a branch it would like to write to.
  Corrector( TString name = "correction" );
  virtual ~Corrector();

  /// Copy this Corrector for parallelization
  virtual   Corrector*  Copy              ();

  /// Set the file containing the correction histogram by name.
  void                  SetCorrectionFile ( TString fileName );
  /// Set the correction histogram name within the correction file.
  void                  SetCorrectionHist ( TString histName );
  /// Set two histograms to divide in order to obtain the correction histogram.
  void                  SetCorrectionHist ( TString hist1, TString hist2 );

  /// Evaluate the TTree pointer fInTree at its current entry and return a status bit corresponding to cut.
  std::pair<bool, Float_t>    EvaluateWithFlag    ();
  /// Evaluate the TTree pointer fInTree at its current entry.
  Float_t    Evaluate                             ();

  /// Compares the file name to the Regex and returns true if corrections will be applied
  Bool_t                CompareFileName   ( TString fileName );

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
  virtual    void       SetInTree         ( TTree* tree )       { fInTree = tree; InitializeTree();                        }

  /**
     Used to set the way to read the correction histogram.
     Note that the uncertainty is added in quadrature, so the unity-centered uncertainty
     can safely be used for both up and down histograms.
  */
  enum HistReader {
    eValue = 0,         ///< Simply returns the value of the histogram
    eZeroCenteredUnc,   ///< The fractional uncertainty is just the value of the histogram
    eUnityCenteredUnc   ///< For when reading an uncertainty from a histogram where the perfect precision would be at unity
  };

  /// Set the type of histogram reader
  void                  SetHistReader    ( HistReader reader )   { fHistReader = reader;                                    }

  /// Set a RegEx for the Corrector to check against the filename. If no match, corrections are not applied
  void                  SetMatchFileName ( TString regexpr )        { fMatchFileName = regexpr;                             }

  bool                  Merge = true;     ///< Flag determining whether or not to merge into the CorrectorApplicator branch

 protected:

  TString               fName;                        ///< Name of branch to write to
  void                  InitializeTree    ();         ///< Function to initialize TTreeFormula on the tree
  virtual  Float_t      DoEval            ();         ///< Function that actually does the evaluation of the correction factor

  TTree*                fInTree = NULL;               ///< Pointer to tree being read
  TString               fInCut = "1";                 ///< Corrector cut
  TTreeFormula*         fCutFormula = NULL;           ///< Formula for cut
  Bool_t                fIsCopy = false;              ///< Track if instance is a copy
  Bool_t                fMatchedFileName;             ///< A flag telling this corrector if the file has been matched

  /// Evaluate one of the formulae. If use_lims, then don't give a result that would go off the relevant axis
  Double_t              GetFormulaResult            ( Int_t index, Bool_t use_mins = true );
  /// Get the number of dimensions that the formulas hold
  Int_t                 GetNumDims                  () { return fNumDims; }

 private:
  TFile*                fCorrectionFile = NULL;       ///< Name of file containing correction histogram
  TH1*                  fCorrectionHist = NULL;       ///< Name of correction histogram
  void                  SetMinMax         ();         ///< Set the mininum and maximum values for each histogram axis

  Int_t                       fNumDims = 0;           ///< Number of dimensions in the correction histogram
  std::vector<TString>        fInExpressions;         ///< Expressions to read from tree
  std::vector<TTreeFormula*>  fFormulas;              ///< Formulae of fInExpressions
  std::vector<Double_t>       fMins;                  ///< Minimum possible values of each axis
  std::vector<Double_t>       fMaxs;                  ///< Maximum possible values of each axis
  HistReader                  fHistReader = eValue;   ///< The method to reading histograms

  TString               fMatchFileName = "";

  ClassDef(Corrector,1)
};

#endif
