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

// added for WChSandbox style output
#include "TTree.h"
const int kmrdhitnmax = 1000; 
const int kpmthitnmax = 100000;
// end additions
const int klappdhitnmax = 100000;
//const int klappdhitnmax2 = 100000;

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
		     WCSimWCTriggeredDigitsCollection* 
                     //,
		    // WCSimWCDigitsCollection*,
		    // WCSimWCTriggeredDigitsCollection*
		    );
  WCSimRunAction* GetRunAction(){return runAction;}
  void SetDigitizerChoice(G4String digitizer) { DigitizerChoice = digitizer; }
  void SetTriggerChoice  (G4String trigger)   { TriggerChoice   = trigger;   }

private:
  G4int WCSimEventFindStartingVolume( G4ThreeVector vtx);
  G4int WCSimEventFindStoppingVolume( G4String stopVolumeName);

  ///Create instances of the user-chosen digitizer and trigger classes
  void  CreateDAQInstances();

  G4String DigitizerChoice;
  G4String TriggerChoice;
  bool     ConstructedDAQClasses;
  
  // Additions for WChSandbox style SD readout of MRD/Veto
  //======================================================
  // out text file
  std::fstream* textout;
  std::ofstream* unaccountedparticlesandprocesses;
  
  G4int ConvertProcessNameToCode(TString processName);
  G4int ConvertParticleNameToCode(TString particleName);
  
  G4int eventcount;
  
  G4double hitPosx, hitPosy, hitPosz, hitTime, hitEdep, hitWavelength;
  G4int hitPartCode, hitProcessCode, hitCopyNum, hitTrackID, hitPMTnumber, hitParentID, objnum, copynum;
  G4String hitParticleName, hitProcessName; 
  std::string hitPhysical;
 
  TFile *mrdfile;
  TTree *mrdtree;
  TTree *facctree;
  TTree *ncvtree;
  TTree *mrdpmttree;
  TTree *faccpmttree;
  G4int mrd_numhits;
  G4int facc_numhits;
  G4int ncv_numhits;
  G4int mrdpmt_numhits;
  G4int faccpmt_numhits;
 
  TFile *LAPPDfile;
  TTree *LAPPDtree;
  G4int lappd_numhits;
 
  G4double *mrdhit_x;
  G4double *mrdhit_y;
  G4double *mrdhit_z;
  G4double *mrdhit_t;
  G4int *mrdhit_process;
  G4int *mrdhit_particleID;
  G4int *mrdhit_trackID;
  G4double *mrdhit_edep;
  G4int *mrdhit_copynum;
  G4int *mrdhit_objnum;

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
  G4double *lappdhit_t;
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
  std::vector<float> lappdhit_truetime2, lappdhit_smeartime2;
  std::vector<int>   lappdhit_primaryParentID2;	
  std::vector<int> lappdhit_stripnum;

  G4double *facchit_x;
  G4double *facchit_y;
  G4double *facchit_z;
  G4double *facchit_t;
  G4int *facchit_process;
  G4int *facchit_particleID;
  G4int *facchit_trackID;
  G4double *facchit_edep;
  G4int *facchit_copynum;
  G4int *facchit_objnum;
  
  G4double *mrdpmthit_x;
  G4double *mrdpmthit_y;
  G4double *mrdpmthit_z;
  G4double *mrdpmthit_t;
  G4int *mrdpmthit_process;
  G4int *mrdpmthit_trackID;
  G4int *mrdpmthit_parentID;
  G4double *mrdpmthit_wavelength;
  G4int *mrdpmthit_copynum; 
 
  G4double *faccpmthit_x;
  G4double *faccpmthit_y;
  G4double *faccpmthit_z;
  G4double *faccpmthit_t;
  G4int *faccpmthit_process;
  G4int *faccpmthit_trackID;
  G4int *faccpmthit_parentID;
  G4double *faccpmthit_wavelength;
  G4int *faccpmthit_copynum;
  
  G4double *ncvhit_x;
  G4double *ncvhit_y;
  G4double *ncvhit_z;
  G4double *ncvhit_t;
  G4int *ncvhit_process;
  G4int *ncvhit_trackID;
  G4int *ncvhit_parentID;
  G4int *ncvhit_particleID;
  G4double *ncvhit_edep;
  
};


#endif

    
