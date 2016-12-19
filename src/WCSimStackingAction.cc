#include "WCSimStackingAction.hh"
#include "WCSimDetectorConstruction.hh"

#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4VProcess.hh"
#include "G4VPhysicalVolume.hh"
#include "Randomize.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>

//class WCSimDetectorConstruction;

WCSimStackingAction::WCSimStackingAction(WCSimDetectorConstruction* myDet):DetConstruct(myDet) {;}
WCSimStackingAction::~WCSimStackingAction(){;}


G4ClassificationOfNewTrack WCSimStackingAction::ClassifyNewTrack
(const G4Track* aTrack) 
{
  
  G4String WCIDCollectionName = DetConstruct->GetIDCollectionName();
  G4String WCIDCollectionName2 = DetConstruct->GetIDCollectionName2();
  G4ClassificationOfNewTrack classification    = fWaiting;
  G4ParticleDefinition*      particleType      = aTrack->GetDefinition();

  //return classification;	//bypass QE photon check
  
  
  // Make sure it is an optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
    {
      G4float photonWavelength = (2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV);
      G4float ratio = 1./(1.0-0.25);
      G4float wavelengthQE = 0;
      if(aTrack->GetCreatorProcess()==NULL) {
	wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,1,240,660,ratio);
	if( G4UniformRand() > wavelengthQE ) {
	  classification = fKill;
	  // G4cout<<"stage 0 PMTS.."<<G4endl;
	}
      }
      else if (((G4VProcess*)(aTrack->GetCreatorProcess()))->GetProcessType()!=3)
	{
	  G4float photonWavelength = (2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV);
	  // MF : translated from skdetsim : better to increase the number of photons
	  // than to throw in a global factor  at Digitization time !
	  G4float ratio = 1./(1.0-0.25);
	  // XQ: get the maximum QE and multiply it by the ratio
	  // only work for the range between 240 nm and 660 nm for now 
	  // Even with WLS
	  G4float wavelengthQE = 0;
	  if (DetConstruct->GetPMT_QE_Method()==1){
	    wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,1,240,660,ratio);
	  }else if (DetConstruct->GetPMT_QE_Method()==2){
	    wavelengthQE  = DetConstruct->GetPMTQE(WCIDCollectionName,photonWavelength,0,240,660,ratio);
	  }else if (DetConstruct->GetPMT_QE_Method()==3){
	    wavelengthQE = 1.1;
	  }
	  
	  if( G4UniformRand() > wavelengthQE )
	    classification = fKill;
	}
    }
  
  //----------- for lappds ----------
  /*if( WCIDCollectionName2 =="ANNIEp2-glassFaceWCONLYLAPPDS"){
  // Make sure it is an optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
    {
      G4float photonWavelength = (2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV);
      G4float ratio = 1./(1.0-0.25);
      G4float wavelengthQE = 0;
      if(aTrack->GetCreatorProcess()==NULL) {
	wavelengthQE  = 0.3;
	if( G4UniformRand() > wavelengthQE )
	  classification = fKill;
      }
    }*/

  if( WCIDCollectionName2 =="ANNIEp2-glassFaceWCONLYLAPPDS"){
  // Make sure it is an optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
    {
      //G4cout<<"entering ANNIEp2-glassFaceWCONLYLAPPDS in StackingAction....."<<G4endl; 
      G4float photonWavelength = (2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV);
      G4float ratio = 1./(1.0-0.25);
      G4float wavelengthQE2 = 0;
      if(aTrack->GetCreatorProcess()==NULL) {
	wavelengthQE2  = DetConstruct->GetLAPPDQE(WCIDCollectionName2,photonWavelength,1,240,660,ratio);
	if( G4UniformRand() > wavelengthQE2 ){
	  classification = fKill;
	  //G4cout<<"stage 0.."<<G4endl;
	}
      }
      else if (((G4VProcess*)(aTrack->GetCreatorProcess()))->GetProcessType()!=3)
	{
	  G4float photonWavelength = (2.0*M_PI*197.3)/(aTrack->GetTotalEnergy()/CLHEP::eV);
	  // MF : translated from skdetsim : better to increase the number of photons
	  // than to throw in a global factor  at Digitization time !
	  G4float ratio = 1./(1.0-0.25);
	  // XQ: get the maximum QE and multiply it by the ratio
	  // only work for the range between 240 nm and 660 nm for now 
	  // Even with WLS
	  G4float wavelengthQE2 = 0;
	  if (DetConstruct->GetLAPPD_QE_Method()==1){ // G4cout<<"stage 1.."<<G4endl;
	    wavelengthQE2  = DetConstruct->GetLAPPDQE(WCIDCollectionName2,photonWavelength,1,240,660,ratio);
	  }else if (DetConstruct->GetLAPPD_QE_Method()==2){  //G4cout<<"stage 2.."<<G4endl;
	    wavelengthQE2  = DetConstruct->GetLAPPDQE(WCIDCollectionName2,photonWavelength,0,240,660,ratio);
	  }else if (DetConstruct->GetLAPPD_QE_Method()==3){ // G4cout<<"stage 3.."<<G4endl;
	    wavelengthQE2 = 1.1;
	  }
	  
	  if( G4UniformRand() > wavelengthQE2 )
	    classification = fKill;
	}
    }
  }//for lappds

  return classification;
}

void WCSimStackingAction::NewStage() {;}
void WCSimStackingAction::PrepareNewEvent() {;}

