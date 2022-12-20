#include "WCSimEventAction.hh"
#include "WCSimTrajectory.hh"
#include "WCSimRunAction.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimWCHit.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCDigitizer.hh"
#include "WCSimWCTrigger.hh"
#include "WCSimWCAddDarkNoise.hh"
#include "WCSimWCPMT.hh"
#include "WCSimWCLAPPD.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimEventInformation.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4UImanager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh" 
#include "G4Navigator.hh" 
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include <set>
#include <iomanip>
#include <string>
#include <vector>
#include <climits>
#include <exception>

#include "jhfNtuple.h"
#include "TTree.h"
#include "TFile.h"
#include "WCSimRootEvent.hh"
#include "TStopwatch.h"
#include "TH1D.h"
#include "TH2D.h"

// GENIE headers
#ifndef NO_GENIE
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/Ntuple/NtpMCTreeHeader.h"
#include "Framework/Interaction/Interaction.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/ParticleData/PDGLibrary.h"
#endif
#include "NCVSD.hh"

#ifndef _SAVE_RAW_HITS
#define _SAVE_RAW_HITS
#ifndef _SAVE_RAW_HITS_VERBOSE
//#define _SAVE_RAW_HITS_VERBOSE
//#define NPMTS_VERBOSE 10
#endif
#endif
#ifndef SAVE_DIGITS_VERBOSE
//#define SAVE_DIGITS_VERBOSE
#endif
#ifndef TIME_DAQ_STEPS
//#define TIME_DAQ_STEPS
#endif

#ifndef HYPER_VERBOSITY
//#define HYPER_VERBOSITY
#endif

#ifndef NCAPTURE_VERBOSE
//#define NCAPTURE_VERBOSE
#endif

WCSimEventAction::WCSimEventAction(WCSimRunAction* myRun, 
				   WCSimDetectorConstruction* myDetector, 
				   WCSimPrimaryGeneratorAction* myGenerator)
  :runAction(myRun), generatorAction(myGenerator), 
   detectorConstructor(myDetector),
   ConstructedDAQClasses(false), LAPPDfile(0), SavedOptions(false)
{
  DAQMessenger = new WCSimWCDAQMessenger(this);
  DAQMessenger->AddDAQMessengerInstance("tank");
  DAQMessenger->AddDAQMessengerInstance("mrd");
  DAQMessenger->AddDAQMessengerInstance("facc");

  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  //create PMT response module
  WCSimWCPMT* WCDMPMT = new WCSimWCPMT( "WCReadoutPMT", myDetector, "tank");
  DMman->AddNewModule(WCDMPMT);

  //create dark noise module
  WCSimWCAddDarkNoise* WCDNM = new WCSimWCAddDarkNoise("WCDarkNoise", detectorConstructor, "tank");
  DMman->AddNewModule(WCDNM);
  
  isANNIE = detectorConstructor->GetIsANNIE();
  // Repeat for MRD
  if(isANNIE){
#ifdef HYPER_VERBOSITY
  G4cout<<"WCSimEventAction::WCSimEventAction ☆ making new WCSimWCPMT for mrd with name WCReadoutPMT_MRD"<<G4endl;
#endif
  WCSimWCPMT* WCDMPMT_MRD = new WCSimWCPMT( "WCReadoutPMT_MRD", myDetector, "mrd");
  DMman->AddNewModule(WCDMPMT_MRD);
#ifdef HYPER_VERBOSITY
  G4cout<<"WCSimEventAction::WCSimEventAction ☆ making new WCSimWCAddDarkNoise for mrd with name WCDarkNoise_MRD"<<G4endl;
#endif
  WCSimWCAddDarkNoise* WCDNM_MRD = new WCSimWCAddDarkNoise("WCDarkNoise_MRD", detectorConstructor, "mrd");
  DMman->AddNewModule(WCDNM_MRD);
  // Repeat for FACC
  WCSimWCAddDarkNoise* WCDNM_FACC = new WCSimWCAddDarkNoise("WCDarkNoise_FACC", detectorConstructor, "facc");
  DMman->AddNewModule(WCDNM_FACC);
  WCSimWCPMT* WCDMPMT_FACC = new WCSimWCPMT( "WCReadoutPMT_FACC", myDetector, "facc");
  DMman->AddNewModule(WCDMPMT_FACC);
  
  WCSimWCLAPPD* WCDMLAPPD = new WCSimWCLAPPD( "WCReadoutLAPPD", myDetector);
  DMman->AddNewModule(WCDMLAPPD);
  
 //----------------------
 // ****************
 //  LAPPD TRUTH HITS
 // ****************
  lappdhit_x = new G4double[klappdhitnmax]; // where/when was the hit?
  lappdhit_y = new G4double[klappdhitnmax];
  lappdhit_z = new G4double[klappdhitnmax];
  lappdhit_process = new G4int[klappdhitnmax];    // what was the interaction process?
  lappdhit_particleID = new G4int[klappdhitnmax]; // what was the particle type interacting?
  lappdhit_trackID = new G4int[klappdhitnmax];    // what was the track ID
  lappdhit_edep = new G4double[klappdhitnmax];    // how much energy was deposited?
  lappdhit_objnum = new G4int[klappdhitnmax];     // which geometry object was hit?
  //lappdhit_copynum = new G4int[klappdhitnmax];    // which copy was hit?
  }
  
  // Read files if present that tell us where the upstream files came from (for grid jobs)
  std::ifstream fin ("geniedirectory.txt"); // look for a file with this name in build dir
  if(not fin.is_open()){
    G4cout<<"WARNING: No geniedirectory.txt file found, upstream file path will not be recorded!"<<G4endl;
  } else {
    std::string Line;
    std::stringstream ssL;
    genieDirectory="";
    while (getline(fin, Line)){
      if (Line.empty()) continue;    // skip empty lines
      if (Line[0] == '#') continue;  // skip comment lines
      ssL.str("");                   // empty the stringstream
      ssL.clear();                   // clear the IO error status
      ssL << Line;                   // read the line
      if (ssL.str() != ""){          // skip empty lines
        ssL >> genieDirectory;       // space delimited
        break;                       // take the first accepted line
      }
    }
    G4cout<<"Upstream genie directory is: "<<genieDirectory<<G4endl;
    fin.close();
  }
  // repeat for the dirt directory
  fin.open("dirtdirectory.txt");
  if(not fin.is_open()){
    G4cout<<"WARNING: No dirtdirectory.txt file found, upstream file path will not be recorded!"<<G4endl;
  } else {
    std::string Line;
    std::stringstream ssL;
    dirtDirectory="";
    while (getline(fin, Line)){
      if (Line.empty()) continue;    // skip empty lines
      if (Line[0] == '#') continue;  // skip comment lines
      ssL.str("");                   // empty the stringstream
      ssL.clear();                   // clear the IO error status
      ssL << Line;                   // read the line
      if (ssL.str() != ""){          // skip empty lines
        ssL >> dirtDirectory;        // space delimited
        break;                       // take the first accepted line
      }
    }
    G4cout<<"Upstream dirt directory is: "<<dirtDirectory<<G4endl;
    fin.close();
  }

}

WCSimEventAction::~WCSimEventAction()
{
  delete DAQMessenger;
  
  if(isANNIE){
  // Actions to cleanup LAPPD truth hits:
  // ====================================
   if(LAPPDfile){LAPPDfile->Close(); delete LAPPDfile; LAPPDfile=0;}
   
   delete[] lappdhit_x;
   delete[] lappdhit_y;
   delete[] lappdhit_z;
   delete[] lappdhit_process;
   delete[] lappdhit_particleID;
   delete[] lappdhit_trackID;
   delete[] lappdhit_edep;
   delete[] lappdhit_objnum;
   }
}

void WCSimEventAction::CreateDAQInstances()
{
  if(ConstructedDAQClasses) {
    G4cerr << "WCSimEventAction::CreateDAQInstances() has already been called. Exiting..." << G4endl;
    return;
    exit(-1);
  }

  G4cout << "Creating digitizer and trigger class instances in WCSimEventAction::CreateDAQInstances()" << G4endl;

  G4DigiManager* DMman = G4DigiManager::GetDMpointer();

  //create your choice of digitizer module
  if(DigitizerChoices.at("tank") == "SKI") {
    WCSimWCDigitizerSKI* WCDM = new WCSimWCDigitizerSKI("WCReadoutDigits", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCDM);
  }
  else {
    G4cerr << "Unknown DigitizerChoice " << DigitizerChoices.at("tank") << G4endl;
    exit(-1);
  }

  //create your choice of trigger module
  if(TriggerChoices.at("tank") == "NDigits") {
    WCSimWCTriggerNDigits* WCTM = new WCSimWCTriggerNDigits("WCReadout", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCTM);
  }
  else if(TriggerChoices.at("tank") == "NDigits2") {
    WCSimWCTriggerNDigits2* WCTM = new WCSimWCTriggerNDigits2("WCReadout", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCTM);
  }
  else if(TriggerChoices.at("tank") == "NoTrigger") {
    WCSimWCTriggerNoTrigger* WCTM = new WCSimWCTriggerNoTrigger("WCReadout", detectorConstructor, DAQMessenger, "tank");
    DMman->AddNewModule(WCTM);
  }
  else {
    G4cerr << "Unknown TriggerChoice " << TriggerChoices.at("tank") << G4endl;
    exit(-1);
  }

  if(isANNIE){
    // Repeat for MRD
    if(DigitizerChoices.at("mrd")=="SKI"){
#ifdef HYPER_VERBOSITY
      G4cout<<"WCSimEventAction::CreateDAQInstances ☆ making new WCSimWCDigitizerSKI for mrd with name WCReadoutDigits_MRD"<<G4endl;
#endif
      WCSimWCDigitizerSKI* WCDM_MRD = new WCSimWCDigitizerSKI("WCReadoutDigits_MRD", detectorConstructor, DAQMessenger, "mrd");
      DMman->AddNewModule(WCDM_MRD);
    } else {
      G4cerr << "Unknown MRD DigitizerChoice " << DigitizerChoices.at("mrd") << G4endl;
      exit(-1);
    }
    // repeat for FACC
    if(DigitizerChoices.at("facc")=="SKI"){
      WCSimWCDigitizerSKI* WCDM_FACC = new WCSimWCDigitizerSKI("WCReadoutDigits_FACC", detectorConstructor, DAQMessenger, "facc");
      DMman->AddNewModule(WCDM_FACC);
    } else {
      G4cerr << "Unknown FACC DigitizerChoice " << DigitizerChoices.at("facc") << G4endl;
      exit(-1);
    }
    
    
#ifdef HYPER_VERBOSITY
    G4cout<<"WCSimEventAction::CreateDAQInstances ☆ making new WCSimWCTriggerOnTankDigits for mrd with name WCReadout_MRD"<<G4endl;
#endif
    if(TriggerChoices.at("mrd") == "NoTrigger") {
      // for 'NoTrigger' trigger, which just reads out all hits/digits, need to use the same for MRD/FACC.
      // Used for things like MRD testing, where we fire muons into the MRD
      WCSimWCTriggerNoTrigger* WCTM_MRD = new WCSimWCTriggerNoTrigger("WCReadout_MRD", detectorConstructor, DAQMessenger, "mrd");
      DMman->AddNewModule(WCTM_MRD);
    } else if (TriggerChoices.at("mrd")=="NDigits"){
      WCSimWCTriggerNDigits* WCTM_MRD = new WCSimWCTriggerNDigits("WCReadout_MRD", detectorConstructor, DAQMessenger, "mrd");
      DMman->AddNewModule(WCTM_MRD);
    } else if (TriggerChoices.at("mrd")=="NDigits2"){
      WCSimWCTriggerNDigits2* WCTM_MRD = new WCSimWCTriggerNDigits2("WCReadout_MRD", detectorConstructor, DAQMessenger, "mrd");
      DMman->AddNewModule(WCTM_MRD);
    } else if(TriggerChoices.at("mrd")=="TankDigits"){
      // for an ANNIE-style tank trigger, MRD and FACC read out whatever digits are within tank trigger windows
      WCSimWCTriggerOnTankDigits* WCTM_MRD = new WCSimWCTriggerOnTankDigits("WCReadout_MRD", detectorConstructor, DAQMessenger, "mrd");
      DMman->AddNewModule(WCTM_MRD);
    } else {
      G4cerr<<"Unknown trigger choice for mrd: "<<TriggerChoices.at("mrd") <<G4endl;
      exit(-1);
    }
    
      // repeat for facc
    if(TriggerChoices.at("facc") == "NoTrigger") {
      WCSimWCTriggerNoTrigger* WCTM_FACC = new WCSimWCTriggerNoTrigger("WCReadout_FACC", detectorConstructor, DAQMessenger, "facc");
      DMman->AddNewModule(WCTM_FACC);
    } else if (TriggerChoices.at("facc")=="NDigits"){
      WCSimWCTriggerNDigits* WCTM_FACC = new WCSimWCTriggerNDigits("WCReadout_FACC", detectorConstructor, DAQMessenger, "facc");
      DMman->AddNewModule(WCTM_FACC);
    } else if (TriggerChoices.at("facc")=="NDigits2"){
      WCSimWCTriggerNDigits2* WCTM_FACC = new WCSimWCTriggerNDigits2("WCReadout_FACC", detectorConstructor, DAQMessenger, "facc");
      DMman->AddNewModule(WCTM_FACC);
    } else if(TriggerChoices.at("facc")=="TankDigits"){
      // for an ANNIE-style tank trigger, MRD and FACC read out whatever digits are within tank trigger windows
      WCSimWCTriggerOnTankDigits* WCTM_FACC = new WCSimWCTriggerOnTankDigits("WCReadout_FACC", detectorConstructor, DAQMessenger, "facc");
      DMman->AddNewModule(WCTM_FACC);
    } else {
      G4cerr<<"Unknown trigger choice for mrd: "<<TriggerChoices.at("mrd") <<G4endl;
      exit(-1);
    }
    
  }
  
  ConstructedDAQClasses = true;
}


