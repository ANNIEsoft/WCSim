#include "WCSimTrackInformation.hh"
#include "G4ios.hh"

G4Allocator<WCSimTrackInformation> aWCSimTrackInfoAllocator;

WCSimTrackInformation::WCSimTrackInformation(const G4Track* /*atrack*/)
{
  saveit = true;
  numreflections=0;
  parentPdg=0;
}

void WCSimTrackInformation::Print() const
{
  G4cout << "WCSimTrackInformation : " << saveit << G4endl;
}
