/* vim:set noexpandtab tabstop=4 wrap */
#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimEventInformation.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "TFile.h"
#include "TLorentzVector.h"
#include "globals.hh"
#include "Randomize.hh"
#include <fstream>
#include <vector>
#include <string>
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <math.h> 
#include <libgen.h>

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"

// GENIE headers
#ifndef NO_GENIE
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepUtils.h"
#include "Framework/Ntuple/NtpMCTreeHeader.h"
#include "Framework/Interaction/Interaction.h"
#endif

// when loading dirt primaries, skip entries that are from upstream rock interactions. 
#ifndef ONLY_TANK_EVENTS
//#define ONLY_TANK_EVENTS
#endif

// as help for reconstruction, a sample where light is only from muons, no other primary particles from the event.
#ifndef ONLY_MUONS
//#define ONLY_MUONS
#endif

using std::vector;
using std::string;
using std::fstream;

vector<string> tokenize( string separators, string input );

inline vector<string> readInLine(fstream& inFile, int lineSize, char* inBuf)
{
  // Read in line break it up into tokens
  inFile.getline(inBuf,lineSize);
  return tokenize(" $", inBuf);
}

inline float atof( const string& s ) {return std::atof( s.c_str() );}
inline int   atoi( const string& s ) {return std::atoi( s.c_str() );}

WCSimPrimaryGeneratorAction::WCSimPrimaryGeneratorAction(
					  WCSimDetectorConstruction* myDC)
  :myDetector(myDC), loadNewPrimaries(true), inputdata(0), primariesDirectory(""), neutrinosDirectory(""), vectorFileName("")
{
  //T. Akiri: Initialize GPS to allow for the laser use 
  MyGPS = new G4GeneralParticleSource();

  // Initialize to zero
  mode = 0;
  nvtxs = 0;
  for( Int_t u=0; u<MAX_N_PRIMARIES; u++){
    vtxsvol[u] = 0;
    vtxs[u] = G4ThreeVector(0.,0.,0.);
  }
  nuEnergy = 0.;
  _counterRock=0; // counter for generated in Rock
  _counterCublic=0; // counter generated
  
  //---Set defaults. Do once at beginning of session.
  
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  particleGun->SetParticleEnergy(1.0*CLHEP::GeV);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.0));
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4IonTable* ionTable = G4IonTable::GetIonTable();
  G4String particleName;
  particleGun->
    SetParticleDefinition(particleTable->FindParticle(particleName="mu+"));

  particleGun->
    SetParticlePosition(G4ThreeVector(0.*CLHEP::m,0.*CLHEP::m,0.*CLHEP::m));
    
  messenger = new WCSimPrimaryGeneratorMessenger(this);
  useMulineEvt = false;
  useGunEvt = false;
  useLaserEvt = false;
  useBeamEvt = true;
  useGPSEvt = false;

#ifndef NO_GENIE      
  genierecordval = new genie::NtpMCEventRecord;
#endif
}

WCSimPrimaryGeneratorAction::~WCSimPrimaryGeneratorAction()
{
  if (IsGeneratingVertexInRock()){
    G4cout << "Fraction of Rock volume is : " << G4endl;
      G4cout << " Random number generated in Rock / in Cublic = " 
             << _counterRock << "/" << _counterCublic 
             << " = " << _counterRock/(G4double)_counterCublic << G4endl;
  }
  inputFile.close();
  delete particleGun;
  delete MyGPS;   //T. Akiri: Delete the GPS variable
  delete messenger;
  
  if(useBeamEvt){
    if(inputdata){
      inputdata->ResetBranchAddresses();
      metadata->ResetBranchAddresses();
      delete inputdata;
      delete metadata;
#ifndef NO_GENIE
      if(geniedata) geniedata->ResetBranchAddresses();
      if(geniedata) delete geniedata;
      if(genierecordval) delete genierecordval;
#endif
    }
  }

}

void WCSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  // We will need a particle table
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  // Temporary kludge to turn on/off vector text format 

  G4bool useNuanceTextFormat = true;

  WCSimEventInformation *eventInfo = new WCSimEventInformation();
  anEvent->SetUserInformation((G4VUserEventInformation*) eventInfo);

  // Do for every event

  if (useMulineEvt)
  {

    if ( !inputFile.is_open() )
    {
      G4cout << "Set a vector file using the command /mygen/vecfile name"
	     << G4endl;
      exit(-1);
    }

    //
    // Documentation describing the nuance text format can be found here: 
    // http://neutrino.phy.duke.edu/nuance-format/
    //
    // The format must be strictly adhered to for it to be processed correctly.
    // The lines and their meanings from begin through info are fixed, and then
    // a variable number of tracks may follow.
    //
    if (useNuanceTextFormat)
      {
	const int lineSize=100;
	char      inBuf[lineSize];
	vector<string> token(1);
	
	token = readInLine(inputFile, lineSize, inBuf);
	  
        if (token.size() == 0) 
	  {
	    G4cout << "end of nuance vector file!" << G4endl;
	  }
	else if (token[0] != "begin")
	  {
	    G4cout << "unexpected line begins with " << token[0] << G4endl;
	  }
	else   // normal parsing begins here
	  {
	    // Read the nuance line (ignore value now)

	    token = readInLine(inputFile, lineSize, inBuf);
	    mode = atoi(token[1]);

	    // Read the Vertex line
	    token = readInLine(inputFile, lineSize, inBuf);
	    vtxs[0] = G4ThreeVector(atof(token[1])*cm,
				    atof(token[2])*cm,
				    atof(token[3])*cm);
	    
            // true : Generate vertex in Rock , false : Generate vertex in WC tank
            SetGenerateVertexInRock(false);

	    // Next we read the incoming neutrino and target
	    
	    // First, the neutrino line

	    token=readInLine(inputFile, lineSize, inBuf);
	    beampdgs[0] = atoi(token[1]);
	    beamenergies[0] = atof(token[2])*MeV;
	    beamdirs[0] = G4ThreeVector(atof(token[3]),
					atof(token[4]),
					atof(token[5]));

	    // Now read the target line

	    token=readInLine(inputFile, lineSize, inBuf);
	    targetpdgs[0] = atoi(token[1]);
	    targetenergies[0] = atof(token[2])*MeV;
	    targetdirs[0] = G4ThreeVector(atof(token[3]),
					  atof(token[4]),
					  atof(token[5]));

	    // Read the info line, basically a dummy
	    token=readInLine(inputFile, lineSize, inBuf);
	    G4cout << "Vector File Record Number " << token[2] << G4endl;
            vecRecNumber = atoi(token[2]);
	    
	    // Now read the outgoing particles
	    // These we will simulate.


	    while ( token=readInLine(inputFile, lineSize, inBuf),
		    token[0] == "track" )
	      {
		// We are only interested in the particles
		// that leave the nucleus, tagged by "0"


		if ( token[6] == "0")
		  {
		    G4int pdgid = atoi(token[1]);
		    G4double energy = atof(token[2])*CLHEP::MeV;
		    G4ThreeVector dir = G4ThreeVector(atof(token[3]),
						      atof(token[4]),
						      atof(token[5]));
		    std::cout<<"PDGcode "<<pdgid<<"\n";
		    //std::cout <<"dirX: "<<token[3]<<", dirY: "<<token[4]<<", dirZ: "<<token[5]<<std::endl;
		    //must handle the case of an ion speratly from other particles
		    //check PDG code if we have an ion.
		    //PDG code format for ions ±10LZZZAAAI
		    char strPDG[11];
		    char strA[10]={0};
		    char strZ[10]={0};
		    

		    long int A=0,Z=0;
		    //		    A=strotl(strPDG,&str);
		    if(abs(pdgid) >= 1000000000)
		      {
			//ion
			sprintf(strPDG,"%i",abs(pdgid));
			strncpy(strZ, &strPDG[3], 3);
			strncpy(strA, &strPDG[6], 3);
			strA[3]='\0';
			strZ[3]='\0';
			A=atoi(strA);
			Z=atoi(strZ);
			G4ParticleDefinition* ion;
			ion =  G4IonTable::GetIonTable()->GetIon(Z, A, 0.);
			particleGun->SetParticleDefinition(ion);
			particleGun->SetParticleCharge(0);
		      }
		    else {
		      //not ion
		      particleGun->
			SetParticleDefinition(particleTable->
		      FindParticle(pdgid));
		    }
		    G4double mass = 
		      particleGun->GetParticleDefinition()->GetPDGMass();

		    G4double ekin = energy - mass;

		    particleGun->SetParticleEnergy(ekin);
		    //G4cout << "Particle: " << pdgid << " KE: " << ekin << G4endl;
		    particleGun->SetParticlePosition(vtxs[0]);
		    particleGun->SetParticleMomentumDirection(dir);
		    particleGun->GeneratePrimaryVertex(anEvent);
		  }
	      }
	  }
      }
    else 
      {    // old muline format  
	inputFile >> nuEnergy >> energy >> xPos >> yPos >> zPos 
		  >> xDir >> yDir >> zDir;
	
	G4double random_z = ((myDetector->GetWaterTubePosition())
			     - .5*(myDetector->GetWaterTubeLength()) 
			     + 1.*CLHEP::m + 15.0*CLHEP::m*G4UniformRand())/CLHEP::m;
	zPos = random_z;
	G4ThreeVector vtx = G4ThreeVector(xPos, yPos, random_z);
	G4ThreeVector dir = G4ThreeVector(xDir,yDir,zDir);

	particleGun->SetParticleEnergy(energy*CLHEP::MeV);
	particleGun->SetParticlePosition(vtx);
	particleGun->SetParticleMomentumDirection(dir);
	particleGun->GeneratePrimaryVertex(anEvent);
      }
  }

  else if (useGunEvt)
  {      // manual gun operation
    particleGun->GeneratePrimaryVertex(anEvent);

    //To prevent occasional seg fault from an un assigned targetpdg 
    targetpdgs[0] = 2212; //ie. proton

    G4ThreeVector P  =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
    G4ThreeVector vtx=anEvent->GetPrimaryVertex()->GetPosition();
    G4double m       =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
    G4int pdg        =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();
   

    char strPDG[11];
    char strA[10]={0};
    char strZ[10]={0};
    
    
    long int A=0,Z=0;
    //		    A=strotl(strPDG,&str);
    if(abs(pdg) >= 1000000000)
      {
	//ion
	sprintf(strPDG,"%i",abs(pdg));
	strncpy(strZ, &strPDG[3], 3);
	strncpy(strA, &strPDG[6], 3);
	strA[3]='\0';
	strZ[3]='\0';
	A=atoi(strA);
	Z=atoi(strZ);

	G4ParticleDefinition* ion   = G4IonTable::GetIonTable()->GetIon(Z, A, 0);
	ion->SetPDGStable(false);
	ion->SetPDGLifeTime(0.);
	
	G4ParticleDefinition* ion2   = G4IonTable::GetIonTable()->GetIon(Z, A, 0);
	std::cout<<"ion2 "<<ion2->GetPDGLifeTime()<<"\n";
      }
    
    
    G4ThreeVector dir  = P.unit();
    G4double E         = std::sqrt((P.dot(P))+(m*m));

//     particleGun->SetParticleEnergy(E);
//     particleGun->SetParticlePosition(vtx);
//     particleGun->SetParticleMomentumDirection(dir);

    SetVtx(vtx);
    SetBeamEnergy(E);
    SetBeamDir(dir);
    SetBeamPDG(pdg);
  }
  else if (useLaserEvt)
    {
      targetpdgs[0] = 2212; //ie. proton 
      //T. Akiri: Create the GPS LASER event
      MyGPS->GeneratePrimaryVertex(anEvent);
      
      G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4double m        =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();      // this is rest mass
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();
      
      G4ThreeVector dir  = P.unit();
      G4double E         = std::sqrt((P.dot(P))+(m*m));
      
      SetVtx(vtx);   // required to store the true vertex for Bonsai!
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);
      const G4ParticleDefinition* parttype = anEvent->GetPrimaryVertex()->GetPrimary()->GetParticleDefinition();
      G4String particlename;
      if(parttype == G4OpticalPhoton::OpticalPhotonDefinition() ) {particlename="opticalphoton";}
      else { (parttype) ? particlename=parttype->GetParticleName() : particlename=std::to_string(pdg); }
      
      double tote = anEvent->GetPrimaryVertex()->GetPrimary()->GetTotalEnergy();
      double ke = anEvent->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy();
      
      G4cout<<"Generating laser primary "<<particlename<<" with total energy "
            <<tote/MeV<<"MeV and kinetic energy "<<ke/MeV
            <<"MeV at ("<<vtx.x()/cm<<","<<vtx.y()/cm<<","<<vtx.z()/cm<<") in direction ("
            <<dir.x()<<", "<<dir.y()<<", "<<dir.z()<<") ";
      G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4VPhysicalVolume* primaryPV = theNavigator->LocateGlobalPointAndSetup(vtx);
      std::string vtxvol = ( (primaryPV) ? primaryPV->GetName() : "<<no-primaryPV>>" );
      G4cout<<" in "<<vtxvol<<G4endl;
    }
  
  else if (useBeamEvt)
    {
		// Load the next entry, with all required trees and files
		// ------------------------------------------------------
		loadbeamentry:
		if(loadNewPrimaries){ LoadNewPrimaries(); } // update TChain if a new file is loaded by messenger
		//inputdata has already had tree loaded at the end of last event's GeneratePrimaries call
		//localEntry will already be the value of the NEXT entry
		metadata->LoadTree(inputEntry);
		
		Int_t nextTreeNumber = inputdata->GetTreeNumber();
		if(treeNumber!=nextTreeNumber){
			G4cout<< "Reached end of Tree. Last entries' tree number was "
						<< treeNumber <<", this entries' tree number is "<< nextTreeNumber <<G4endl;
			dirtFileName = inputdata->GetCurrentFile()->GetName(); // new tree, new file
			char* dirtFileNameAsChar = strdup(dirtFileName.c_str());
			dirtFileName = basename(dirtFileNameAsChar);
			inputdata->SetBranchAddress("run",&runbranchval,&runBranch);
			inputdata->SetBranchAddress("ntank",&ntankbranchval,&nTankBranch);
			inputdata->SetBranchAddress("nupdg",&nupdgval,&nupdgBranch);
			inputdata->SetBranchAddress("nuvtxx",&nuvtxxval,&nuvtxxBranch);
			inputdata->SetBranchAddress("nuvtxy",&nuvtxyval,&nuvtxyBranch);
			inputdata->SetBranchAddress("nuvtxz",&nuvtxzval,&nuvtxzBranch);
			inputdata->SetBranchAddress("nuvtxt",&nuvtxtval,&nuvtxtBranch);
			inputdata->SetBranchAddress("vtxvol",&nupvval,&nuPVBranch);
			inputdata->SetBranchAddress("vtxmat",&numatval,&nuvtxmatBranch);
			inputdata->SetBranchAddress("entry",&genieentrybranchval,&genieentryBranch);
			metadata->SetBranchAddress("inputFluxName",&nufluxfilenameval,&nufluxfilenameBranch);
#ifndef NO_GENIE
			geniedata->SetBranchAddress("gmcrec",&genierecordval,&genierecordBranch);
			genierecordBranch->SetAutoDelete(kTRUE);
#else
			genierecordBranch=(TBranch*)1;
#endif
			vtxxBranch=inputdata->GetBranch("vx");
			vtxyBranch=inputdata->GetBranch("vy");
			vtxzBranch=inputdata->GetBranch("vz");
			vtxtBranch=inputdata->GetBranch("vt");
			pxBranch=inputdata->GetBranch("px");
			pyBranch=inputdata->GetBranch("py");
			pzBranch=inputdata->GetBranch("pz");
			EBranch=inputdata->GetBranch("E");
			KEBranch=inputdata->GetBranch("kE");
			pdgBranch=inputdata->GetBranch("pdgtank");
			nuprimaryBranch=inputdata->GetBranch("primary");
		
			if(runBranch==0||nTankBranch==0||vtxxBranch==0||vtxyBranch==0||vtxzBranch==0||vtxtBranch==0||pxBranch==0||pyBranch==0||pzBranch==0||EBranch==0||KEBranch==0||pdgBranch==0||nupdgBranch==0||nuvtxxBranch==0||nuvtxyBranch==0||nuvtxzBranch==0||nuvtxtBranch==0||nuPVBranch==0||nuvtxmatBranch==0||nuprimaryBranch==0||nufluxfilenameBranch==0||genierecordBranch==0){
				G4cout<<"BRANCHES ARE ZOMBIES ARGH!"<<G4endl;
			} else { G4cout<<"entries in this tree: "<<vtxxBranch->GetEntries()<<G4endl; }
			
			entriesInThisTree = runBranch->GetEntries();
			treeNumber=nextTreeNumber;
		}
		
		G4cout<<"Loading primaries from entry "<<inputEntry<<", localentry "<<localEntry<<"/"<<entriesInThisTree<<G4endl;
		//runBranch->GetEntry(localEntry);
		//G4cout<<"Run is "<<runbranchval<<G4endl;
		
		nTankBranch->GetEntry(localEntry);
		nupdgBranch->GetEntry(localEntry);
		nuvtxxBranch->GetEntry(localEntry);
		nuvtxyBranch->GetEntry(localEntry);
		nuvtxzBranch->GetEntry(localEntry);
		nuvtxtBranch->GetEntry(localEntry);
		nuPVBranch->GetEntry(localEntry);
		nuvtxmatBranch->GetEntry(localEntry);
		genieentryBranch->GetEntry(localEntry);
		nufluxfilenameBranch->GetEntry(localEntry);
		
		// note info about this input event for recording into output file
		dirtEntryNum = localEntry;
		genieEntryNum = genieentrybranchval;
		genieFileName = nufluxfilenameval;
		
#ifdef ONLY_TANK_EVENTS
		if(strcmp(numatval,"TankWater")!=0){ // nu intx not in tank
		G4cout<<"---------------SKIPPING NON-TANK ENTRY----------------"<<G4endl;
		inputEntry++; 
		localEntry = inputdata->LoadTree(inputEntry);
		if(localEntry<0){
			// get the pointer to the UI manager
			G4UImanager* UI = G4UImanager::GetUIpointer();
			UI->ApplyCommand("/run/abort 0");	// abort without processing current event
			G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			G4cout<<"@#@#@#@#@#@#@#@#@#@ REACHED END OF INPUT FILE! #@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
		} else { goto loadbeamentry; } // load the next entry
		}
#endif

#ifndef NO_GENIE		
		Long64_t genielocalEntry = geniedata->LoadTree(genieentrybranchval);
		// load the appropriate genie entry. we assume 1:1 correspondance of genie:g4dirt files.
		// So the following should not be necessary, as the files should be loaded synchronously
		if(genielocalEntry<0){
			// get the pointer to the UI manager
			G4UImanager* UI = G4UImanager::GetUIpointer();
			UI->ApplyCommand("/run/abort 1");	// abort after processing current event
			G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			G4cout<<"@#@#@#@#@#@#@#@#@#@ REACHED END OF GENIE FILE! #@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
		}
		genierecordBranch->GetEntry(genieentrybranchval);
#endif
		
		G4ParticleDefinition* parttype = particleTable->FindParticle(nupdgval);
		TLorentzVector neutrinovertex(nuvtxtval*CLHEP::s, nuvtxxval*CLHEP::m, nuvtxyval*CLHEP::m, nuvtxzval*CLHEP::m);	// position in m, times in s, convert to cm and ns
		G4cout<<"The origin interaction was a "<<(parttype->GetParticleName())<<" at ("<<nuvtxtval*1000000000.<<","<<nuvtxxval*100.<<","<<nuvtxyval*100.<<","<<nuvtxzval*100.<<")[ns, cm] in "<<nupvval<<" "<<numatval<<G4endl;
		G4cout<<"This entry has "<<ntankbranchval<<" primaries"<<G4endl;
		
		if(vtxxbranchval){delete[] vtxxbranchval;}
		if(vtxybranchval){delete[] vtxybranchval;}
		if(vtxzbranchval){delete[] vtxzbranchval;}
		if(vtxtbranchval){delete[] vtxtbranchval;}
		if(pxbranchval){delete[] pxbranchval;}
		if(pybranchval){delete[] pybranchval;}
		if(pzbranchval){delete[] pzbranchval;}
		if(ebranchval){delete[] ebranchval;}
		if(kebranchval){delete[] kebranchval;}
		if(pdgbranchval){delete[] pdgbranchval;}
		if(nuprimarybranchval){delete[] nuprimarybranchval;}
		
		vtxxbranchval = new Double_t[ntankbranchval];
		vtxybranchval = new Double_t[ntankbranchval];
		vtxzbranchval = new Double_t[ntankbranchval];
		vtxtbranchval = new Double_t[ntankbranchval];
		pxbranchval = new Double_t[ntankbranchval];
		pybranchval = new Double_t[ntankbranchval];
		pzbranchval = new Double_t[ntankbranchval];
		ebranchval = new Double_t[ntankbranchval];
		kebranchval = new Double_t[ntankbranchval];
		pdgbranchval = new Int_t[ntankbranchval];
		nuprimarybranchval = new Int_t[ntankbranchval];
		
		if(vtxxbranchval==0||vtxybranchval==0||vtxzbranchval==0||vtxtbranchval==0||pxbranchval==0||pybranchval==0||pzbranchval==0||ebranchval==0||kebranchval==0||pdgbranchval==0||nuprimarybranchval==0){
			G4cout<<"Arrays are zombies!"<<G4endl;
		}
		
		//G4cout<<"Setting branch addresses"<<G4endl;
		vtxxBranch->SetAddress(vtxxbranchval);
		vtxyBranch->SetAddress(vtxybranchval);
		vtxzBranch->SetAddress(vtxzbranchval);
		vtxtBranch->SetAddress(vtxtbranchval);
		pxBranch->SetAddress(pxbranchval);
		pyBranch->SetAddress(pybranchval);
		pzBranch->SetAddress(pzbranchval);
		EBranch->SetAddress(ebranchval);
		KEBranch->SetAddress(kebranchval);
		pdgBranch->SetAddress(pdgbranchval);
		nuprimaryBranch->SetAddress(nuprimarybranchval);
		
		//G4cout<<"Getting primary arrays"<<G4endl;
		vtxxBranch->GetEntry(localEntry);
		vtxyBranch->GetEntry(localEntry);
		vtxzBranch->GetEntry(localEntry);
		vtxtBranch->GetEntry(localEntry);
		pxBranch->GetEntry(localEntry);
		pyBranch->GetEntry(localEntry);
		pzBranch->GetEntry(localEntry);
		EBranch->GetEntry(localEntry);
		KEBranch->GetEntry(localEntry);
		pdgBranch->GetEntry(localEntry);
		nuprimaryBranch->GetEntry(localEntry);
		
		// OK, actually use the loaded information
		// ----------------------------------------
#ifdef ONLY_TANK_EVENTS
		// this *should* always be true, since tank nu events will generate primaries...
		Bool_t primariesinthisentry=false;
		for(int i=0;i<ntankbranchval;i++){
			if(nuprimarybranchval[i]==1){ primariesinthisentry=true; break; }
		}
		if(!primariesinthisentry){ // not genie primaries... (this shouldn't happen)
			G4cout<<"---------------SKIPPING ENTRY WITH NO GENIE PRIMARIES----------------"<<G4endl;
			inputEntry++;
			localEntry = inputdata->LoadTree(inputEntry);
			if(localEntry<0){
				// get the pointer to the UI manager
				G4UImanager* UI = G4UImanager::GetUIpointer();
				UI->ApplyCommand("/run/abort 0");	// abort without processing current event
				G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
				G4cout<<"@#@#@#@#@#@#@#@#@#@ REACHED END OF INPUT FILE! #@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
				G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			} else { goto loadbeamentry; } // load the next entry
		}
#endif
#ifdef ONLY_MUONS
		// ensure the event has at least one muon and skip it if not
		Bool_t muonsinthisentry=false;
		for(int i=0;i<ntankbranchval;i++){
			if(pdgbranchval[i]==13){ muonsinthisentry=true; break; }
		}
		if(!muonsinthisentry){ // no muons in the event
			G4cout<<"---------------SKIPPING ENTRY WITH NO MUONS ----------------"<<G4endl;
			inputEntry++;
			localEntry = inputdata->LoadTree(inputEntry);
			if(localEntry<0){
				// get the pointer to the UI manager
				G4UImanager* UI = G4UImanager::GetUIpointer();
				UI->ApplyCommand("/run/abort 0");	// abort without processing current event
				G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
				G4cout<<"@#@#@#@#@#@#@#@#@#@ REACHED END OF INPUT FILE! #@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
				G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			} else { goto loadbeamentry; } // load the next entry
		}
#endif
		// First the genie information (largely unused as not currently stored in wcsim output)
		// ===========================
#ifndef NO_GENIE
		// genie information available: (not all is currently stored)
		genie::EventRecord* gevtRec = genierecordval->event;
		genie::Interaction* genieint = gevtRec->Summary();
		// we write files with v2_8_6, but read with v2_12_0, so /*V*/ indicates reading is validated
	
		// process information:
//		TString procinfostring = genieint->ProcInfo().AsString();
//		TString scatteringtypestring = genieint->ScatteringTypeAsString();
//		TString interactiontypestring = genieint->InteractionTypeAsString();
//		Bool_t isQE = interaction.ProcInfo().IsQuasiElastic();
//		Double_t neutrinoq2 = genieint->Kine().Q2();
//		TLorentzVector& k1 = *(gevtRec->Probe()->P4());
//		TLorentzVector& k2 = *(gevtRec->FinalStatePrimaryLepton()->P4());
//		Double_t costhfsl = TMath::Cos( k2.Vect().Angle(k1.Vect()) );
//		TLorentzVector* genieVtx = gevtRec->Vertex();
//		G4double x = genieVtx->X() * m;         // same info as nuvtx in g4dirt file
//		G4double y = genieVtx->Y() * m;         // GENIE uses meters
//		G4double z = genieVtx->Z() * m;         // GENIE uses meters
//		G4double t = genieVtx->T() * second;    // GENIE uses seconds for time
		Int_t neutinteractioncode = genie::utils::ghep::NeutReactionCode(gevtRec); /*V*/
//		Int_t nuanceinteractioncode  = genie::utils::ghep::NuanceReactionCode(gevtRec);
		
		// neutrino information:
		Double_t probeenergy = genieint->InitState().ProbeE(genie::kRfLab) * GeV;  /*V*/
//		TSring probepartname = genieint->InitState().Probe()->GetName();
		Int_t probepdg = genieint->InitState().Probe()->PdgCode();                 /*V*/
		TLorentzVector* probemomentum = gevtRec->Probe()->P4();                    /*V*/
		TVector3 probethreemomentum = probemomentum->Vect();
		TVector3 probemomentumdir = probethreemomentum.Unit();
		// n.b.  genieint->InitState().Probe != gevtRec->Probe()
		
		// target nucleon:
//		int targetnucleonpdg = genieint->InitState().Tgt().HitNucPdg();
//		TString targetnucleonname;
//		if ( genie::pdg::IsNeutronOrProton(targetnucleonpdg) ) {
//			TParticlePDG * p = genie::PDGLibrary::Instance()->Find(targetnucleonpdg);
//			targetnucleonname = p->GetName();
//		} else {
//			targetnucleonname = targetnucleonpdg;
//		}
		TLorentzVector* targetnucleonmomentum=0;
		TVector3 targetnucleonthreemomentum(0.,0.,0.);
		Double_t targetnucleonenergy =0;
		if(gevtRec->HitNucleon()){
			targetnucleonmomentum = gevtRec->HitNucleon()->P4();               /*V*/
			targetnucleonthreemomentum = targetnucleonmomentum->Vect();
			targetnucleonenergy = targetnucleonmomentum->Energy() * GeV;
		}
		
		// target nucleus:
		Int_t targetnucleuspdg = genieint->InitState().Tgt().Pdg();                /*V*/
//		TParticlePDG * targetnucleus = genie::PDGLibrary::Instance()->Find( targetnucleuspdg );
//		TString targetnucleusname = "unknown";
//		if(targetnucleus){ targetnucleusname = nucleartarget->GetName(); }
//		Int_t targetnucleusZ = genieint->InitState().Tgt().Z();
//		Int_t targetnucleusA = genieint->InitState().Tgt().A();
	
		// remnant nucleus:
//		int remnucpos = gevtRec->RemnantNucleusPosition(); 
//		TString remnantnucleusname="n/a";
//		Double_t remnantnucleusenergy=-1. * GeV;
//		if(remnucpos>-1){
//			remnantnucleusname = gevtRec->Particle(remnucpos)->Name();
//			remnantnucleusenergy = gevtRec->Particle(remnucpos)->Energy(); //GeV
//		}
	
		// final state lepton:
//		int fsleppos = gevtRec->FinalStatePrimaryLeptonPosition();
//		TString fsleptonname="n/a";
//		Double_t fsleptonenergy=-1. * GeV;
//		if(fsleppos>-1){
//			fsleptonname = gevtRec->Particle(ipos)->Name();
//			fsleptonenergy = gevtRec->Particle(ipos)->Energy();
//		}
	
		// other remnants:
//		Int_t numfsprotons = genieint->ExclTag().NProtons();
//		Int_t numfsneutrons = genieint->ExclTag().NNeutrons();
//		Int_t numfspi0 = genieint->ExclTag().NPi0();
//		Int_t numfspiplus = genieint->ExclTag().NPiPlus();
//		Int_t numfspiminus = genieint->ExclTag().NPiMinus();
		
		//  The following information is retrieved from the PrimaryGeneratorAction in EndOfEventAction:
		//  For each neutrino vertex, the neutrino, target, + any nue, gamma and e daughters are stored.
		///////////////////////
		//	vecRecNumber           // entry number in input file. we should store filename too.
		//	mode;                  // neutrino interaction mode
		//	nvtxs;                 // number of neutrino(?) vertices.
		//	npar;                  // number of primary particles? not used.
		//	vtxsvol[nvtxs]         // neutrino vertex volume indices. not used, looked up in EndOfEventAction.
		//	vtxs[nvtxs]            // neutrino interaction vertices.
		//	beampdgs[nvtxs]        // pdgs of probe neutrinos
		//	beamenergies[nvtxs]    // energies of probe neutrinos, MeV (just as num)
		//	beamdirs[nvtxs]        // directions of probe neutrinos
		//	targetpdgs[nvtxs]      // pdgs of neutrino targets
		//	targetenergies[nvtxs]  // target nucleon energies, MeV (just as num)
		//	targetdirs[nvtxs]      // momentum direction unit vector of target
		///////////////////////
		
		vecRecNumber = genieentrybranchval;
		mode = neutinteractioncode;
		nvtxs = 1;
		npar = -1;                                              // ? not used.
		for(int i=0; i<nvtxs; i++){                             // we only ever have 1 neutrino intx
			vtxsvol[i] = -10;                                   // looked up in EndOfEventAction
			// neutrino vertices are stored in m not cm
			vtxs[i] = G4ThreeVector(nuvtxxval*CLHEP::m, nuvtxyval*CLHEP::m, nuvtxzval*CLHEP::m);
			beampdgs[i] = probepdg;
			beamenergies[i] = probeenergy;
			targetpdgs[i] = targetnucleuspdg;
			targetenergies[i] = targetnucleonenergy;
			G4ThreeVector probemomdir;                          // convert TVector3 to G4ThreeVector
			G4ThreeVector targetnucleonmomdir;
			for(int comp=0; comp<2; comp++){
				probemomdir[comp] = probemomentumdir[comp];
				targetnucleonmomdir[comp] = targetnucleonthreemomentum.Unit()[comp];
			}
			beamdirs[i] = probemomdir;
			targetdirs[i] = targetnucleonmomdir;
		}
#else
		// without genie we don't have the primary interaction information....
		vecRecNumber = genieentrybranchval;
		mode = -999;
		nvtxs = 1;
		npar = -1;                                              // ? not used.
		for(int i=0; i<nvtxs; i++){                             // we only ever have 1 neutrino intx
			vtxsvol[i] = -10;                               // looked up in EndOfEventAction
			vtxs[i] = G4ThreeVector(-999., -999., -999.);   // this is the NEUTRINO info
			beampdgs[i] = -999;                             // NOT THE PRIMARY PARTICLE INFO
			beamenergies[i] = -999.;
			beamdirs[i] = G4ThreeVector(-999., -999., -999.);
			targetpdgs[i] = -999;
			targetenergies[i] = -999.;
			targetdirs[i] = G4ThreeVector(-0., -0., -1.);
		}
#endif
		
		// Now read the outgoing particles: These we will simulate
		// =======================================================
		//G4cout<<"Looping over primaries"<<G4endl;
		for(int i=0;i<ntankbranchval;i++){
			//G4cout<<"Loading details of primary "<<i<<G4endl;
			vtxxval=vtxxbranchval[i]*CLHEP::cm;
			vtxyval=vtxybranchval[i]*CLHEP::cm;
			vtxzval=vtxzbranchval[i]*CLHEP::cm;
			vtxtval=vtxtbranchval[i]*CLHEP::ns;
			pxval=pxbranchval[i]*GeV;
			pyval=pybranchval[i]*GeV;
			pzval=pzbranchval[i]*GeV;
			eval=ebranchval[i]*GeV;
			keval=kebranchval[i]*GeV;
			pdgval=pdgbranchval[i];
			nuprimaryval=nuprimarybranchval[i];
			G4ThreeVector thevtx = G4ThreeVector(vtxxval, vtxyval, vtxzval);
			G4ThreeVector thepdir = G4ThreeVector(pxval, pyval, pzval);
			thepdir.unit();		// normalise to unit vector
			
			// correct for cases where keval == 0 by giving them a little boost
			if(keval==0){keval+=1.*eV; eval+=1.*eV; thepdir=G4ThreeVector(0.,0.,1.);}
			
			//must handle the case of an ion speratly from other particles
			//check PDG code if we have an ion: PDG code format for ions ±10LZZZAAAI
			char strPDG[11];
			char strA[10]={0};
			char strZ[10]={0};
			long int A=0,Z=0;
#ifdef ONLY_MUONS
			if(abs(pdgval)!=13){
				// skip non-muon primary
				continue;
			}
#endif
			if(abs(pdgval) >= 1000000000){
				//ion
				sprintf(strPDG,"%i",abs(pdgval));
				strncpy(strZ, &strPDG[3], 3);
				strncpy(strA, &strPDG[6], 3);
				strA[3]='\0';
				strZ[3]='\0';
				A=atoi(strA);
				Z=atoi(strZ);
				parttype =  G4IonTable::GetIonTable()->GetIon(Z, A, 0.);
				if(parttype){
					particleGun->SetParticleDefinition(parttype);
					particleGun->SetParticleCharge(0);
				} else {
					G4cerr << "skipping primary with PDG " << pdgval << G4endl;
					continue;
				}
			} else {
				//not ion
				parttype = particleTable->FindParticle(pdgval);
				if(parttype){
					particleGun->SetParticleDefinition(parttype);
				} else {
					G4cerr << "skipping primary with PDG " << pdgval << G4endl;
					continue;
				}
			}
			if(nuprimaryval==1){G4cout<<"genie primary ";} else {G4cout<<"genie secondary ";}
			G4cout<<keval/GeV<<" GeV ";
			if(parttype==0){G4cout<<"PDG: "<<pdgval;} else {G4cout<<parttype->GetParticleName();}
			G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
			G4VPhysicalVolume* primaryPV = theNavigator->LocateGlobalPointAndSetup(thevtx); 
			G4cout<<" in "<< primaryPV->GetName()<<G4endl;
			
			particleGun->SetParticleEnergy(keval);       // !!!kinetic!!! energy
			particleGun->SetParticlePosition(thevtx);
			//particleGun->SetParticleTime(vtxtval);     // set event time t=0 for prompt trigger.
			particleGun->SetParticleMomentumDirection(thepdir);
			particleGun->GeneratePrimaryVertex(anEvent); //anEvent provided by G4 when invoking the method
			//G4cout<<"Vertex set"<<G4endl;
		}
		
		// check if this is to be the last event and soft abort the run
		// (abort at the end of this event) if so
		inputEntry++;
		localEntry = inputdata->LoadTree(inputEntry);
		if(localEntry<0){
			// get the pointer to the UI manager
			G4UImanager* UI = G4UImanager::GetUIpointer();
			UI->ApplyCommand("/run/abort 1");	// abort after processing current event
			G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			G4cout<<"@#@#@#@#@#@#@#@#@#@ REACHED END OF INPUT FILE! #@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
			G4cout<<"@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#"<<G4endl;
		}
		
	}

  else if (useGPSEvt)
    {
      MyGPS->GeneratePrimaryVertex(anEvent);
      
      G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4double m        =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass(); // this is rest mass
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();
      
      G4ThreeVector dir  = P.unit();
      G4double E         = std::sqrt((P.dot(P))+(m*m));
      
      SetVtx(vtx);
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);
            
      double tote = anEvent->GetPrimaryVertex()->GetPrimary()->GetTotalEnergy();
      double ke = anEvent->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy();
      
      int nprimaryvertices = anEvent->GetNumberOfPrimaryVertex();
      G4cout<<"Generating event with "<<nprimaryvertices<<" primary vertices"<<G4endl;
      for(int evtvtxi=0; evtvtxi<nprimaryvertices; evtvtxi++){
        G4PrimaryVertex* thevertex = anEvent->GetPrimaryVertex(evtvtxi);
        int nparticles = thevertex->GetNumberOfParticle();
        G4cout<<"Primary vertex "<<evtvtxi<<" has "<<nparticles<<" particles"<<G4endl;
        vtx  = thevertex->GetPosition();
        
        for(int parti=0; parti<nparticles; parti++){
          G4PrimaryParticle* theprimary = thevertex->GetPrimary(parti);
          pdg  = theprimary->GetPDGcode();
          tote = theprimary->GetTotalEnergy();
          ke   = theprimary->GetKineticEnergy();
          dir  = theprimary->GetMomentum().unit();
          
          G4ParticleDefinition* parttype = particleTable->FindParticle(pdg);
          G4String particlename;
          particlename = (parttype!=0) ? (std::string(parttype->GetParticleName())) : (std::to_string(pdg));
          G4cout<<"Generating primary "<<particlename<<" with total energy "
                <<tote/MeV<<"MeV and kinetic energy "<<ke/MeV
                <<"MeV at ("<<vtx.x()<<", "<<vtx.y()<<", "<<vtx.z()<<") in direction ("
                <<dir.x()<<", "<<dir.y()<<", "<<dir.z()<<") "<<G4endl;
        }
      }
    }
}

