///////////////////////////////////////////////////////////////////////////////
//                   Spectrum of radiative neutron capture by Gadolinium           
//                                    version 1.0.0                                
//                                    (Sep.09.2005)                               

//                Author : karim.zbiri@subatech.in2p3.fr                  

//Modified class from original G4NeutronHPCaptureFS class to deexcite and
//add correctly the secondary to the hadronic final state

// Karim Zbiri, Aug, 2005
///////////////////////////////////////////////////////////////////////////////

#include "GdNeutronHPCaptureFSANNRI.hh"
#include "G4Gamma.hh"
#include "G4ReactionProduct.hh"
#include "G4Nucleus.hh"
#include "G4PhotonEvaporation.hh"
#include "G4Fragment.hh"
#include "G4ParticleTable.hh" 
#include "G4IonTable.hh"
#include "G4NeutronHPDataUsed.hh"
#include "ANNRIGd_GdNCaptureGammaGenerator.hh"
#include "ANNRIGd_GeneratorConfigurator.hh"
#include "ANNRIGd_OutputConverter.hh"
namespace AGd = ANNRIGdGammaSpecModel;

#define File_Name "GdNeutronHPCaptureFSANNRI.cc"

// use a static generator to avoid the construction and loading of data
// files for each Gd isotope
AGd::ANNRIGd_GdNCaptureGammaGenerator* GdNeutronHPCaptureFSANNRI::sAnnriGammaGen = 0;

////////////////////////////////////////////////////////////////////////////////////////
GdNeutronHPCaptureFSANNRI::GdNeutronHPCaptureFSANNRI(G4int _gdcapture, G4int _gdcascade) :
////////////////////////////////////////////////////////////////////////////////////////
nucleus(0), theTwo(0), targetMass(0)
{
	Gd_CAPTURE = _gdcapture;
	Gd_CASCADE = _gdcascade;
	hasXsec = false;
	//if(MODEL == 4 and not sAnnriGammaGen) InitANNRIGdGenerator();
	if(not sAnnriGammaGen) InitANNRIGdGenerator();
}

////////////////////////////////////////////////////////////////////////////////////////
GdNeutronHPCaptureFSANNRI::~GdNeutronHPCaptureFSANNRI()
////////////////////////////////////////////////////////////////////////////////////////
{;}

