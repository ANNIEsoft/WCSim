#ifndef WCSimEventInformation_h
#define WCSimEventInformation_h 1


#include "globals.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4VUserEventInformation.hh"
#include "TH1D.h"
#include "TH2D.h"

// Maximilien Fechner, december 2004
// Information class for counting number of Cherenkov photons in event
class WCSimEventInformation : public G4VUserEventInformation {
private:

public:
  WCSimEventInformation() : numCherenPhoton(0) {
    hCher = TH1D("hCher","Cherenkov distribution",200,0,1000);
    hCherWCSim = TH1D("hCherWCSim","Cherenkov distribution (WCSim range)",200,200,800);
    hCherXZ = TH2D("hCherXZ","Cherenkov vtx XZ",200,0,4,200,-2,2);
    hCherYZ = TH2D("hCherYZ","Cherenkov vtx YZ",200,0,4,200,-2.5,2.5);
  }
  WCSimEventInformation(const WCSimEventInformation* aninfo){
    numCherenPhoton = aninfo->numCherenPhoton;
    hCher = TH1D("hCher","Cherenkov distribution",200,0,1000);
    hCherWCSim = TH1D("hCherWCSim","Cherenkov distribution (WCSim range)",200,200,800);
    hCherXZ = TH2D("hCherXZ","Cherenkov vtx XZ",200,0,4,200,-2,2);
    hCherYZ = TH2D("hCherYZ","Cherenkov vtx YZ",200,0,4,200,-2.5,2.5);
  }
  virtual ~WCSimEventInformation() {}
  
  void SetNumCherenPhoton(G4int num) {numCherenPhoton = num;}
  G4int GetNumCherenPhoton() {return numCherenPhoton;}

  G4int  numCherenPhoton = 0;
  G4int  numCherenPhotonWCSim = 0;
  TH1D hCher;
  TH1D hCherWCSim;
  TH2D hCherXZ;
  TH2D hCherYZ;

  inline void *operator new(size_t);
  inline void operator delete(void *anEventInfo);
  inline int operator ==(const WCSimEventInformation& right) const
  {return (this==&right);}

  void Print() const;

};

extern G4Allocator<WCSimEventInformation> aWCSimEventInfoAllocator;

inline void* WCSimEventInformation::operator new(size_t)
{ void* anEventInfo;
 anEventInfo = (void*)aWCSimEventInfoAllocator.MallocSingle();
 return anEventInfo;
}

inline void WCSimEventInformation::operator delete(void *anEventInfo)
{ aWCSimEventInfoAllocator.FreeSingle((WCSimEventInformation*)anEventInfo);}


#endif