void WCSimPrimaryGeneratorAction::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  if(useMulineEvt)
    wcopt->SetVectorFileName(vectorFileName);
  else
    wcopt->SetVectorFileName("");
  wcopt->SetGeneratorType(GetGeneratorTypeString());
}

G4String WCSimPrimaryGeneratorAction::GetGeneratorTypeString()
{
  if(useMulineEvt)
    return "muline";
  else if(useGunEvt)
    return "gun";
  else if(useGPSEvt)
    return "gps";
  else if(useLaserEvt)
    return "laser";
  else if(useBeamEvt)
    return "beam";
  return "";
}

// Returns a vector with the tokens
vector<string> tokenize( string separators, string input ) 
{
  std::size_t startToken = 0, endToken; // Pointers to the token pos
  vector<string> tokens;  // Vector to keep the tokens
  
  if( separators.size() > 0 && input.size() > 0 ) 
    {
    
      while( startToken < input.size() )
	{
	  // Find the start of token
	  startToken = input.find_first_not_of( separators, startToken );
      
	  // If found...
	  if( startToken != input.npos ) 
	    {
	      // Find end of token
	      endToken = input.find_first_of( separators, startToken );
	      if( endToken == input.npos )
		// If there was no end of token, assign it to the end of string
		endToken = input.size();
        
	      // Extract token
	      tokens.push_back( input.substr( startToken, endToken - startToken ) );
        
	      // Update startToken
	      startToken = endToken;
	    }
	}
    }
  
  return tokens;
}

