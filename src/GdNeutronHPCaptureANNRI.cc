
///////////////////////////////////////////////////////////////////////////////
//                   Spectrum of radiative neutron capture by Gadolinium            
//                                    version 1.0.0                                
//                                    (Sep.09.2005)                               

//Modified class from original G4NeutronHPCapture class to include 
//the gammas spectrum of radiative neutron capture by Gadolinium.

// Takatomi Yano, Oct, 2013
// Karim Zbiri, April, 2005
///////////////////////////////////////////////////////////////////////////////


/////////#include "OtherHPCaptureFS.hh"
#include "G4SystemOfUnits.hh"
#include "G4NeutronHPCaptureFS.hh"
#include "G4NeutronHPDeExGammas.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4NeutronHPManager.hh"
#include "GdNeutronHPCaptureFSANNRI.hh"
#include "GdNeutronHPCaptureANNRI.hh"

//////////////////////////////////////////////////
GdNeutronHPCaptureANNRI::GdNeutronHPCaptureANNRI(G4String _gdcomposition, G4String _gdcascade)
	:G4HadronicInteraction("NeutronHPCapture")
	 //////////////////////////////////////////////////
{

	if (_gdcomposition == "Natural") Gd_CAPTURE=1;
	else if (_gdcomposition == "157Gd-enr") Gd_CAPTURE=2;
	else if (_gdcomposition == "155Gd-enr") Gd_CAPTURE=3;
	else {
		G4cout <<" Specified Gd composition option "<<_gdcomposition<<" does not exist!!! Using Natural composition... (default)"<< G4endl;
		Gd_CAPTURE=1;
	}

	if (_gdcascade == "PeakAndCont") Gd_CASCADE=1;
	else if (_gdcascade == "Peak") Gd_CASCADE=2;
	else if (_gdcascade == "Cont") Gd_CASCADE=3;
	else {
		G4cout <<"Specified Gd cascade model "<<_gdcascade<<" does not exist!!! Using Peak + Continuum model"<<G4endl;
		Gd_CASCADE=1;
	}

	SetMinEnergy( 0.0 );
	SetMaxEnergy( 20.*MeV );
	//    G4cout << "Capture : start of construction!!!!!!!!"<<G4endl;
	if(!getenv("G4NEUTRONHPDATA")) 
		throw G4HadronicException(__FILE__, __LINE__, "Please setenv G4NEUTRONHPDATA to point to the neutron cross-section files.");
	dirName = getenv("G4NEUTRONHPDATA");
	G4String tString = "/Capture";
	dirName = dirName + tString;
	numEle = G4Element::GetNumberOfElements();
	//    G4cout << "+++++++++++++++++++++++++++++++++++++++++++++++++"<<G4endl;
	//    G4cout <<"Disname="<<dirName<<" numEle="<<numEle<<G4endl;
	//theCapture = new G4NeutronHPChannel[numEle];
	//    G4cout <<"G4NeutronHPChannel constructed"<<G4endl;
	G4NeutronHPCaptureFS * theFS = new G4NeutronHPCaptureFS;
	//    OtherHPCaptureFS * theFS = new OtherHPCaptureFS; 
	GdNeutronHPCaptureFSANNRI * theGdFS = new GdNeutronHPCaptureFSANNRI(Gd_CAPTURE,Gd_CASCADE);

	for ( G4int i = 0 ; i < numEle ; i++ ) {
		if((*(G4Element::GetElementTable()))[i]->GetZ() != 64){
			theCapture.push_back( new G4NeutronHPChannel );
			(*theCapture[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
			/*
			   (*theCapture[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
			   G4cout<<(*(G4Element::GetElementTable()))[i]->GetName()<<G4endl;
			   */
			(*theCapture[i]).Register(theFS);
		}else {
			theCapture.push_back( new G4NeutronHPChannel );
			(*theCapture[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
			/*
			   (*theCapture[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
			   G4cout<<(*(G4Element::GetElementTable()))[i]->GetName()<<G4endl;
			   */
			(*theCapture[i]).Register(theGdFS);
		}
	}
	delete theFS;
	delete theGdFS;
	//    G4cout << "-------------------------------------------------"<<G4endl;
	//    G4cout << "Leaving GdNeutronHPCaptureANNRI::GdNeutronHPCaptureANNRI"<<G4endl;
}

//////////////////////////////////////////////////
GdNeutronHPCaptureANNRI::~GdNeutronHPCaptureANNRI()
	//////////////////////////////////////////////////
{
	//delete [] theCapture;
	//    G4cout << "Leaving GdNeutronHPCaptureANNRI::~GdNeutronHPCaptureANNRI"<<G4endl;
	for ( std::vector<G4NeutronHPChannel*>::iterator 
			ite = theCapture.begin() ; ite != theCapture.end() ; ite++ )
	{
		delete *ite;
	}
	theCapture.clear();
}

#include "G4NeutronHPThermalBoost.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
G4HadFinalState * GdNeutronHPCaptureANNRI::ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aNucleus )
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
{

	if ( numEle < (G4int)G4Element::GetNumberOfElements() ) addChannelForNewElement();

	G4NeutronHPManager::GetInstance()->OpenReactionWhiteBoard();
	if(getenv("NeutronHPCapture")) G4cout <<" ####### GdNeutronHPCaptureANNRI called"<<G4endl;
	const G4Material * theMaterial = aTrack.GetMaterial();
	G4int n = theMaterial->GetNumberOfElements();
	G4int index = theMaterial->GetElement(0)->GetIndex();
	if(n!=1)
	{
		xSec = new G4double[n];
		G4double sum=0;
		G4int i;
		const G4double * NumAtomsPerVolume = theMaterial->GetVecNbOfAtomsPerVolume();
		G4double rWeight;    
		G4NeutronHPThermalBoost aThermalE;
		for (i=0; i<n; i++)
		{
			index = theMaterial->GetElement(i)->GetIndex();
			rWeight = NumAtomsPerVolume[i];

			xSec[i] = (*theCapture[index]).GetXsec(aThermalE.GetThermalEnergy(aTrack,
						theMaterial->GetElement(i),
						theMaterial->GetTemperature()));
			xSec[i] *= rWeight;
			sum+=xSec[i];
		}
		G4double random = G4UniformRand();
		G4double running = 0;
		for (i=0; i<n; i++)
		{
			running += xSec[i];
			index = theMaterial->GetElement(i)->GetIndex();
			//if(random<=running/sum) break;
			if( sum == 0 || random <= running/sum ) break;
		}
		if(i==n) i=std::max(0, n-1);
		delete [] xSec;
	}

	//return theCapture[index].ApplyYourself(aTrack);

	G4HadFinalState* result = (*theCapture[index]).ApplyYourself(aTrack);
	aNucleus.SetParameters(G4NeutronHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargA(),G4NeutronHPManager::GetInstance()->GetReactionWhiteBoard()->GetTargZ());
	G4NeutronHPManager::GetInstance()->CloseReactionWhiteBoard();
	return result; 
}

/////////////////////////////////////////////////////////////////////////////////////////
const std::pair<G4double, G4double> GdNeutronHPCaptureANNRI::GetFatalEnergyCheckLevels() const
/////////////////////////////////////////////////////////////////////////////////////////
{
	//return std::pair<G4double, G4double>(10*perCent,10*GeV);
	return std::pair<G4double, G4double>(10*perCent,DBL_MAX);
}

//////////////////////////////////////////////////
void GdNeutronHPCaptureANNRI::addChannelForNewElement()
	//////////////////////////////////////////////////
{
	GdNeutronHPCaptureFSANNRI* theFS = new GdNeutronHPCaptureFSANNRI(Gd_CAPTURE,Gd_CASCADE);
	for ( G4int i = numEle ; i < (G4int)G4Element::GetNumberOfElements() ; i++ ) 
	{
		//		G4cout << "GdNeutronHPCaptureANNRI Prepairing Data for the new element of " << (*(G4Element::GetElementTable()))[i]->GetName() << G4endl;
		theCapture.push_back( new G4NeutronHPChannel );
		(*theCapture[i]).Init((*(G4Element::GetElementTable()))[i], dirName);
		(*theCapture[i]).Register(theFS);
	}
	delete theFS;
	numEle = (G4int)G4Element::GetNumberOfElements();
}
