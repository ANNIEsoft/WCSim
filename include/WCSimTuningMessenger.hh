#ifndef WCSimTuningMessenger_h
#define WCSimTuningMessenger_h 1

// Contains detector parameters that need to be set up before detector is constructed
// KS Mar 2010

#include "globals.hh"
#include "G4UImessenger.hh"

class WCSimTuningParameters;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithADouble;
class G4UIcmdWithABool; //jl145


class WCSimTuningMessenger: public G4UImessenger
{
public:
  WCSimTuningMessenger(WCSimTuningParameters*);
  ~WCSimTuningMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

private:

  WCSimTuningParameters* WCSimTuningParams;

  G4UIdirectory*      WCSimDir;
  G4UIcmdWithADouble* Rayff;
  G4UIcmdWithADouble* Bsrff;
  G4UIcmdWithADouble* Abwff;
  G4UIcmdWithADouble* Rgcff;
  G4UIcmdWithADouble* RgcffR7081;
  G4UIcmdWithADouble* Mieff;

  //ANNIE commands
  G4UIcmdWithADouble* Teflonrff;
  G4UIcmdWithADouble* Linerrff;
  G4UIcmdWithADouble* Holderrff;
  G4UIcmdWithADouble* HolderrffLUX;
  G4UIcmdWithABool *Holder;
  G4UIcmdWithADouble* Qeratio;
  G4UIcmdWithADouble* QeratioWB;
  G4UIcmdWithABool *PMTwiseQE;

  //RATPAC comparison commands
  G4UIcmdWithABool* MaterialRAT;
  G4UIcmdWithABool* Photons60nm;

  //For Top Veto - jl145
  G4UIcmdWithADouble* TVSpacing;
  G4UIcmdWithABool* TopVeto;

};

#endif
