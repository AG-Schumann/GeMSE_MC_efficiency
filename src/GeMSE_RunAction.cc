// Make this appear first!
#include "G4Timer.hh"

#include "GeMSE_Analysis.hh"
#include "GeMSE_RunAction.hh"
#include "GeMSE_TrackingAction.hh"
#include "GeMSE_RunMessenger.hh"

#include <sys/time.h>
#include <vector>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TSystem.h"
#include <TParameter.h>

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Version.hh"

using std::string;

//GeMSE_RunAction::GeMSE_RunAction(TTree* tree) {
GeMSE_RunAction::GeMSE_RunAction(G4String OutputFolder) {
  //ftree = tree;
  fOutputFolder = OutputFolder;
  selectedAction = "default"; // ToDo: messenger for this guy!!
  
  timer = new G4Timer;

  // create run analysis
  fRunAnalysis = new GeMSE_Analysis();
  m_hVersionTag = "0.0.0";

  // create a messenger for this class
  runMessenger = new GeMSE_RunMessenger(fRunAnalysis);
}

GeMSE_RunAction::~GeMSE_RunAction() {
  delete timer;
  // delete fRunAnalysis;
  delete runMessenger;
}

void GeMSE_RunAction::BeginOfRunAction(const G4Run* aRun) {
  // Add random seed
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  struct timeval hTimeValue;
  gettimeofday(&hTimeValue, NULL);
  aSeed = hTimeValue.tv_usec;
  // aSeed=(G4long)time(NULL)+(G4long)getpid();
  // aSeed=(G4long)getrandom()+(G4long)time(NULL);
  CLHEP::HepRandom::setTheSeed(aSeed);
  G4cout << "Setting the seed for this run to " << aSeed << G4endl;

  TString ResultFileName;  
  G4int RunID = aRun->GetRunID();

  if (selectedAction=="default") {
    std::ostringstream convert;   // stream used for the conversion
    convert << RunID; // insert the textual representation in the characters in the stream
    TString RunName=convert.str();
    ResultFileName = "results_run" + RunName + ".root";
  }
  else
    ResultFileName = selectedAction;

  // Try to open results directory
  if (!gSystem->OpenDirectory(fOutputFolder)) {
    // if directory does not exist make one
    if (gSystem->MakeDirectory(fOutputFolder)==-1)
      G4cout << "###### ERROR: could not create directory " << fOutputFolder
             << G4endl;
  }

  ResultFile = new TFile(fOutputFolder+"/"+ResultFileName,"Create");

  if (ResultFile->IsZombie()) {
    G4cout << "##### Warning: " << ResultFileName << " already exists! Overwriting!" << G4endl;
    ResultFile = new TFile(fOutputFolder+"/"+ResultFileName,"recreate");
  }

  // Create trees
  tree = new TTree("tree", "Ge efficiency calculation");
  GeHitTree = new TTree("GeHits", "All energy-depositing Ge hits");
  PrimariesTree = new TTree("Primaries", "Information on all generated particles in the run");
  RunTree = new TTree("RunInfo", "Run information");
  
  GeHitTree->Branch("EventID", &HEventID);
  GeHitTree->Branch("NHits", &NHits);
  GeHitTree->Branch("TotEdep", &TotEdep);
  GeHitTree->Branch("TrackID", &HTrackID);
  GeHitTree->Branch("ParticleID", &HParticleID);
  GeHitTree->Branch("Edep", &Edep);
  GeHitTree->Branch("xPos", &xPos);
  GeHitTree->Branch("yPos", &yPos);
  GeHitTree->Branch("zPos", &zPos);
  GeHitTree->Branch("Time", &Time);
  GeHitTree->Branch("Ekin", &HEkin);

  PrimariesTree->Branch("EventID", &PEventID);
  PrimariesTree->Branch("TrackID", &PTrackID);
  PrimariesTree->Branch("ParentID", &ParentID);
  PrimariesTree->Branch("Ekin", &PEkin);
  PrimariesTree->Branch("xDir", &xDir);
  PrimariesTree->Branch("yDir", &yDir);
  PrimariesTree->Branch("zDir", &zDir);
  PrimariesTree->Branch("ParticleID", &PParticleID);
  PrimariesTree->Branch("CreatorProcess", &Process);

  RunTree->Branch("nEvents", &NEvents);
  RunTree->Branch("nDecays", &NDecays);
  
  NEvents = aRun->GetNumberOfEventToBeProcessed();
  
  fNDecays = 0;

  // set Nb of events
  fRunAnalysis->SetNEvents(NEvents);

  G4cout << "### Run " << RunID << " started with " << NEvents << " events."
         << G4endl;
  timer->Start();
}

void GeMSE_RunAction::EndOfRunAction(const G4Run* aRun) {
  // if a tree is defined
  if (tree) {
    // calculate efficiencies
    fRunAnalysis->CalcEfficiencies();

    // fill ROOT Tree
    G4double energy, efficiency, efficiency_err, eff_BR;
    tree->Branch("energy", &energy);
    tree->Branch("efficiency", &efficiency);
    tree->Branch("efficiency_err", &efficiency_err);
    tree->Branch("eff_BR", &eff_BR);
    int nlines = fRunAnalysis->GetNLines();
    for (int i = 0; i < nlines; ++i) {
      energy = fRunAnalysis->GetEnergy(i);
      efficiency = fRunAnalysis->GetEfficiency(i);
      efficiency_err = fRunAnalysis->GetEfficiency_err(i);
      eff_BR = fRunAnalysis->GetEffBR(i);

      tree->Fill();
    }

    // clear data
    fRunAnalysis->Clear();
  }

  timer->Stop();
  
  NDecays=fNDecays;
  
  G4cout << "\n### Finished ###" << G4endl;
  G4cout << "Runtime: " << *timer << G4endl;
  
  // Write trees and close file
  ResultFile->cd();
  
  // Info to the output file
  G4double dt = timer->GetRealElapsed();
  RunTree->Branch("RunTime", &dt);
  G4double Seed = aSeed;
  RunTree->Branch("Seed", &Seed);
  
  
  //TParameter<G4int> *m_pRanSeed = new TParameter<int>("RANDOM_SEED", aSeed);
  //m_pRanSeed->Write();
  TNamed *G4version = new TNamed("G4VERSION_TAG", G4VERSION_TAG);
  G4version->Write();
  TNamed *MCversiontag = new TNamed("MCVERSION_TAG", m_hVersionTag);
  /*MCversiontag->Write();
  TParameter<G4int> *decays = new TParameter<int>("nDecays", NDecays);
  decays->Write();
  TParameter<G4double> *Runtime = new TParameter<double>("RunTime", dt);
  Runtime->Write();*/
  

  RunTree->Fill();
  //NDecays->Write();
  //NEvents->Write();
  tree->Write();
  GeHitTree->Write();
  PrimariesTree->Write();
  RunTree->Write();  

  ResultFile->Close();

  //delete tree;
  //delete GeHitTree;
  //delete GammaTree;
  //delete ResultFile;
}

TTree* GeMSE_RunAction::GetGeHitTree()
{
  fGeHitTree=GeHitTree;
  return fGeHitTree;
}


TTree* GeMSE_RunAction::GetPrimariesTree()
{
  fPrimariesTree=PrimariesTree;
  return fPrimariesTree;
}

void GeMSE_RunAction::AddDecay()
{
  fNDecays++;
}