////////////////////////////////////////////////////////////////////////////////////////
G4HadFinalState * GdNeutronHPCaptureFSANNRI::ApplyYourself(const G4HadProjectile & theTrack)
///////////////////////////////////////////////////////////////////////////////////////
{
	G4int i;
	theResult.Clear();

	////////////////prepare neutron//////////////////////
	G4double eKinetic = theTrack.GetKineticEnergy();
	const G4HadProjectile *incidentParticle = &theTrack;
	//theNeutron = const_cast<G4ParticleDefinition *>(incidentParticle->GetDefinition()) ;
	G4ReactionProduct theNeutron( const_cast<G4ParticleDefinition *>(incidentParticle->GetDefinition()) );
	theNeutron.SetMomentum( incidentParticle->Get4Momentum().vect() );
	theNeutron.SetKineticEnergy( eKinetic );

	/////////////////prepare target////////////////////
	G4ReactionProduct theTarget; 
	G4Nucleus aNucleus;
	G4double eps = 0.0001;
	//if(targetMass<500*MeV)
	if(targetMass<500*CLHEP::MeV)
		//targetMass = ( G4NucleiPropertiesTable::GetNuclearMass(static_cast<G4int>(theBaseZ+eps), static_cast<G4int>(theBaseA+eps))) /
		targetMass = ( G4NucleiProperties::GetNuclearMass( static_cast<G4int>(theBaseA+eps) , static_cast<G4int>(theBaseZ+eps) )) /
			G4Neutron::Neutron()->GetPDGMass();
	G4ThreeVector neutronVelocity = 1./G4Neutron::Neutron()->GetPDGMass()*theNeutron.GetMomentum();
	G4double temperature = theTrack.GetMaterial()->GetTemperature();
	theTarget = aNucleus.GetBiasedThermalNucleus(targetMass, neutronVelocity, temperature);

	//////////////go to nucleus rest system////////////////
	theNeutron.Lorentz(theNeutron, -1*theTarget);
	eKinetic = theNeutron.GetKineticEnergy();

	///////////////////dice the photons////////////////////
	G4ReactionProductVector * thePhotons = NULL;    
/*
	if(MODEL==1){
		thePhotons = theFinalgammas_ggarnet.GetGammas();
	}else if(MODEL==2){
		thePhotons = theFinalgammas_glg4sim.GetGammas();
	}else if(MODEL==4){
		thePhotons = GenerateWithANNRIGdGenerator();
	}else{
		Printing.Error(File_Name,"MODEL");
	}
*/
	thePhotons = GenerateWithANNRIGdGenerator();
	////////////////////update the nucleus/////////////////
	G4ThreeVector aCMSMomentum = theNeutron.GetMomentum()+theTarget.GetMomentum();
	G4LorentzVector p4(aCMSMomentum, theTarget.GetTotalEnergy() + theNeutron.GetTotalEnergy());
	nucleus = new G4Fragment(static_cast<G4int>(theBaseA+1), static_cast<G4int>(theBaseZ) ,p4);
	
	G4int nPhotons = 0;
	if(thePhotons!=NULL) nPhotons=thePhotons->size();
	for(i=0;i<nPhotons;i++){
		G4Fragment * theOne;
		G4ThreeVector pGamma(thePhotons->operator[](i)->GetMomentum());
		G4LorentzVector gamma(pGamma,thePhotons->operator[](i)->GetTotalEnergy());
		theOne= new G4Fragment(gamma,G4Gamma::GammaDefinition());
		UpdateNucleus(theOne,thePhotons->operator[](i)->GetTotalEnergy());
	}
	theTwo = new G4DynamicParticle;
	//theTwo->SetDefinition(G4ParticleTable::GetParticleTable()
	theTwo->SetDefinition(G4IonTable::GetIonTable()
			->GetIon(static_cast<G4int>(theBaseZ), static_cast<G4int>(theBaseA+1)));
			//->FindIon(static_cast<G4int>(theBaseZ), static_cast<G4int>(theBaseA+1), 0, static_cast<G4int>(theBaseZ)));
	theTwo->SetMomentum(nucleus->GetMomentum());

	/////////////add them to the final state///////////////
	G4int nParticles = nPhotons;
	if(1==nPhotons) nParticles = 2;

	/////////////////back to lab system///////////////////
	for(i=0; i<nPhotons; i++){
		thePhotons->operator[](i)->Lorentz(*(thePhotons->operator[](i)), theTarget);
	}

	//////////////Recoil, if only one gamma//////////////
	if (1==nPhotons){
		G4DynamicParticle * theOne = new G4DynamicParticle;
		//G4ParticleDefinition * aRecoil = G4ParticleTable::GetParticleTable()
		G4ParticleDefinition * aRecoil = G4IonTable::GetIonTable()
			->GetIon(static_cast<G4int>(theBaseZ), static_cast<G4int>(theBaseA+1));
			//->FindIon(static_cast<G4int>(theBaseZ), static_cast<G4int>(theBaseA+1), 0, static_cast<G4int>(theBaseZ));
		theOne->SetDefinition(aRecoil);
		// Now energy; 
		// Can be done slightly better @
		G4ThreeVector aMomentum =  theTrack.Get4Momentum().vect()
			+theTarget.GetMomentum()
			-thePhotons->operator[](0)->GetMomentum();

		G4ThreeVector theMomUnit = aMomentum.unit();
		G4double aKinEnergy =  theTrack.GetKineticEnergy()
			+theTarget.GetKineticEnergy(); // gammas come from Q-value
		G4double theResMass = aRecoil->GetPDGMass();
		G4double theResE = aRecoil->GetPDGMass()+aKinEnergy;
		G4double theAbsMom = std::sqrt(theResE*theResE - theResMass*theResMass);
		G4ThreeVector theMomentum = theAbsMom*theMomUnit;
		theOne->SetMomentum(theMomentum);
		theResult.AddSecondary(theOne);
	}

	//////////////Now fill in the gammas.////////////////////
	for(i=0; i<nPhotons; i++){
		// back to lab system
		G4DynamicParticle * theOne = new G4DynamicParticle;
		theOne->SetDefinition(thePhotons->operator[](i)->GetDefinition());
		theOne->SetMomentum(thePhotons->operator[](i)->GetMomentum());
		// G4cout << "PID: gamma is generated by Gd. The momentum is:" << thePhotons->operator[](i)->GetMomentum() << G4endl;//Added by Yano, to see the function working correctly.
		theResult.AddSecondary(theOne);
		delete thePhotons->operator[](i);
	}
	delete thePhotons; 

	///////////////ADD deexcited nucleus////////////////////
	theResult.AddSecondary(theTwo);

	/////////////clean up the primary neutron///////////////
	theResult.SetStatusChange(stopAndKill);
	return &theResult;
}

