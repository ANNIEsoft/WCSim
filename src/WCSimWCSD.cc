#include "WCSimWCSD.hh"
#include "WCSimTuningParameters.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4ParticleTypes.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4ios.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>
#include <limits>
#include <iostream>
#include <fstream>

#include "WCSimDetectorConstruction.hh"
#include "WCSimTrackInformation.hh"

WCSimWCSD::WCSimWCSD(G4String CollectionName, G4String name,WCSimDetectorConstruction* myDet, G4String detectorElement="tank")
:G4VSensitiveDetector(name), detectorElement(detectorElement)
{
  // Place the name of this collection on the list.  We can have more than one
  // in principle.  CollectionName is a vector.

  // Note there is some sort of problem here.  If I use the name
  // Which has a "/" in it, I can find this collection later using 
  // GetCollectionID()

  collectionName.insert(CollectionName);
  
  fdet = myDet;
  
  HCID = -1;

  WCSimTuningParameters *tuning = (WCSimTuningParameters*) fdet->Get_TuningParams();
  G4bool pmtwiseqe = tuning->GetPMTwiseQE();
  if (pmtwiseqe) ReadInPMTWiseQE();
}

WCSimWCSD::~WCSimWCSD() {}

void WCSimWCSD::Initialize(G4HCofThisEvent* HCE)
{
  // Make a new hits collection. With the name we set in the constructor
  hitsCollection = new WCSimWCHitsCollection(SensitiveDetectorName,collectionName[0]);
  //G4cout<<"collectionName[0]= ******* "<<collectionName[0]<<G4endl;
  // This is a trick.  We only want to do this once.  When the program
  // starts HCID will equal -1.  Then it will be set to the pointer to
  // this collection.
  
  // Get the Id of the "0th" collection
  if (HCID<0){
    HCID =  GetCollectionID(0); 
  }
  
  // Add it to the Hit collection of this event.
  HCE->AddHitsCollection( HCID, hitsCollection ); 

  // Initilize the Hit map to all tubes not hit.
  PMTHitMap.clear();

  // clear maximum PE of a hit in this event
  WCSimWCHit::SetMaxPe(0);
}