void WCSimPrimaryGeneratorAction::LoadNewPrimaries(){
	if(primariesDirectory==""){
		G4cout<<"No primary files specified! Cannot generate beam events!"<<G4endl;
		assert(false);
	}
	//TODO: should also figure out how to check if loading the tchain is successful
	G4cout<<"loading new primary TChain from: "<<primariesDirectory<<G4endl;
	if(inputdata){ inputdata->ResetBranchAddresses(); delete inputdata; }
	inputdata = new TChain("tankflux");	// input is name of tree in contributing files
	inputdata->Add(primariesDirectory);
	inputdata->LoadTree(0);
	if(metadata){ metadata->ResetBranchAddresses(); delete metadata; }
	metadata = new TChain("tankmeta");
	metadata->Add(primariesDirectory);
	metadata->LoadTree(0);
#ifndef NO_GENIE
	if(geniedata){ geniedata->ResetBranchAddresses(); delete geniedata; }
	geniedata = new TChain("gtree");
	geniedata->Add(neutrinosDirectory);
	geniedata->LoadTree(0);
#endif
	
	inputdata->SetBranchAddress("run",&runbranchval,&runBranch);
	inputdata->SetBranchAddress("ntank",&ntankbranchval,&nTankBranch);
	inputdata->SetBranchAddress("nupdg",&nupdgval,&nupdgBranch);
	inputdata->SetBranchAddress("nuvtxx",&nuvtxxval,&nuvtxxBranch);
	inputdata->SetBranchAddress("nuvtxy",&nuvtxyval,&nuvtxyBranch);
	inputdata->SetBranchAddress("nuvtxz",&nuvtxzval,&nuvtxzBranch);
	inputdata->SetBranchAddress("nuvtxt",&nuvtxtval,&nuvtxtBranch);
	inputdata->SetBranchAddress("vtxvol",&nupvval,&nuPVBranch);
	inputdata->SetBranchAddress("vtxmat",&numatval,&nuvtxmatBranch);
	inputdata->SetBranchAddress("entry",&genieentrybranchval,&genieentryBranch);
	metadata->SetBranchAddress("inputFluxName",&nufluxfilenameval,&nufluxfilenameBranch);
#ifndef NO_GENIE
	geniedata->SetBranchAddress("gmcrec",&genierecordval,&genierecordBranch);
#else
	genierecordBranch=(TBranch*)1;
#endif
	
	vtxxBranch=inputdata->GetBranch("vx");
	vtxyBranch=inputdata->GetBranch("vy");
	vtxzBranch=inputdata->GetBranch("vz");
	vtxtBranch=inputdata->GetBranch("vt");
	pxBranch=inputdata->GetBranch("px");
	pyBranch=inputdata->GetBranch("py");
	pzBranch=inputdata->GetBranch("pz");
	EBranch=inputdata->GetBranch("E");
	KEBranch=inputdata->GetBranch("kE");
	pdgBranch=inputdata->GetBranch("pdgtank");
	nuprimaryBranch=inputdata->GetBranch("primary");
	
	if(runBranch==0||nTankBranch==0||vtxxBranch==0||vtxyBranch==0||vtxzBranch==0||vtxtBranch==0||pxBranch==0||pyBranch==0||pzBranch==0||EBranch==0||KEBranch==0||pdgBranch==0||nupdgBranch==0||nuvtxxBranch==0||nuvtxyBranch==0||nuvtxzBranch==0||nuvtxtBranch==0||nuPVBranch==0||nuvtxmatBranch==0||nuprimaryBranch==0 || nufluxfilenameBranch==0||genierecordBranch==0){
		G4cout<<"BRANCHES ARE ZOMBIES ARGH!"<<G4endl;
	}
	
	entriesInThisTree = runBranch->GetEntries();
	inputEntry=primariesoffset;
	primariesoffset=0;
	runBranch->GetEntry(inputEntry);
	G4cout<<"first run: "<<runbranchval<<G4endl;
	treeNumber=inputdata->GetTreeNumber();
	localEntry = inputdata->LoadTree(inputEntry);
	dirtFileName = inputdata->GetCurrentFile()->GetName(); // new tree, new file
	char* dirtFileNameAsChar = strdup(dirtFileName.c_str());
	dirtFileName = basename(dirtFileNameAsChar);
	
	loadNewPrimaries=false;
}

