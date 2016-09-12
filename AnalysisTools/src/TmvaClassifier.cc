#include <iostream>
#include <fstream>
#include <stdio.h>

#include "TFile.h"
#include "TGraph.h"
#include "TTreeFormula.h"
#include "TH1D.h"

#include "PlotHists.h"
#include "TmvaClassifier.h"
#include "TreeContainer.h"

#include "TMVA/Factory.h"
#include "TMVA/Reader.h"

//--------------------------------------------------------------------
TmvaClassifier::TmvaClassifier() :
fSignalCut(""),
  fBackgroundCut(""),
  fWeight("1"),
  fJobName("TmvaClassifier"),
  fMethodName("BDT"),
  fBDTDef(""),
  fBDTName("bdt_test"),
  fOutputName("TMVA.root"),
  fConfigFile(""),
  fUniformVariable(""),
  fApplicationDirectory(""),
  fApplicationTree("Events"),
  fApplicationOutput("WithBDT"),
  fReportFrequency(0)
{
  // Constructor
  fSignalFileNames.resize(0);
  fSignalTreeNames.resize(0);
  fBackgroundFileNames.resize(0);
  fBackgroundTreeNames.resize(0);

  fVariables.resize(0);
  fVarTypes.resize(0);
  fSpectatorVariables.resize(0);
}

//--------------------------------------------------------------------
TmvaClassifier::~TmvaClassifier()
{
}

//--------------------------------------------------------------------
void
TmvaClassifier::SetConfigFile(TString name)
{
  fConfigFile = name;

  TString BDTName;
  std::ifstream configFile;
  configFile.open(fConfigFile.Data());
  TString tempFormula;

  std::vector<TString> Strings;
  
  configFile >> fBDTName >> fUniformVariable;

  while(!configFile.eof()){
    configFile >> tempFormula;
    if(tempFormula != ""){
      AddVariable(tempFormula);
    }
  }
  
}

//--------------------------------------------------------------------
void
TmvaClassifier::TmvaClassify()
{
  TFile *TMVAOutput = new TFile(fOutputName, "RECREATE");
  TMVA::Factory *factory = new TMVA::Factory(fJobName, TMVAOutput,
                                             "V:!Silent:Color:DrawProgressBar:Transformations=I;N");

  for (UInt_t iVar = 0; iVar != fVariables.size(); ++iVar)
    factory->AddVariable(fVariables[iVar]);

  for (UInt_t iSpec = 0; iSpec != fSpectatorVariables.size(); ++iSpec)
    factory->AddSpectator(fSpectatorVariables[iSpec]);

  if (fUniformVariable != "")
    factory->AddSpectator(fUniformVariable);

  std::vector<TreeContainer*> SignalTrees;
  TreeContainer *tempTree;
  for (UInt_t iTree = 0; iTree != fSignalFileNames.size(); ++iTree) {
    tempTree = new TreeContainer(fSignalFileNames[iTree]);
    if (fSignalCut != "")
      tempTree->SetSkimmingCut(fSignalCut);

    factory->AddSignalTree(tempTree->ReturnTree(fSignalTreeNames[iTree]), 1.0);
    SignalTrees.push_back(tempTree);                                             // Save TreeContainer for deleting at the end
  }

  std::vector<TreeContainer*> BackgroundTrees;
  for (UInt_t iTree = 0; iTree != fBackgroundFileNames.size(); ++iTree) {
    tempTree = new TreeContainer(fBackgroundFileNames[iTree]);
    if (fBackgroundCut != "")
      tempTree->SetSkimmingCut(fBackgroundCut);

    factory->AddBackgroundTree(tempTree->ReturnTree(fBackgroundTreeNames[iTree]), 1.0);
    BackgroundTrees.push_back(tempTree);                                         // Save TreeContainer for deleting at the end
  }

  factory->SetWeightExpression(fWeight.GetTitle());
  factory->PrepareTrainingAndTestTree("1","SplitMode=Alternate:NormMode=NumEvents:V");

  factory->BookMethod(TMVA::Types::kBDT,fMethodName,fBDTDef);
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  TMVAOutput->Close();

  delete factory;

  for (UInt_t iTree = 0; iTree != SignalTrees.size(); ++iTree)
    delete SignalTrees[iTree];

  for (UInt_t iTree = 0; iTree != BackgroundTrees.size(); ++iTree)
    delete BackgroundTrees[iTree];

  SignalTrees.resize(0);
  BackgroundTrees.resize(0);
}

