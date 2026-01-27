#ifndef WCSimWCHit_h
#define WCSimWCHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4ios.hh"
// for sort, find, count_if
#include <algorithm>
//for less_equal, bind2nd,...
#include <functional>
#include <assert.h>

// compose2 is not part of the C++ standard
// use this kludgy technique to use it
#include <ext/functional>
using __gnu_cxx::identity_element;
using __gnu_cxx::unary_compose;
using __gnu_cxx::binary_compose;
using __gnu_cxx::compose1;
using __gnu_cxx::compose2;
using __gnu_cxx::identity;
using __gnu_cxx::select1st;
using __gnu_cxx::select2nd;
using __gnu_cxx::project1st;
using __gnu_cxx::project2nd;
using __gnu_cxx::constant_void_fun;
using __gnu_cxx::constant_unary_fun;
using __gnu_cxx::constant_binary_fun;
using __gnu_cxx::constant0;
using __gnu_cxx::constant1;
using __gnu_cxx::constant2;
using __gnu_cxx::subtractive_rng;
using __gnu_cxx::mem_fun1;
using __gnu_cxx::mem_fun1_ref;



class WCSimWCHit : public G4VHit
{
 public:
  
  WCSimWCHit();
  ~WCSimWCHit();
  WCSimWCHit(const WCSimWCHit&);
  const WCSimWCHit& operator=(const WCSimWCHit&);
  G4int operator==(const WCSimWCHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  //map<double,G4ThreeVector> position;
  std::vector<G4ThreeVector> position; // position within the local coordinates of this touchable
  std::vector<G4ThreeVector> globalposition; // position of the hit within the world volume global coordinates
  
  void Draw();
  void Print();
  void PrintLAPPD();
  
 public:
  
  void SetTubeID       (G4int tube)                 { tubeID = tube; };
  void SetTrackID      (G4int track)                { trackID = track; };
  void SetEdep         (G4double de)                { edep = de; };
  void SetPos          (G4ThreeVector xyz)          { pos = xyz; };
  void SetRot          (G4RotationMatrix rotMatrix) { rot = rotMatrix; };
  void SetLogicalVolume(G4LogicalVolume* logV)      { pLogV = logV;}
  void AddPrimaryParentID(G4int primParentID)
  { primaryParentID.push_back(primParentID); }
  void AddDirectParentID(G4int directParID)
  { directParentID.push_back(directParID); }
  // This is temporarily used for the drawing scale
  static void SetMaxPe(G4int number = 0)  {maxPe   = number;};

  void AddPe(G4float hitTime)  
  {
    // First increment the totalPe number
    totalPe++; 

    if (totalPe > maxPe) 
      maxPe = totalPe;

    time.push_back(hitTime);
  }
 
  G4int         GetTubeID()     { return tubeID; };
  G4int         GetTrackID()    { return trackID; };
  G4ThreeVector GetPos()        { return pos; };
  G4ThreeVector GetStripPosition(int i)  { return position.at(i);};
  G4ThreeVector GetGlobalPos(int i) { return globalposition.at(i);};
  
  void AddStripPosition(G4ThreeVector pos) { position.push_back(pos); }
  void AddHitPos(G4ThreeVector pos) { globalposition.push_back(pos); }
  G4int         GetTotalPe()    { return totalPe;};
  G4float       GetTime(int i)  { return time.at(i);};
  G4int         GetPrimaryParentID(int i) { return primaryParentID.at(i);};
  G4int         GetDirectParentID(int i) { return directParentID.at(i);};
  
  G4LogicalVolume* GetLogicalVolume() {return pLogV;};

  void SortHitTimes() {   sort(time.begin(),time.end()); }


  // low is the trigger time, up is trigger+950ns (end of event)
  G4float GetFirstHitTimeInGate(G4float low,G4float upevent)
  {
    G4float firsttime;
    std::vector<G4float>::iterator tfirst = time.begin();
    std::vector<G4float>::iterator tlast = time.end();
  
    std::vector<G4float>::iterator found = 
      std::find_if(tfirst,tlast,
		   compose2(std::logical_and<bool>(),
			    std::bind2nd(std::greater_equal<G4float>(),low),
			    std::bind2nd(std::less_equal<G4float>(),upevent)
			    )
		   );
    if ( found != tlast ) {
      firsttime = *found; // first hit time
    }
    else {
      firsttime = -10000.; //error code.
    }
    //G4cout << "firsthit time " << firsttime << "\n";
    return firsttime;
  }


  // pmtgate  and evgate are durations, ie not absolute times

  G4int GetPeInGate(double low, double pmtgate,double evgate) {
    // M Fechner; april 2005
    // assumes that time has already been sorted
    std::vector<G4float>::iterator tfirst = time.begin();
    std::vector<G4float>::iterator tlast = time.end();
    // select min time
    G4float mintime = (pmtgate < evgate) ? pmtgate : evgate;
    
    // return number of hits in the time window...
    
    G4int number = std::count_if(tfirst,tlast,
				 compose2(std::logical_and<bool>(),
					  std::bind2nd(std::greater_equal<G4float>(),low),
					  std::bind2nd(std::less_equal<G4float>(),mintime)
					  )
				 );
    
    totalPeInGate = number;
    //    G4cout << "numer = " <<  number <<"\n";
    return number;
  }


 private:
  
  G4int            tubeID;
  G4int            lappdID;
  G4int            trackID;
  G4double         edep;
  G4ThreeVector    pos; // position of the touchable within global coordinates
  G4RotationMatrix rot;
  G4LogicalVolume* pLogV;

  // This is temporarily used for the drawing scale
  // Since its static *every* WChit sees the same value for this.

  static G4int     maxPe;

  G4int                 totalPe;
  std::vector<G4float>  time;
  std::vector<G4int>    primaryParentID;
  G4int                 totalPeInGate;
  std::vector<G4int>    directParentID;
};

typedef G4THitsCollection<WCSimWCHit> WCSimWCHitsCollection;

extern G4Allocator<WCSimWCHit> WCSimWCHitAllocator;

inline void* WCSimWCHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) WCSimWCHitAllocator.MallocSingle();
  return aHit;
}

inline void WCSimWCHit::operator delete(void *aHit)
{
  WCSimWCHitAllocator.FreeSingle((WCSimWCHit*) aHit);
}

#endif
