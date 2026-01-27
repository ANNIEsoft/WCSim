#include "WCSimTrackInformation.hh"
#include "G4ios.hh"

G4Allocator<WCSimTrackInformation> aWCSimTrackInfoAllocator;

WCSimTrackInformation::WCSimTrackInformation(const G4Track* /*atrack*/)
{
  saveit = true;
  parentPdg=0;
  primaryParentID=-1;
  directParentID=-1;
  //numreflections=0;
}

void WCSimTrackInformation::Print() const
{
  G4cout << "WCSimTrackInformation : " << saveit << G4endl;
}
