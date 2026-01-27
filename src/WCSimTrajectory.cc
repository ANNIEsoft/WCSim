#include "WCSimTrajectory.hh"

#include "G4TrajectoryPoint.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <sstream>

//G4Allocator<WCSimTrajectory> aTrajectoryAllocator;
G4Allocator<WCSimTrajectory> myTrajectoryAllocator;

WCSimTrajectory::WCSimTrajectory()
  :  positionRecord(0), fTrackID(0), fParentID(0), fPrimaryParentID(-1), fParentPdg(0),
     PDGEncoding( 0 ), PDGCharge(0.0), ParticleName(""),
     initialMomentum( G4ThreeVector() ), finalMomentum( G4ThreeVector() ),
     SaveIt(false),creatorProcess(""), 
     globalTime(0.0), globalTimeEnd(0.), thisStepsProcess(""),lastStepsProcess(""),
     tankExitPoint(G4ThreeVector()), momentumOnTankExit(G4ThreeVector())
{;}

WCSimTrajectory::WCSimTrajectory(const G4Track* aTrack)
{
  G4ParticleDefinition * fpParticleDefinition = aTrack->GetDefinition();
  ParticleName = fpParticleDefinition->GetParticleName();
  PDGCharge = fpParticleDefinition->GetPDGCharge();
  PDGEncoding = fpParticleDefinition->GetPDGEncoding();
  if(fpParticleDefinition==G4OpticalPhoton::OpticalPhotonDefinition()){PDGEncoding=100;}
  fTrackID = aTrack->GetTrackID();
  fParentID = aTrack->GetParentID();
  fPrimaryParentID = -1;  // Will be set later by WCSimTrackingAction
  initialMomentum = aTrack->GetMomentum();
  finalMomentum = aTrack->GetMomentum();
  globalTimeEnd = aTrack->GetGlobalTime();
  positionRecord = new TrajectoryPointContainer();
  // Following is for the first trajectory point
  positionRecord->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));

  stoppingPoint  = aTrack->GetPosition();
  stoppingVolume = aTrack->GetVolume();
  if ( aTrack->GetUserInformation() != 0 ){
    SaveIt = true;
  } else {
    SaveIt = false;
  }
  globalTime = aTrack->GetGlobalTime();
  if (aTrack->GetCreatorProcess() != 0 )
    {
      const G4VProcess* tempproc = aTrack->GetCreatorProcess();
      creatorProcess = tempproc->GetProcessName();
      thisStepsProcess=creatorProcess;
    }
  else 
    {
      creatorProcess = "";
      thisStepsProcess=creatorProcess;
    }
  lastStepsProcess="";
  momentumOnTankExit=G4ThreeVector();
  tankExitPoint = G4ThreeVector();
}

WCSimTrajectory::WCSimTrajectory(WCSimTrajectory & right):G4VTrajectory()
{
  ParticleName = right.ParticleName;
  PDGCharge = right.PDGCharge;
  PDGEncoding = right.PDGEncoding;
  fTrackID = right.fTrackID;
  fParentID = right.fParentID;
  fPrimaryParentID = right.fPrimaryParentID;
  fParentPdg = right.fParentPdg;
  initialMomentum = right.initialMomentum;
  finalMomentum = right.finalMomentum;
  positionRecord = new TrajectoryPointContainer();
  
  stoppingPoint  = right.stoppingPoint;
  stoppingVolume = right.stoppingVolume;
  SaveIt = right.SaveIt;
  creatorProcess = right.creatorProcess;
  thisStepsProcess = right.thisStepsProcess;
  lastStepsProcess = right.lastStepsProcess;
  momentumOnTankExit = right.momentumOnTankExit;
  tankExitPoint    = right.tankExitPoint;

  for(size_t i=0;i<right.positionRecord->size();i++)
  {
    G4TrajectoryPoint* rightPoint = (G4TrajectoryPoint*)((*(right.positionRecord))[i]);
    positionRecord->push_back(new G4TrajectoryPoint(*rightPoint));
  }
  globalTime = right.globalTime;
  globalTimeEnd = right.globalTimeEnd;
}

WCSimTrajectory::~WCSimTrajectory()
{
  //  positionRecord->clearAndDestroy();
  size_t i;
  for(i=0;i<positionRecord->size();i++){
    delete  (*positionRecord)[i];
  }
  positionRecord->clear();
  
  delete positionRecord;
}

void WCSimTrajectory::ShowTrajectory(std::ostream& os) const
{
  // Invoke the default implementation in G4VTrajectory...
  G4VTrajectory::ShowTrajectory(os);
  // ... or override with your own code here.
}

void WCSimTrajectory::DrawTrajectory(G4int i_mode) const
{
  // Invoke the default implementation in G4VTrajectory...
  // i_mode is removed in 4.10 - FDL
  //G4VTrajectory::DrawTrajectory(i_mode);
  G4VTrajectory::DrawTrajectory();
  // ... or override with your own code here.
}

