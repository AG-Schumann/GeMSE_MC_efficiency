#ifndef GeMSE_RunAction_h
#define GeMSE_RunAction_h 1

#include "GeMSE_Analysis.hh"

#include "TTree.h"
#include "TFile.h"
#include "globals.hh"
#include <vector>
#include <string>

#include "G4ThreeVector.hh"
#include "G4UserRunAction.hh"

using std::string;

class G4Timer;
class G4Run;
class GeMSE_RunMessenger;

class GeMSE_RunAction : public G4UserRunAction {
 public:
  //GeMSE_RunAction(TTree* tree);
  GeMSE_RunAction(G4String Outputfolder);
  ~GeMSE_RunAction();  

 public:
  void BeginOfRunAction(const G4Run* aRun);
  void EndOfRunAction(const G4Run* aRun);
  void SetVersionTag(const G4String &hVersionTag) {
    m_hVersionTag = hVersionTag;
  }

  TTree* GetGeHitTree();
  TTree* GetPrimariesTree();

  void AddDecay();

  void SelectAction(G4String string_action) { selectedAction = string_action; };
  G4String GetSelectedAction() { return selectedAction; };

  GeMSE_Analysis* GetRunAnalysis() { return fRunAnalysis; };

 private:
  G4Timer* timer;

  G4int NDecays;
  G4int fNDecays;
  G4String m_hVersionTag;

  G4int NEvents;
  G4int HEventID;
  G4int NHits;
  G4double TotEdep;
  
  std::vector<double> Edep;
  std::vector<double> HEkin;
  std::vector<double> Time;
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  std::vector<int> HParticleID;
  std::vector<int> HTrackID;

  G4int PEventID;
  G4int PTrackID;
  G4int ParentID;
  string* PParticleID;
  string* Process;
  G4double PEkin;
  G4double xDir;
  G4double yDir;
  G4double zDir;

  TFile* ResultFile;

  G4String selectedAction;
  G4String fOutputFolder;

  TTree* tree;
  TTree* GeHitTree;
  TTree* PrimariesTree;
  TTree* RunTree;

  TTree* ftree;
  TTree* fGeHitTree;
  TTree* fPrimariesTree;

  G4long aSeed;
  GeMSE_Analysis* fRunAnalysis;
  GeMSE_RunMessenger* runMessenger;
};

#endif
