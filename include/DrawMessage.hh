#ifndef DRAWMESSAGE_HH
#define DRAWMESSAGE_HH

#include "G4String.hh"

using namespace std;

class DrawMessage
{
public:
	void Error(G4String file_name, G4String parametera);
	G4String DefDuct(G4int duct);
	G4String DefParticle(G4int parti);
	G4String DefTarget(G4int target, G4int model, G4int capture, G4int cascade);
	G4String DefVETO(G4int veto);
	
};

#endif