G4bool WCSimWCSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // Only register hits from OpticalPhotons that are not 'fAlive'
  // ============================================================
  // alternatively, register hits for non-photons if energyDeposition == 0.0 ?
  G4ParticleDefinition *particleDefinition = aStep->GetTrack()->GetDefinition();
  if( (particleDefinition != G4OpticalPhoton::OpticalPhotonDefinition()) ||
      (aStep->GetTrack()->GetTrackStatus() == fAlive) ) {
    return false;
  }
  
  // Get the name & type of the sensor being hit, and the wavelength of photon, to retrieve QE
  // ==========================================================================================
  G4String volumeName  = aStep->GetTrack()->GetVolume()->GetName();
  bool isPMT = (volumeName != fdet->GetIDCollectionName2());
  G4float  stepEnergy  = aStep->GetTrack()->GetTotalEnergy()/CLHEP::eV;
  G4float  wavelength  = (stepEnergy>std::numeric_limits<float>::min()) ? (2.0*M_PI*197.3)/(stepEnergy) : 0;
  
  // Get PMT ID before calculating the QE (PMT-by-PMT QE values)
  // ==========================================================
  G4StepPoint*       preStepPoint = aStep->GetPreStepPoint();
   G4TouchableHandle  theTouchable = preStepPoint->GetTouchableHandle();
   std::stringstream tubeTag;
   for (G4int i = theTouchable->GetHistoryDepth()-1 ; i >= 0; i--){
     tubeTag << ":" << theTouchable->GetVolume(i)->GetName();
     tubeTag << "-" << theTouchable->GetCopyNumber(i);
   }
   G4int replicaNumber;
   if(isPMT){
     if(detectorElement=="tank"){
       replicaNumber = WCSimDetectorConstruction::GetTubeID(tubeTag.str());
     }
   }

  // Determine from QE whether to reject the hit
  // ===========================================
  G4float ratio = 1.;
  WCSimTuningParameters *tuning = (WCSimTuningParameters*) fdet->Get_TuningParams();
  bool pmtwiseqe = tuning->GetPMTwiseQE();
  double pmtwiseratio=1.;
  //G4cout <<"replicaNumber: "<<replicaNumber<<G4endl;
  //G4cout <<"replicaNumber < 133: "<<(replicaNumber<133)<<G4endl;
  if (pmtwiseqe && detectorElement=="tank" && replicaNumber<133 && replicaNumber>0) pmtwiseratio = vector_pmtqe.at(replicaNumber-1);

  G4double QEratio = tuning->GetQERatio();
  G4double QEratioWB = tuning->GetQERatioWB();
  //G4cout <<"QERatio: "<<QEratio<<", QEratioWB: "<<QEratioWB<<G4endl;
  //G4cout <<"volumeName: "<<volumeName<<", replicaNumber: "<<replicaNumber<<", pmtwiseratio: "<<pmtwiseratio<<G4endl;
  if (volumeName == "ANNIEp2v7-glassFaceWCPMT_R7081" || volumeName == "ANNIEp2v7-glassFaceWCPMT_R7081HQE" || volumeName == "ANNIEp2v7-glassFaceWCPMT_D784KFLB"){
    ratio = QEratioWB;
  } else { 
    ratio = QEratio;
  }
  ratio *= pmtwiseratio;
  //G4cout <<"ratio: "<<ratio<<G4endl; 
  //G4cout <<"QEratio: "<<QEratio<<G4endl;
  G4float maxQE;
  G4float photonQE;
  if(isPMT){
    //----- for pmts -------
    if (fdet->GetPMT_QE_Method()==1){
      photonQE = 1.1;
    }else if (fdet->GetPMT_QE_Method()==2){
      maxQE = fdet->GetPMTQE(volumeName,wavelength,0,240,660,QEratio);
      photonQE = fdet->GetPMTQE(volumeName, wavelength,1,240,660,ratio);
      photonQE = photonQE/maxQE;
    }else if (fdet->GetPMT_QE_Method()==3){
      //ratio = 1./(1.-0.25);
      //ratio = 1.;
      photonQE = fdet->GetPMTQE(volumeName, wavelength,1,240,660,ratio);
    }else if (fdet->GetPMT_QE_Method()==4){
      //maxQE = fdet->GetPMTQE(fdet->GetIDCollectionName(), wavelength,0,240,660,ratio);
      maxQE = fdet->GetPMTQE(fdet->GetIDCollectionName(), wavelength,0,240,660,QEratio);
      if(maxQE==0){
        G4cerr<<"MAXQE FOR PHOTON HIT ON VOLUME "<<volumeName<<" IS 0!!"<<G4endl;
        return false;
      }
      photonQE = fdet->GetPMTQE(volumeName, wavelength,1,240,660,ratio);
      photonQE = photonQE/maxQE;
    }else{ photonQE=0.3; }
  } else {
    //----- for lappds -------
    if (fdet->GetLAPPD_QE_Method()==1){
      photonQE = 1.1;
    }else if (fdet->GetLAPPD_QE_Method()==2){
      maxQE = fdet->GetLAPPDQE(volumeName,wavelength,0,240,660,QEratio);
      photonQE = fdet->GetLAPPDQE(volumeName, wavelength,1,240,660,ratio);
      photonQE = photonQE/maxQE;
    }else if (fdet->GetLAPPD_QE_Method()==3){
      photonQE = fdet->GetLAPPDQE(volumeName, wavelength,1,240,660,ratio);
    }else if (fdet->GetLAPPD_QE_Method()==4){
      maxQE = fdet->GetPMTQE(fdet->GetIDCollectionName(), wavelength,0,240,660,QEratio);
      if(maxQE==0){
        G4cerr<<"MAXQE FOR PHOTON HIT ON VOLUME "<<volumeName<<" IS 0!!"<<G4endl;
        return false;
      }
      photonQE = fdet->GetLAPPDQE(volumeName, wavelength,1,240,660,ratio);
      photonQE = photonQE/maxQE;
    }else{ photonQE=0.3; }
  }
  
  if (G4UniformRand() <= photonQE){
    
    // Hit passes QE test! Next check if it passes collection efficiency test
    // ======================================================================
    bool makethehit = false;
    
    // Get the sensor position and orientation
    // ---------------------------------------
    //G4StepPoint*       preStepPoint = aStep->GetPreStepPoint();
    //G4TouchableHandle  theTouchable = preStepPoint->GetTouchableHandle();
    G4VPhysicalVolume* thePhysical  = theTouchable->GetVolume();
    
    G4ThreeVector worldPosition = preStepPoint->GetPosition();
    G4ThreeVector localPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
    G4ThreeVector worldDirection = preStepPoint->GetMomentumDirection();
    //G4ThreeVector localDirection = theTouchable->GetHistory()->GetTopTransform().TransformAxis(worldDirection);
    
    // check if we're bypassing collection efficiency
    // ----------------------------------------------
    G4int Use_Coll_Eff = (isPMT) ? fdet->UsePMT_Coll_Eff() : fdet->UseLAPPD_Coll_Eff();
    
    if(Use_Coll_Eff!=0){
      // Calculate the angle of incidence
      // --------------------------------
      G4double local_x = localPosition.x();
      G4double local_y = localPosition.y();
      G4double local_z = localPosition.z();
      //G4cout<<"local pos = ("<<local_x<<", "<<local_y<<", "<<local_z<<")"<<G4endl;
      G4double localabspos = sqrt(pow(local_x,2)+pow(local_y,2)+pow(local_z,2));
      G4float theta_angle;
      if(localabspos!=0) theta_angle = acos(fabs(local_z)/localabspos)/3.1415926*180.;
      else theta_angle = 0;
      
      // Retrieve the threshold of detection
      // -----------------------------------
      G4float effectiveAngularEfficiency;
      if(isPMT) effectiveAngularEfficiency = fdet->GetPMTCollectionEfficiency(theta_angle, volumeName);
      else      effectiveAngularEfficiency = fdet->GetLAPPDCollectionEfficiency(theta_angle, volumeName);
      
      // check if we pass the collection efficiency test
      // ----------------------------------------------
      makethehit = (G4UniformRand() <= effectiveAngularEfficiency);
    } else {
      makethehit = true; // skip collection efficiency test
    }
    
    if(makethehit){
      // QE and CE tests passed! Make a hit! First, retrieve necessary information
      // =========================================================================
      
      // Get information about the photon track
      // ======================================
      G4int trackID           = aStep->GetTrack()->GetTrackID();
      
      // Get information about the parent track
      // ======================================
      WCSimTrackInformation* trackinfo = (WCSimTrackInformation*)(aStep->GetTrack()->GetUserInformation());
      G4int primParentID;
      if (trackinfo){
        primParentID = trackinfo->GetPrimaryParentID();
      } else if (aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        // if there is no trackinfo, then it is a primary particle!
        primParentID = aStep->GetTrack()->GetTrackID();
      } else {
        // it is a primary photon
        primParentID=-1;
      }
      
      // Get information about the hit
      // =============================
      G4double hitTime           = preStepPoint->GetGlobalTime();
      G4double energyDeposition  = aStep->GetTotalEnergyDeposit();
      
      // Get information about the sensor
      // ================================
      // Make the sensor tubeTag based on the replica numbers
      // Then use the tubeTag to get the tube ID
      // See WCSimDetectorConstruction::DescribeAndRegisterPMT() for tag construction.
     /* std::stringstream tubeTag;
      for (G4int i = theTouchable->GetHistoryDepth()-1 ; i >= 0; i--){
        tubeTag << ":" << theTouchable->GetVolume(i)->GetName();
        tubeTag << "-" << theTouchable->GetCopyNumber(i);
      }
      G4int replicaNumber;*/
      if(isPMT){
        if(detectorElement=="tank"){
          replicaNumber = WCSimDetectorConstruction::GetTubeID(tubeTag.str());
        } else if(detectorElement=="mrd"){
          replicaNumber = WCSimDetectorConstruction::GetMrdTubeID(tubeTag.str());
        } else if(detectorElement=="facc"){
          replicaNumber = WCSimDetectorConstruction::GetFaccTubeID(tubeTag.str());
        }
      } else {
        replicaNumber = WCSimDetectorConstruction::GetLAPPDID(tubeTag.str());
      }
      
      // Retrieve the pointer to the appropriate hit collection.
      // Since volumeName is the same as the SD name, this works.
      G4SDManager* SDman = G4SDManager::GetSDMpointer();
      G4RunManager* Runman = G4RunManager::GetRunManager();
      G4int collectionID = SDman->GetCollectionID(volumeName);
      const G4Event* currentEvent = Runman->GetCurrentEvent();
      G4HCofThisEvent* HCofEvent = currentEvent->GetHCofThisEvent();
      hitsCollection = (WCSimWCHitsCollection*)(HCofEvent->GetHC(collectionID));
      
      // If this tube hasn't been hit add it to the collection
      if (PMTHitMap[replicaNumber] == 0){
        //G4cout<<"_________ new PMT hit_______"<<G4endl;
        WCSimWCHit* newHit = new WCSimWCHit();
        newHit->SetTubeID(replicaNumber);
        newHit->SetTrackID(trackID);
        newHit->SetEdep(energyDeposition); 
        newHit->SetLogicalVolume(thePhysical->GetLogicalVolume());
        
        G4AffineTransform aTrans = theTouchable->GetHistory()->GetTopTransform();
        newHit->SetRot(aTrans.NetRotation());
        aTrans.Invert();
        newHit->SetPos(aTrans.NetTranslation());
        
        //G4cout<<"worldPosition : "<<worldPosition(0)<<","<<worldPosition(1)<<","<<worldPosition(2)<<G4endl;
        //G4cout<<"localPosition : "<<localPosition(0)<<","<<localPosition(1)<<","<<localPosition(2)<<G4endl;
        //G4cout<<"aTrans.NetTranslation()= "<<aTrans.NetTranslation().x()<<","
        //      <<aTrans.NetTranslation().y()<<","<<aTrans.NetTranslation().z()<<G4endl;
        //G4cout<<"HITdist_diffLV: "
        //      <<sqrt( (localPosition(0)-aTrans.NetTranslation().x())*
        //              (localPosition(0)-aTrans.NetTranslation().x()) +
        //              (localPosition(1)-aTrans.NetTranslation().y())*
        //              (localPosition(1)-aTrans.NetTranslation().y()) +
        //              (localPosition(2)-aTrans.NetTranslation().z())*
        //              (localPosition(2)-aTrans.NetTranslation().z()) )<<G4endl;
        //G4cout<<"HITdist_RLV: "
        //      <<sqrt( (localPosition(0)-aTrans.NetTranslation().x())*
        //              (localPosition(0)-aTrans.NetTranslation().x()) +
        //              (localPosition(1)-aTrans.NetTranslation().y())*
        //              (localPosition(1)-aTrans.NetTranslation().y()) )<<G4endl;
        
        // Set the hitMap value to the collection hit number
        PMTHitMap[replicaNumber] = hitsCollection->insert( newHit );
        (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime);
        (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(primParentID);
        (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddHitPos(worldPosition);
        if(not isPMT){
          (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddStripPosition(localPosition);
        }
      } else {
        (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddPe(hitTime);
        (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddParentID(primParentID);
        (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddHitPos(worldPosition);
        if(not isPMT){
          (*hitsCollection)[PMTHitMap[replicaNumber]-1]->AddStripPosition(localPosition);
        }
      }
    }   // pass collection efficiency test
  }     // pass QE test
  
  return true;
}

void WCSimWCSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) 
  { 
    G4int numHits = hitsCollection->entries();
    
    G4cout << "There are " << numHits << " hits in "<<detectorElement
           << " collection "<<collectionName[0]<<" : "<<G4endl;
    for (G4int i=0; i < numHits; i++) 
      (*hitsCollection)[i]->Print();
  }
}

void WCSimWCSD::ReadInPMTWiseQE(){

   std::ifstream pmtqefile("../WCSim/pmt_qe.txt");
   double temp_qe;
   while (!pmtqefile.eof()){
     pmtqefile >> temp_qe;
     vector_pmtqe.push_back(temp_qe);
     if (pmtqefile.eof()) break;
   }
   pmtqefile.close();

 }
