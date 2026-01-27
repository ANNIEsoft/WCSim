#ifndef WCSimTrackInformation_h
#define WCSimTrackInformation_h 1


#include "globals.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"

// Maximilien Fechner, december 2004
// Information class for flagging the secondaries
// I'm interested in (namely gammas from pi0s and secondaries
// from muon decay
class WCSimTrackInformation : public G4VUserTrackInformation {
private:
  G4bool saveit;
  G4int  primaryParentID;
  G4int directParentID;
  G4int  parentPdg;
  //long long int numreflections;

public:
  WCSimTrackInformation() : saveit(false), primaryParentID(-1), directParentID(-1), parentPdg(0)/*, numreflections(-1)*/ {}
  WCSimTrackInformation(const WCSimTrackInformation* aninfo){
    saveit = aninfo->saveit;
    primaryParentID = aninfo->primaryParentID;
    directParentID = aninfo->directParentID;
    parentPdg = aninfo->parentPdg;
    //numreflections = aninfo->numreflections;
  }
  virtual ~WCSimTrackInformation() {}
  WCSimTrackInformation(const G4Track* );
  
  G4bool isSaved() { return saveit;}
  void WillBeSaved(G4bool choice) { saveit = choice;}

  void SetPrimaryParentID(G4int i) { primaryParentID = i;}
  G4int GetPrimaryParentID() {return primaryParentID;}

  void SetDirectParentID(G4int i) { directParentID = i;}
  G4int GetDirectParentID() {return directParentID;}

  void SetParentPdg(G4int i) { parentPdg = i;}
  G4int GetParentPdg() { return parentPdg;}
  
  //void IncrementNumReflections() {numreflections++;}
  //long long int GetNumReflections() {return numreflections;}
  

  inline void *operator new(size_t);
  inline void operator delete(void *aTrackInfo);
  inline int operator ==(const WCSimTrackInformation& right) const
  {return (this==&right);}

  void Print() const;

};

extern G4Allocator<WCSimTrackInformation> aWCSimTrackInfoAllocator;

inline void* WCSimTrackInformation::operator new(size_t)
{ void* aTrackInfo;
 aTrackInfo = (void*)aWCSimTrackInfoAllocator.MallocSingle();
 return aTrackInfo;
}

inline void WCSimTrackInformation::operator delete(void *aTrackInfo)
{ aWCSimTrackInfoAllocator.FreeSingle((WCSimTrackInformation*)aTrackInfo);}


#endif
