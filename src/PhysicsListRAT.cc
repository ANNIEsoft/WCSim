#include <string>
#include <stdexcept>
#include <Shielding.hh>
#include <G4Cerenkov.hh>
#include <G4Scintillation.hh>
#include <G4RayleighScattering.hh>
#include <G4FastSimulationManagerProcess.hh>
#include <G4HadronicInteractionRegistry.hh>
#include <G4HadronicProcess.hh>
#include <G4HadronicProcessType.hh>
#include <G4Neutron.hh>
#include <G4NeutronHPThermalScattering.hh>
#include <G4NeutronHPThermalScatteringData.hh>
#include <G4OpticalPhoton.hh>
#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessVector.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4OpAbsorption.hh>
#include <G4OpRayleigh.hh>
#include <G4RunManager.hh>
#include <PhysicsListRAT.hh>

PhysicsListRAT::PhysicsListRAT() : Shielding() {
  //new PhysicsListMessenger(this);
}

PhysicsListRAT::~PhysicsListRAT() {}

void PhysicsListRAT::ConstructParticle() {
  Shielding::ConstructParticle();
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void PhysicsListRAT::ConstructProcess() {
  G4cout <<"PhysicsList:: ConstructProcess is called"<<G4endl;
  AddParameterization();
  Shielding::ConstructProcess();
  ConstructOpticalProcesses();
  EnableThermalNeutronScattering();
}

void PhysicsListRAT::EnableThermalNeutronScattering() {

  // Get the particle definition for neutrons
  G4ParticleDefinition* n_definition = G4Neutron::Definition();

  // Get the elastic scattering process used for neutrons
  G4HadronicProcess* n_elastic_process = NULL;
  G4ProcessVector* proc_vec = n_definition->GetProcessManager()
    ->GetProcessList();
  for (int i = 0; i < proc_vec->size(); i++) {
    G4VProcess* proc = proc_vec->operator[](i);
    if (proc->GetProcessSubType() == fHadronElastic
      && proc->GetProcessType() == fHadronic)
    {
      n_elastic_process = dynamic_cast<G4HadronicProcess*>(proc);
      break;
    }
  }
  if (!n_elastic_process) {
    std::cerr << "PhysicsList::EnableThermalNeutronScattering: "
      << " couldn't find hadron elastic scattering process.\n";
    throw std::runtime_error(std::string("Missing") + " hadron elastic"
      + " scattering process in PhysicsList");
  }

  // Get the "regular" neutron HP elastic scattering model
  G4HadronicInteraction* n_elastic_hp
    = G4HadronicInteractionRegistry::Instance()->FindModel("NeutronHPElastic");
  if (!n_elastic_hp) {
    std::cerr << "PhysicsList::EnableThermalNeutronScattering: "
      << " couldn't find high-precision neutron elastic"
      << " scattering interaction.\n";
    throw std::runtime_error(std::string("Missing") + " NeutronHPElastic"
      + " scattering interaction in PhysicsList");
  }

  // Exclude the thermal scattering region (below 4 eV) from the "regular"
  // elastic scattering model
  n_elastic_hp->SetMinEnergy(4.*eV);

  // Use the more detailed HP thermal scattering treatment below 4 eV instead
  n_elastic_process->RegisterMe(new G4NeutronHPThermalScattering);
  n_elastic_process->AddDataSet(new G4NeutronHPThermalScatteringData);
}

void PhysicsListRAT::ConstructOpticalProcesses() {
  // Cherenkov: default G4Cerenkov
  //
  // Request that Cerenkov photons be tracked first, before continuing
  // originating particle step.  Otherwise, we get too many secondaries!
  G4Cerenkov* cerenkovProcess = new G4Cerenkov();
  cerenkovProcess->SetTrackSecondariesFirst(true);
  cerenkovProcess->SetMaxNumPhotonsPerStep(1);
    cerenkovProcess->SetMaxBetaChangePerStep(10.0);

  //Replace RAT-specific classes with default Geant4 classes
  // Attenuation: RAT's GLG4OpAttenuation
  //
  // GLG4OpAttenuation implements Rayleigh scattering.
  G4OpAbsorption *absorptionProcess = new G4OpAbsorption();
 /* GLG4OpAttenuation* attenuationProcess = new GLG4OpAttenuation();

  // Scintillation: RAT's GLG4Scint
  //
  // Create three scintillation processes which depend on the mass.
  G4double protonMass = G4Proton::Proton()->GetPDGMass();
  G4double alphaMass = G4Alpha::Alpha()->GetPDGMass();
  GLG4Scint* defaultScintProcess = new GLG4Scint();
  GLG4Scint* nucleonScintProcess = new GLG4Scint("nucleon", 0.9 * protonMass);
  GLG4Scint* alphaScintProcess = new GLG4Scint("alpha", 0.9 * alphaMass);
  OpRayleigh* rayleighProcess = new OpRayleigh();*/
  // Optical boundary processes: default G4
  G4Scintillation *defaultScintProcess = new G4Scintillation();
  G4OpRayleigh *rayleighProcess = new G4OpRayleigh();
  G4OpBoundaryProcess* opBoundaryProcess = new G4OpBoundaryProcess();

  // Set verbosity
  if (verboseLevel > 0) {
    cerenkovProcess->DumpInfo();
    absorptionProcess->DumpInfo();
    //attenuationProcess->DumpInfo();
    defaultScintProcess->DumpInfo();
    //nucleonScintProcess->DumpInfo();
    //alphaScintProcess->DumpInfo();
    opBoundaryProcess->DumpInfo();
    rayleighProcess->DumpInfo();
  }

  cerenkovProcess->SetVerboseLevel(verboseLevel-1);
  absorptionProcess->SetVerboseLevel(verboseLevel-1);
  //attenuationProcess->SetVerboseLevel(verboseLevel-1);
  defaultScintProcess->SetVerboseLevel(verboseLevel-1);
  //nucleonScintProcess->SetVerboseLevel(verboseLevel-1);
  //alphaScintProcess->SetVerboseLevel(verboseLevel-1);
  opBoundaryProcess->SetVerboseLevel(verboseLevel-1);
  rayleighProcess->SetVerboseLevel(verboseLevel-1);
  // Apply processes to all particles where applicable
  theParticleIterator->reset();
  while((*theParticleIterator)()) {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (cerenkovProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(cerenkovProcess);
      pmanager->SetProcessOrdering(cerenkovProcess, idxPostStep);
    }
    if (particleName == "opticalphoton") {
      //pmanager->AddDiscreteProcess(attenuationProcess);
      pmanager->AddDiscreteProcess(opBoundaryProcess);
      pmanager->AddDiscreteProcess(rayleighProcess);
    }
  }
}

void PhysicsListRAT::AddParameterization() {
  G4FastSimulationManagerProcess* fastSimulationManagerProcess =
    new G4FastSimulationManagerProcess();
  theParticleIterator->reset();
  while((*theParticleIterator)()) {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (particle->GetParticleName() == "opticalphoton") {
      pmanager->AddProcess(fastSimulationManagerProcess, -1, -1, 1);
    }
  }
}

void PhysicsListRAT::SetCuts()
{
  //Just for testing!!!!! Remove
  /*
  SetCutValue(1*mm,"gamma");
  SetCutValue(1*mm,"e-");
  SetCutValue(1*mm,"e+");
  */
}


