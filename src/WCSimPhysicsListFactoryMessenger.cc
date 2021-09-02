#include "WCSimPhysicsListFactoryMessenger.hh"
#include "WCSimPhysicsListFactory.hh"

#include "G4UIdirectory.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4UIcmdWithAString.hh"

WCSimPhysicsListFactoryMessenger::WCSimPhysicsListFactoryMessenger(WCSimPhysicsListFactory* WCSimPhysFactory, G4String inValidListsString)
  :thisWCSimPhysicsListFactory(WCSimPhysFactory), ValidListsString(inValidListsString)
{
 
  G4String defaultList="FTFP_BERT";
  
  physListCmd = new G4UIcmdWithAString("/WCSim/physics/list",this);
  G4String cmd_hint = "Available options: " + ValidListsString;
  physListCmd->SetGuidance(cmd_hint);
  physListCmd->SetGuidance("See http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/useCases.shtml");
  physListCmd->SetGuidance("    http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/referencePL.shtml");
  physListCmd->SetGuidance("Note: Physics list is locked-in after initialization");
  
  physListCmd->SetDefaultValue(defaultList);
  physListCmd->SetCandidates(ValidListsString);  // TODO get list of physics lists from G4PhysicsListFactory

  SetNewValue(physListCmd, defaultList);

  G4String captureModelsString = "Default HP Rad GLG4Sim ANNRI";
  G4String captureModelGuidance = "Available options: " + captureModelsString;
  nCaptureModelCmd = new G4UIcmdWithAString("/WCSim/physics/nCapture",this);
  nCaptureModelCmd->SetGuidance(captureModelGuidance);
  nCaptureModelCmd->SetDefaultValue("Default");
  nCaptureModelCmd->SetCandidates(captureModelsString);

  G4String gdCompositionString = "Natural 157Gd-enr 155Gd-enr";
  G4String gdCompositionGuidance = "Available options: " + gdCompositionString;
  gdCompositionCmd = new G4UIcmdWithAString("/WCSim/physics/GdComposition",this);
  gdCompositionCmd->SetGuidance(gdCompositionGuidance);
  gdCompositionCmd->SetDefaultValue("Natural");
  gdCompositionCmd->SetCandidates(gdCompositionString);

  G4String gdCascadeString = "PeakAndCont Peak Cont";
  G4String gdCascadeGuidance = "Available options: " + gdCascadeString;
  gdCascadeCmd = new G4UIcmdWithAString("/WCSim/physics/GdCascade",this);
  gdCascadeCmd->SetGuidance(gdCascadeGuidance);
  gdCascadeCmd->SetDefaultValue("PeakAndCont");
  gdCascadeCmd->SetCandidates(gdCascadeString);
}

WCSimPhysicsListFactoryMessenger::~WCSimPhysicsListFactoryMessenger()
{
  delete physListCmd;
  //delete WCSimDir;
  delete nCaptureModelCmd;
  delete gdCompositionCmd;
  delete gdCascadeCmd;

}

void WCSimPhysicsListFactoryMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == physListCmd)
    thisWCSimPhysicsListFactory->SetList(newValue);
  else if (command == nCaptureModelCmd){
     thisWCSimPhysicsListFactory->SetnCaptModel(newValue);
   } else if (command == gdCompositionCmd){
     thisWCSimPhysicsListFactory->SetgdComposition(newValue);
   } else if (command == gdCascadeCmd){
     thisWCSimPhysicsListFactory->SetgdCascade(newValue);
   }

}
