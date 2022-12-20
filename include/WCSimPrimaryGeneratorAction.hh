#ifndef WCSimPrimaryGeneratorAction_h
#define WCSimPrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "TTree.h"
#include "TChain.h"
#include "TRandom3.h"
#include "jhfNtuple.h"
#include <vector>
#include <fstream>

// GENIE headers
#ifndef NO_GENIE
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/Ntuple/NtpMCTreeHeader.h"
#include "Framework/Ntuple/NtpMCEventRecord.h"
#include "Framework/Interaction/Interaction.h"
#endif
#include "WCSimRootOptions.hh"

class WCSimDetectorConstruction;
class G4ParticleGun;
class G4GeneralParticleSource;
class G4Event;
class WCSimPrimaryGeneratorMessenger;
class G4Generator;

class WCSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  WCSimPrimaryGeneratorAction(WCSimDetectorConstruction*);
  ~WCSimPrimaryGeneratorAction();
  
public:
  void GeneratePrimaries(G4Event* anEvent);

  // Normal gun setting calls these functions to fill jhfNtuple and Root tree
  void SetVtx(G4ThreeVector i)     { vtxs[0] = i; nvtxs = 1; };
  void SetBeamEnergy(G4double i, G4int n = 0)   { beamenergies[n] = i;};
  void SetBeamDir(G4ThreeVector i, G4int n = 0) { beamdirs[n] = i;};
  void SetBeamPDG(G4int i, G4int n = 0)         { beampdgs[n] = i;};
  void SetNvtxs(G4int i)     { nvtxs = i; };
  void SetVtxs(G4int i, G4ThreeVector v)     { vtxs[i] = v; };

  // These go with jhfNtuple
  G4int GetVecRecNumber(){return vecRecNumber;}
  G4int GetMode() {return mode;};
  G4int GetNvtxs() {return nvtxs;};
  G4int GetVtxVol(G4int n = 0) {return vtxsvol[n];};
  G4ThreeVector GetVtx(G4int n = 0) {return vtxs[n];}
  G4int GetNpar() {return npar;};
  G4int GetBeamPDG(G4int n = 0) {return beampdgs[n];};
  G4double GetBeamEnergy(G4int n = 0) {return beamenergies[n];};
  G4ThreeVector GetBeamDir(G4int n = 0) {return beamdirs[n];};
  G4int GetTargetPDG(G4int n = 0) {return targetpdgs[n];};
  G4double GetTargetEnergy(G4int n = 0) {return targetenergies[n];};
  G4ThreeVector GetTargetDir(G4int n = 0) {return targetdirs[n];};
  // ANNIE: trace upstream sources
  G4String  GetDirtFileName(){return dirtFileName;}
  G4String GetGenieFileName(){return genieFileName;}
  G4int    GetDirtEntryNum(){return dirtEntryNum;}
  G4int   GetGenieEntryNum(){return genieEntryNum;}

  // older ...
  G4double GetNuEnergy() {return nuEnergy;};
  G4double GetEnergy() {return energy;};
  G4double GetXPos() {return xPos;};
  G4double GetYPos() {return yPos;};
  G4double GetZPos() {return zPos;};
  G4double GetXDir() {return xDir;};
  G4double GetYDir() {return yDir;};
  G4double GetZDir() {return zDir;};

  G4String GetGeneratorTypeString();
  
  void SaveOptionsToOutput(WCSimRootOptions * wcopt);

