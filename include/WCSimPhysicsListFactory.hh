#ifndef WCSimPhysicsListFactory_h
#define WCSimPhysicsListFactory_h 1

#include "globals.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4UnitsTable.hh"
#include "G4OpticalPhysics.hh"

#include <G4FastSimulationManagerProcess.hh>
#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>

#include "WCSimPhysicsListFactoryMessenger.hh"
#include "WCSimRootOptions.hh"
#include "PhysicsListRAT.hh"

//class WCSimPhysicsList;

class WCSimPhysicsListFactory : public G4VModularPhysicsList
{
  public:
    WCSimPhysicsListFactory();
    ~WCSimPhysicsListFactory();

    void SetList(G4String newvalue);  // called by messenger
    void SetnCaptModel(G4String newvalue);  // called by messenger
    void SetgdComposition(G4String newvalue);  // called by messenger
    void SetgdCascade(G4String newvalue);  // called by messenger
    void InitializeList();

    //G4String GetPhysicsListName() {return PhysicsListName;}

    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();
    void AddParameterization();

    void SaveOptionsToOutput(WCSimRootOptions * wcopt);

    PhysicsListRAT *rat;

  private:

    G4String PhysicsListName;
    G4String ValidListsString;

    G4String nCaptModelChoice;
    G4String gdCompositionChoice;
    G4String gdCascadeChoice;

    WCSimPhysicsListFactoryMessenger* PhysicsMessenger;
    G4PhysListFactory* factory;

};

#endif
