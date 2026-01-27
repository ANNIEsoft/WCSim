#include "WCSimWCPMT.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"
#include "WCSimPMTObject.hh"


#include <vector>
// for memset
#include <cstring>
#include <exception>

#ifndef HYPER_VERBOSITY
//#define HYPER_VERBOSITY
#endif

extern "C" void skrn1pe_(float* );
//extern "C" void rn1pe_(float* ); // 1Kton

WCSimWCPMT::WCSimWCPMT(G4String name,
				   WCSimDetectorConstruction* myDetector, G4String detectorElement)
  :G4VDigitizerModule(name), detectorElement(detectorElement)
{
  //G4String colName = "WCRawPMTSignalCollection";
  this->myDetector = myDetector;
  if(detectorElement=="tank"){
  	collectionName.push_back("WCRawPMTSignalCollection");
  } else if(detectorElement=="mrd"){
  	collectionName.push_back("WCRawPMTSignalCollection_MRD");
  } else if(detectorElement=="facc"){
  	collectionName.push_back("WCRawPMTSignalCollection_FACC");
  }
  DigiHitMapPMT.clear();
#ifdef HYPER_VERBOSITY
  G4cout<<"WCSimWCPMT::WCSimWCPMT ☆ recording collection name "<<collectionName[0]<<G4endl;
#endif

}

WCSimWCPMT::~WCSimWCPMT(){
 
}

G4double WCSimWCPMT::rn1pe(WCSimPMTObject* PMT){

  G4int i;
  G4double random = G4UniformRand();
  G4double random2 = G4UniformRand(); 
  G4float *qpe0;
  qpe0 = PMT->Getqpe();
  for(i = 0; i < 501; i++){
    
    if (random <= *(qpe0+i)) break;
  }
  if(i==500)
    random = G4UniformRand();
  
  return (G4double(i-50) + random2)/22.83;
  
}


void WCSimWCPMT::Digitize()
{
  // Create a DigitCollection and retrieve the appropriate hitCollection ID based on detectorElement
  G4String WCCollectionName;
  std::vector<G4String> WCCollectionNames;
  G4String DigitsCollectionName;
  if(detectorElement=="tank"){
    DigitsCollectionName="WCDigitizedCollectionPMT";
    WCCollectionNames = myDetector->GetIDCollectionNames();
  } else if(detectorElement=="mrd"){
    DigitsCollectionName="WCDigitizedCollection_MRDPMT";
    WCCollectionName = myDetector->GetMRDCollectionName();
    WCCollectionNames.push_back(WCCollectionName);
  } else if(detectorElement=="facc"){
    DigitsCollectionName="WCDigitizedCollection_FACCPMT";
    WCCollectionName = myDetector->GetFACCCollectionName();
    WCCollectionNames.push_back(WCCollectionName);
  }
  DigitsCollection = new WCSimWCDigitsCollection (DigitsCollectionName,collectionName[0]);
  
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
  // now loop over all Sensitive Detector hits collections (possibly >1) and construct digits:
  int itt=0;
  for(auto aWCCollectionName : WCCollectionNames){
  // Get the hit collection ID from the name
  G4int WCHCID = DigiMan->GetHitsCollectionID(aWCCollectionName);
  WCSimWCHitsCollection* WCHC = (WCSimWCHitsCollection*)(DigiMan->GetHitsCollection(WCHCID));

#ifdef HYPER_VERBOSITY
  if(itt==0) {
    G4cout<<"WCSimWCPMT::Digitize ☆ Making digits collection (WCSimWCDigitsCollection*)"
          <<DigitsCollectionName<<" for "<<detectorElement 
          <<" and calling MakePeCorrection on "<<aWCCollectionName<<" to fill it."<<G4endl;
  } else {
    G4cout<<"WCSimWCPMT::Digitize ☆ Adding to digits collection (WCSimWCDigitsCollection*)"
          <<DigitsCollectionName<<" for "<<detectorElement 
          <<" by calling MakePeCorrection on "<<aWCCollectionName<<"."<<G4endl;
  }
  itt++;
#endif
  if (WCHC) {
    MakePeCorrection(WCHC);
  }

  }

#ifdef HYPER_VERBOSITY
  G4cout<<"WCSimWCPMT::Digitize ☆ Storing "<<DigitsCollectionName<<" for "<<detectorElement
        <<", which has "<<DigitsCollection->entries()<<" entries"<<G4endl;
#endif

  StoreDigiCollection(DigitsCollection);

}


void WCSimWCPMT::MakePeCorrection(WCSimWCHitsCollection* WCHC)
{ 
  if(WCHC->entries()==0) return;
  G4LogicalVolume* alogvol = (*WCHC)[0]->GetLogicalVolume();
  G4String WCCollectionName = alogvol->GetName();
  
  //Get the PMT info for hit time smearing
  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCCollectionName);

#ifdef HYPER_VERBOSITY
  G4cout<<"WCSimWCPMT::MakePeCorrection ☆ making PE correction for ";
  if(WCHC){G4cout<<WCHC->entries();} else {G4cout<<"0";} G4cout<<" entries"<<G4endl;
#endif
  
  for (G4int i=0; i < WCHC->entries(); i++)
    {

      //G4double peCutOff = .3;
      // MF, based on S.Mine's suggestion : global scaling factor applied to
      // all the smeared charges.
      // means that we need to increase the collected light by
      // (efficiency-1)*100% to
      // match K2K 1KT data  : maybe due to PMT curvature ?

      //G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action

      // Get the information from the hit
      G4int   tube         = (*WCHC)[i]->GetTubeID();
      G4double peSmeared = 0.0;
      double time_PMT, time_true;

	  for (G4int ip =0; ip < (*WCHC)[i]->GetTotalPe(); ip++){
	    try{
	      time_true = (*WCHC)[i]->GetTime(ip);
	    }
	    catch (...){
	      G4cout<<"Error in WCSimWCPMT::MakePeCorrection call of WCSimWCHit::GetTime()"<<G4endl;
	      assert(false);
	    }
	    peSmeared = rn1pe(PMT);
	    int parent_id = (*WCHC)[i]->GetPrimaryParentID(ip);
      int directparent_id = (*WCHC)[i]->GetDirectParentID(ip);

	    //apply time smearing
	    float Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	    time_PMT = time_true + PMT->HitTimeSmearing(Q);

	    if ( DigiHitMapPMT[tube] == 0) {
	      //G4cout<<"WCSimWCPMT::MakePeCorrection ☆ New Digi in DigitsCollection for PMT "<<tube<<G4endl;
	      WCSimWCDigi* Digi = new WCSimWCDigi();
	      Digi->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      Digi->AddPe(time_PMT);	
	      Digi->SetTubeID(tube);
	      Digi->SetPe(ip,peSmeared);
	      Digi->SetTime(ip,time_PMT);
	      Digi->SetPreSmearTime(ip,time_true);
	      Digi->SetPrimaryParentID(ip,parent_id);
        Digi->SetDirectParentID(ip,directparent_id);
	      DigiHitMapPMT[tube] = DigitsCollection->insert(Digi);
	    }	
	    else {
	      //G4cout<<"WCSimWCPMT::MakePeCorrection ☆ Adding to Digi in DigitsCollection for PMT "<<tube<<G4endl;
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->AddPe(time_PMT);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTubeID(tube);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPe(ip,peSmeared);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTime(ip,time_PMT);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPreSmearTime(ip,time_true);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPrimaryParentID(ip,parent_id);
        (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetDirectParentID(ip,directparent_id);
	    }
      
	  } // Loop over hits in each PMT
    }// Loop over PMTs
}