void WCSimEventAction::BeginOfEventAction(const G4Event* evt)
{
  if(!ConstructedDAQClasses)
    CreateDAQInstances();
  
  // equivalent of beginrunaction using event id
  if(isANNIE&&(evt->GetEventID()==0)){ CreateNewLAPPDFile(); }
}

void WCSimEventAction::EndOfEventAction(const G4Event* evt)
{
  G4cout<<G4endl;
  G4cout<<"############# WCSIM  BEGIN END OF EVENT ACTION  ################"<<G4endl;
  G4int         event_id = evt->GetEventID();
  // ----------------------------------------------------------------------
  //  Get Particle Table
  // ----------------------------------------------------------------------
  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  
  // ----------------------------------------------------------------------
  //  Get Trajectory Container
  // ----------------------------------------------------------------------
  
  G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
  
  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();
    
  // ----------------------------------------------------------------------
  //  Get WC Hit Collection
  // ----------------------------------------------------------------------
    
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Get Hit collections of this event
  G4HCofThisEvent* HCE         = evt->GetHCofThisEvent();
  WCSimWCHitsCollection* WCHC = 0;

  G4String WCIDCollectionName = detectorConstructor->GetIDCollectionName();
  std::vector<G4String> tankcollectionnames = detectorConstructor->GetIDCollectionNames();
  if(tankcollectionnames.size()==0) tankcollectionnames.push_back(WCIDCollectionName);
  G4int collectionID;

  G4cout<<"-------------------------> I'm in event: event_id: "<<event_id<<G4endl;
  if (HCE)
  {
  for(auto atankcollectionname : tankcollectionnames){
    collectionID = SDman->GetCollectionID(atankcollectionname);
    WCHC = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
    //G4cout<<atankcollectionname<<" has "<<WCHC->entries()<<" entries"<<G4endl;
    //G4cout<<"-----------name= "<<name<<" name2= "<<name2<<"--------"<<G4endl;
  
  // To use Do like This:
  // --------------------
  //   if (WCHC)
  //     for (G4int i=0; i< WCHC->entries() ;i++)
  //       G4cout << (*WCHC)[i]->GetTotalPe() << G4endl;
  
  // ----------------------------------------------------------------------
  //  Get Digitized Hit Collection
  // ----------------------------------------------------------------------
  }
  } else {G4cerr<<"Could not find hit colletion of event!"<<G4endl;}
  /** used if save raw hits is enabled in preprocessor - WCHC is a member variable used in FillRootEvent() */
  
  // Get a pointer to the Digitizing Module Manager
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  
  // Get a pointer to the WC PMT module
  WCSimWCPMT* WCDMPMT =
    (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT");
  
  // new MFechner, aug 2006
  // need to clear up the old info inside PMT
  WCDMPMT->ReInitialize();
  
#ifdef TIME_DAQ_STEPS
  TStopwatch* ms = new TStopwatch();
  ms->Start();
#endif

  //Convert the hits to PMT pulse
  G4cout<<"Dititizing WCDMPMT"<<G4endl;
  WCDMPMT->Digitize();
  
  // Do the Dark Noise, then Digitization, then Trigger
  // First, add Dark noise hits before digitizing
    
  //Get a pointer to the WC Dark Noise Module
  WCSimWCAddDarkNoise* WCDNM =
    (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise");
  
  //Add the dark noise
  G4cout<<"Adding dark noise with WCDNM"<<G4endl;
  WCDNM->AddDarkNoise();

  // Next, do the digitization
  
  //Get a pointer to the WC Digitizer Module
  WCSimWCDigitizerBase* WCDM =
    (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits");

  //Digitize the hits
  G4cout<<"Digitizing WCDM"<<G4endl;
  WCDM->Digitize();

  // Finally, apply the trigger
  
  //Get a pointer to the WC Trigger Module
  WCSimWCTriggerBase* WCTM =
    (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");
  
  //tell it the dark noise rate (for calculating the average dark occupancy -> can adjust the NDigits threshold)
  G4cout<<"Setting dark rate for WCDNM"<<G4endl;
  WCTM->SetDarkRate(WCDNM->GetDarkRate());
  
  //Apply the trigger
  // This takes the digits, and places them into trigger gates
  // Also throws away digits not contained in an trigger gate
  G4cout<<"Digitizing WCTM"<<G4endl;
  WCTM->Digitize();

#ifdef TIME_DAQ_STEPS
  ms->Stop();
  G4cout << " Digtization :  Real = " << ms->RealTime() 
    	    << " ; CPU = " << ms->CpuTime() << "\n";  
#endif

   // Get the post-noise hit collection for the WC
   G4int WCDChitsID = DMman->GetDigiCollectionID("WCRawPMTSignalCollection");
   WCSimWCDigitsCollection * WCDC_hits = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsID);
  
   // Get the digitized collection for the WC
   G4int WCDCID = DMman->GetDigiCollectionID("WCDigitizedCollection");
   WCSimWCTriggeredDigitsCollection * WCDC = (WCSimWCTriggeredDigitsCollection*) DMman->GetDigiCollection(WCDCID);
  
  /*
  // To use Do like This:
  // -------------------
  if(WCDC){
    for (G4int i=0; i < WCDC->entries(); i++) {
      G4int   tubeID         = (*WCDC)[i]->GetTubeID();
      G4float photoElectrons = (*WCDC)[i]->GetPe(i);
      //	 G4cout << "time " << i << " " <<time << G4endl; 
      //	 G4cout << "tubeID " << i << " " <<tubeID << G4endl; 
      //	 G4cout << "Pe " << i << " " <<photoElectrons << G4endl; 
      //   (*WCDC)[i]->Print();
    }
  }
  */
  
  WCSimWCDigitsCollection* WCDC_hits_MRD;
  WCSimWCTriggeredDigitsCollection* WCDC_MRD;
  WCSimWCDigitsCollection* WCDC_hits_FACC;
  WCSimWCTriggeredDigitsCollection* WCDC_FACC;
  if(isANNIE){
    if(detectorConstructor->GetTotalNumLAPPDs()!=0){
    WCSimWCHitsCollection* WCHClappd = 0;
    G4String WCIDCollectionName2 = detectorConstructor->GetIDCollectionName2();
    G4int collectionID2;
    collectionID2 = SDman->GetCollectionID(WCIDCollectionName2);
    WCHClappd = (WCSimWCHitsCollection*)HCE->GetHC(collectionID2);
    std::vector<int> objnumv;
    
    // ----------------------------------------------------------------------
    //  Get Digitized Hit Collection
    // ----------------------------------------------------------------------
    //G4cout<<"WCIDCollectionName= "<<WCIDCollectionName<<" WCIDCollectionName2= "<<WCIDCollectionName2<<G4endl;
    //G4cout<<"____________ WCHC->entries()= "<<int(WCHC->entries())<<G4endl;
    //G4cout<<"____________ WCHClappd->entries()= "<<int(WCHClappd->entries())<<G4endl;
    //if(WCHClappd->entries()>0.){ G4cout<<"GOTIT!!!!!!"<<G4endl; }
    for (G4int ii=0; ii< WCHClappd->entries() ;ii++){
      //G4cout<<"total pe @ LAPPDs: "<< (*WCHClappd)[ii]->GetTotalPe() << G4endl;
      G4int   lappdID         = (*WCHClappd)[ii]->GetTubeID();
      objnumv.push_back(lappdID);
    }
    //--------- STORE lappd HITS -------------
    //=========================================
    lappdevt=evt->GetEventID();
    G4double totalEnergy2 = 0.;
    G4int numberOfHits2 = WCHClappd->GetSize();
    //G4cout << "&&&&  A total of " << numberOfHits2 << " hits on lappd were recorded!" << G4endl;
    if (WCHClappd!=0) {
      G4int totalpes_perevt = 0; G4int lappd_numhits0=0;
      for (G4int hitnum=0; hitnum<numberOfHits2; hitnum++) {
         lappd_numhits0++;
         WCSimWCHit* aHit = (*WCHClappd)[hitnum];
         G4ThreeVector hitPos = aHit->GetPos();
         hitPosx=hitPos.x();
         hitPosy=hitPos.y();
         hitPosz=hitPos.z();
         //hitParticleName = aHit->GetParticleName();
         hitTrackID = aHit->GetTrackID();
         for (G4int ip =0; ip < (*WCHClappd)[hitnum]->GetTotalPe(); ip++){
           lappdhit_process[hitnum] = hitProcessCode;
           double strip_coorx = ((*WCHClappd)[hitnum]->GetStripPosition(ip).x());
           double strip_coory = ((*WCHClappd)[hitnum]->GetStripPosition(ip).y());
           double strip_coorz = ((*WCHClappd)[hitnum]->GetStripPosition(ip).z());
           double strip_coort;
           try{
             strip_coort = aHit->GetTime(ip);
           } 
           catch (...){
             G4cerr<<"Error in WCSimEventAction::EndOfEventAction() calling WCSimWCHit::GetTime "<<G4endl;
             //assert(false);
             strip_coort = -997.;
           }
           double global_coorx = ((*WCHClappd)[hitnum]->GetGlobalPos(ip).x());
           double global_coory = ((*WCHClappd)[hitnum]->GetGlobalPos(ip).y());
           double global_coorz = ((*WCHClappd)[hitnum]->GetGlobalPos(ip).z());
           //G4cout<<"totalpes_perevt= "<<totalpes_perevt<<"--->GetStripPosition= "
           //      <<(*WCHClappd)[hitnum]->GetStripPosition(ip)
           //      <<" strip_coorx= "<<strip_coorx<<" strip_coory= "<<strip_coory<<G4endl;
           lappdhit_stripcoorx.push_back(strip_coorx);
           lappdhit_stripcoory.push_back(strip_coory);
           lappdhit_stripcoorz.push_back(strip_coorz);
           lappdhit_stripcoort.push_back(strip_coort);
           lappdhit_globalcoorx.push_back(global_coorx);
           lappdhit_globalcoory.push_back(global_coory);
           lappdhit_globalcoorz.push_back(global_coorz);
           totalpes_perevt++;
         }
         //hitPartCode = aHit->GetParticleID();
         //hitPartCode = ConvertParticleNameToCode(hitParticleName); // or use hitParticleID - from PDGEncoding?
         //hitProcessName = aHit->GetProcessName();
         //hitProcessCode = ConvertProcessNameToCode(hitProcessName);
         //if(hitProcessCode<0){G4cout<<"lappd hit process unaccounted"<<G4endl;}
         hitEdep = (*WCHClappd)[hitnum]->GetTotalPe();  //aHit->GetEdeposit();
         //hitCopyNum = aHit->GetCopyNum();
         //hitPhysical = (std::string)aHit->GetPhysical();
         //aHit->Print();
         lappdhit_x[hitnum] = hitPosx;
         lappdhit_y[hitnum] = hitPosy;
         lappdhit_z[hitnum] = hitPosz;
         lappdhit_particleID[hitnum] = hitPartCode;
         lappdhit_trackID[hitnum] = hitTrackID;
         lappdhit_edep[hitnum] = hitEdep;
         //lappdhit_copynum[hitnum] = hitCopyNum;
         lappdhit_objnum[hitnum] = (objnumv[hitnum]);
         //G4cout<<"lappdhit_objnum[hitnum] = "<<lappdhit_objnum[hitnum]<<G4endl;
      }
      lappd_numhits = lappd_numhits0;
      lappdhit_totalpes_perevt = totalpes_perevt;
      
      //G4cout<<"lappd_numhits= "<<lappd_numhits<<" lappdhit_totalpes_perevt = "<<lappdhit_totalpes_perevt<<G4endl;
      //LAPPDtree ->Fill();
      /*
      for(int m=0; m<totalpes_perevt; m++){
        G4cout<<"tellme: "<<lappdhit_stripcoorx[m]<<","<<lappdhit_stripcoory[m]<<","<<lappdhit_stripcoorz[m]<<G4endl; 
      }
      */
    }
////////////////////
// y  G4String WCMRDCollectionName = detectorConstructor->GetMRDCollectionName();
// y  if(HCE){
// y  collectionID = SDman->GetCollectionID(WCMRDCollectionName);
// y  WCSimWCHitsCollection* WCHC_MRD = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
// y  }
// y  WCSimWCPMT* WCDMPMT_MRD = (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT_MRD");
// y  if(WCDMPMT_MRD==0){G4cerr<<"WCReadoutPMT_MRD digitzer module not found!"<<G4endl;}
// y  WCDMPMT_MRD->ReInitialize();
// y  WCDMPMT_MRD->Digitize();   <<< constructs digit collection (1 digit per PMT containing all photon hits)
// -------
// n  WCSimWCAddDarkNoise* WCDNM_MRD = (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise_MRD");
// n  if(WCDNM_MRD==0){G4cerr<<"WCDarkNoise_MRD dark noise module not found!"<<G4endl;}
// n  WCDNM_MRD->AddDarkNoise(); <<< adds dark noise to digits collection, i.e. fake photon hits
// n  WCSimWCDigitizerBase* WCDM_MRD = (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits_MRD");
// n  if(WCDM_MRD==0){G4cerr<<"WCReadoutDigits_MRD digitizer module not found!"<<G4endl;}
// n  WCDM_MRD->Digitize();      <<< clusters hits according to integration window (bunch of photons->digit)
// n  WCSimWCTriggerBase* WCTM_MRD = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_MRD");
// n  if(WCTM_MRD==0){G4cerr<<"WCReadout_MRD trigger module not found!"<<G4endl;}
// n  WCTM_MRD->SetDarkRate(WCDNM_MRD->GetDarkRate());
// n  WCTM_MRD->Digitize();      <<< constructs triggered digit collection (separates digits per trigger)
// n  /** these are retrieved to pass to FillRootEvent() */
// y  G4int WCDChitsID_MRD = DMman->GetDigiCollectionID("WCRawPMTSignalCollection_MRD");
// y  WCDC_hits_MRD = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsID_MRD);
// n  G4int WCDCID_MRD = DMman->GetDigiCollectionID("WCDigitizedCollection_MRD");
// n  WCDC_MRD = (WCSimWCTriggeredDigitsCollection*) DMman->GetDigiCollection(WCDCID_MRD);
////////////////////
    WCSimWCLAPPD* WCDMLAPPD = (WCSimWCLAPPD*)DMman->FindDigitizerModule("WCReadoutLAPPD");
    WCDMLAPPD->ReInitialize();
    
    G4cout<<"Digizing WCDMLAPPD"<<G4endl;
    WCDMLAPPD->Digitize(); // fills WCRawLAPPDSignalCollection
    
    G4int WCDChitsIDlappd = DMman->GetDigiCollectionID("WCRawLAPPDSignalCollection");
    WCSimWCDigitsCollection * WCDC_hitslappd = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsIDlappd);
    
    if (WCDC_hitslappd) {
     // add the truth raw hits
#ifdef _SAVE_RAW_HITS_VERBOSE
    G4cout<<"RAW HITS"<<G4endl;
#endif
     // wcsimrootevent->SetNumTubesHit(WCDC_hitslappd->entries());
     // std::vector<float> truetime2, smeartime2;
     // std::vector<int>   primaryParentID2;
     double hit_time_smear, hit_time_true;
     int hit_parentid; G4int id0=0;
     //loop over the DigitsCollection
     for(int idigi = 0; idigi < WCDC_hitslappd->entries(); idigi++) {
        int digi_tubeid = (*WCDC_hitslappd)[idigi]->GetTubeID();
        //G4cout<<"digi_lappdid= "<<digi_tubeid<<"/"<<(*WCDC_hitslappd)[idigi]->GetTotalPe()<<G4endl;
        lappdhit_totalpes_perlappd2.push_back((*WCDC_hitslappd)[idigi]->GetTotalPe());
        
        for(G4int id = 0; id < (*WCDC_hitslappd)[idigi]->GetTotalPe(); id++){
          id0++;
          hit_time_true  = (*WCDC_hitslappd)[idigi]->GetPreSmearTime(id);
          hit_parentid = (*WCDC_hitslappd)[idigi]->GetParentID(id);
          //G4cout<<"0___LAPPD idigi= "<<idigi<<" id= "<<id<<"/"<<(*WCDC_hitslappd)[idigi]->GetTotalPe()<<G4endl;
          //G4cout<<"id0= "<<id0<<" hit_time_true= "<<hit_time_true<<" hit_parentid= "<<hit_parentid<<G4endl;
          lappdhit_truetime2.push_back(hit_time_true);
          lappdhit_primaryParentID2.push_back(hit_parentid);
          ////---strip number and digitised hits-----
          int stripno = (*WCDC_hitslappd)[idigi]->GetStripNo(id);
          lappdhit_stripnum.push_back(stripno);
          //G4cout<<"LAPPD idigi= "<<idigi<<" id= "<<id<<" stripno= "<<stripno<<G4endl;
          std::map<int,double> stripno_peak=(*WCDC_hitslappd)[idigi]->GetNeighStripNo(id);
          int stiphit=0;
          for(std::map<int,double>::iterator m1=stripno_peak.begin(); m1!=stripno_peak.end(); ++m1){
             //G4cout<<"DIGImap____"<<(m1)->first<<","<<(m1)->second<<G4endl;
             stiphit++;
             lappdhit_neighstripnum.push_back( (m1)->first );
             lappdhit_neighstrippeak.push_back( (m1)->second );
          }
          stripno_peak.clear();
          //G4cout<<"We had "<<stiphit<<" neighbouring strips hit!"<<G4endl;
          lappdhit_NoOfneighstripsHit.push_back(stiphit);
         
          std::map<int,double> stripno_time=(*WCDC_hitslappd)[idigi]->GetNeighStripTime(id);
          for(std::map<int,double>::iterator m2=stripno_time.begin(); m2!=stripno_time.end(); ++m2){
            //G4cout<<"DIGItime____"<<(m2)->first<<","<<(m2)->second<<G4endl;
            lappdhit_neighstrip_time.push_back( (m2)->second );
          }
          stripno_time.clear();
          std::map<int,double> stripno_lefttime=(*WCDC_hitslappd)[idigi]->GetNeighStripLeftTime(id);
          for(std::map<int,double>::iterator m3=stripno_lefttime.begin(); m3!=stripno_lefttime.end(); ++m3){
            //G4cout<<"DIGIlefttime____"<<(m3)->first<<","<<(m3)->second<<G4endl;
            lappdhit_neighstrip_lefttime.push_back( (m3)->second );
          }
          stripno_lefttime.clear();
          std::map<int,double> stripno_righttime=(*WCDC_hitslappd)[idigi]->GetNeighStripRightTime(id);
          for(std::map<int,double>::iterator m4=stripno_righttime.begin(); m4!=stripno_righttime.end(); ++m4){
            //G4cout<<"DIGIrighttime____"<<(m4)->first<<","<<(m4)->second<<G4endl;
            lappdhit_neighstrip_righttime.push_back( (m4)->second );
          }
          stripno_righttime.clear();
          //-------
          hit_time_smear = (*WCDC_hitslappd)[idigi]->GetTime(id);
          lappdhit_smeartime2.push_back(hit_time_smear);
          //G4cout<<"hit_time_smear= "<<hit_time_smear<<G4endl;
        } //id
#ifdef _SAVE_RAW_HITS_VERBOSE
        if(digi_tubeid < NPMTS_VERBOSE) {
          G4cout << "Adding " << lappdhit_truetime2.size()
                 << " Cherenkov hits in tube " << digi_tubeid
                 << " with truetime:smeartime:primaryparentID";
          for(G4int id = 0; id < lappdhit_truetime2.size(); id++) {
             G4cout << " " << lappdhit_truetime2[id]
                    << ":" << lappdhit_smeartime2[id]
                    << ":" << lappdhit_primaryParentID2[id];
          }//id
         G4cout << G4endl;
        }
#endif
        /*
        smeartime2.clear();
        truetime2.clear();
        primaryParentID2.clear();
        */
      } //idigi
    }
    LAPPDtree->Fill();
    LAPPDfile->cd();
    LAPPDfile->Write("",TObject::kOverwrite);
    lappdhit_NoOfneighstripsHit.clear();
    lappdhit_stripcoorx.clear();
    lappdhit_stripcoory.clear();
    lappdhit_stripcoorz.clear();
    lappdhit_stripcoort.clear();
    lappdhit_globalcoorx.clear();
    lappdhit_globalcoory.clear();
    lappdhit_globalcoorz.clear();
    lappdhit_totalpes_perlappd2.clear();
    objnumv.clear();
    lappdhit_smeartime2.clear();
    lappdhit_truetime2.clear();
    lappdhit_primaryParentID2.clear();
    lappdhit_stripnum.clear();
    lappdhit_neighstripnum.clear();
    lappdhit_neighstrippeak.clear();
    lappdhit_neighstrip_time.clear();
    lappdhit_neighstrip_lefttime.clear();
    lappdhit_neighstrip_righttime.clear();
    
    } // end of if TotalNumLAPPDs!=0
    
    // Repeat the steps for the MRD and FACC
    G4cout<<G4endl<<G4endl;
    G4String WCMRDCollectionName = detectorConstructor->GetMRDCollectionName();
    if(HCE){
    collectionID = SDman->GetCollectionID(WCMRDCollectionName);
    WCSimWCHitsCollection* WCHC_MRD = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
  #ifdef HYPER_VERBOSITY
      G4cout<<"WCSimEventAction::EndOfEventAction ☆ (WCSimWCHitsCollection*)"<<WCMRDCollectionName
            <<" has "<<WCHC_MRD->entries()<<" entries"<<G4endl;
  #endif
    }
    WCSimWCPMT* WCDMPMT_MRD = (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT_MRD");
    if(WCDMPMT_MRD==0){G4cerr<<"WCReadoutPMT_MRD digitzer module not found!"<<G4endl;}
    WCDMPMT_MRD->ReInitialize();
  #ifdef HYPER_VERBOSITY
    G4cout<<"WCSimEventAction::EndOfEventAction ☆ Calling Digitize on (WCSimWCPMT*)WCReadoutPMT_MRD"<<G4endl;
  #endif
    WCDMPMT_MRD->Digitize();
    WCSimWCAddDarkNoise* WCDNM_MRD = (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise_MRD");
    if(WCDNM_MRD==0){G4cerr<<"WCDarkNoise_MRD dark noise module not found!"<<G4endl;}
  #ifdef HYPER_VERBOSITY
    G4cout<<"WCSimEventAction::EndOfEventAction ☆ Calling AddDarkNoise on (WCSimWCAddDarkNoise*)WCDarkNoise_MRD"<<G4endl;
  #endif
    WCDNM_MRD->AddDarkNoise();
    WCSimWCDigitizerBase* WCDM_MRD = (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits_MRD");
    if(WCDM_MRD==0){G4cerr<<"WCReadoutDigits_MRD digitizer module not found!"<<G4endl;}
  #ifdef HYPER_VERBOSITY
    G4cout<<"WCSimEventAction::EndOfEventAction ☆ Calling Digitize on (WCSimWCDigitizerBase*)WCReadoutDigits_MRD"<<G4endl;
  #endif
    WCDM_MRD->Digitize();
    WCSimWCTriggerBase* WCTM_MRD = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_MRD");
    if(WCTM_MRD==0){G4cerr<<"WCReadout_MRD trigger module not found!"<<G4endl;}
    WCTM_MRD->SetDarkRate(WCDNM_MRD->GetDarkRate());
  #ifdef HYPER_VERBOSITY
    G4cout<<"WCSimEventAction::EndOfEventAction ☆ Calling Digitize on (WCSimWCTriggerBase*)WCReadout_MRD"<<G4endl;
  #endif
    WCTM_MRD->Digitize();
    /** these are retrieved to pass to FillRootEvent() */
  #ifdef HYPER_VERBOSITY
    G4cout<<"WCSimEventAction::EndOfEventAction ☆ retrieving raw hits (WCSimWCDigitsCollection*)WCRawPMTSignalCollection_MRD for FillRootEvent, which has ";
  #endif
    G4int WCDChitsID_MRD = DMman->GetDigiCollectionID("WCRawPMTSignalCollection_MRD");
    WCDC_hits_MRD = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsID_MRD);
  #ifdef HYPER_VERBOSITY
    if(WCDC_hits_MRD){G4cout<<WCDC_hits_MRD->entries();} else {G4cout<<"no";} G4cout<<" entries"<<G4endl;
    G4cout<<"WCSimEventAction::EndOfEventAction ☆ retrieving readout hits (WCSimWCTriggeredDigitsCollection*)WCDigitizedCollection_MRD for FillRootEvent, which has ";
  #endif
    G4int WCDCID_MRD = DMman->GetDigiCollectionID("WCDigitizedCollection_MRD");
    WCDC_MRD = (WCSimWCTriggeredDigitsCollection*) DMman->GetDigiCollection(WCDCID_MRD);
  #ifdef HYPER_VERBOSITY
    if(WCDC_hits_MRD){G4cout<<WCDC_MRD->entries();} else {G4cout<<"no";} G4cout<<" entries"<<G4endl;
  #endif
    ///////////////////////////////
    // Repeat for FACC
    G4cout<<G4endl<<G4endl;
    G4String WCFACCCollectionName = detectorConstructor->GetFACCCollectionName();
    if(HCE){
    collectionID = SDman->GetCollectionID(WCFACCCollectionName);
    WCSimWCHitsCollection* WCHC_FACC = (WCSimWCHitsCollection*)HCE->GetHC(collectionID);
    }
    WCSimWCPMT* WCDMPMT_FACC = (WCSimWCPMT*)DMman->FindDigitizerModule("WCReadoutPMT_FACC");
    WCDMPMT_FACC->ReInitialize();
    WCDMPMT_FACC->Digitize();
    WCSimWCAddDarkNoise* WCDNM_FACC = (WCSimWCAddDarkNoise*)DMman->FindDigitizerModule("WCDarkNoise_FACC");
    WCDNM_FACC->AddDarkNoise();
    WCSimWCDigitizerBase* WCDM_FACC = (WCSimWCDigitizerBase*)DMman->FindDigitizerModule("WCReadoutDigits_FACC");
    WCDM_FACC->Digitize();
    WCSimWCTriggerBase* WCTM_FACC = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_FACC");
    WCTM_FACC->SetDarkRate(WCDNM_FACC->GetDarkRate());
    WCTM_FACC->Digitize();
    G4int WCDChitsID_FACC = DMman->GetDigiCollectionID("WCRawPMTSignalCollection_FACC");
    WCDC_hits_FACC = (WCSimWCDigitsCollection*) DMman->GetDigiCollection(WCDChitsID_FACC);
    G4int WCDCID_FACC = DMman->GetDigiCollectionID("WCDigitizedCollection_FACC");
    WCDC_FACC = (WCSimWCTriggeredDigitsCollection*) DMman->GetDigiCollection(WCDCID_FACC);
    /////////////////////////////////////////////////////
    
  // ----------------------------------------------------------------------
  //  Get Event Information
  // ----------------------------------------------------------------------
  // pull information about the genie primary interactions from the PrimaryGeneratorAction
#ifdef NO_GENIE
  G4cout<<" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<G4endl;
  G4cout<<" !!! Genie not loaded!!! Primary interaction information will not be filled! !!!"<<G4endl;
  G4cout<<" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<G4endl;
#endif
  }
  
  G4int         mode     = generatorAction->GetMode();			// neut interaction code
  G4int         nvtxs   = generatorAction->GetNvtxs();			// always 1
  G4ThreeVector vtxs[MAX_N_PRIMARIES];					// interaction vertices of the neutrino
  G4int         vtxsvol[MAX_N_PRIMARIES];				// volumes of the vertices if applicable
  G4int         vecRecNumber = generatorAction->GetVecRecNumber();	// 

  // ----------------------------------------------------------------------
  //  Fill Ntuple
  // ----------------------------------------------------------------------
   jhfNtuple.mode   = mode;                   // interaction mode
   jhfNtuple.nvtxs = nvtxs;                   // number of neutrino vertices
   jhfNtuple.vecRecNumber = vecRecNumber;     // vectorfile record number
   
   // mustop, pstop, npar will be filled later
   // Next in the ntuple is an array of tracks. 
   // We will keep count with npar: there are twice as many primary 'tracks' as primary vertices,
   // for storing information about the primary probe and target.
   G4int npar = 0;
   
   // First two tracks for each vertex are special: beam and target
   for( Int_t u=0; u<nvtxs; u++ ){
     
     // neutrino vertex information
     vtxs[u]      = generatorAction->GetVtx(u);
     //vtxsvol[u]   = WCSimEventFindStartingVolume(vtxs[u]);
     vtxsvol[u]   = WCSimEventFindVertexVolume(vtxs[u]);
     jhfNtuple.vtxs[u][0] = vtxs[u][0]/CLHEP::cm;    // interaction vertex
     jhfNtuple.vtxs[u][1] = vtxs[u][1]/CLHEP::cm;    // interaction vertex
     jhfNtuple.vtxs[u][2] = vtxs[u][2]/CLHEP::cm;    // interaction vertex
     jhfNtuple.vtxsvol[u] = vtxsvol[u];              // looked up at start of EndOfEventAction
     /* !! jhfNtuple.vtxs & jhfNtuple.vtxsvol arrays have half as many elements as others in jhfNtuple!! */
     
     /////////////////////////////////
     // npar = 0        NEUTRINO /////
     /////////////////////////////////
   
     G4int         beampdg;
     G4double      beamenergy;
     G4ThreeVector beamdir;
     
     beampdg    = generatorAction->GetBeamPDG(u);
     beamenergy = generatorAction->GetBeamEnergy(u);
     beamdir    = generatorAction->GetBeamDir(u);
     
     jhfNtuple.ipnu[npar]    = beampdg;               // pdg
     jhfNtuple.flag[npar]    = -1;                    // incoming neutrino
     jhfNtuple.m[npar]       = 0.0;                   // mass (always a neutrino)
     jhfNtuple.p[npar]       = beamenergy;            // momentum magnitude
     jhfNtuple.E[npar]       = beamenergy/MeV;        // energy 
     jhfNtuple.startvol[npar]= -1000;                 // starting volume, BNB target
     jhfNtuple.stopvol[npar] = vtxsvol[u];            // stopping volume 
     jhfNtuple.dir[npar][0]  = beamdir[0];            // direction 
     jhfNtuple.dir[npar][1]  = beamdir[1];            // direction 
     jhfNtuple.dir[npar][2]  = beamdir[2];            // direction 
     jhfNtuple.pdir[npar][0] = beamenergy*beamdir[0]; // momentum-vector 
     jhfNtuple.pdir[npar][1] = beamenergy*beamdir[1]; // momentum-vector 
     jhfNtuple.pdir[npar][2] = beamenergy*beamdir[2]; // momentum-vector 
     jhfNtuple.start[npar][0] = 0./CLHEP::cm;         // start is BNB target
     jhfNtuple.start[npar][0] = 0./CLHEP::cm;         // start is BNB target
     jhfNtuple.start[npar][0] = 0./CLHEP::cm;         // start is BNB target
     jhfNtuple.stop[npar][0] = vtxs[u][0]/CLHEP::cm;  // stopping point (neutrino intx vertex)
     jhfNtuple.stop[npar][1] = vtxs[u][1]/CLHEP::cm;  // stopping point (neutrino intx vertex)
     jhfNtuple.stop[npar][2] = vtxs[u][2]/CLHEP::cm;  // stopping point (neutrino intx vertex)
     jhfNtuple.parent[npar] = 0;                      // parentpdg = 0: primary

     npar++;
     /////////////////////////////////
     // npar = 1        TARGET ///////
     /////////////////////////////////
     
     G4double      targetpmag = 0.0, targetmass = 0.0;
     G4int         targetpdg    = generatorAction->GetTargetPDG(u);
     G4double      targetenergy = generatorAction->GetTargetEnergy(u);
     G4ThreeVector targetdir    = generatorAction->GetTargetDir(u);

     if (targetpdg!=0) {                 // protects against seg-fault
       if (targetpdg > 999) {            // 16O nucleus not in pdg table
         targetmass = targetenergy;      // 16O is at rest, so E = m
       } else {
         G4ParticleDefinition* targetparticle = particleTable->FindParticle(targetpdg);
         if(targetparticle){
            targetmass = targetparticle->GetPDGMass();
         } else {
            G4cout<<"unknown particle, pdg: "<<targetpdg<<G4endl;
            targetmass = targetenergy;   //?
         }
       }
       if (targetenergy > targetmass) {
         targetpmag = sqrt(targetenergy*targetenergy - targetmass*targetmass);
       } else { // protect against NaN
         targetpmag = 0.0;
       }
     }
     
     jhfNtuple.ipnu[npar]     = targetpdg;                      // pdg
     jhfNtuple.flag[npar]     = -2;                             // target
     jhfNtuple.m[npar]        = targetmass;                     // mass (always a neutrino)
     jhfNtuple.p[npar]        = targetpmag;                     // momentum magnitude
     jhfNtuple.E[npar]        = targetenergy/MeV;               // energy (total!) 
     jhfNtuple.startvol[npar] = vtxsvol[u];                     // starting volume 
     jhfNtuple.stopvol[npar]  = -1;                             // stopping volume 
     jhfNtuple.dir[npar][0]   = targetdir[0];                   // direction 
     jhfNtuple.dir[npar][1]   = targetdir[1];                   // direction 
     jhfNtuple.dir[npar][2]   = targetdir[2];                   // direction 
     jhfNtuple.pdir[npar][0]  = targetpmag*targetdir[0];        // momentum-vector 
     jhfNtuple.pdir[npar][1]  = targetpmag*targetdir[1];        // momentum-vector 
     jhfNtuple.pdir[npar][2]  = targetpmag*targetdir[2];        // momentum-vector 
     jhfNtuple.start[npar][0] = vtxs[u][0]/CLHEP::cm;           // starting point (neutrino intx vertex)
     jhfNtuple.start[npar][1] = vtxs[u][1]/CLHEP::cm;           // starting point (neutrino intx vertex)
     jhfNtuple.start[npar][2] = vtxs[u][2]/CLHEP::cm;           // starting point (neutrino intx vertex)
     jhfNtuple.stop[npar][0]  = 0./cm;                          // stopping point (for target nucleus, N/A)
     jhfNtuple.stop[npar][1]  = 0./cm;                          // stopping point (for target nucleus, N/A)
     jhfNtuple.stop[npar][2]  = 0./cm;                          // stopping point (for target nucleus, N/A)
     jhfNtuple.parent[npar]   = 0;                              // parent pdg=0: primary

     npar++;
   }

  Bool_t store_primary_daughters=false;
  if(store_primary_daughters){
    /////////////////////////////////////////
    // npar > 2*nvtxs      DECAY DAUGHTERS //
    /////////////////////////////////////////
    
    // Now look for decay products of primaries. We only record e, nu_e, and gammas. 
    // We require their parent trackID is this primarytrackid PLUS 1
    // (don't get this...not direct decay products of primaries - maybe products of pions from primary intx?)
    // and record only the particle matching this criteria with the lowest trackdID.
    // 
    // parent pdg is not recorded (always 0). remember these are primaries' decay products.
    // stopping (???) volume is taken as the start vertex of the associated primary 
    // (even though that's not it's parent?)
    // starting vertex is not set (jhfNtuple.start isn't called, can't tell what it will be).
    // starting and stopping volumes aren't looked up (always -1)
    // flag is set to 3 (custom)
    // ALL THIS WILL BE SAVED? any trajectories that are primaries have SetSaveFlag(true) set in TrackingAction
    // so all primary trajectories are duplicated, as will all particles meeting criteria in TrackingAction.

    G4int PDG_e=11,PDG_v_e=12,PDG_gam=22;
    for( Int_t u=0; u<nvtxs; u++ ){
      G4int trkid_e=INT_MAX,trkid_v_e=INT_MAX,trkid_gam=INT_MAX;
      G4int idx_e=INT_MAX,idx_v_e=INT_MAX,idx_gam=INT_MAX;
      for (G4int i=0; i < n_trajectories; i++) {
          WCSimTrajectory* trj = (WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[i]);
          if(abs(trj->GetPDGEncoding())==PDG_e && trj->GetParentID()==(u+1) && trj->GetTrackID()<trkid_e){
            trkid_e = trj->GetTrackID();
            idx_e = i;
          }
          if(abs(trj->GetPDGEncoding())==PDG_v_e && trj->GetParentID()==(u+1) && trj->GetTrackID()<trkid_v_e){
            trkid_v_e = trj->GetTrackID();
            idx_v_e = i;
          }
          if(abs(trj->GetPDGEncoding())==PDG_gam && trj->GetParentID()==(u+1) && trj->GetTrackID()<trkid_gam){
            trkid_gam = trj->GetTrackID();
            idx_gam = i;
          }
      }

      // if we found a daughter electron, add it to the stored tracks
      if(idx_e != INT_MAX) {
        WCSimTrajectory* trj =
          (WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[idx_e]);
        jhfNtuple.ipnu[npar]     = trj->GetPDGEncoding();                   // pdg
        jhfNtuple.flag[npar]    = 3;                                        // decay daughter
        jhfNtuple.m[npar]       = particleTable->FindParticle(trj->GetPDGEncoding())->GetPDGMass();    // mass
        jhfNtuple.p[npar]       = trj->GetInitialMomentum().mag();          // momentum magnitude
        jhfNtuple.E[npar]       = sqrt(jhfNtuple.m[npar]*jhfNtuple.m[npar]+jhfNtuple.p[npar]*jhfNtuple.p[npar]) /MeV;  // energy (total!) 
        jhfNtuple.startvol[npar] = -1;                                      // starting volume 
        jhfNtuple.stopvol[npar] = -1;                                       // stopping volume 
        jhfNtuple.dir[npar][0]  = trj->GetInitialMomentum().unit().getX();  // direction 
        jhfNtuple.dir[npar][1]  = trj->GetInitialMomentum().unit().getY();  // direction 
        jhfNtuple.dir[npar][2]  = trj->GetInitialMomentum().unit().getZ();  // direction 
        jhfNtuple.pdir[npar][0] = trj->GetInitialMomentum().getX();         // momentum-vector 
        jhfNtuple.pdir[npar][1] = trj->GetInitialMomentum().getY();         // momentum-vector 
        jhfNtuple.pdir[npar][2] = trj->GetInitialMomentum().getZ();         // momentum-vector 
        jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;                            // stopping point (not meaningful)
        jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;                            // stopping point (not meaningful)
        jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;                            // stopping point (not meaningful)
        jhfNtuple.parent[npar]  = trj->GetParentPdg();                      // parent pdg
        
        npar++; 
      }
      
      // if we found a daughter electron neutrino, add it to the stored tracks
      if(idx_v_e != INT_MAX) {
        WCSimTrajectory* trj =
          (WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[idx_v_e]);
        jhfNtuple.ipnu[npar]    = trj->GetPDGEncoding();                    // id
        jhfNtuple.flag[npar]    = 3;                                        // decay daughter
        jhfNtuple.m[npar]       = particleTable->FindParticle(trj->GetPDGEncoding())->GetPDGMass();    // mass
        jhfNtuple.p[npar]       = trj->GetInitialMomentum().mag();          // momentum magnitude
        jhfNtuple.E[npar]       = sqrt(jhfNtuple.m[npar]*jhfNtuple.m[npar]+jhfNtuple.p[npar]*jhfNtuple.p[npar]) /MeV;  // energy (total!) 
        jhfNtuple.startvol[npar] = -1;                                      // starting volume 
        jhfNtuple.stopvol[npar] = -1;                                       // stopping volume 
        jhfNtuple.dir[npar][0]  = trj->GetInitialMomentum().unit().getX();  // direction 
        jhfNtuple.dir[npar][1]  = trj->GetInitialMomentum().unit().getY();  // direction 
        jhfNtuple.dir[npar][2]  = trj->GetInitialMomentum().unit().getZ();  // direction 
        jhfNtuple.pdir[npar][0] = trj->GetInitialMomentum().getX();         // momentum-vector 
        jhfNtuple.pdir[npar][1] = trj->GetInitialMomentum().getY();         // momentum-vector 
        jhfNtuple.pdir[npar][2] = trj->GetInitialMomentum().getZ();         // momentum-vector 
        jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;                            // stopping point (not meaningful)
        jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;                            // stopping point (not meaningful)
        jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;                            // stopping point (not meaningful)
        jhfNtuple.parent[npar]  = trj->GetParentPdg();                      // parent pdg
        
        npar++; 
      }
      
      // if we found a daughter gamma, add it to the stored tracks
      if(idx_gam != INT_MAX) {
        WCSimTrajectory* trj =
          (WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[idx_gam]);
        jhfNtuple.ipnu[npar]     = trj->GetPDGEncoding();                   // id
        jhfNtuple.flag[npar]    = 3;                                        // decay daughter
        jhfNtuple.m[npar]       = particleTable->FindParticle(trj->GetPDGEncoding())->GetPDGMass();    // mass
        jhfNtuple.p[npar]       = trj->GetInitialMomentum().mag();          // momentum magnitude
        jhfNtuple.E[npar]       = sqrt(jhfNtuple.m[npar]*jhfNtuple.m[npar]+jhfNtuple.p[npar]*jhfNtuple.p[npar]) / MeV;  // energy (total!) 
        jhfNtuple.startvol[npar] = -1;                                      // starting volume 
        jhfNtuple.stopvol[npar] = -1;                                       // stopping volume 
        jhfNtuple.dir[npar][0]  = trj->GetInitialMomentum().unit().getX();  // direction 
        jhfNtuple.dir[npar][1]  = trj->GetInitialMomentum().unit().getY();  // direction 
        jhfNtuple.dir[npar][2]  = trj->GetInitialMomentum().unit().getZ();  // direction 
        jhfNtuple.pdir[npar][0] = trj->GetInitialMomentum().getX();         // momentum-vector 
        jhfNtuple.pdir[npar][1] = trj->GetInitialMomentum().getY();         // momentum-vector 
        jhfNtuple.pdir[npar][2] = trj->GetInitialMomentum().getZ();         // momentum-vector 
        jhfNtuple.stop[npar][0] = vtxs[u][0]/cm;                            // stopping point (not meaningful)
        jhfNtuple.stop[npar][1] = vtxs[u][1]/cm;                            // stopping point (not meaningful)
        jhfNtuple.stop[npar][2] = vtxs[u][2]/cm;                            // stopping point (not meaningful)
        jhfNtuple.parent[npar]  = trj->GetParentPdg();;                     // parent pdg
        
        npar++; 
      }
    }
  }

   // Draw Charged Tracks
   for (G4int i=0; i < n_trajectories; i++) {
     WCSimTrajectory* trj = (WCSimTrajectory*)((*(evt->GetTrajectoryContainer()))[i]);
     if (trj->GetCharge() != 0.) trj->DrawTrajectory(50);
   }

   //G4cout << "end________________ Filling Root Event: "<<event_id<<" lappdevt: "<<lappdevt<< G4endl;
   G4cout<<"Filling Root Event: "<<event_id<<G4endl;

   //   G4cout << "event_id: " << &event_id << G4endl;
   // G4cout << "jhfNtuple: " << &jhfNtuple << G4endl;
   //  G4cout << "WCHC: " << &WCHC << G4endl;
   //  G4cout << "WCDC: " << &WCDC << G4endl;
   //  G4cout << "WCFVHC: " << &WCFVHC << G4endl;
   //  G4cout << "WCFVDC: " << &WCFVDC << G4endl;
   // G4cout << "lArDHC: " << &lArDHC << G4endl;
   // G4cout << "FGDxHC: " << &FGDxHC << G4endl;
   // G4cout << "FGDyHC: " << &FGDyHC << G4endl;
   // G4cout << "MRDxHC: " << &MRDxHC << G4endl;
   // G4cout << "MRDyHC: " << &MRDyHC << G4endl;
   
  jhfNtuple.npar = npar;
  
  FillRootEvent(event_id,
		jhfNtuple,
		trajectoryContainer,
		WCDC_hits,
		WCDC,
		"tank");
		
	if(isANNIE){
  G4cout<<"Filling MRD Root Event"<<G4endl;
  FillRootEvent(event_id,
		jhfNtuple,
		trajectoryContainer,
		WCDC_hits_MRD,
		WCDC_MRD,
		"mrd");
  G4cout<<"Filling FACC Root Event"<<G4endl;
  FillRootEvent(event_id,
		jhfNtuple,
		trajectoryContainer,
		WCDC_hits_FACC,
		WCDC_FACC,
		"facc");
	}
  
  TTree* tree = GetRunAction()->GetTree();
  TBranch* tankeventbranch = tree->GetBranch("wcsimrootevent");
  tree->SetEntries(tankeventbranch->GetEntries());
  //tree->SetEntries(GetRunAction()->GetNumberOfEventsGenerated());
  TFile* hfile = tree->GetCurrentFile();
  hfile->cd();
  // MF : overwrite the trees -- otherwise we have as many copies of the tree
  // as we have events. All the intermediate copies are incomplete, only the
  // last one is useful --> huge waste of disk space.
  hfile->Write("",TObject::kOverwrite);
  
  G4cout<<"events generated so far: "<<(GetRunAction()->GetNumberOfEventsGenerated())<<G4endl;
  if(event_id%10000==0&&event_id!=0){
    GetRunAction()->CreateNewOutputFile(); // careful: we should maintain 1:1:1 files genie:g4dirt:wcsim
    if(isANNIE) CreateNewLAPPDFile(); // this must always come *after* the runaction version
  }
  
  G4cout<<"############# WCSIM FINISH END OF EVENT ACTION  ################"<<G4endl;

  //save DAQ options here. This ensures that when the user selects a default option
  // (e.g. with -99), the saved option value in the output reflects what was run
  if(!SavedOptions) {
    WCSimRootOptions * wcsimopt = runAction->GetRootOptions();
    //Dark noise
    WCDNM->SaveOptionsToOutput(wcsimopt);
    //Digitizer
    WCDM->SaveOptionsToOutput(wcsimopt);
    //Trigger
    WCTM->SaveOptionsToOutput(wcsimopt);
    //Generator
    generatorAction->SaveOptionsToOutput(wcsimopt);
    
    SavedOptions = true;
  }
}

G4int WCSimEventAction::WCSimEventFindStartingVolume(G4ThreeVector vtx)
{
  // Get volume of starting point (see GEANT4 FAQ)

  G4int vtxvol = -1;

  G4Navigator* tmpNavigator = 
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

  G4VPhysicalVolume* tmpVolume = tmpNavigator->LocateGlobalPointAndSetup(vtx);
  G4String       vtxVolumeName = tmpVolume!=0 ? tmpVolume->GetName() : "";


  // are all these depreciated? Not in ANNIE geometry.
  if ( vtxVolumeName == "outerTube" ||
        vtxVolumeName == "innerTube" ||
        vtxVolumeName == "rearEndCap"|| 
        vtxVolumeName == "frontEndCap" )
    vtxvol = 10;

  else if ( vtxVolumeName.contains("WC") && !vtxVolumeName.contains("FV") )
  {
      if ((vtxVolumeName.contains("WCBarrel"))|| (vtxVolumeName.contains("Tank")))    //aah I added "Tank" as MB equivalent of Barrel
      vtxvol = 10;
    else if (vtxVolumeName == "WCBox")
      vtxvol = -2;
    else if (vtxVolumeName.contains("PMT") ||
         vtxVolumeName.contains("Cap") ||
         vtxVolumeName.contains("Cell"))
      vtxvol = 11;
    else if (vtxVolumeName.contains("OD"))
      vtxvol = 12;
    else
    {
      G4cout << vtxVolumeName << " unkown vtxVolumeName " << G4endl;
      vtxvol = -3;
    }
  }
  else if ( vtxVolumeName == "expHall" )
    vtxvol = 0;
  else if ( vtxVolumeName == "catcher" )
    vtxvol = 40;
  
  if(vtxvol<0){
    //G4cout<<"############# unkown vertex volume: "<<vtxVolumeName<<" ################"<<G4endl;
  }
  return vtxvol;
}

G4int WCSimEventAction::WCSimEventFindStoppingVolume(G4String stopVolumeName)
{
  G4int stopvol = -1;

  if ( stopVolumeName.contains("WC") && !stopVolumeName.contains("FV") )
  {
//aah Original line->    if (stopVolumeName.contains("WCBarrel"))
      if ((stopVolumeName.contains("WCBarrel"))|| (stopVolumeName.contains("Tank")))    // aah I added "Tank" as MB equivalent of Barrel
      stopvol = 10;
    else if (stopVolumeName == "WCBox")
      stopvol = -2;
    else if (stopVolumeName.contains("PMT") ||
             stopVolumeName.contains("Cap") ||
             stopVolumeName.contains("Cell"))
      stopvol = 11;
    else if (stopVolumeName.contains("OD"))
      stopvol = 12;
    else
    {
      G4cout << stopVolumeName << " unkown stopVolumeName " << G4endl;
      stopvol = -3;
    }
  }

  else if ( stopVolumeName.contains("FV") )
  {
    if (stopVolumeName == "WCFVBarrel" ||
        stopVolumeName == "WCFVAnnulus" ||
        stopVolumeName == "WCFVRing" )
      stopvol = 10;
    else if (stopVolumeName.contains("FVPMT"))
      stopvol = 13;
    else
    {
      G4cout << stopVolumeName << " unkown stopVolumeName " << G4endl;
      stopvol = -3;
    }
  }
  else if ( stopVolumeName == "expHall" )
    stopvol = 0;
  else if ( stopVolumeName == "catcher" )
    stopvol = 40;

  if(stopvol<0){
    //G4cout<<"############# unkown vertex volume: "<<stopVolumeName<<" ################"<<G4endl;
  }
  return stopvol;
}


G4int WCSimEventAction::WCSimEventFindVertexVolume(G4ThreeVector vtx){
  // climb the touchable hierarchy until we find a meaningful volume name (depth 3 in the geometry tree)
  // then record that name
  G4Navigator* tmpNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  G4VPhysicalVolume* primaryPV = tmpNavigator->LocateGlobalPointAndSetup(vtx);
  G4TouchableHistory* touchable = tmpNavigator->CreateTouchableHistory();  // we have deletion responsibility
  Int_t depth = touchable->GetHistoryDepth();
  int breaker=0;
  while(depth>3){
    //G4cout<<"volume at depth "<<depth<<" has name "<<touchable->GetVolume()->GetName()<<G4endl;
    touchable->MoveUpHistory(1);
    int lastdepth = depth;
    depth = touchable->GetHistoryDepth();
    breaker++;
    if(breaker>10) break;
    if(depth==lastdepth) assert(false);
  }
  //G4cout<<"volume at depth "<<depth<<" is: "<<touchable->GetVolume()->GetName()<<G4endl;
  G4String volumename = touchable->GetVolume()->GetName();
  delete touchable;
  if(volumename=="waterTank") return 10;
  if(volumename=="totVetoPhys") return 20;
  if(volumename=="totMRDphys") return 30;
  // fallback: dunno. 
  return -1;
}

void WCSimEventAction::FillRootEvent(G4int event_id, 
				     const struct ntupleStruct& jhfNtuple,
				     G4TrajectoryContainer* TC,
				     WCSimWCDigitsCollection* WCDC_hits,
				     WCSimWCTriggeredDigitsCollection* WCDC,
				     G4String detectorElement
				     )
{
  // Fill up a Root event with stuff from the ntuple

  WCSimRootEvent* wcsimrootsuperevent = GetRunAction()->GetRootEvent(detectorElement);

  // start with the first "sub-event"
  // if the WC digitization requires it, we will add another subevent
  // for the WC.
  // all the rest goes into the first "sub-event".
  WCSimRootTrigger* wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  G4DigiManager* DMman = G4DigiManager::GetDMpointer();
  WCSimWCTriggerBase* WCTM;
  
  if(detectorElement=="tank"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout");
  } else if(detectorElement=="mrd"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_MRD");
  } else if(detectorElement=="facc"){
    WCTM = (WCSimWCTriggerBase*)DMman->FindDigitizerModule("WCReadout_FACC");
  }
  
  // get number of gates
  int ngates = WCTM->NumberOfGatesInThisEvent(); 

  G4cout << "ngates "<<detectorElement<<" =  " << ngates << "\n";
  for (int index = 0 ; index < ngates ; index++) 
    {
      if (index >=1 ) {
	wcsimrootsuperevent->AddSubEvent();
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	wcsimrootevent->SetHeader(event_id,0,  // evtnum, run, date, subeventnum
				   0,index+1); // date & # of subevent 
	wcsimrootevent->SetMode(jhfNtuple.mode);
      }
      wcsimrootevent->SetTriggerInfo(WCTM->GetTriggerType(index),
				     WCTM->GetTriggerInfo(index));
    }
  

  // Fill the header
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  wcsimrootevent->SetHeader(event_id,0,0); // set evtnum, run=0, date=0, and subevtnum = 1<<< STARTS FROM 1
                                           // Run and date are added below

  // add the information about upstream source
  G4String      dirtFileName = generatorAction->GetDirtFileName();
  G4String     genieFileName = generatorAction->GetGenieFileName();
  G4int      dirtEventNumber = generatorAction->GetDirtEntryNum();
  G4int     genieEventNumber = generatorAction->GetGenieEntryNum();
  WCSimRootEventHeader* theheader = wcsimrootevent->GetHeader();
  theheader->SetDirtFileName(dirtDirectory+"/"+dirtFileName);
  theheader->SetGenieFileName(genieDirectory+"/"+genieFileName);
  theheader->SetDirtEntryNum(dirtEventNumber);
  theheader->SetGenieEntryNum(genieEventNumber);

  G4Event *event = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
  WCSimEventInformation *evInfo = dynamic_cast<WCSimEventInformation*>(event->GetUserInformation());
  Int_t num_cheren = evInfo->numCherenPhoton;
  wcsimrootevent->SetNCherenkov(num_cheren);
  wcsimrootevent->SetNCherenkovWCSim(evInfo->numCherenPhotonWCSim);
  
  // -------
  // Cherenkov photon histograms are disabled at the moment, can be uncommented if needed 
  // HCher: 1D Histogram (60nm - 700nm), HCherWCSim: 1D Histogram (200nm - 700nm)
  // -------

  //wcsimrootevent->SetHCher(&(evInfo->hCher));
  //wcsimrootevent->SetHCherWCSim(&(evInfo->hCherWCSim));
  //wcsimrootevent->SetHCherXZ(&(evInfo->hCherXZ));
  //wcsimrootevent->SetHCherYZ(&(evInfo->hCherYZ));

  // Fill other info for this event

  wcsimrootevent->SetMode(jhfNtuple.mode);

  wcsimrootevent->SetNvtxs(jhfNtuple.nvtxs);
  for( Int_t u=0; u<jhfNtuple.nvtxs; u++ ){
    wcsimrootevent->SetVtxsvol(u,jhfNtuple.vtxsvol[u]);
    for (int j=0;j<3;j++)
      {
	wcsimrootevent->SetVtxs(u,j,jhfNtuple.vtxs[u][j]);
      }
  }
  wcsimrootevent->SetJmu(jhfNtuple.jmu);
  wcsimrootevent->SetJp(jhfNtuple.jp);
  wcsimrootevent->SetNpar(jhfNtuple.npar);
  wcsimrootevent->SetVecRecNumber(jhfNtuple.vecRecNumber);

  // Add the tracks with the particle information
  // First two tracks come from jhfNtuple, as they are special
  // only do this for tank event - no need to duplicate particle information in mrd & veto
  int k;
  if(detectorElement=="tank"){
    runAction->incrementEventsGenerated();
    //Modify to add decay products
    for (k=0;k<jhfNtuple.npar;k++){ // should be just 2
      float dir[3];
      float pdir[3];
      float pdir2[3];
      float stop[3];
      float start[3];
      for (int l=0;l<3;l++){
        dir[l]=jhfNtuple.dir[k][l];
        pdir[l]=jhfNtuple.pdir[k][l];
        pdir2[l]=0.; // stopping momentum not stored in jhfNtuple
        stop[l]=jhfNtuple.stop[k][l];
        start[l]=jhfNtuple.start[k][l];
          //G4cout<< "start[" << k << "][" << l <<"]: "<< jhfNtuple.start[k][l] <<G4endl;
      }
      
      // Add the track to the TClonesArray
      wcsimrootevent->AddTrack(jhfNtuple.ipnu[k],         // particle PDG
                               jhfNtuple.flag[k],         // neutrino probe, target or other
                               jhfNtuple.m[k],            // particle rest mass
                               jhfNtuple.p[k],            // initial momentum mag
                               jhfNtuple.E[k],            // intial relativistic energy
                               -1.,                       // stopping mommag not stored in jhfNtuple
                               -1.,                       // stopping E not stored in jhfNtuple
                               jhfNtuple.startvol[k],     // intial volume code (10=tank, 20=veto, 30=mrd)
                               jhfNtuple.stopvol[k],      // final volume code 
                               dir,                       // intial direction unit vector
                               pdir,                      // intial momentum vector
                               pdir2,                     // final momentum vector
                               stop,                      // stopping vertex
                               start,                     // start vertex 
                               jhfNtuple.parent[k],       // parent PDG
                               jhfNtuple.time[k],         // start time (global)
                               0,                         // stopping time not stored in jhfNtuple
                               0,                         // WCSim TrackID not applicable
                               "",                        // creator process name
                               "NuIntx",                  // end process name
                               pdir2,                     // tank exit position (N/A)
                               0,                         // tank exit energy (relativistic)
                               pdir2);                    // tank exit 3-momentum (N/A)
    }
    
    // the rest of the tracks come from WCSimTrajectory
    
    // Pi0 specific variables
    Float_t pi0Vtx[3];
    Int_t   gammaID[2];
    Float_t gammaE[2];
    Float_t gammaVtx[2][3];
    Int_t   r = 0;
    
    G4int n_trajectories = 0;
    if (TC) n_trajectories = TC->entries();
    
    // M Fechner : removed this limit to get to the primaries...
    //if (n_trajectories>50)  // there is no need for this limit, but it has
    //n_trajectories=50;    // existed in previous versions of the code.  It also
                            // makes the ROOT file smaller.  
    
    // Process primary tracks or the secondaries from pizero or muons...
    for (int i=0; i <n_trajectories; i++) {
      WCSimTrajectory* trj = (WCSimTrajectory*)(*TC)[i];
      
      // we want to record tracks for neutrons. But we don't want many tracks representing the same neutron,
      // which geant has a habit of doing. so, only record neutrons with an end process of ncapture. 
      // we may need to override scintillation process, as it can sometimes obscure the true end process.
      // (G4Scintillation is always invoked, and is applied to all particles except photons and short lived)
      bool ignoreneutron=false;
//      if(isANNIE){
//        G4int trajectorypdg = trj->GetPDGEncoding();
//        if(trajectorypdg==2112){
//          G4String theProcess = trj->GetCurrentProcess();
//          G4ThreeVector StopVtx = trj->GetStoppingPoint();
//          G4int stopvolint= WCSimEventFindVertexVolume(StopVtx);
//          if(theProcess=="Scintillation"&&(stopvolint==10)){   // fake "scintillation" process.
//            theProcess = trj->GetLastProcess();
//          }
//          if(theProcess!="nCapture"||stopvolint!=10){
//            ignoreneutron=true;
//            //G4cout<<"NOT STORING NEUTRON TRACK, STOPPED IN VOL INDEX "<<stopvolint<<" WITH PROCESS "<<theProcess<<G4endl;
//          } else {
//            //G4cout<<"STORING NEUTRON TRACK, STOPPED IN VOL INDEX "<<stopvolint<<" WITH PROCESS "<<theProcess<<", created with process "<<trj->GetCreatorProcessName()<<G4endl;
//          }
//        }
//      }
      // XXX only trajectories for particles / processes listed in WCSimTrackingAction are saved!! XXX
      if ( trj->GetSaveFlag() && !ignoreneutron){
        // initial point of the trajectory
        G4TrajectoryPoint* aa =   (G4TrajectoryPoint*)trj->GetPoint(0);
        
        G4int         ipnu   = trj->GetPDGEncoding();
        G4int         id     = trj->GetTrackID();
        G4int         flag   = 0;
        G4double      mass   = trj->GetParticleDefinition()->GetPDGMass();
        G4ThreeVector mom    = trj->GetInitialMomentum();
        G4double      mommag = mom.mag();
        G4double      energy = sqrt(mom.mag2() + mass*mass);
	//G4cout <<"ipnu: "<<ipnu<<", energy: "<<energy<<std::endl;
        G4ThreeVector momend = trj->GetFinalMomentum();
        G4double      mommagend = momend.mag();
        G4double      energyend = sqrt(momend.mag2() + mass*mass);
        G4ThreeVector Stop   = trj->GetStoppingPoint();
        G4ThreeVector Start  = aa->GetPosition();
        G4ThreeVector tankExitPos = trj->GetTankExitPoint();
        G4ThreeVector tankExitMom = trj->GetMomentumOnTankExit();
        G4double      tankExitE = (tankExitPos==G4ThreeVector()) ? 0 : sqrt(tankExitMom.mag2() + mass*mass);
        
        G4String stopVolumeName = trj->GetStoppingVolume()->GetName();
        G4int    stopvol, startvol;
        if(!isANNIE){
          stopvol     = WCSimEventFindStoppingVolume(stopVolumeName);
          startvol    = WCSimEventFindStartingVolume(Start);
        } else {
          stopvol     = WCSimEventFindVertexVolume(Stop);
          startvol    = WCSimEventFindVertexVolume(Start);
        }
        
        G4double ttime = trj->GetGlobalTime(); 
        G4double ttimeend = trj->GetGlobalTimeEnd(); 
        
        // Right now only secondaries whose parents are pi0's are stored
        // This may change later
        // M Fechner : dec 16, 2004 --> added decay e- from muons
        // GetParentID stores the *trackID* of the parent, but we need the *trajectory* ID
        // to retrieve the parent's PDG. Old method kept lists of the track IDs of particles
        // of notable types. We'd then have to check if the trackID was in any of these lists.
        // This meant we could only identify the parent PDG if it was of a limited subset.
        
        // new method: store the parent PDG in the UserTrackInfo in PostUserTrackingAction 
        // (via GimmeSecondaries), and copy it from Info into the Trajectory after tracking.
        G4int parentType=trj->GetParentPdg();
        
        // G4cout << parentType << " " << ipnu << " " 
        //             << id << " " << energy << "\n";
        
        // fill ntuple
        float dir[3];
        float pdir[3];
        float pdir2[3];
        float stop[3];
        float start[3];
        float tankexit[3];
        float tankexitp[3];
        for (int l=0;l<3;l++)
        {
          dir[l]= (mommag!=0) ? mom[l]/mommag : 0; // direction 
          pdir[l]=mom[l];        // momentum-vector 
          pdir2[l]=momend[l];    // end momentum-vector 
          stop[l]=Stop[l]/CLHEP::cm; // stopping point 
          start[l]=Start[l]/CLHEP::cm; // starting point 
          //G4cout<<"part 2 start["<<l<<"]: "<< start[l] <<G4endl;
          tankexit[l]=tankExitPos[l]/CLHEP::cm; // 
          tankexitp[l]=tankExitMom[l];
        }
        
        // Add the track to the TClonesArray, watching out for times
        if( (ipnu==22) &&                                      // if it's a gamma
            (trj->GetCreatorProcessName()=="nCapture") &&      // from ncapture
            (detectorConstructor->SaveCaptureInfo()==false)    // but SaveCaptureInfo is *false*
          ){
            /* do nothing */                                   // ignore it
        } else {                                               // otherwise, save it
          
          G4String startProcess = trj->GetCreatorProcessName();
          G4String endProcess = trj->GetCurrentProcess();
          G4ThreeVector StopVtx = trj->GetStoppingPoint();
          G4int stopvolint= WCSimEventFindVertexVolume(StopVtx);
          if(endProcess=="Scintillation"&&(stopvolint==10)){   // fake "scintillation" process.
            endProcess = trj->GetLastProcess();
          }
          
//          G4cout<<"#########################################################~~~"<<G4endl;
//          G4cout<<"Scanning "<<ngates<<" triggers for track starting at time "<<ttime<<G4endl;
//          for(int trigi=0; trigi<ngates; trigi++){
//            G4cout<<"Trigger "<<trigi<<" was of type "<<WCTM->GetTriggerType(trigi)
//                  <<", began at "<<WCTM->GetTriggerTime(trigi)
//                  <<", had a post-trigger window size of "
//                  <<WCTM->GetPostTriggerWindow(WCTM->GetTriggerType(trigi))
//                  <<" and ended at "
//                  <<WCTM->GetTriggerTime(trigi)+ WCTM->GetPostTriggerWindow(WCTM->GetTriggerType(trigi))
//                  <<G4endl;
//          }
          
          int choose_event=0;
          while (choose_event<(ngates-1)){
            auto trigger_end_time = WCTM->GetWCTriggerOffset()  // legacy digit time offset in trigger window
                                  + WCTM->GetTriggerTime(choose_event)
                                  + WCTM->GetPostTriggerWindow(WCTM->GetTriggerType(choose_event));
            if ( ttime < trigger_end_time )
              break;  // did this track start before the end of the current trigger? if so, it's in the current trigger
            else 
              choose_event++;  // otherwise go to next trigger
          } // exit (without check) if this is the last trigger
          wcsimrootevent= wcsimrootsuperevent->GetTrigger(choose_event);
          
          wcsimrootevent->AddTrack(ipnu,          // particle PDG
                                   flag,          // neutrino probe, target or other
                                   mass,          // particle rest mass
                                   mommag,        // initial momentum mag
                                   energy,        // intial relativistic energy
                                   mommagend,     // final momentum mag
                                   energyend,     // final relativistic energy
                                   startvol,      // intial volume code (10=tank, 20=veto, 30=mrd)
                                   stopvol,       // final volume code 
                                   dir,           // intial direction unit vector
                                   pdir,          // intial momentum vector
                                   pdir2,         // final momentum vector
                                   stop,          // stopping vertex
                                   start,         // start vertex 
                                   parentType,    // parent PDG
                                   ttime,         // start time (global)
                                   ttimeend,      // end time (global)
                                   id,            // wcsim TrackID
                                   startProcess,  // start process name
                                   endProcess,    // end process name
                                   tankexit,      // tank exit position
                                   tankExitE,     // tank exit energy (relativistic)
                                   tankexitp);    // tank exit 3-momentum
        }
        
        
        if (detectorConstructor->SavePi0Info() == true){         // if we're saving pi0 gammas separately as well
          //G4cout<<"Pi0 parentType: " << parentType <<G4endl;   // (technically, if we're saving pi0 daughters)
          if (parentType == 111){  // pi0                        // and this particle's parent was a pi0
            if (r>1){                                            // complain if we've found more than 2 daughters
              G4cout<<"WARNING: more than 2 primary gammas found"<<G4endl;
            } else {
              for (int y=0;y<3;y++){
                pi0Vtx[y] = start[y];                            // pi0 decay vtx is gamma start vtx
                gammaVtx[r][y] = stop[y];                        // gamma vtx is gamma stop vtx
              }
              
              gammaID[r] = id;                                   // track ID of gamma in WCSimRootTrack array
              gammaE[r] = energy;                                // gamma energy
              r++;
              
              //amb79
              //G4cout<<"Pi0 data: " << id <<G4endl;
              wcsimrootevent->SetPi0Info(pi0Vtx, gammaID, gammaE, gammaVtx);  // record the gamma
            }
          }
        }
        
        // TODO figure out what this neutron capture class is doing.
        // it's adding *gammas* (not neutrons - creator process is ncapture), but not using the AddGamma method
        // and relates them to the parent via parentID (not ideal?)
        if (detectorConstructor->SaveCaptureInfo() && trj->GetCreatorProcessName()=="nCapture"){
  #ifdef NCAPTURE_VERBOSE
            G4cout << " Capture particle: " << trj->GetParticleName()
                   << " Parent: " << trj->GetParentID()
                   << " T:" << ttime
                   << " vtx:(" << start[0] << "," << start[1] << "," << start[2]
                   << " dir:(" << dir[0] << "," << dir[1] << "," << dir[2]
                   << " E:" << energy << G4endl;
  #endif
            wcsimrootevent->SetCaptureParticle(trj->GetParentID(), ipnu, ttime, start, dir, energy, id);
        }
      } // end loop over saved trajectories
    }   // end loop over trajectories
  }     // end if tank wcsimrootevent
  
  // Add the Cherenkov hits
  wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
  wcsimrootevent->SetNumTubesHit(jhfNtuple.numTubesHit);
#ifdef _SAVE_RAW_HITS


  if (WCDC_hits) 
  {
    //add the truth raw hits
    // Both the pre- and post-PMT smearing hit times are accessible
    // Choose to save just the pre-smeared times for now
#ifdef _SAVE_RAW_HITS_VERBOSE
    G4cout<<"RAW HITS"<<G4endl;
#endif
    wcsimrootevent->SetNumTubesHit(WCDC_hits->entries());
    std::vector<float> truetime, smeartime;
    std::vector<int>   primaryParentID;
    double hit_time_smear, hit_time_true;
    int hit_parentid;
    //loop over the DigitsCollection
    for(int idigi = 0; idigi < WCDC_hits->entries(); idigi++) {
      int digi_tubeid = (*WCDC_hits)[idigi]->GetTubeID();
      for(G4int id = 0; id < (*WCDC_hits)[idigi]->GetTotalPe(); id++){
	hit_time_true  = (*WCDC_hits)[idigi]->GetPreSmearTime(id);
	hit_parentid = (*WCDC_hits)[idigi]->GetParentID(id);
	truetime.push_back(hit_time_true);
	primaryParentID.push_back(hit_parentid);
#ifdef _SAVE_RAW_HITS_VERBOSE
	hit_time_smear = (*WCDC_hits)[idigi]->GetTime(id);
	smeartime.push_back(hit_time_smear);
#endif
      }//id
#ifdef _SAVE_RAW_HITS_VERBOSE
      if(digi_tubeid < NPMTS_VERBOSE) {
	G4cout << "Adding " << truetime.size()
	       << " Cherenkov hits in tube " << digi_tubeid
	       << " with truetime:smeartime:primaryparentID";
	for(G4int id = 0; id < truetime.size(); id++) {
	  G4cout << " " << truetime[id]
		 << ":" << smeartime[id]
		 << ":" << primaryParentID[id];
	}//id
	G4cout << G4endl;
      }
#endif
      wcsimrootevent->AddCherenkovHit(digi_tubeid,
				      truetime,
				      primaryParentID);
      smeartime.clear();
      truetime.clear();
      primaryParentID.clear();
    }//idigi
  }//if(WCDC_hits)
#endif //_SAVE_RAW_HITS

  // Add the digitized hits
  if (WCDC) 
  {
#ifdef SAVE_DIGITS_VERBOSE
    G4cout << "DIGI HITS" << G4endl;
#endif

    G4float sumq_tmp = 0.;
    
    for ( int index = 0 ; index < ngates ; index++)
      {	
	sumq_tmp = 0.0;	
	G4float gatestart;
	int countdigihits = 0;
	wcsimrootevent = wcsimrootsuperevent->GetTrigger(index);
	for (k=0;k<WCDC->entries();k++)
	  {
	    if ( (*WCDC)[k]->HasHitsInGate(index)) {
	      std::vector<float> vec_pe                  = (*WCDC)[k]->GetPe(index);
	      std::vector<float> vec_time                = (*WCDC)[k]->GetTime(index);
	      std::vector<std::vector<int> > vec_digicomp = (*WCDC)[k]->GetDigiCompositionInfo(index);
	      const int tubeID                           = (*WCDC)[k]->GetTubeID();
	      assert(vec_pe.size() == vec_time.size());
	      assert(vec_pe.size() == vec_digicomp.size());
	      for(unsigned int iv = 0; iv < vec_pe.size(); iv++) {
#ifdef SAVE_DIGITS_VERBOSE
		if(tubeID < NPMTS_VERBOSE) {
		  G4cout << "Adding digit " << iv 
			 << " for PMT " << tubeID
			 << " pe "   << vec_pe[iv]
			 << " time " << vec_time[iv]
			 << " digicomp";
		  for(unsigned int ivv = 0; ivv < vec_digicomp[iv].size(); ivv++)
		    G4cout << " " << vec_digicomp[iv][ivv];
		  G4cout << G4endl;
		}
#endif
		assert(vec_digicomp[iv].size() > 0);
		wcsimrootevent->AddCherenkovDigiHit(vec_pe[iv], vec_time[iv],
						    tubeID, vec_digicomp[iv]);
		sumq_tmp += vec_pe[iv];
		countdigihits++;
	      }//iv
	    }//Digit exists in Gate
	  }//k
	wcsimrootevent->SetNumDigitizedTubes(countdigihits);
	wcsimrootevent->SetSumQ(sumq_tmp);

#ifdef SAVE_DIGITS_VERBOSE
	G4cout << "checking digi hits ...\n";
	G4cout << "hits collection size (number of PMTs hit) =  " << 
	  wcsimrootevent->GetCherenkovHits()->GetEntries() << "\n";
	G4cout << "hits collection size (number of true photon + dark noise hits) =  " << 
	  wcsimrootevent->GetCherenkovHitTimes()->GetEntries() << "\n";
	G4cout << "digihits collection size =  " << 
	  wcsimrootevent->GetCherenkovDigiHits()->GetEntries() << "\n";
	G4cout << "tracks collection size =  " << 
	  wcsimrootevent->GetTracks()->GetEntries() 
	       <<" get ntracks = " <<  wcsimrootevent->GetNtrack() << "\n";
#endif
	gatestart = WCTM->GetTriggerTime(index);
	WCSimRootEventHeader *HH = wcsimrootevent->GetHeader();
	HH->SetDate(int(gatestart));
      }//index (loop over ngates)
    
    // end of loop over WC trigger gates --> back to the main sub-event
    wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);
    
  }

//  for (int i = 0 ; i < wcsimrootsuperevent->GetNumberOfEvents(); i++) {
//    wcsimrootevent = wcsimrootsuperevent->GetTrigger(i);
//    G4cout << ">>>Root event "
//	   <<std::setw(5)<<wcsimrootevent->GetHeader()->GetEvtNum()<<"\n";
//       if (WCDC){
//     G4cout <<"WC digi:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovdigihits()<<"  ";
//     G4cout <<"WC digi sumQ:"<<std::setw(4)<<wcsimrootevent->GetSumQ()<<"  ";
//  }
  
#ifdef _SAVE_RAW_HITS
  //if (WCHC)
  //     G4cout <<"WC:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovhits()<<"  ";
  //    if (WCFVHC)
  //G4cout <<"WCFV:"<<std::setw(4)<<wcsimrootevent->GetINcherenkovhits()<<"  ";
#endif
  
  //  if (WCFVDC){
  //G4cout <<"WCFV digi:"<<std::setw(4)<<wcsimrootevent->GetNcherenkovdigihits()<<"  ";
  //G4cout <<"WCFV digi sumQ:"<<std::setw(4)<<wcsimrootevent->GetSumQ()<<"  ";
  //  }
  TBranch* branch = GetRunAction()->GetBranch(detectorElement);
  branch->Fill();
//  TFile* hfile = tree->GetCurrentFile();
//  // MF : overwrite the trees -- otherwise we have as many copies of the tree
//  // as we have events. All the intermediate copies are incomplete, only the
//  // last one is useful --> huge waste of disk space.
//  hfile->Write("",TObject::kOverwrite);
  
  // M Fechner : reinitialize the super event after the writing is over
  wcsimrootsuperevent->ReInitialize();
  
}

void WCSimEventAction::CreateNewLAPPDFile(){
  if(LAPPDfile){LAPPDfile->Close(); delete LAPPDfile; LAPPDfile=0;}
  // LAPPD file
  LAPPDRootFileName = GetRunAction()->GetRootFileNameBase() + "_lappd_" + std::to_string(GetRunAction()->GetOutputFileNum()) + ".root";
  LAPPDfile = new TFile(LAPPDRootFileName.c_str(),"RECREATE","WCSim LAPPD file");
  LAPPDfile->SetCompressionLevel(2);
  LAPPDfile->cd();
  LAPPDtree = new TTree("LAPPDTree","LAPPDTree");
  // Create the branches
  LAPPDtree->Branch("lappdevt",&lappdevt);
  LAPPDtree->Branch("lappd_numhits",&lappd_numhits, "lappd_numhits/I");
  LAPPDtree->Branch("lappdhit_totalpes_perevt", &lappdhit_totalpes_perevt, "lappdhit_totalpes_perevt/I");
  LAPPDtree->Branch("lappdhit_totalpes_perlappd2", &lappdhit_totalpes_perlappd2);

  LAPPDtree->Branch("lappdhit_x",lappdhit_x,"lappdhit_x[lappd_numhits]/D");
  LAPPDtree->Branch("lappdhit_y",lappdhit_y,"lappdhit_y[lappd_numhits]/D");
  LAPPDtree->Branch("lappdhit_z",lappdhit_z,"lappdhit_z[lappd_numhits]/D");
  LAPPDtree->Branch("lappdhit_stripcoorx", &lappdhit_stripcoorx);
  LAPPDtree->Branch("lappdhit_stripcoory", &lappdhit_stripcoory);
  LAPPDtree->Branch("lappdhit_stripcoorz", &lappdhit_stripcoorz);
  LAPPDtree->Branch("lappdhit_stripcoort", &lappdhit_stripcoort);
  LAPPDtree->Branch("lappdhit_globalcoorx", &lappdhit_globalcoorx);
  LAPPDtree->Branch("lappdhit_globalcoory", &lappdhit_globalcoory);
  LAPPDtree->Branch("lappdhit_globalcoorz", &lappdhit_globalcoorz);
  LAPPDtree->Branch("lappdhit_process",lappdhit_process,"lappdhit_process[lappd_numhits]/I");
  LAPPDtree->Branch("lappdhit_particleID",lappdhit_particleID,"lappdhit_particleID[lappd_numhits]/I");
  LAPPDtree->Branch("lappdhit_trackID",lappdhit_trackID,"lappdhit_trackID[lappd_numhits]/I");
  LAPPDtree->Branch("lappdhit_edep",lappdhit_edep,"lappdhit_edep[lappd_numhits]/D");
  LAPPDtree->Branch("lappdhit_objnum",lappdhit_objnum,"lappdhit_objnum[lappd_numhits]/I");
  //LAPPDtree->Branch("lappdhit_copynum",lappdhit_copynum,"lappdhit_copynum[lappd_numhits]/I");
  LAPPDtree->Branch("lappdhit_stripnum", &lappdhit_stripnum);
  LAPPDtree->Branch("lappdhit_truetime2",&lappdhit_truetime2);
  LAPPDtree->Branch("lappdhit_smeartime2", &lappdhit_smeartime2);
  LAPPDtree->Branch("lappdhit_primaryParentID2",&lappdhit_primaryParentID2);
  LAPPDtree->Branch("lappdhit_NoOfneighstripsHit", &lappdhit_NoOfneighstripsHit);
  LAPPDtree->Branch("lappdhit_neighstripnum", &lappdhit_neighstripnum);
  LAPPDtree->Branch("lappdhit_neighstrippeak", &lappdhit_neighstrippeak);
  LAPPDtree->Branch("lappdhit_neighstrip_time", &lappdhit_neighstrip_time);
  LAPPDtree->Branch("lappdhit_neighstrip_lefttime", &lappdhit_neighstrip_lefttime);
  LAPPDtree->Branch("lappdhit_neighstrip_righttime", &lappdhit_neighstrip_righttime);
}
