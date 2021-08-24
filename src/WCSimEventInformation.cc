#include "WCSimEventInformation.hh"
#include "G4ios.hh"

G4Allocator<WCSimEventInformation> aWCSimEventInfoAllocator;

void WCSimEventInformation::Print() const
{
  G4cout << "WCSimEventInformation : Num Cherenkov photons: " << numCherenPhoton << G4endl;
}
