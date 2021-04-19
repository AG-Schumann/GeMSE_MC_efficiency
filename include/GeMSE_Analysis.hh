#ifndef GeMSE_Analysis_h
#define GeMSE_Analysis_h 1

#include <TH1D.h>
#include <math.h>

class GeMSE_Analysis {
 public:
  GeMSE_Analysis();
  ~GeMSE_Analysis();

 public:
  void CalcEfficiencies();
  void Clear();

  void SetNEvents(int NEvents) { fNEvents = NEvents; }

  void AddEnergy(double energy) { fenergy.push_back(energy); }
  void AddG4BR(double G4BR) { fG4BR.push_back(G4BR); }
  void AddNuDatBR(double NuDatBR) { fNuDatBR.push_back(NuDatBR); }
  void AddSigRegion(double SigRegion) { fwidth_sig.push_back(SigRegion); }
  void AddBkgRegion(double BkgRegion) { fwidth_bkg.push_back(BkgRegion); }

  int GetNLines() { return fnlines; }
  double GetEnergy(int i) { return fenergy[i]; }
  double GetEfficiency(int i) { return fefficiency[i]; }
  double GetEfficiency_err(int i) { return fefficiency_err[i]; }
  double GetEffBR(int i) { return feffBR[i]; }

  TH1D* GetHisto() { return fhTotEdep; }

 private:
  std::vector<double> fenergy;
  std::vector<double> fG4BR;
  std::vector<double> fNuDatBR;
  std::vector<double> fwidth_sig;
  std::vector<double> fwidth_bkg;

  std::vector<double> fefficiency;
  std::vector<double> fefficiency_err;
  std::vector<double> feffBR;

  TH1D* fhTotEdep;
  int fnlines;
  int fNEvents;
};

#endif
