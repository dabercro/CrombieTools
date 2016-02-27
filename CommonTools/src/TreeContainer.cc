#include <iostream>

#include "TROOT.h"
#include "TSystem.h"
#include "TList.h"
#include "TSystemDirectory.h"
#include "TreeContainer.h"

ClassImp(TreeContainer)

//--------------------------------------------------------------------

/**
   TreeContainer Constructor.
   @param fileName can either be the name of a single file or a directory.
   If it's a file, that file is opened. If it's a directory, all the 
   .root files in that directory are opened */

TreeContainer::TreeContainer(TString fileName)
{
  if (fileName != "") {
    if (fileName.Contains(".root"))
      AddFile(fileName);
    else
      AddDirectory(fileName);
  }
}

//--------------------------------------------------------------------
TreeContainer::~TreeContainer()
{
  if (fTree)
    delete fTree;

  for (UInt_t iFile = 0; iFile != fFileList.size(); ++iFile) {
    if (fFileList[iFile]->IsOpen())
      fFileList[iFile]->Close();
  }
}

//--------------------------------------------------------------------
void TreeContainer::AddFile(TString fileName)
{
  tempFile = TFile::Open(fileName);
  if (fPrinting)
    std::cout << "File: " << tempFile << std::endl;

  if (tempFile != NULL) {
    fFileList.push_back(tempFile);
    fFileNames.push_back(fileName);
  }
}

//--------------------------------------------------------------------
void TreeContainer::AddDirectory(TString directoryName,TString searchFor)
{
  TString tempName;
  TSystemDirectory *dir = new TSystemDirectory(directoryName,directoryName);
  TList *fileNameList = dir->GetListOfFiles();

  for (Int_t iFile = 0; iFile != fileNameList->GetEntries(); ++iFile) {
    TNamed *tempMember = (TNamed*) fileNameList->At(iFile);
    TString tempName = TString(tempMember->GetName());
    if (tempName.Contains(searchFor)) {
      if (fPrinting)
        std::cout << "Opening " << tempName << std::endl;

      tempFile = TFile::Open(directoryName+"/"+tempName);
      if (tempFile != NULL) {
        fFileList.push_back(tempFile);
        fFileNames.push_back(tempName);             // Note this is different from AddFile in that it doesn't keep full filename
      }
    }
  }
  delete dir;
}

//--------------------------------------------------------------------
TTree* TreeContainer::SkimTree(TTree *tree, TFile *inFile)
{
  if (fKeepBranches.size() > 0) {
    tree->SetBranchStatus("*",0);
    for (UInt_t iBranch = 0; iBranch != fKeepBranches.size(); iBranch++)
      tree->SetBranchStatus(fKeepBranches[iBranch],1);
  }

  if (inFile)
    inFile->cd();
  else
    gROOT->cd();

  TTree *outTree = 0;
  if (fSkimmingCut == "")
    outTree = tree->CloneTree(-1,"fast");
  else
    outTree = tree->CopyTree(fSkimmingCut);

  return outTree;
}

//--------------------------------------------------------------------
TTree* TreeContainer::ReturnTree(TString Name, TFile* inFile)
{
  if (fTree)
    return fTree;

  if (Name != "")
    SetTreeName(Name);

  TList *treeList = new TList;
  ReturnTreeList();
  for (UInt_t i0 = 0; i0 != fTreeList.size(); ++i0)
    treeList->Add(fTreeList[i0]);

  if (inFile)
    inFile->cd();
  else
    gROOT->cd();

  if (treeList->GetEntries() > 1)
    fTree = TTree::MergeTrees(treeList,"fast");
  else if (treeList->GetEntries() == 1)
    fTree = tempTree;
  else
    fTree = NULL;

  delete treeList;
  return fTree;
}

//--------------------------------------------------------------------
std::vector<TTree*> TreeContainer::ReturnTreeList(TString Name)
{
  if (fTreeList.size() != 0)
    return fTreeList;

  if (Name != "")
    SetTreeName(Name);

  for (UInt_t i0 = 0; i0 < fFileList.size(); i0++) {
    if (fTreeName.Contains("/"))
      tempTree = (TTree*) fFileList[i0]->Get(fTreeName);
    else
      tempTree = (TTree*) fFileList[i0]->FindObjectAny(fTreeName);

    if(fPrinting)
      std::cout << "Getting " << fTreeName << " from " << fFileList[i0]->GetName() << std::endl;

    for (UInt_t i1 = 0; i1 < fFriendNames.size(); i1++) {
      if (fFriendNames[i1].Contains("/"))
        tempFriend = (TTree*) fFileList[i0]->Get(fFriendNames[i1]);
      else
        tempFriend = (TTree*) fFileList[i0]->FindObjectAny(fFriendNames[i1]);
      
      tempTree->AddFriend(tempFriend);
    }
    
    if ((fSkimmingCut != "") || (fKeepBranches.size() != 0))
      tempTree = SkimTree(tempTree,0);

    fTreeList.push_back(tempTree);
  }
  return fTreeList;
}

//--------------------------------------------------------------------
void TreeContainer::MakeFile(TString fileName, TString treeName)
{
  if (fileName != "")
    SetOutputFileName(fileName);

  if (treeName != "")
    SetTreeName(treeName);

  TFile *outFile = new TFile(fOutputFileName,"RECREATE");
  if (!fTree)
    ReturnTree(fTreeName,outFile);

  outFile->cd();
  fTree->Write();
  outFile->Close();
  delete outFile;
}
