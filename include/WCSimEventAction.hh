#ifndef WCSimEventAction_h
#define WCSimEventAction_h 1


#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4ios.hh"

#include "WCSimDetectorConstruction.hh"
#include "G4TrajectoryContainer.hh"
#include "WCSimWCHit.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCTrigger.hh"
#include "WCSimWCDAQMessenger.hh"

// added for LAPPD output
#include "TTree.h"
const int klappdhitnmax = 100000;
class WCSimRunAction;
class WCSimPrimaryGeneratorAction;
class G4Event;

class WCSimEventAction : public G4UserEventAction
{
private:
  WCSimRunAction* runAction;
  WCSimPrimaryGeneratorAction* generatorAction;
  WCSimDetectorConstruction*   detectorConstructor;
  WCSimWCDAQMessenger* DAQMessenger;
  
public:
  WCSimEventAction(WCSimRunAction*, WCSimDetectorConstruction*,
		   WCSimPrimaryGeneratorAction*);
  ~WCSimEventAction();
  
public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void FillRootEvent(G4int, 
		     const struct ntupleStruct&, 
		     G4TrajectoryContainer*,
		     WCSimWCDigitsCollection*,
		     WCSimWCTriggeredDigitsCollection*,
		     G4String detectorElement
		     );
  WCSimRunAction* GetRunAction(){return runAction;}
  void SetDigitizerChoice(G4String digitizer, G4String detectorElement){
    DigitizerChoices[detectorElement] = digitizer;
  }
  void SetTriggerChoice  (G4String trigger, G4String detectorElement){
    TriggerChoices[detectorElement] = trigger;
  }
  G4bool isANNIE;
  void CreateNewLAPPDFile();

private:
  G4int WCSimEventFindStartingVolume( G4ThreeVector vtx);
  G4int WCSimEventFindStoppingVolume( G4String stopVolumeName);
  G4int WCSimEventFindVertexVolume(G4ThreeVector vtx);

  ///Create instances of the user-chosen digitizer and trigger classes
  void  CreateDAQInstances();

  std::map<std::string, std::string> DigitizerChoices;
  std::map<std::string, std::string> TriggerChoices;
  bool     ConstructedDAQClasses;
  bool     SavedOptions;
  
  // Additions for truth hit readout of LAPPDs
  //==========================================
  G4int eventcount;
  
  G4double hitPosx, hitPosy, hitPosz, hitTime, hitEdep, hitWavelength;
  G4int hitPartCode, hitProcessCode, hitCopyNum, hitTrackID, hitPMTnumber, hitParentID, objnum, copynum;
  G4String hitParticleName, hitProcessName; 
  std::string hitPhysical;
 
  TFile *LAPPDfile;
  TTree *LAPPDtree;
  G4String LAPPDRootFileName;
  G4int lappd_numhits;
  G4String dirtDirectory;  // retrieved from file if present
  G4String genieDirectory;
 
  G4int lappdevt;
  std::vector<int> lappdhit_NoOfneighstripsHit;
  std::vector<int> lappdhit_neighstripnum;
  std::vector<double> lappdhit_neighstrippeak;
  std::vector<double> lappdhit_neighstrip_time;
  std::vector<double> lappdhit_neighstrip_lefttime;
  std::vector<double> lappdhit_neighstrip_righttime;
  G4double *lappdhit_x;
  G4double *lappdhit_y;
  G4double *lappdhit_z;
  G4int *lappdhit_process;
  G4int *lappdhit_particleID;
  G4int *lappdhit_trackID;
  G4double *lappdhit_edep;
  G4int *lappdhit_copynum;
  G4int *lappdhit_objnum;  
  G4int lappdhit_totalpes_perevt;

  std::vector<int> lappdhit_totalpes_perlappd2;
  std::vector<double> lappdhit_stripcoorx;
  std::vector<double> lappdhit_stripcoory;  
  std::vector<double> lappdhit_stripcoorz;
  std::vector<double> lappdhit_stripcoort;
  std::vector<double> lappdhit_globalcoorx;
  std::vector<double> lappdhit_globalcoory; 
  std::vector<double> lappdhit_globalcoorz;
  std::vector<float> lappdhit_truetime2, lappdhit_smeartime2;
  std::vector<int>   lappdhit_primaryParentID2;	
  std::vector<int>   lappdhit_directParentID2;
  std::vector<int> lappdhit_stripnum;
};


#endif

    