private:
  WCSimDetectorConstruction*      myDetector;
  G4ParticleGun*                  particleGun;
  G4GeneralParticleSource*        MyGPS;  //T. Akiri: GPS to run Laser
  WCSimPrimaryGeneratorMessenger* messenger;
  
  // Variables set by the messenger
  G4bool   useMulineEvt;
  G4bool   useGunEvt;
  G4bool   useLaserEvt;  //T. Akiri: Laser flag
  G4bool   useBeamEvt;
  G4bool   useGPSEvt;
  std::fstream inputFile;
  G4String vectorFileName;
  G4bool   GenerateVertexInRock;
  
  G4String dirtFileName;
  G4String genieFileName;
  G4int dirtEntryNum;
  G4int genieEntryNum;

  // These go with jhfNtuple
  G4int mode;
  G4int nvtxs;
  G4int vtxsvol[MAX_N_PRIMARIES];
  G4ThreeVector vtxs[MAX_N_PRIMARIES];
  G4int npar;
  G4int beampdgs[MAX_N_PRIMARIES], targetpdgs[MAX_N_PRIMARIES];
  G4ThreeVector beamdirs[MAX_N_PRIMARIES], targetdirs[MAX_N_PRIMARIES];
  G4double beamenergies[MAX_N_PRIMARIES], targetenergies[MAX_N_PRIMARIES];
  G4int vecRecNumber;

  G4double nuEnergy;
  G4double energy;
  G4double xPos, yPos, zPos;
  G4double xDir, yDir, zDir;

  G4int    _counterRock; 
  G4int    _counterCublic; 
  
  TChain* inputdata;
  TChain* metadata;
  TChain* geniedata;
	
	Long64_t localEntry;
	Int_t inputEntry;
	Int_t entriesInThisTree;
	Int_t treeNumber;
	TBranch* runBranch=0, *vtxxBranch=0, *vtxyBranch=0, *vtxzBranch=0, *vtxtBranch=0, *pxBranch=0, *pyBranch=0, *pzBranch=0, *EBranch=0, *KEBranch=0, *pdgBranch=0, *nTankBranch=0, *nupdgBranch=0, *nuvtxxBranch=0, *nuvtxyBranch=0, *nuvtxzBranch=0, *nuvtxtBranch=0, *nuPVBranch=0, *nuvtxmatBranch=0, *nuprimaryBranch=0, *nufluxfilenameBranch=0, *genieentryBranch=0, *genierecordBranch=0;
	Int_t runbranchval, entrybranchval, ntankbranchval, nupdgval, genieentrybranchval, pdgval, nuprimaryval;
	Double_t vtxxval, vtxyval, vtxzval, vtxtval, pxval, pyval, pzval, eval, keval, nuvtxxval, nuvtxyval, nuvtxzval, nuvtxtval;
	Int_t* pdgbranchval=0, *nuprimarybranchval=0;
	Double_t* vtxxbranchval=0, *vtxybranchval=0, *vtxzbranchval=0, *vtxtbranchval=0, *pxbranchval=0, *pybranchval=0, *pzbranchval=0, *ebranchval=0, *kebranchval=0;
	Char_t nupvval[100];
	Char_t numatval[100];
	Char_t nufluxfilenameval[100];
	genie::NtpMCEventRecord* genierecordval;
	
	G4String primariesDirectory;
	G4String neutrinosDirectory;
	G4bool loadNewPrimaries;
	G4int primariesoffset;
	
public:

  inline void SetMulineEvtGenerator(G4bool choice) { useMulineEvt = choice; }
  inline G4bool IsUsingMulineEvtGenerator() { return useMulineEvt; }

  inline void SetGunEvtGenerator(G4bool choice) { useGunEvt = choice; }
  inline G4bool IsUsingGunEvtGenerator()  { return useGunEvt; }

  //T. Akiri: Addition of function for the laser flag
  inline void SetLaserEvtGenerator(G4bool choice) { useLaserEvt = choice; }
  inline G4bool IsUsingLaserEvtGenerator()  { return useLaserEvt; }
  
  inline void SetBeamEvtGenerator(G4bool choice) { useBeamEvt = choice; }
  inline G4bool IsUsingBeamEvtGenerator()  { return useBeamEvt; }

  inline void SetGPSEvtGenerator(G4bool choice) { useGPSEvt = choice; }
  inline G4bool IsUsingGPSEvtGenerator()  { return useGPSEvt; }

  inline void OpenVectorFile(G4String fileName) 
  {
    if ( inputFile.is_open() ) 
      inputFile.close();

    vectorFileName = fileName;
    inputFile.open(vectorFileName, std::fstream::in);

    if ( !inputFile.is_open() ) {
      G4cout << "Vector file " << vectorFileName << " not found" << G4endl;
      exit(-1);
    }
  }
  
  inline void SetPrimaryFilesDirectory(G4String directoryName) { primariesDirectory = directoryName; }
  inline void SetNeutrinoFilesDirectory(G4String directoryName) { neutrinosDirectory = directoryName; }
  inline void SetNewPrimariesFlag(G4bool flagin){ loadNewPrimaries=flagin; }
  void LoadNewPrimaries();
  void SetPrimariesOffset(G4int offset){ primariesoffset=offset; }
  inline G4bool IsGeneratingVertexInRock() { return GenerateVertexInRock; }
  inline void SetGenerateVertexInRock(G4bool choice) { GenerateVertexInRock = choice; }
  genie::NtpMCEventRecord* GetGenieRecord() { return genierecordval; }

};

#endif


