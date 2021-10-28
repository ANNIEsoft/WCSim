

///////////////////////////////////////////////////////////////////////////////
//                    Spectrum of radiative neutron capture by Gadolinium            
//                                    version 1.0.0                                
//                                    (Sep.09.2005)                               

//                Author : karim.zbiri@subatech.in2p3.fr                  

//Modified class from original G4NeutronHPCaptureFS class to deexcite and
//add correctly the secondary to the hadronic final state

// Karim Zbiri, Aug, 2005
///////////////////////////////////////////////////////////////////////////////



#ifndef GdNeutronHPCaptureFSANNRI_h
#define GdNeutronHPCaptureFSANNRI_h 1

#include "globals.hh"
#include "G4HadProjectile.hh"
#include "G4HadFinalState.hh"
#include "G4NeutronHPFinalState.hh"
#include "G4ReactionProductVector.hh"
#include "G4NeutronHPNames.hh"
#include "G4NeutronHPPhotonDist.hh"
#include "G4Nucleus.hh"
#include "G4Fragment.hh"

//#include "GdCaptureGammas_ggarnet.hh"
//#include "GdCaptureGammas_glg4sim.hh"
#include "DrawMessage.hh"

// Forward declaration
namespace ANNRIGdGammaSpecModel { class ANNRIGd_GdNCaptureGammaGenerator; }
namespace AGd = ANNRIGdGammaSpecModel;

class GdNeutronHPCaptureFSANNRI : public G4NeutronHPFinalState
{
	public:

		GdNeutronHPCaptureFSANNRI(G4int _gdcapture, G4int _gdcascade);
		~GdNeutronHPCaptureFSANNRI();

		void   UpdateNucleus( const G4Fragment* , G4double );
		void Init (G4double A, G4double Z, G4int M, G4String & dirName, G4String & aFSType);
		G4HadFinalState * ApplyYourself(const G4HadProjectile & theTrack);
		G4NeutronHPFinalState * New() 
		{
			GdNeutronHPCaptureFSANNRI * theNew = new GdNeutronHPCaptureFSANNRI(Gd_CAPTURE,Gd_CASCADE);
			return theNew;
		}

	private:

		void InitANNRIGdGenerator();
		G4ReactionProductVector* GenerateWithANNRIGdGenerator();

		G4int    Gd_CAPTURE; //1:natural , 2:enriched 157Gd, 3:enriched 155Gd
		G4int    Gd_CASCADE; //1:discrete + continuum; 2:discrete, 3:continuum
		G4String Gd157_ROOTFile="../WCSim/cont_dat/158GdContTbl__E1SLO4__HFB.root";
		G4String Gd155_ROOTFile="../WCSim/cont_dat/156GdContTbl__E1SLO4__HFB.root";

		G4Fragment * nucleus;

		G4DynamicParticle * theTwo ;
		G4ReactionProduct theTarget; 
		G4Nucleus aNucleus;
		G4ReactionProduct theNeutron;

		G4double targetMass;

		G4NeutronHPPhotonDist theFinalStatePhotons;
		G4NeutronHPNames theNames;
		
	//	GdCaptureGammas_ggarnet theFinalgammas_ggarnet;
	//	GdCaptureGammas_glg4sim theFinalgammas_glg4sim;

		G4double theCurrentA;
		G4double theCurrentZ;

		DrawMessage Printing;

		G4HadFinalState theResult;

	private:

		static AGd::ANNRIGd_GdNCaptureGammaGenerator* sAnnriGammaGen;
};
#endif