const std::map<G4String,G4AttDef>* WCSimTrajectory::GetAttDefs() const
{
  G4bool isNew;
  std::map<G4String,G4AttDef>* store
    = G4AttDefStore::GetInstance("WCSimTrajectory",isNew);
  if (isNew) {
    
    G4String ID("ID");
    (*store)[ID] = G4AttDef(ID,"Track ID","Bookkeeping","","G4int");
    
    G4String PID("PID");
    (*store)[PID] = G4AttDef(PID,"Parent ID","Bookkeeping","","G4int");
    
    G4String PN("PN");
    (*store)[PN] = G4AttDef(PN,"Particle Name","Physics","","G4String");
    
    G4String Ch("Ch");
    (*store)[Ch] = G4AttDef(Ch,"Charge","Physics","","G4double");
    
    G4String PDG("PDG");
    (*store)[PDG] = G4AttDef(PDG,"PDG Encoding","Physics","","G4int");
    
    G4String IMom("IMom");
    (*store)[IMom] = G4AttDef(IMom, "Momentum of track at start of trajectory",
			      "Physics","","G4ThreeVector");
    
    G4String NTP("NTP");
    (*store)[NTP] = G4AttDef(NTP,"No. of points","Physics","","G4int");
    
  }
  return store;
}

std::vector<G4AttValue>* WCSimTrajectory::CreateAttValues() const
{
  char c[100];
  //std::ostrstream s(c,100);
  std::ostringstream s(c);
  
  std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
  
  s.seekp(std::ios::beg);
  s << fTrackID << std::ends;
  values->push_back(G4AttValue("ID",c,""));
  
  s.seekp(std::ios::beg);
  s << fParentID << std::ends;
  values->push_back(G4AttValue("PID",c,""));
  
  values->push_back(G4AttValue("PN",ParticleName,""));
  
  s.seekp(std::ios::beg);
  s << PDGCharge << std::ends;
  values->push_back(G4AttValue("Ch",c,""));

  s.seekp(std::ios::beg);
  s << PDGEncoding << std::ends;
  values->push_back(G4AttValue("PDG",c,""));

  s.seekp(std::ios::beg);
  s << G4BestUnit(initialMomentum,"Energy") << std::ends;
  values->push_back(G4AttValue("IMom",c,""));

  s.seekp(std::ios::beg);
  s << GetPointEntries() << std::ends;
  values->push_back(G4AttValue("NTP",c,""));

  return values;
}

void WCSimTrajectory::AppendStep(const G4Step* aStep)
{
  positionRecord->push_back( new G4TrajectoryPoint(aStep->GetPostStepPoint()->
						   GetPosition() ));
  lastStepsProcess=thisStepsProcess;
  G4String theProcess;
  if(aStep->GetPostStepPoint()->GetProcessDefinedStep() != 0){
    if(aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!=""){
      thisStepsProcess = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    } else {
      thisStepsProcess = "MysteryProcess";
    }
  } else {
    thisStepsProcess="Undefined";
  }
  
  // record the track properties if this step was the particle leaving the tank
  if(aStep->GetPostStepPoint()->GetStepStatus()==fGeomBoundary){  // if changing volumes
    G4VPhysicalVolume* thePostPV = aStep->GetPostStepPoint()->GetPhysicalVolume();
    //G4cout<<"Particle on boundary "<<aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()
    //      <<" -> "<<((thePostPV) ? thePostPV->GetName() : "VOID")<<G4endl;
    if(thePostPV==nullptr) return;                                // if exiting the world, nevermind
    G4String thePostPVname = thePostPV->GetName();
    if(thePostPVname=="Hall"){                                    // if entering the hall...
      G4String thePrePVname  = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
      if(thePrePVname=="waterTank"){                                 // from the tank...
        if(tankExitPoint==G4ThreeVector()){
          tankExitPoint = aStep->GetPostStepPoint()->GetPosition(); // record the properties
          momentumOnTankExit = aStep->GetTrack()->GetMomentum();
        } else {
          // why are we exiting the tank again?
          G4cerr<<"WCSimTrajectory::AppendStep with PrePV Tank and PostPV Hall called more than once?!"<<G4endl;
        }  // already have a recorded tank exit
      }    // else not exiting the tank
    }      // else not entering the hall
  }        // else not on a geometry boundary
  
}

G4ParticleDefinition* WCSimTrajectory::GetParticleDefinition()
{
  return (G4ParticleTable::GetParticleTable()->FindParticle(ParticleName));
}

void WCSimTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory) return;

  WCSimTrajectory* seco         = (WCSimTrajectory*)secondTrajectory;

  stoppingPoint  = seco->stoppingPoint;
  stoppingVolume = seco->stoppingVolume;
  if(seco->GetMomentumOnTankExit()!=G4ThreeVector()){
    if(momentumOnTankExit!=G4ThreeVector()){
      G4cerr<<"Mergeing two WCSimTrajectories that both have energyOnTankExit defined?!"<<G4endl;
    }
    momentumOnTankExit = seco->GetMomentumOnTankExit();
  }
  if(seco->GetTankExitPoint()!=G4ThreeVector()){
    if(tankExitPoint!=G4ThreeVector()){
       G4cerr<<"Merging two WCSimTrajectories that both have tankExitPoint defined?!"<<G4endl;
    }
    tankExitPoint = seco->GetTankExitPoint();
  }
  
  G4int ent = seco->GetPointEntries();
  for(G4int i=1;i<ent;i++) // initial point of the second trajectory should not be merged
  { 
    positionRecord->push_back((*(seco->positionRecord))[i]);
    //    positionRecord->push_back(seco->positionRecord->removeAt(1));
  }
  delete (*seco->positionRecord)[0];
  seco->positionRecord->clear();
}