//--------------------------------------------------------------------
void
TmvaClassifier::Apply()
{
  Apply(1, 0., 0.);
}

//--------------------------------------------------------------------
void
TmvaClassifier::Apply(Int_t NumBins, Double_t VarMin, Double_t VarMax, Int_t NumMapPoints)
{
  Double_t binWidth = (VarMax - VarMin)/NumBins;
  Double_t VarVals[NumBins+1];
  for (Int_t i0 = 0; i0 != NumBins + 1; ++i0)
    VarVals[i0] = VarMin + i0 * binWidth;

  Apply(NumBins, VarVals, NumMapPoints);
}

//--------------------------------------------------------------------
void
TmvaClassifier::Apply(Int_t NumBins, Double_t *VarVals, Int_t NumMapPoints)
{
  // Now, look into setting up uniform distribution
  
  std::vector<TGraph*> transformGraphs;
  TGraph *tempGraph;

  if (fUniformVariable != "") {
    // First scale the BDT to be uniform
    // Make the background shape

    TreeContainer *TrainingTreeContainer = new TreeContainer(fOutputName);
    TrainingTreeContainer->SetSkimmingCut("classID == 1");

    TTree *BackgroundTree = TrainingTreeContainer->ReturnTree("TrainTree");

    PlotHists *HistPlotter = new PlotHists();
    HistPlotter->SetDefaultTree(BackgroundTree);
    HistPlotter->SetDefaultExpr(fMethodName);

    Double_t binWidth = 2.0/(NumMapPoints - 1);
    Double_t BDTBins[NumMapPoints];
    for (Int_t i0 = 0; i0 != NumMapPoints; ++i0)
      BDTBins[i0] = i0 * binWidth - 1;

    for (Int_t iVarBin = 0; iVarBin != NumBins; ++iVarBin) {
      char buffer [1023];
      sprintf(buffer, "(%s>=%f&&%s<%f)",
              fUniformVariable.Data(), VarVals[iVarBin],
              fUniformVariable.Data(), VarVals[iVarBin+1]);
      TCut BinCut = TCut(buffer);
      HistPlotter->AddWeight(fWeight * "classID == 1" + BinCut);
    }

    std::cout << "Making hists." << std::endl;

    std::vector<TH1D*> BDTHists = HistPlotter->MakeHists(NumMapPoints,-1,1);

    std::cout << "Finished hists." << std::endl;

//     delete BackgroundTree;
    delete TrainingTreeContainer;

    std::cout << "Deleted containers." << std::endl;

    for (Int_t iVarBin = 0; iVarBin != NumBins; ++iVarBin) {
      tempGraph = new TGraph(NumMapPoints);
      transformGraphs.push_back(tempGraph);
      Double_t FullIntegral = BDTHists[iVarBin]->Integral();
      for (Int_t iMapPoint = 0; iMapPoint != NumMapPoints; ++iMapPoint) {
        transformGraphs[iVarBin]->SetPoint(iMapPoint, BDTBins[iMapPoint],
                                           BDTHists[iVarBin]->Integral(0,iMapPoint)/FullIntegral);
      }
    }

    std::cout << "Got map points." << std::endl;

    for (UInt_t iHist = 0; iHist != BDTHists.size(); ++iHist)
      delete BDTHists[iHist];
  }

  std::cout << "About to apply." << std::endl;

  // Then apply the BDT
  TMVA::Reader* reader = new TMVA::Reader("Color:!Silent");

  std::vector<Int_t>         discreteVars;
  std::vector<Float_t>       continuousVars;
  std::vector<TTreeFormula*> discreteFormulae;
  std::vector<TTreeFormula*> continuousFormulae;

  Int_t numDiscrete   = 0;
  Int_t numContinuous = 0;

  TTreeFormula* tempFormula;
    
  TreeContainer *ApplicationTreesContainer = new TreeContainer();
  ApplicationTreesContainer->AddDirectory(fApplicationDirectory);
  std::vector<TTree*>  ApplicationTrees = ApplicationTreesContainer->ReturnTreeList(fApplicationTree);
  std::vector<TTree*>  CopiedTrees;
  std::vector<TString> FileList = ApplicationTreesContainer->ReturnFileNames();

  Float_t UniformVar  = 0.;
  TTreeFormula *UniformFormula = new TTreeFormula(fUniformVariable, fUniformVariable, ApplicationTrees[0]);

  if (fUniformVariable != "")
    reader->AddSpectator(fUniformVariable, &UniformVar);

  for (UInt_t iVar = 0; iVar != fVariables.size(); ++iVar) {
    if (fVarTypes[iVar] == 'I') {
      discreteVars.push_back(0);
      tempFormula = new TTreeFormula(fVariables[iVar],fVariables[iVar],ApplicationTrees[0]);
      discreteFormulae.push_back(tempFormula);
      reader->AddVariable(fVariables[iVar], &discreteVars[numDiscrete]);
      ++numDiscrete;
    }
    else if (fVarTypes[iVar] == 'F') {
      continuousVars.push_back(0);
      tempFormula = new TTreeFormula(fVariables[iVar],fVariables[iVar],ApplicationTrees[0]);
      continuousFormulae.push_back(tempFormula);
      reader->AddVariable(fVariables[iVar], &continuousVars[numContinuous]);
      ++numContinuous;
    }
  }

  for (UInt_t iVar = 0; iVar != fSpectatorVariables.size(); ++iVar) {
    continuousVars.push_back(0);
    tempFormula = new TTreeFormula(fSpectatorVariables[iVar],fSpectatorVariables[iVar],ApplicationTrees[0]);
    continuousFormulae.push_back(tempFormula);
    reader->AddSpectator(fSpectatorVariables[iVar], &continuousVars[numContinuous]);
    ++numContinuous;
  }
  
  reader->BookMVA(fMethodName,TString("weights/") + fJobName + TString("_") + fMethodName +".weights.xml");

  for (UInt_t iTree = 0; iTree != ApplicationTrees.size(); ++iTree) {
    TFile *newFile = new TFile(fApplicationOutput+"/"+FileList[iTree], "RECREATE");
    TTree *tempTree = ApplicationTrees[iTree]->CloneTree();

    Float_t BDTOutput = 0.;
    tempTree->Branch(fBDTName,&BDTOutput,fBDTName+"/F");

    for (UInt_t iForm = 0; iForm != discreteFormulae.size(); ++iForm)
      discreteFormulae[iForm]->SetTree(ApplicationTrees[iTree]);

    Int_t NEntries = ApplicationTrees[iTree]->GetEntriesFast();
    for (Int_t iEntry = 0; iEntry != NEntries; ++iEntry) {
      if (fReportFrequency > 0 && iEntry % fReportFrequency == 0)
        std::cout << "Processing event... " << iEntry << ": " << float(iEntry)/NEntries * 100 << "%" << std::endl;

      ApplicationTrees[iTree]->GetEntry(iEntry);

      for (UInt_t iForm = 0; iForm != discreteFormulae.size(); ++iForm)
        discreteVars[iForm] = discreteFormulae[iForm]->EvalInstance();

      for (UInt_t iForm = 0; iForm != continuousFormulae.size(); ++iForm)
        continuousVars[iForm] = continuousFormulae[iForm]->EvalInstance();

      BDTOutput = reader->EvaluateMVA(fMethodName);

      if (fUniformVariable != "") {
        UniformVar = UniformFormula->EvalInstance();

        if (UniformVar >= VarVals[0] && UniformVar < VarVals[NumBins]) {
          for (Int_t iBin = 0; iBin != NumBins; ++iBin) {
            if (UniformVar < VarVals[iBin + 1]) {
              BDTOutput = transformGraphs[iBin]->Eval(BDTOutput);
              break;
            }
          }
        }
      }
      tempTree->Fill();
    }
    tempTree->Write(0, TObject::kOverwrite);
    newFile->Close();
  }
  
  if (fUniformVariable != "")
    delete UniformFormula;
  
  for (UInt_t iForm = 0; iForm != discreteFormulae.size(); ++iForm)
    delete discreteFormulae[iForm];
  
  for (UInt_t iForm = 0; iForm != continuousFormulae.size(); ++iForm)
    delete continuousFormulae[iForm];
  
  delete ApplicationTreesContainer;
  
  delete reader;
  
  for (Int_t iGraph = 0; iGraph != NumBins; ++iGraph)
    delete transformGraphs[iGraph];
}