//////////////////////////////////////////////////////////////////////////////////////
void GdNeutronHPCaptureFSANNRI::UpdateNucleus( const G4Fragment* gamma , G4double eGamma )
	//////////////////////////////////////////////////////////////////////////////////////
{
	G4LorentzVector p4Gamma = gamma->GetMomentum();
	G4ThreeVector pGamma(p4Gamma.vect());

	G4LorentzVector p4Nucleus(nucleus->GetMomentum() );

	G4ParticleTable* theTable = G4ParticleTable::GetParticleTable();

	//G4double m1 = theTable->FindIon(static_cast<G4int>(nucleus->GetZ()), static_cast<G4int>(nucleus->GetA()), 0, static_cast<G4int>(nucleus->GetZ())) ->GetPDGMass();
	
	//G4double m1 = G4IonTable::GetIonTable()->FindIon(static_cast<G4int>(nucleus->GetZ()), static_cast<G4int>(nucleus->GetA()), 0, static_cast<G4int>(nucleus->GetZ())) ->GetPDGMass();
	G4double m1 = G4IonTable::GetIonTable()->GetIon(static_cast<G4int>(nucleus->GetZ()), static_cast<G4int>(nucleus->GetA())) ->GetPDGMass();

	//G4double m1 = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIonMass(static_cast<G4int>(nucleus->GetZ()),
	//static_cast<G4int>(nucleus->GetA()));
	G4double m2 = nucleus->GetZ() *  G4Proton::Proton()->GetPDGMass() + 
		(nucleus->GetA()- nucleus->GetZ())*G4Neutron::Neutron()->GetPDGMass();

	G4double Mass = std::min(m1,m2);

	G4double newExcitation = p4Nucleus.mag() - Mass - eGamma;

	//G4cout<<" Egamma = "<<eGamma<<G4endl;
	//G4cout<<" NEW EXCITATION ENERGY = "<< newExcitation <<G4endl;

	if(newExcitation < 0) newExcitation = 0;

	G4ThreeVector p3Residual(p4Nucleus.vect() - pGamma);
	G4double newEnergy = std::sqrt(p3Residual * p3Residual +
			(Mass + newExcitation) * (Mass + newExcitation));
	G4LorentzVector p4Residual(p3Residual, newEnergy);

	// Update excited nucleus parameters
	nucleus->SetMomentum(p4Residual);

	//  G4cout<<"nucleus excitation energy = "<<nucleus->GetExcitationEnergy() <<G4endl;  

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//void GdNeutronHPCaptureFSANNRI::Init (G4double A, G4double Z, G4String & dirName, G4String & )
#include <sstream> 
//  void GdNeutronHPCaptureFSANNRI::Init (G4double A, G4double Z, G4int M, G4String & dirName, G4String & )
void GdNeutronHPCaptureFSANNRI::Init (G4double A, G4double Z, G4int ,G4String & dirName, G4String & )
	///////////////////////////////////////////////////////////////////////////////////////////////////
{
	G4String tString = "/FS/";
	G4bool dbool;
	G4NeutronHPDataUsed aFile = theNames.GetName(static_cast<G4int>(A), static_cast<G4int>(Z), dirName, tString, dbool);

	G4String filename = aFile.GetName();
	theBaseA = A;
	theBaseZ = G4int(Z+.5);
	if(!dbool || ( Z<2.5 && ( std::abs(theBaseZ - Z)>0.0001 || std::abs(theBaseA - A)>0.0001)))
	{
		hasAnyData = false;
		hasFSData = false; 
		hasXsec = false;
		return;
	}
	std::ifstream theData(filename, std::ios::in);

	hasFSData = theFinalStatePhotons.InitMean(theData); 
	if(hasFSData)
	{
		targetMass = theFinalStatePhotons.GetTargetMass();
		theFinalStatePhotons.InitAngular(theData); 
		theFinalStatePhotons.InitEnergies(theData); 
	}
	theData.close();
}

////////////////////////////////////////////////////////////////////////////////
void GdNeutronHPCaptureFSANNRI::InitANNRIGdGenerator()
////////////////////////////////////////////////////////////////////////////////
{
	if(sAnnriGammaGen) delete sAnnriGammaGen;
	sAnnriGammaGen = new AGd::ANNRIGd_GdNCaptureGammaGenerator();

	AGd::ANNRIGd_GeneratorConfigurator::Configure(*sAnnriGammaGen,
			Gd_CAPTURE, Gd_CASCADE, Gd155_ROOTFile, Gd157_ROOTFile);
}

////////////////////////////////////////////////////////////////////////////////
G4ReactionProductVector* GdNeutronHPCaptureFSANNRI::GenerateWithANNRIGdGenerator()
////////////////////////////////////////////////////////////////////////////////
{
	G4ReactionProductVector* theProducts = 0;

	if(sAnnriGammaGen) {
		AGd::ReactionProductVector products;
		if(Gd_CAPTURE == 1)                          // nat. Gd
			products = sAnnriGammaGen->Generate_NatGd();
		else if(Gd_CAPTURE == 2 and Gd_CASCADE == 1)  // 157Gd, discrete + continuum
			products = sAnnriGammaGen->Generate_158Gd();
		else if(Gd_CAPTURE == 2 and Gd_CASCADE == 2)  // 157Gd, discrete
			products = sAnnriGammaGen->Generate_158Gd_Discrete();
		else if(Gd_CAPTURE == 2 and Gd_CASCADE == 3)  // 157Gd, continuum
			products = sAnnriGammaGen->Generate_158Gd_Continuum();
		else if(Gd_CAPTURE == 3 and Gd_CASCADE == 1)  // 155Gd, discrete + continuum
			products = sAnnriGammaGen->Generate_156Gd();
		else if(Gd_CAPTURE == 3 and Gd_CASCADE == 2)  // 155Gd, discrete
			products = sAnnriGammaGen->Generate_156Gd_Discrete();
		else if(Gd_CAPTURE == 3 and Gd_CASCADE == 3)  // 155Gd, continuum
			products = sAnnriGammaGen->Generate_156Gd_Continuum();

		theProducts = AGd::ANNRIGd_OutputConverter::ConvertToG4(products);
	}
	else {
		Printing.Error(File_Name,"sAnnriGammaGen");
		theProducts = new G4ReactionProductVector();
	}

	return theProducts;
}
