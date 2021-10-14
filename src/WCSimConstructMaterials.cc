#include "WCSimDetectorConstruction.hh"
#include "WCSimTuningParameters.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


void WCSimDetectorConstruction::ConstructMaterials()
{
  //****Materials Definitions****

  G4double density;
  G4double a;

  //---Vacuum

  density     = CLHEP::universe_mean_density;              //from PhysicalConstants.h
  G4double pressure    = 1.e-19*CLHEP::pascal;
  G4double temperature = 0.1*CLHEP::kelvin;
  a = 1.01*CLHEP::g/CLHEP::mole;
  G4Material* Vacuum = 
    new G4Material("Vacuum", 1., a, density,
                   kStateGas,temperature,pressure);

  //---Water
  
  a = 1.01*CLHEP::g/CLHEP::mole;
  G4Element* elH 
    = new G4Element("Hydrogen","H", 1,a);
  
  a = 16.00*CLHEP::g/CLHEP::mole;
  G4Element* elO 
    = new G4Element("Oxygen","O", 8,a);
  
  density = 1.00*CLHEP::g/CLHEP::cm3;
  G4Material* Water 
    = new G4Material("Water",density,2);
  Water->AddElement(elH, 2);
  Water->AddElement(elO, 1);

 //---Gd doped Water

 a = 157.25*CLHEP::g/CLHEP::mole;
 G4Element* Gd
    = new G4Element("Gadolinium","Gd", 64,a);

 density = 1.00*CLHEP::g/CLHEP::cm3;
 G4Material* DopedWater
    = new G4Material("Doped Water",density,2);
 DopedWater->AddMaterial(Water,99.9*CLHEP::perCent);
 DopedWater->AddElement(Gd,0.1*CLHEP::perCent);

 //---Ice 
 
 density = 0.92*CLHEP::g/CLHEP::cm3;//Ice
 G4Material* Ice = new G4Material("Ice",density,2);
 Ice->AddElement(elH, 2);
 Ice->AddElement(elO, 1);

 //---Steel
  
  a= 12.01*CLHEP::g/CLHEP::mole;
  G4Element* elC 
    = new G4Element("Carbon","C", 6,a);
  
  a = 55.85*CLHEP::g/CLHEP::mole;
  G4Element* elFe
    = new G4Element("Iron","Fe", 26,a);
  
  density = 7.8*CLHEP::g/CLHEP::cm3;
  Steel = new G4Material("Steel",density,2);
  Steel->AddElement(elC, 1.*CLHEP::perCent);
  Steel->AddElement(elFe, 99.*CLHEP::perCent);
  
  //---Stainless steel 304L (simple example, particular alloy can be different)
  
  a = 51.9961*CLHEP::g/CLHEP::mole;
  G4Element* elCr = new G4Element("Chromium", "Cr", 24., a);
  
  a = 58.6934*CLHEP::g/CLHEP::mole;
  G4Element* elNi = new G4Element("Nickel", "Ni", 28., a);

  a = 54.938*CLHEP::g/CLHEP::mole;
  G4Element* elMn = new G4Element("Manganese", "Mn", 25., a); 

  a = 30.974*CLHEP::g/CLHEP::mole;
  G4Element* elP = new G4Element("Phosphore", "P", 15., a);

  a = 28.09*CLHEP::g/CLHEP::mole;
  G4Element* elSi = new G4Element("Silicon", "Si", 14., a); 

  a = 32.065*CLHEP::g/CLHEP::mole;
  G4Element* elS = new G4Element("Sulphur", "S", 16., a);

  density = 7.81*CLHEP::g/CLHEP::cm3;
  G4Material* StainlessSteel = new G4Material("StainlessSteel", density, 8);
 
  StainlessSteel->AddElement(elFe, 70.44*CLHEP::perCent);
  StainlessSteel->AddElement(elCr, 18*CLHEP::perCent);
  StainlessSteel->AddElement(elC,  0.08*CLHEP::perCent);
  StainlessSteel->AddElement(elNi, 8*CLHEP::perCent); 
  StainlessSteel->AddElement(elP, 0.45*CLHEP::perCent);
  StainlessSteel->AddElement(elSi,  1*CLHEP::perCent);
  StainlessSteel->AddElement(elMn, 2*CLHEP::perCent);
  StainlessSteel->AddElement(elS, 0.03*CLHEP::perCent);
  

  //---Solid Dry Ice

  density = 1.563*CLHEP::g/CLHEP::cm3;
  G4Material* DryIce = new G4Material("SolidDryIce", density, 2);
  DryIce->AddElement(elC, 1);
  DryIce->AddElement(elO, 2);

  //---PVC

  a = 35.45*CLHEP::g/CLHEP::mole;
  G4Element *elCl
    = new G4Element("Chlorine","Cl",17,a);

  density = 1.380*CLHEP::g/CLHEP::cm3;
  G4Material* PVC = new G4Material("PVC",density,3);
  PVC->AddElement(elH,4.84*CLHEP::perCent);
  PVC->AddElement(elC,38.44*CLHEP::perCent);
  PVC->AddElement(elCl,56.73*CLHEP::perCent);

  //---Air
  
  a = 14.01*CLHEP::g/CLHEP::mole;
  G4Element* elN 
    = new G4Element("Nitrogen","N", 7,a);
  
  density = 1.290*CLHEP::mg/CLHEP::cm3;
  Air = new G4Material("Air",density,2);
  Air->AddElement(elN, 70.*CLHEP::perCent);
  Air->AddElement(elO, 30.*CLHEP::perCent);
  
  //---Plastic
  
  density = 0.95*CLHEP::g/CLHEP::cm3;
  G4Material* Plastic
    = new G4Material("Plastic",density,2);
  Plastic->AddElement(elC, 1);
  Plastic->AddElement(elH, 2);

  //---Aluminum (element and material)

  a = 26.98*CLHEP::g/CLHEP::mole;
  G4Element* elAl = new G4Element("Aluminum", "Al", 13, a);  

  density = 2.7*CLHEP::g/CLHEP::cm3;
  G4Material* Aluminum
    = new G4Material("Aluminum",density,1);
  Aluminum->AddElement(elAl, 1);

  //---Black sheet

  density = 0.95*CLHEP::g/CLHEP::cm3;
  G4Material* Blacksheet
    = new G4Material("Blacksheet",density,2);
  Blacksheet->AddElement(elC, 1);
  Blacksheet->AddElement(elH, 2);

  //---Tyvek - jl145

  density = 0.38*CLHEP::g/CLHEP::cm3;  //cf. DuPont product handbook
  G4Material* Tyvek
    = new G4Material("Tyvek",density,2);
  Tyvek->AddElement(elC, 1);  //polyethylene
  Tyvek->AddElement(elH, 2);

  //---Glass
 
  density = 2.20*CLHEP::g/CLHEP::cm3;
  G4Material* SiO2 = new G4Material("SiO2",density,2);
  SiO2->AddElement(elSi, 1);
  SiO2->AddElement(elO , 2);

  a = 10.81*CLHEP::g/CLHEP::mole;
  G4Element* elB = new G4Element("Boron", "B", 5, a);  

  density = 2.46*CLHEP::g/CLHEP::cm3;
  G4Material* B2O3 = new G4Material("B2O3",density,2);
  B2O3->AddElement(elB, 2);
  B2O3->AddElement(elO, 3);

  a = 22.99*CLHEP::g/CLHEP::mole;
  G4Element* elNa = new G4Element("Sodium", "Na", 11, a);  

  density = 2.27*CLHEP::g/CLHEP::cm3;
  G4Material* Na2O = new G4Material("Na2O",density,2);
  Na2O->AddElement(elNa, 2);
  Na2O->AddElement(elO, 1);

  density = 4.00*CLHEP::g/CLHEP::cm3;
  G4Material* Al2O3 = new G4Material("Al2O3",density,2);
  Al2O3->AddElement(elAl, 2);
  Al2O3->AddElement(elO, 3);

//   G4Material* blackAcryl
//     = new G4Material("blackAcryl", density, 3);
//   blackAcryl -> AddElement(elH, 6);
//   blackAcryl -> AddElement(elC, 4);
//   blackAcryl -> AddElement(elO, 2);

  density = 2.23*CLHEP::g/CLHEP::cm3;
  G4Material* Glass
    = new G4Material("Glass",density,4);
  //G4Material* Glass
  //= new G4Material("Glass",density,8);  //Put in 8 materials later
  
  Glass->AddMaterial(SiO2, 80.6*CLHEP::perCent);
  Glass->AddMaterial(B2O3, 13.0*CLHEP::perCent);
  Glass->AddMaterial(Na2O, 4.0*CLHEP::perCent);
  Glass->AddMaterial(Al2O3, 2.4*CLHEP::perCent);
  //Glass->AddMaterial(Al2O3, 2.3*CLHEP::perCent);  
  //Put in 2.3 percent if the other 4 materials = 0.1 percent

  G4Material *GlassR7081 = new G4Material("GlassR7081",density,4);
  GlassR7081->AddMaterial(SiO2, 80.6*CLHEP::perCent);
  GlassR7081->AddMaterial(B2O3, 13.0*CLHEP::perCent);
  GlassR7081->AddMaterial(Na2O, 4.0*CLHEP::perCent);
  GlassR7081->AddMaterial(Al2O3, 2.4*CLHEP::perCent);

  //---Rock
 
  //  a = 16.00*CLHEP::g/CLHEP::mole;  G4Element* elO  = new G4Element("Oxygen","O", 8,a);
  //  a = 28.09*CLHEP::g/CLHEP::mole;  G4Element* elSi = new G4Element("Silicon", "Si", 14., a); 
  //  a = 26.98*CLHEP::g/CLHEP::mole;  G4Element* elAl = new G4Element("Aluminum", "Al", 13, a);  
  //  a = 55.85*CLHEP::g/CLHEP::mole;  G4Element* elFe = new G4Element("Iron","Fe", 26,a);
  a = 40.08*CLHEP::g/CLHEP::mole;  G4Element* elCa = new G4Element("Calcium","Ca", 20,a);
  //  a = 22.99*CLHEP::g/CLHEP::mole;  G4Element* elNa = new G4Element("Sodium", "Na", 11, a);  
  a = 39.10*CLHEP::g/CLHEP::mole;  G4Element* elK = new G4Element("Potassium","K", 19,a);
  a = 24.30*CLHEP::g/CLHEP::mole;  G4Element* elMg = new G4Element("Magnesium","Mg",12,a);
 
  density = 2.7*CLHEP::g/CLHEP::cm3; 
  G4Material* Rock = new G4Material("Rock", density, 8);
  
  //From Daya-Bay 
  Rock->AddElement(elO,  48.50*CLHEP::perCent);
  Rock->AddElement(elSi, 34.30*CLHEP::perCent);
  Rock->AddElement(elAl,  8.00*CLHEP::perCent);
  Rock->AddElement(elFe,  2.00*CLHEP::perCent);
  Rock->AddElement(elCa,  0.20*CLHEP::perCent);
  Rock->AddElement(elNa,  2.40*CLHEP::perCent);
  Rock->AddElement(elK,   4.50*CLHEP::perCent);
  Rock->AddElement(elMg,  0.10*CLHEP::perCent);



  // Potential alternative material definitions for HK
  // Get nist material manager
  //G4NistManager* nist = G4NistManager::Instance();
  //nist->FindOrBuildMaterial("G4_AIR");
  //nist->FindOrBuildMaterial("G4_WATER");
  //nist->FindOrBuildMaterial("G4_Galactic");
  //nist->FindOrBuildMaterial("G4_Pyrex_Glass");
  //nist->FindOrBuildMaterial("G4_POLYETHYLENE");

// **** ANNIE additions ***
  G4int z,n;
  G4int nelements;
  G4double fracMass;
// **** end ANNIE additions ***

// **** NCV additions *****
  // Definition of gadolinium
  // natural gadolinium
  G4Isotope* Gd152 = new G4Isotope("Gd152", z= 64, n= 152, a= 152.0*g/mole);
  G4Isotope* Gd154 = new G4Isotope("Gd154", z= 64, n= 154, a= 154.0*g/mole);
  G4Isotope* Gd155 = new G4Isotope("Gd155", z= 64, n= 155, a= 155.0*g/mole);
  G4Isotope* Gd156 = new G4Isotope("Gd156", z= 64, n= 156, a= 156.0*g/mole);
  G4Isotope* Gd157 = new G4Isotope("Gd157", z= 64, n= 157, a= 157.0*g/mole);
  G4Isotope* Gd158 = new G4Isotope("Gd158", z= 64, n= 158, a= 158.0*g/mole);
  G4Isotope* Gd160 = new G4Isotope("Gd160", z= 64, n= 160, a= 160.0*g/mole);

  /*G4Element**/ Gd = new G4Element("Gadolinium", "Gd",7);
  Gd->AddIsotope(Gd152,  0.2*perCent); // beware : it is abundance, not fraction mass
  Gd->AddIsotope(Gd154,  2.2*perCent);
  Gd->AddIsotope(Gd155, 14.9*perCent);
  Gd->AddIsotope(Gd156, 20.6*perCent);
  Gd->AddIsotope(Gd157, 15.7*perCent);
  Gd->AddIsotope(Gd158, 24.7*perCent);
  Gd->AddIsotope(Gd160, 21.7*perCent);
  
  // --- Mineral Oil  (CH2)n ------
  density = 0.77*g/cm3;
  nelements = 2;
  G4Material* mineral_oil 
   = new G4Material("MineralOil", density, nelements);
  mineral_oil -> AddElement(elC, 1);
  mineral_oil -> AddElement(elH, 2);
  
// --- Pseudo-cumene (C9 H12) also called 1,2,4-Trimethybenzene
  density = 0.8758 *g/cm3;  // at T=20 deg C
  nelements=2;
  G4Material* pseudocumene 
    = new G4Material("pseudocumene",density,nelements);
  pseudocumene->AddElement(elC, 9);
  pseudocumene->AddElement(elH, 12); 
   
// --- PPO (C15 H11 N 0) -- also called DPO, 2,5-diphenyloxazole
  density = 1.0 *g/cm3;  // ??? at T=?
  G4Material* PPO 
     = new G4Material("PPO",density,nelements=4);
  PPO->AddElement(elC, 15);
  PPO->AddElement(elH, 11);
  PPO->AddElement(elN, 1);
  PPO->AddElement(elO, 1);
   
// --- Bis-MSB ------
  density= 1.3*g/cm3;
  G4Material* BisMSB 
    = new G4Material("Bis-MSB",density, nelements= 2);  // density unknown
  BisMSB -> AddElement(elC, 24);
  BisMSB -> AddElement(elH, 2);
  
// --- NCVliquid - EJ-335 (0.25% Gd), composition taken from the simulation of the Nucifer experiment ---
// --- Concentrations (IN VOLUME): 60% mineral oil, 40% pseudocumene, 6e-3 g/cm3 of PPO, 2e-5 g/cm3 of bis-MSB
// --- Mail from Eljen -> 45% oil, 45% PC, the rest is unknown 
// --- These volume concentrations need to be multiplied by the density to be handled as mass concentrations 
  density = 0.89*g/cm3;
  G4double PPO_fraction= 6.*g/(1e3*cm3*density);
  G4double BisMSB_fraction= 2.*g/(1e5*cm3*density);
  G4double Gd_fraction= 0.25*perCent;
  G4Material* NCVliquid = new G4Material("NCVliquid",density,5);
  NCVliquid->AddMaterial(mineral_oil, 60.*perCent / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) ); G4cout << 60.*perCent / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) << G4endl;
  NCVliquid->AddMaterial(pseudocumene, 40.*perCent / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) ); G4cout << 40.*perCent / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) << G4endl;
  NCVliquid->AddMaterial(PPO, PPO_fraction / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) ); G4cout << PPO_fraction / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) << G4endl;
  NCVliquid->AddMaterial(BisMSB, BisMSB_fraction / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) ); G4cout << BisMSB_fraction / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) << G4endl;
  NCVliquid->AddElement(Gd, Gd_fraction / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) ); G4cout << Gd_fraction / (1.0 + PPO_fraction + BisMSB_fraction + Gd_fraction) << G4endl;
  
   //               H H 
  // --- Acrylic  -C-C- --------------------
  //               H COOCH3
  density = 1.14*g/cm3;
  nelements = 3;
  G4Material* Acrylic = new G4Material("Acrylic", density, nelements);
  Acrylic -> AddElement(elH, 6);
  Acrylic -> AddElement(elC, 4);
  Acrylic -> AddElement(elO, 2);

// **** END NCV additions ****

  //*****************SciBooNE integration
  
  G4NistManager* man = G4NistManager::Instance();
  man->SetVerbose(1);
  
  // Polystyrene scintillator for SciBar (scintillator, WLS fiber) and MRD
  //G4Material* Scinti = man->FindOrBuildMaterial("G4_POLYSTYRENE");	// not found?
  
  density= 1.021 *g/cm3;
  Scinti = new G4Material("Scinti", density, 2);
  Scinti-> AddElement(elC, 8);
  Scinti-> AddElement(elH, 8);
  
  
  // Iron for MRD
  a= 55.847 *g/mole;
  density= 7.841 *g/cm3;
  MRDIron = new G4Material("Iron", 26., a, density);
  
  // Aluminum for MRD support structure
  Al  = man->FindOrBuildMaterial("G4_Al");
  
  // Mylar for MRD scintillator paddle cladding
  // from http://hypernews.slac.stanford.edu/HyperNews/geant4/get/AUX/2011/06/01/03.27-76314-2DetectorConstruction.txt
  G4Material* Mylar = man->FindOrBuildMaterial("G4_MYLAR");
  /*
  density = 1.397*g/cm3;
  G4Material* Mylar = new G4Material("Mylar", density, 3);
  Mylar->AddElement(elO,2);
  Mylar->AddElement(elC,5);
  Mylar->AddElement(elH,4);
  */
  
  G4MaterialPropertiesTable* mptMylar = new G4MaterialPropertiesTable();
  G4double AbsorptionLengthMylar = 100.*cm;
  G4double refractiveIndexMylar = 1.640;
  mptMylar->AddConstProperty("RINDEX",refractiveIndexMylar);
  mptMylar->AddConstProperty("ABSLENGTH",AbsorptionLengthMylar);
  Mylar->SetMaterialPropertiesTable(mptMylar);
  
  //****************/SciBooNE integration

//--------------------------------------------------
// Silicone (Template for Optical Grease)
//--------------------------------------------------
   
   std::vector<G4int> natoms;
   std::vector<G4String> elements;
   
   elements.push_back("C");  natoms.push_back(2);
   elements.push_back("H");  natoms.push_back(6);
   
   density = 1.060*g/cm3;
   man->SetVerbose(0); 	//suppress unnecessary warning from the following line
   G4Material* Silicone = man->ConstructNewMaterial("Silicone", elements, natoms, density);
   elements.clear();
   natoms.clear();

// -------------------------------------------------------------
// Generate & Add Material Properties Table
// -------------------------------------------------------------


  const G4int NUMENTRIES = 32;
 
  G4double PPCKOV[NUMENTRIES] =
    { 2.034E-9*CLHEP::GeV, 2.068E-9*CLHEP::GeV, 2.103E-9*CLHEP::GeV, 2.139E-9*CLHEP::GeV,
      2.177E-9*CLHEP::GeV, 2.216E-9*CLHEP::GeV, 2.256E-9*CLHEP::GeV, 2.298E-9*CLHEP::GeV,
      2.341E-9*CLHEP::GeV, 2.386E-9*CLHEP::GeV, 2.433E-9*CLHEP::GeV, 2.481E-9*CLHEP::GeV,
      2.532E-9*CLHEP::GeV, 2.585E-9*CLHEP::GeV, 2.640E-9*CLHEP::GeV, 2.697E-9*CLHEP::GeV,
      2.757E-9*CLHEP::GeV, 2.820E-9*CLHEP::GeV, 2.885E-9*CLHEP::GeV, 2.954E-9*CLHEP::GeV,
      3.026E-9*CLHEP::GeV, 3.102E-9*CLHEP::GeV, 3.181E-9*CLHEP::GeV, 3.265E-9*CLHEP::GeV,
      3.353E-9*CLHEP::GeV, 3.446E-9*CLHEP::GeV, 3.545E-9*CLHEP::GeV, 3.649E-9*CLHEP::GeV,
      3.760E-9*CLHEP::GeV, 3.877E-9*CLHEP::GeV, 4.002E-9*CLHEP::GeV, 4.136E-9*CLHEP::GeV };


  // default values
  /*
  G4double RINDEX1[NUMENTRIES] =
    { 1.3435, 1.344,  1.3445, 1.345,  1.3455,
      1.346,  1.3465, 1.347,  1.3475, 1.348,
      1.3485, 1.3492, 1.35,   1.3505, 1.351,
      1.3518, 1.3522, 1.3530, 1.3535, 1.354,
      1.3545, 1.355,  1.3555, 1.356,  1.3568,
      1.3572, 1.358,  1.3585, 1.359,  1.3595,
      1.36,   1.3608};
  */
  
  // M Fechner,  values from refsg.F in apdetsim
  /*  G4double RINDEX1[NUMENTRIES] = 
    { 1.33332, 1.333364, 1.333396, 1.3343, 1.33465,
      1.33502, 1.3354, 1.33579, 1.3362, 1.33663, 1.33709,
      1.33756, 1.33806, 1.3386, 1.33915, 1.33974,
      1.34037, 1.34105, 1.34176, 1.34253, 1.34336,
      1.34425, 1.34521, 1.34626, 1.3474, 1.34864,
      1.35002, 1.35153, 1.35321, 1.35507, 1.35717, 1.35955 };
  */

   //From SFDETSIM water absorption
   const G4int NUMENTRIES_water=60;

   G4double ENERGY_water[NUMENTRIES_water] =
     { 1.56962e-09*CLHEP::GeV, 1.58974e-09*CLHEP::GeV, 1.61039e-09*CLHEP::GeV, 1.63157e-09*CLHEP::GeV, 
       1.65333e-09*CLHEP::GeV, 1.67567e-09*CLHEP::GeV, 1.69863e-09*CLHEP::GeV, 1.72222e-09*CLHEP::GeV, 
       1.74647e-09*CLHEP::GeV, 1.77142e-09*CLHEP::GeV,1.7971e-09*CLHEP::GeV, 1.82352e-09*CLHEP::GeV, 
       1.85074e-09*CLHEP::GeV, 1.87878e-09*CLHEP::GeV, 1.90769e-09*CLHEP::GeV, 1.93749e-09*CLHEP::GeV, 
       1.96825e-09*CLHEP::GeV, 1.99999e-09*CLHEP::GeV, 2.03278e-09*CLHEP::GeV, 2.06666e-09*CLHEP::GeV,
       2.10169e-09*CLHEP::GeV, 2.13793e-09*CLHEP::GeV, 2.17543e-09*CLHEP::GeV, 2.21428e-09*CLHEP::GeV, 
       2.25454e-09*CLHEP::GeV, 2.29629e-09*CLHEP::GeV, 2.33962e-09*CLHEP::GeV, 2.38461e-09*CLHEP::GeV, 
       2.43137e-09*CLHEP::GeV, 2.47999e-09*CLHEP::GeV, 2.53061e-09*CLHEP::GeV, 2.58333e-09*CLHEP::GeV, 
       2.63829e-09*CLHEP::GeV, 2.69565e-09*CLHEP::GeV, 2.75555e-09*CLHEP::GeV, 2.81817e-09*CLHEP::GeV, 
       2.88371e-09*CLHEP::GeV, 2.95237e-09*CLHEP::GeV, 3.02438e-09*CLHEP::GeV, 3.09999e-09*CLHEP::GeV,
       3.17948e-09*CLHEP::GeV, 3.26315e-09*CLHEP::GeV, 3.35134e-09*CLHEP::GeV, 3.44444e-09*CLHEP::GeV, 
       3.54285e-09*CLHEP::GeV, 3.64705e-09*CLHEP::GeV, 3.75757e-09*CLHEP::GeV, 3.87499e-09*CLHEP::GeV, 
       3.99999e-09*CLHEP::GeV, 4.13332e-09*CLHEP::GeV, 4.27585e-09*CLHEP::GeV, 4.42856e-09*CLHEP::GeV, 
       4.59258e-09*CLHEP::GeV, 4.76922e-09*CLHEP::GeV, 4.95999e-09*CLHEP::GeV, 5.16665e-09*CLHEP::GeV, 
       5.39129e-09*CLHEP::GeV, 5.63635e-09*CLHEP::GeV, 5.90475e-09*CLHEP::GeV, 6.19998e-09*CLHEP::GeV };



      // Air
   G4double RINDEX_air[NUMENTRIES_water] = 
     { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
       

   // M Fechner : new ; define the water refraction index using refsg.F 
   //from skdetsim using the whole range.   
    G4double RINDEX1[NUMENTRIES_water] = 
     {1.32885, 1.32906, 1.32927, 1.32948, 1.3297, 1.32992, 1.33014, 
      1.33037, 1.3306, 1.33084, 1.33109, 1.33134, 1.3316, 1.33186, 1.33213,
      1.33241, 1.3327, 1.33299, 1.33329, 1.33361, 1.33393, 1.33427, 1.33462,
      1.33498, 1.33536, 1.33576, 1.33617, 1.3366, 1.33705, 1.33753, 1.33803,
      1.33855, 1.33911, 1.3397, 1.34033, 1.341, 1.34172, 1.34248, 1.34331,
      1.34419, 1.34515, 1.3462, 1.34733, 1.34858, 1.34994, 1.35145, 1.35312,
      1.35498, 1.35707, 1.35943, 1.36211, 1.36518, 1.36872, 1.37287, 1.37776,
      1.38362, 1.39074, 1.39956, 1.41075, 1.42535};


    G4double ABWFF = 1.0;

    // Get from the tuning parameters
    ABWFF = WCSimTuningParams->GetAbwff();

    //T. Akiri: Values from Skdetsim 
    G4double ABSORPTION_water[NUMENTRIES_water] =
      {
        16.1419*CLHEP::cm*ABWFF,  18.278*CLHEP::cm*ABWFF, 21.0657*CLHEP::cm*ABWFF, 24.8568*CLHEP::cm*ABWFF, 30.3117*CLHEP::cm*ABWFF, 
	38.8341*CLHEP::cm*ABWFF, 54.0231*CLHEP::cm*ABWFF, 81.2306*CLHEP::cm*ABWFF, 120.909*CLHEP::cm*ABWFF, 160.238*CLHEP::cm*ABWFF, 
	193.771*CLHEP::cm*ABWFF, 215.017*CLHEP::cm*ABWFF, 227.747*CLHEP::cm*ABWFF,  243.85*CLHEP::cm*ABWFF, 294.036*CLHEP::cm*ABWFF, 
	321.647*CLHEP::cm*ABWFF,  342.81*CLHEP::cm*ABWFF, 362.827*CLHEP::cm*ABWFF, 378.041*CLHEP::cm*ABWFF, 449.378*CLHEP::cm*ABWFF,
        739.434*CLHEP::cm*ABWFF, 1114.23*CLHEP::cm*ABWFF, 1435.56*CLHEP::cm*ABWFF, 1611.06*CLHEP::cm*ABWFF, 1764.18*CLHEP::cm*ABWFF, 
	2100.95*CLHEP::cm*ABWFF,  2292.9*CLHEP::cm*ABWFF, 2431.33*CLHEP::cm*ABWFF,  3053.6*CLHEP::cm*ABWFF, 4838.23*CLHEP::cm*ABWFF, 
	6539.65*CLHEP::cm*ABWFF, 7682.63*CLHEP::cm*ABWFF, 9137.28*CLHEP::cm*ABWFF, 12220.9*CLHEP::cm*ABWFF, 15270.7*CLHEP::cm*ABWFF, 
	19051.5*CLHEP::cm*ABWFF, 23671.3*CLHEP::cm*ABWFF, 29191.1*CLHEP::cm*ABWFF, 35567.9*CLHEP::cm*ABWFF,   42583*CLHEP::cm*ABWFF,
        49779.6*CLHEP::cm*ABWFF, 56465.3*CLHEP::cm*ABWFF,   61830*CLHEP::cm*ABWFF, 65174.6*CLHEP::cm*ABWFF, 66143.7*CLHEP::cm*ABWFF,   
	  64820*CLHEP::cm*ABWFF,   61635*CLHEP::cm*ABWFF, 57176.2*CLHEP::cm*ABWFF, 52012.1*CLHEP::cm*ABWFF, 46595.7*CLHEP::cm*ABWFF, 
	41242.1*CLHEP::cm*ABWFF, 36146.3*CLHEP::cm*ABWFF, 31415.4*CLHEP::cm*ABWFF, 27097.8*CLHEP::cm*ABWFF, 23205.7*CLHEP::cm*ABWFF, 
	19730.3*CLHEP::cm*ABWFF, 16651.6*CLHEP::cm*ABWFF, 13943.6*CLHEP::cm*ABWFF, 11578.1*CLHEP::cm*ABWFF, 9526.13*CLHEP::cm*ABWFF};

    //Xin Qian: proposed value for absorption length
    // G4double ABSORPTION_water[NUMENTRIES_water] =
//       {22.8154*CLHEP::cm*ABWFF, 28.6144*CLHEP::cm*ABWFF, 35.9923*CLHEP::cm*ABWFF, 45.4086*CLHEP::cm*ABWFF, 57.4650*CLHEP::cm*ABWFF,
//        72.9526*CLHEP::cm*ABWFF, 75*CLHEP::cm*ABWFF,      81.2317*CLHEP::cm*ABWFF, 120.901*CLHEP::cm*ABWFF, 160.243*CLHEP::cm*ABWFF,
//        193.797*CLHEP::cm*ABWFF, 215.045*CLHEP::cm*ABWFF, 227.786*CLHEP::cm*ABWFF, 243.893*CLHEP::cm*ABWFF, 294.113*CLHEP::cm*ABWFF,
//        321.735*CLHEP::cm*ABWFF, 342.931*CLHEP::cm*ABWFF, 362.967*CLHEP::cm*ABWFF, 378.212*CLHEP::cm*ABWFF, 449.602*CLHEP::cm*ABWFF,
//        740.143*CLHEP::cm*ABWFF, 1116.06*CLHEP::cm*ABWFF, 1438.78*CLHEP::cm*ABWFF, 1615.48*CLHEP::cm*ABWFF, 1769.86*CLHEP::cm*ABWFF,
//        2109.67*CLHEP::cm*ABWFF, 2304.13*CLHEP::cm*ABWFF, 2444.97*CLHEP::cm*ABWFF, 3076.83*CLHEP::cm*ABWFF, 4901.5*CLHEP::cm*ABWFF,
//        6666.57*CLHEP::cm*ABWFF, 7873.95*CLHEP::cm*ABWFF, 9433.81*CLHEP::cm*ABWFF, 10214.5*CLHEP::cm*ABWFF, 10845.8*CLHEP::cm*ABWFF,
//        15746.9*CLHEP::cm*ABWFF, 20201.8*CLHEP::cm*ABWFF, 22025.8*CLHEP::cm*ABWFF, 21142.2*CLHEP::cm*ABWFF, 15083.9*CLHEP::cm*ABWFF,
//        11751*CLHEP::cm*ABWFF,   8795.34*CLHEP::cm*ABWFF, 8741.23*CLHEP::cm*ABWFF, 7102.37*CLHEP::cm*ABWFF, 6060.68*CLHEP::cm*ABWFF,
//        4498.56*CLHEP::cm*ABWFF, 3039.56*CLHEP::cm*ABWFF, 2232.2*CLHEP::cm*ABWFF,  1938*CLHEP::cm*ABWFF,    1811.58*CLHEP::cm*ABWFF,
//        1610.32*CLHEP::cm*ABWFF, 1338.7*CLHEP::cm*ABWFF,  1095.3*CLHEP::cm*ABWFF,  977.525*CLHEP::cm*ABWFF, 965.258*CLHEP::cm*ABWFF,
//        1082.86*CLHEP::cm*ABWFF, 876.434*CLHEP::cm*ABWFF, 633.723*CLHEP::cm*ABWFF, 389.87*CLHEP::cm*ABWFF,  142.011*CLHEP::cm*ABWFF
//       };

    /*G4double ABSORPTION_water[NUMENTRIES_water] = //old
     { 22.8154*CLHEP::cm*ABWFF, 28.6144*CLHEP::cm*ABWFF, 35.9923*CLHEP::cm*ABWFF, 45.4086*CLHEP::cm*ABWFF, 57.4650*CLHEP::cm*ABWFF,
       72.9526*CLHEP::cm*ABWFF, 92.9156*CLHEP::cm*ABWFF, 118.737*CLHEP::cm*ABWFF, 152.255*CLHEP::cm*ABWFF, 195.925*CLHEP::cm*ABWFF,
       202.429*CLHEP::cm*ABWFF, 224.719*CLHEP::cm*ABWFF, 236.407*CLHEP::cm*ABWFF, 245.700*CLHEP::cm*ABWFF, 289.017*CLHEP::cm*ABWFF,
       305.810*CLHEP::cm*ABWFF, 316.456*CLHEP::cm*ABWFF, 326.797*CLHEP::cm*ABWFF, 347.222*CLHEP::cm*ABWFF, 414.938*CLHEP::cm*ABWFF,
       636.943*CLHEP::cm*ABWFF, 934.579*CLHEP::cm*ABWFF, 1245.33*CLHEP::cm*ABWFF, 1402.52*CLHEP::cm*ABWFF, 1550.39*CLHEP::cm*ABWFF,
       1745.20*CLHEP::cm*ABWFF, 1883.24*CLHEP::cm*ABWFF, 2016.13*CLHEP::cm*ABWFF, 2442.18*CLHEP::cm*ABWFF, 3831.28*CLHEP::cm*ABWFF,
       4652.89*CLHEP::cm*ABWFF, 5577.04*CLHEP::cm*ABWFF, 6567.08*CLHEP::cm*ABWFF, 7559.88*CLHEP::cm*ABWFF, 8470.06*CLHEP::cm*ABWFF,
       9205.54*CLHEP::cm*ABWFF, 9690.95*CLHEP::cm*ABWFF, 9888.36*CLHEP::cm*ABWFF, 9804.50*CLHEP::cm*ABWFF, 9482.17*CLHEP::cm*ABWFF,
       8982.77*CLHEP::cm*ABWFF, 8369.39*CLHEP::cm*ABWFF, 7680.31*CLHEP::cm*ABWFF, 6902.11*CLHEP::cm*ABWFF, 6183.84*CLHEP::cm*ABWFF,
       5522.27*CLHEP::cm*ABWFF, 4914.33*CLHEP::cm*ABWFF, 4357.09*CLHEP::cm*ABWFF, 3847.72*CLHEP::cm*ABWFF, 3383.51*CLHEP::cm*ABWFF,
       2961.81*CLHEP::cm*ABWFF, 2580.08*CLHEP::cm*ABWFF, 2235.83*CLHEP::cm*ABWFF, 1926.66*CLHEP::cm*ABWFF, 1650.21*CLHEP::cm*ABWFF,
       1404.21*CLHEP::cm*ABWFF, 1186.44*CLHEP::cm*ABWFF, 994.742*CLHEP::cm*ABWFF, 827.027*CLHEP::cm*ABWFF, 681.278*CLHEP::cm*ABWFF};
    */
    
    /*
   G4double ABSORPTION_water[NUMENTRIES_water] = //new
     {25.3504*CLHEP::cm, 31.7938*CLHEP::cm, 39.9915*CLHEP::cm, 50.454*CLHEP::cm, 63.85*CLHEP::cm, 
      81.0584*CLHEP::cm, 103.24*CLHEP::cm, 131.93*CLHEP::cm, 169.172*CLHEP::cm, 217.694*CLHEP::cm, 
      224.921*CLHEP::cm, 249.688*CLHEP::cm, 262.674*CLHEP::cm, 273*CLHEP::cm, 321.13*CLHEP::cm, 339.789*CLHEP::cm,
      351.617*CLHEP::cm, 363.108*CLHEP::cm, 385.802*CLHEP::cm, 461.042*CLHEP::cm, 707.714*CLHEP::cm, 
      1038.42*CLHEP::cm, 1383.7*CLHEP::cm, 1558.36*CLHEP::cm, 1722.65*CLHEP::cm, 1939.11*CLHEP::cm, 
      2092.49*CLHEP::cm, 2240.14*CLHEP::cm, 2962.96*CLHEP::cm, 4967.03*CLHEP::cm, 6368.58*CLHEP::cm, 
      8207.56*CLHEP::cm, 10634.2*CLHEP::cm, 13855.3*CLHEP::cm, 18157.3*CLHEP::cm, 23940.2*CLHEP::cm, 
      31766*CLHEP::cm, 42431.6*CLHEP::cm, 57074.9*CLHEP::cm, 77335.9*CLHEP::cm, 105598*CLHEP::cm, 
      145361*CLHEP::cm, 192434*CLHEP::cm, 183898*CLHEP::cm, 176087*CLHEP::cm, 168913*CLHEP::cm, 162301*CLHEP::cm, 
      156187*CLHEP::cm, 150516*CLHEP::cm, 145243*CLHEP::cm, 140327*CLHEP::cm, 135733*CLHEP::cm, 131430*CLHEP::cm, 
      127392*CLHEP::cm, 123594*CLHEP::cm, 120016*CLHEP::cm, 116640*CLHEP::cm, 113448*CLHEP::cm, 110426*CLHEP::cm, 
      107562*CLHEP::cm};
  */
   // M Fechner: Rayleigh scattering -- as of version 4.6.2 of GEANT,
   // one may use one's own Rayleigh scattering lengths (the buffer is no
   // longer overwritten for "water", see 4.6.2 release notes)

   // RAYFF = 1/ARAS, for those of you who know SKdetsim...
   // actually that's not quite right because the scattering models
   // are different; in G4 there is no scattering depolarization
   // std value at SK = 0.6. But Mie scattering is implemented
   // in SKdetsim and not in G4

   
  // april 2005 : reduced reflections, let's increase scattering...
  // sep 09: for the large detector like superK the old values are muc better
   //G4double RAYFF = 1.0/1.65;  //old
   //    G4double RAYFF = 1.0/1.5;  

   G4double RAYFF = 0.625;

   // Get from the tuning parameters
   RAYFF = WCSimTuningParams->GetRayff();
   //    printf("RAYFF: %f\n",RAYFF);

   //T. Akiri: Values from Skdetsim 
   G4double RAYLEIGH_water[NUMENTRIES_water] = {
      386929*CLHEP::cm*RAYFF,  366249*CLHEP::cm*RAYFF,  346398*CLHEP::cm*RAYFF,  327355*CLHEP::cm*RAYFF,  309097*CLHEP::cm*RAYFF,  
      291603*CLHEP::cm*RAYFF,  274853*CLHEP::cm*RAYFF,  258825*CLHEP::cm*RAYFF,  243500*CLHEP::cm*RAYFF,  228856*CLHEP::cm*RAYFF,  
      214873*CLHEP::cm*RAYFF,  201533*CLHEP::cm*RAYFF,  188816*CLHEP::cm*RAYFF,  176702*CLHEP::cm*RAYFF,  165173*CLHEP::cm*RAYFF,
      154210*CLHEP::cm*RAYFF,  143795*CLHEP::cm*RAYFF,  133910*CLHEP::cm*RAYFF,  124537*CLHEP::cm*RAYFF,  115659*CLHEP::cm*RAYFF,  
      107258*CLHEP::cm*RAYFF, 99318.2*CLHEP::cm*RAYFF, 91822.2*CLHEP::cm*RAYFF,   84754*CLHEP::cm*RAYFF, 78097.3*CLHEP::cm*RAYFF, 
     71836.5*CLHEP::cm*RAYFF,   65956*CLHEP::cm*RAYFF, 60440.6*CLHEP::cm*RAYFF, 55275.4*CLHEP::cm*RAYFF, 50445.6*CLHEP::cm*RAYFF,
       45937*CLHEP::cm*RAYFF, 41735.2*CLHEP::cm*RAYFF, 37826.6*CLHEP::cm*RAYFF, 34197.6*CLHEP::cm*RAYFF, 30834.9*CLHEP::cm*RAYFF, 
     27725.4*CLHEP::cm*RAYFF, 24856.6*CLHEP::cm*RAYFF, 22215.9*CLHEP::cm*RAYFF, 19791.3*CLHEP::cm*RAYFF, 17570.9*CLHEP::cm*RAYFF,   
       15543*CLHEP::cm*RAYFF, 13696.6*CLHEP::cm*RAYFF, 12020.5*CLHEP::cm*RAYFF, 10504.1*CLHEP::cm*RAYFF, 9137.15*CLHEP::cm*RAYFF,
     7909.45*CLHEP::cm*RAYFF,  6811.3*CLHEP::cm*RAYFF, 5833.25*CLHEP::cm*RAYFF,  4966.2*CLHEP::cm*RAYFF, 4201.36*CLHEP::cm*RAYFF, 
     3530.28*CLHEP::cm*RAYFF, 2944.84*CLHEP::cm*RAYFF, 2437.28*CLHEP::cm*RAYFF, 2000.18*CLHEP::cm*RAYFF,  1626.5*CLHEP::cm*RAYFF, 
     1309.55*CLHEP::cm*RAYFF, 1043.03*CLHEP::cm*RAYFF, 821.016*CLHEP::cm*RAYFF,  637.97*CLHEP::cm*RAYFF, 488.754*CLHEP::cm*RAYFF};

   /*G4double RAYLEIGH_water[NUMENTRIES_water] = {
     167024.4*CLHEP::cm*RAYFF, 158726.7*CLHEP::cm*RAYFF, 150742*CLHEP::cm*RAYFF,
     143062.5*CLHEP::cm*RAYFF, 135680.2*CLHEP::cm*RAYFF, 128587.4*CLHEP::cm*RAYFF,
     121776.3*CLHEP::cm*RAYFF, 115239.5*CLHEP::cm*RAYFF, 108969.5*CLHEP::cm*RAYFF,
     102958.8*CLHEP::cm*RAYFF, 97200.35*CLHEP::cm*RAYFF, 91686.86*CLHEP::cm*RAYFF,
     86411.33*CLHEP::cm*RAYFF, 81366.79*CLHEP::cm*RAYFF, 76546.42*CLHEP::cm*RAYFF,
     71943.46*CLHEP::cm*RAYFF, 67551.29*CLHEP::cm*RAYFF, 63363.36*CLHEP::cm*RAYFF,
     59373.25*CLHEP::cm*RAYFF, 55574.61*CLHEP::cm*RAYFF, 51961.24*CLHEP::cm*RAYFF,
     48527.00*CLHEP::cm*RAYFF, 45265.87*CLHEP::cm*RAYFF, 42171.94*CLHEP::cm*RAYFF,
     39239.39*CLHEP::cm*RAYFF, 36462.50*CLHEP::cm*RAYFF, 33835.68*CLHEP::cm*RAYFF,
     31353.41*CLHEP::cm*RAYFF, 29010.30*CLHEP::cm*RAYFF, 26801.03*CLHEP::cm*RAYFF,
     24720.42*CLHEP::cm*RAYFF, 22763.36*CLHEP::cm*RAYFF, 20924.88*CLHEP::cm*RAYFF,
     19200.07*CLHEP::cm*RAYFF, 17584.16*CLHEP::cm*RAYFF, 16072.45*CLHEP::cm*RAYFF,
     14660.38*CLHEP::cm*RAYFF, 13343.46*CLHEP::cm*RAYFF, 12117.33*CLHEP::cm*RAYFF,
     10977.70*CLHEP::cm*RAYFF, 9920.416*CLHEP::cm*RAYFF, 8941.407*CLHEP::cm*RAYFF,
     8036.711*CLHEP::cm*RAYFF, 7202.470*CLHEP::cm*RAYFF, 6434.927*CLHEP::cm*RAYFF,
     5730.429*CLHEP::cm*RAYFF, 5085.425*CLHEP::cm*RAYFF, 4496.467*CLHEP::cm*RAYFF,
     3960.210*CLHEP::cm*RAYFF, 3473.413*CLHEP::cm*RAYFF, 3032.937*CLHEP::cm*RAYFF,
     2635.746*CLHEP::cm*RAYFF, 2278.907*CLHEP::cm*RAYFF, 1959.588*CLHEP::cm*RAYFF,
     1675.064*CLHEP::cm*RAYFF, 1422.710*CLHEP::cm*RAYFF, 1200.004*CLHEP::cm*RAYFF,
     1004.528*CLHEP::cm*RAYFF, 833.9666*CLHEP::cm*RAYFF, 686.1063*CLHEP::cm*RAYFF
     };*/


   // Get from the tuning parameters
   G4double MIEFF = WCSimTuningParams->GetMieff();
   //G4double MIEFF = 0.0;
   //    printf("MIEFF: %f\n",MIEFF);

   //Values extracted from Skdetsim
   G4double MIE_water[NUMENTRIES_water] = {
     7790020*CLHEP::cm*MIEFF, 7403010*CLHEP::cm*MIEFF, 7030610*CLHEP::cm*MIEFF, 6672440*CLHEP::cm*MIEFF, 6328120*CLHEP::cm*MIEFF, 
     5997320*CLHEP::cm*MIEFF, 5679650*CLHEP::cm*MIEFF, 5374770*CLHEP::cm*MIEFF, 5082340*CLHEP::cm*MIEFF, 4802000*CLHEP::cm*MIEFF, 
     4533420*CLHEP::cm*MIEFF, 4276280*CLHEP::cm*MIEFF, 4030220*CLHEP::cm*MIEFF, 3794950*CLHEP::cm*MIEFF, 3570120*CLHEP::cm*MIEFF,
     3355440*CLHEP::cm*MIEFF, 3150590*CLHEP::cm*MIEFF, 2955270*CLHEP::cm*MIEFF, 2769170*CLHEP::cm*MIEFF, 2592000*CLHEP::cm*MIEFF, 
     2423470*CLHEP::cm*MIEFF, 2263300*CLHEP::cm*MIEFF, 2111200*CLHEP::cm*MIEFF, 1966900*CLHEP::cm*MIEFF, 1830120*CLHEP::cm*MIEFF, 
     1700610*CLHEP::cm*MIEFF, 1578100*CLHEP::cm*MIEFF, 1462320*CLHEP::cm*MIEFF, 1353040*CLHEP::cm*MIEFF, 1250000*CLHEP::cm*MIEFF,
     1152960*CLHEP::cm*MIEFF, 1061680*CLHEP::cm*MIEFF,  975936*CLHEP::cm*MIEFF,  895491*CLHEP::cm*MIEFF,  820125*CLHEP::cm*MIEFF, 
      749619*CLHEP::cm*MIEFF,  683760*CLHEP::cm*MIEFF,  622339*CLHEP::cm*MIEFF,  565152*CLHEP::cm*MIEFF,  512000*CLHEP::cm*MIEFF, 
      462688*CLHEP::cm*MIEFF,  417027*CLHEP::cm*MIEFF,  374832*CLHEP::cm*MIEFF,  335923*CLHEP::cm*MIEFF,  300125*CLHEP::cm*MIEFF,
      267267*CLHEP::cm*MIEFF,  237184*CLHEP::cm*MIEFF,  209715*CLHEP::cm*MIEFF,  184704*CLHEP::cm*MIEFF,  162000*CLHEP::cm*MIEFF, 
      141456*CLHEP::cm*MIEFF,  122931*CLHEP::cm*MIEFF,  106288*CLHEP::cm*MIEFF, 91395.2*CLHEP::cm*MIEFF,   78125*CLHEP::cm*MIEFF, 
     66355.2*CLHEP::cm*MIEFF, 55968.2*CLHEP::cm*MIEFF, 46851.2*CLHEP::cm*MIEFF, 38896.2*CLHEP::cm*MIEFF,   32000*CLHEP::cm*MIEFF};

   //Mie scattering length values when assuming 10 times larger than Rayleigh scattering. 
   /*G4double MIE_water[NUMENTRIES_water] = {
      3869290*CLHEP::cm*MIEFF, 3662490*CLHEP::cm*MIEFF,  3463980*CLHEP::cm*MIEFF,  3273550*CLHEP::cm*MIEFF,  3090970*CLHEP::cm*MIEFF,  
      2916030*CLHEP::cm*MIEFF, 2748530*CLHEP::cm*MIEFF,  2588250*CLHEP::cm*MIEFF,  2435000*CLHEP::cm*MIEFF,  2288560*CLHEP::cm*MIEFF,  
      2148730*CLHEP::cm*MIEFF, 2015330*CLHEP::cm*MIEFF,  1888160*CLHEP::cm*MIEFF,  1767020*CLHEP::cm*MIEFF,  1651730*CLHEP::cm*MIEFF,
      1542100*CLHEP::cm*MIEFF, 1437950*CLHEP::cm*MIEFF,  1339100*CLHEP::cm*MIEFF,  1245370*CLHEP::cm*MIEFF,  1156590*CLHEP::cm*MIEFF,  
      1072580*CLHEP::cm*MIEFF,  993182*CLHEP::cm*MIEFF,   918222*CLHEP::cm*MIEFF,   847540*CLHEP::cm*MIEFF,   780973*CLHEP::cm*MIEFF, 
       718365*CLHEP::cm*MIEFF,   65956*CLHEP::cm*MIEFF,   604406*CLHEP::cm*MIEFF,   552754*CLHEP::cm*MIEFF,   504456*CLHEP::cm*MIEFF,
       459370*CLHEP::cm*MIEFF,  417352*CLHEP::cm*MIEFF,   378266*CLHEP::cm*MIEFF,   341976*CLHEP::cm*MIEFF,   308349*CLHEP::cm*MIEFF, 
       277254*CLHEP::cm*MIEFF,  248566*CLHEP::cm*MIEFF,   222159*CLHEP::cm*MIEFF,   197913*CLHEP::cm*MIEFF,   175709*CLHEP::cm*MIEFF,   
       155430*CLHEP::cm*MIEFF,  136966*CLHEP::cm*MIEFF,   120205*CLHEP::cm*MIEFF,   105041*CLHEP::cm*MIEFF,  91371.5*CLHEP::cm*MIEFF,
      79094.5*CLHEP::cm*MIEFF,   68113*CLHEP::cm*MIEFF,  58332.5*CLHEP::cm*MIEFF,    49662*CLHEP::cm*MIEFF,  42013.6*CLHEP::cm*MIEFF, 
      35302.8*CLHEP::cm*MIEFF, 29448.4*CLHEP::cm*MIEFF,  24372.8*CLHEP::cm*MIEFF,  20001.8*CLHEP::cm*MIEFF,    16265*CLHEP::cm*MIEFF, 
      13095.5*CLHEP::cm*MIEFF, 10430.3*CLHEP::cm*MIEFF,  8210.16*CLHEP::cm*MIEFF,   6379.7*CLHEP::cm*MIEFF,  4887.54*CLHEP::cm*MIEFF
   };
   */

   G4double MIE_water_const[3]={0.4,0.,1};// gforward, gbackward, forward backward ratio


   //From SFDETSIM
   /*
   G4double RINDEX_glass[NUMENTRIES] =
     { 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600 };
   */
   // M Fechner : unphysical, I want to reduce reflections
   // back to the old value 1.55

   G4double RINDEX_glass[NUMENTRIES_water] =
     { 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600, 1.600, 1.600, 1.600, 1.600, 1.600,
       1.600, 1.600};

   //G4double RINDEX_blacksheet[NUMENTRIES] =
   //{ 2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500, 2.500, 2.500, 2.500,
   //  2.500, 2.500, 2.500, 2.500 };
   
   
   //G4double ABSORPTION1[NUMENTRIES] =
   //{344.8*CLHEP::cm,  408.2*CLHEP::cm,  632.9*CLHEP::cm,  917.4*CLHEP::cm, 1234.6*CLHEP::cm, 1388.9*CLHEP::cm,
   // 1515.2*CLHEP::cm, 1724.1*CLHEP::cm, 1886.8*CLHEP::cm, 2000.0*CLHEP::cm, 2631.6*CLHEP::cm, 3571.4*CLHEP::cm,
   // 4545.5*CLHEP::cm, 4761.9*CLHEP::cm, 5263.2*CLHEP::cm, 5263.2*CLHEP::cm, 5555.6*CLHEP::cm, 5263.2*CLHEP::cm,
   // 5263.2*CLHEP::cm, 4761.9*CLHEP::cm, 4545.5*CLHEP::cm, 4166.7*CLHEP::cm, 3703.7*CLHEP::cm, 3333.3*CLHEP::cm,
   // 3000.0*CLHEP::cm, 2850.0*CLHEP::cm, 2700.0*CLHEP::cm, 2450.0*CLHEP::cm, 2200.0*CLHEP::cm, 1950.0*CLHEP::cm,
   // 1750.0*CLHEP::cm, 1450.0*CLHEP::cm };
   
   /*   
   G4double ABSORPTION_glass[NUMENTRIES] = 
     { 100.0*CLHEP::cm, 110.0*CLHEP::cm, 120.0*CLHEP::cm, 130.0*CLHEP::cm, 140.0*CLHEP::cm, 150.0*CLHEP::cm, 160.0*CLHEP::cm,
       165.0*CLHEP::cm, 170.0*CLHEP::cm, 175.0*CLHEP::cm, 180.0*CLHEP::cm, 185.0*CLHEP::cm, 190.0*CLHEP::cm, 195.0*CLHEP::cm,
       200.0*CLHEP::cm, 200.0*CLHEP::cm, 200.0*CLHEP::cm, 200.0*CLHEP::cm, 200.0*CLHEP::cm, 195.0*CLHEP::cm, 190.0*CLHEP::cm,
       185.0*CLHEP::cm, 180.0*CLHEP::cm, 175.0*CLHEP::cm, 170.0*CLHEP::cm, 160.0*CLHEP::cm, 150.0*CLHEP::cm, 140.0*CLHEP::cm,
       130.0*CLHEP::cm, 120.0*CLHEP::cm, 110.0*CLHEP::cm, 100.0*CLHEP::cm };
   */
   // M Fechner : the quantum efficiency already takes glass abs into account

   G4double ABSORPTION_glass[NUMENTRIES_water]= 
     { 1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm, 1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,1.0e9*CLHEP::cm,
       1.0e9*CLHEP::cm, 1.0e9*CLHEP::cm};
   
   G4double BLACKABS_blacksheet[NUMENTRIES_water] =
     { 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 
       1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 
       1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm,
       1.0e-9*CLHEP::cm, 1.0e-9*CLHEP::cm};
   
   
   //The following reflectivity for blacksheet is obtained from skdetsim
   //There is a fudge factor of 2.7 for the reflectivity of blacksheet
   //depending on whether SK1 or SK2 simulation is used.  
   //The BlackSheetFudgeFactor is set to true if you want to use the 
   //SK2 values, false if not.
   //G4double SK1SK2FF = 1.0;
   //G4bool BlackSheetFudgeFactor=false;
   //G4bool BlackSheetFudgeFactor=true;
   //   if (BlackSheetFudgeFactor) SK1SK2FF=SK1SK2FF*2.7;
   //if (BlackSheetFudgeFactor) SK1SK2FF=SK1SK2FF*1.55;

   //July 1, 2010, F. Beroz: changed SK1SK2FF to BSRFF to avoid confusion,
   // since this parameter is not from SK.

   G4double BSRFF = 1.0;
   
   // Get from the tuning parameters
   BSRFF = WCSimTuningParams->GetBsrff();

   G4double REFLECTIVITY_blacksheet[NUMENTRIES_water] =
     { 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 0.055*BSRFF, 
       0.055*BSRFF, 0.057*BSRFF, 0.059*BSRFF, 0.060*BSRFF, 
       0.059*BSRFF, 0.058*BSRFF, 0.057*BSRFF, 0.055*BSRFF, 
       0.050*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF, 0.045*BSRFF,
       0.045*BSRFF, 0.045*BSRFF};

   /*
   G4double REFLECTIVITY_blacksheet[NUMENTRIES] =
     { 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.057*SK1SK2FF, 0.059*SK1SK2FF, 0.060*SK1SK2FF, 
       0.059*SK1SK2FF, 0.058*SK1SK2FF, 0.057*SK1SK2FF, 0.055*SK1SK2FF, 
       0.050*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF };
   */
   
   /*
   G4double REFLECTIVITY_blacksheet[NUMENTRIES_water] =
     { 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 0.055*SK1SK2FF, 
       0.055*SK1SK2FF, 0.057*SK1SK2FF, 0.059*SK1SK2FF, 0.060*SK1SK2FF, 
       0.059*SK1SK2FF, 0.058*SK1SK2FF, 0.057*SK1SK2FF, 0.055*SK1SK2FF, 
       0.050*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF,
       0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF, 0.045*SK1SK2FF ,
       0.045*SK1SK2FF, 0.045*SK1SK2FF };
   */

   //utter fiction at this stage
   G4double EFFICIENCY[NUMENTRIES_water] =
     { 0.001*CLHEP::m };
      
   //utter fiction at this stage, does not matter
   G4double RAYLEIGH_air[NUMENTRIES_water] =
     { 0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,
       0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m,0.001*CLHEP::m};
      
       //utter fiction at this stage, does not matter
   G4double MIE_air[NUMENTRIES_water] =
     { 0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,
       0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m,0.1*CLHEP::m};

   G4double MIE_air_const[3]={0.99,0.99,0.8};// gforward, gbackward, forward backward ratio


   //Not used yet, fictional values
   //G4double SPECULARLOBECONSTANT1[NUMENTRIES] =
   //{ 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001 };
   
   //Not used yet, fictional values
   //G4double SPECULARSPIKECONSTANT1[NUMENTRIES] =
   //{ 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001 };
   
   //Not used yet, fictional values
   //G4double BACKSCATTERCONSTANT1[NUMENTRIES] =
   //{ 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001,
   //  0.001, 0.001, 0.001, 0.001 };
   
   G4double EFFICIENCY_blacksheet[NUMENTRIES_water] =
    { 0.0 };

   //	------------- Surfaces --------------

   OpWaterBSSurface =
     new G4OpticalSurface("WaterBSCellSurface");

   OpWaterBSSurface->SetType(dielectric_dielectric);
   OpWaterBSSurface->SetModel(unified); 
   OpWaterBSSurface->SetFinish(groundfrontpainted);
   OpWaterBSSurface->SetSigmaAlpha(0.1);

   const G4int NUM = 2;
   //   G4double PP[NUM] =
   //{ 2.038E-9*CLHEP::GeV, 4.144E-9*CLHEP::GeV };

   G4double PP[NUM] = { 1.4E-9*CLHEP::GeV,6.2E-9*CLHEP::GeV};
   //G4double PP[NUM] = { 1.4E-9*CLHEP::GeV, 20.7e-09*CLHEP::GeV };
   G4double RINDEX_blacksheet[NUM] =
     { 1.6, 1.6 };

   G4double SPECULARLOBECONSTANT[NUM] =
     { 0.3, 0.3 };
   G4double SPECULARSPIKECONSTANT[NUM] =
     { 0.2, 0.2 };
   G4double BACKSCATTERCONSTANT[NUM] =
     { 0.2, 0.2 };

   OpGlassCathodeSurface =
     new G4OpticalSurface("GlassCathodeSurface");
   OpGlassCathodeSurface->SetType(dielectric_dielectric);
   OpGlassCathodeSurface->SetModel(unified);  
   //   OpGlassCathodeSurface->SetFinish(groundbackpainted);
   OpGlassCathodeSurface->SetFinish(polished);
   //OpGlassCathodeSurface->SetSigmaAlpha(0.002);
   // was 1.0
   // totally unphysical anyway 
   G4double RINDEX_cathode[NUM] =
     { 1.0, 1.0 };
   /*   
   G4double SPECULARLOBECONSTANT_glasscath[NUM] =
     { 1.0, 1.0 };
     // { 0.3, 0.3 };
   G4double SPECULARSPIKECONSTANT_glasscath[NUM] =
     { 0.0, 0.0 };
     //     { 0.2, 0.2 };
   G4double BACKSCATTERCONSTANT_glasscath[NUM] =
     {0.0, 0.0};
   //     { 0.2, 0.2 };
   */


  OpGlassCathodeSurface_R7081 = new G4OpticalSurface("GlassCathodeSurface_R7081");
  OpGlassCathodeSurface_R7081->SetType(dielectric_dielectric);
  OpGlassCathodeSurface_R7081->SetModel(unified);
  OpGlassCathodeSurface_R7081->SetFinish(polished);

   G4double RGCFF = 0.0;
   RGCFF = WCSimTuningParams->GetRgcff();
 
   G4double REFLECTIVITY_glasscath[NUM] =
     //{ 0.0+RGCFF, 0.0+RGCFF };
     //{ RGCFF, RGCFF };
     //{ 0.0, 0.0 };
     { 1.0*RGCFF, 1.0*RGCFF };


   G4double EFFICIENCY_glasscath[NUM] =
     { 0.0, 0.0 };


   G4double RGCFFR7081 = 0.0;
    RGCFFR7081 = WCSimTuningParams->GetRgcffR7081();
    G4double REFLECTIVITY_glasscath_R7081[NUM] = {1.0*RGCFFR7081, 1.0*RGCFFR7081};

   // jl145 ----
   //
   OpWaterTySurface =
     new G4OpticalSurface("WaterTyCellSurface");

   OpWaterTySurface->SetType(dielectric_dielectric);
   OpWaterTySurface->SetModel(unified); 
   OpWaterTySurface->SetFinish(groundbackpainted); //a guess, but seems to work
   OpWaterTySurface->SetSigmaAlpha(0.5); //cf. A. Chavarria's ~30deg

   G4double RINDEX_tyvek[NUM] =
     { 1.5, 1.5 }; // polyethylene permittivity is ~2.25
   G4double TySPECULARLOBECONSTANT[NUM] =
     { 0.75, 0.75 }; // crudely estimated from A. Chavarria's thesis
   G4double TySPECULARSPIKECONSTANT[NUM] =
     { 0.0, 0.0 };
   G4double TyBACKSCATTERCONSTANT[NUM] =
     { 0.0, 0.0 };
   // Lambertian prob is therefore 0.25

   G4double TyREFLECTIVITY[NUM] =
     { 0.94, 0.94 }; //cf. DuPont
   //
   // ----

  G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();
  mpt->AddProperty("RINDEX", ENERGY_water, RINDEX1, NUMENTRIES_water);
  mpt->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
  StainlessSteel->SetMaterialPropertiesTable(mpt);

   G4MaterialPropertiesTable *myMPT1 = new G4MaterialPropertiesTable();
   // M Fechner : new   ; wider range for lambda
   myMPT1->AddProperty("RINDEX", ENERGY_water, RINDEX1, NUMENTRIES_water);
   myMPT1->AddProperty("ABSLENGTH",ENERGY_water, ABSORPTION_water, NUMENTRIES_water);
   // M Fechner: new, don't let G4 compute it.
   myMPT1->AddProperty("RAYLEIGH",ENERGY_water,RAYLEIGH_water,NUMENTRIES_water);

  //  myMPT1->AddProperty("MIEHG",ENERGY_water,MIE_water,NUMENTRIES_water);
//    myMPT1->AddConstProperty("MIEHG_FORWARD",MIE_water_const[0]);
//    myMPT1->AddConstProperty("MIEHG_BACKWARD",MIE_water_const[1]);
//    myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO",MIE_water_const[2]);



   Water->SetMaterialPropertiesTable(myMPT1);
   //Gd doped water has the same optical properties as pure water
   DopedWater->SetMaterialPropertiesTable(myMPT1);
   // myMPT1->DumpTable();
   
   G4MaterialPropertiesTable *myMPT2 = new G4MaterialPropertiesTable();
   myMPT2->AddProperty("RINDEX", ENERGY_water, RINDEX_air, NUMENTRIES_water);
   // M Fechner : what is that ?????
   myMPT2->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   myMPT2->AddProperty("RAYLEIGH",ENERGY_water, RAYLEIGH_air, NUMENTRIES_water);

   // myMPT2->AddProperty("MIEHG",ENERGY_water,MIE_air,NUMENTRIES_water);
//       myMPT2->AddConstProperty("MIEHG_FORWARD",MIE_air_const[0]);
//       myMPT2->AddConstProperty("MIEHG_BACKWARD",MIE_air_const[1]);
//       myMPT2->AddConstProperty("MIEHG_FORWARD_RATIO",MIE_air_const[2]);
   
   Air->SetMaterialPropertiesTable(myMPT2);
   
   G4MaterialPropertiesTable *myMPT3 = new G4MaterialPropertiesTable();
   myMPT3->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   myMPT3->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet, NUMENTRIES_water);
   myMPT3->AddProperty("EFFICIENCY",   ENERGY_water, EFFICIENCY, NUMENTRIES_water);
   Plastic->SetMaterialPropertiesTable(myMPT3);
   
   G4MaterialPropertiesTable *myMPT4 = new G4MaterialPropertiesTable();
   myMPT4->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   Blacksheet->SetMaterialPropertiesTable(myMPT4);
   
   G4MaterialPropertiesTable *myMPT5 = new G4MaterialPropertiesTable();
   myMPT5->AddProperty("RINDEX", ENERGY_water, RINDEX_glass, NUMENTRIES_water);
   myMPT5->AddProperty("ABSLENGTH",ENERGY_water, ABSORPTION_glass, NUMENTRIES_water);
   Glass->SetMaterialPropertiesTable(myMPT5);
   GlassR7081->SetMaterialPropertiesTable(myMPT5);    

   // jl145 ----
   // Abs legnth is same as blacksheet, very small.
   G4MaterialPropertiesTable *myMPT6 = new G4MaterialPropertiesTable();
   myMPT6->AddProperty("ABSLENGTH", ENERGY_water, BLACKABS_blacksheet, NUMENTRIES_water);
   Tyvek->SetMaterialPropertiesTable(myMPT6);


   // reflectivity of steel inner structure
   // =====================================
   G4double TEFLONRFF = WCSimTuningParams->GetTeflonrff();
   const G4int arrEntries = 2;
   G4double photEneSteel[] = { 1.56962*CLHEP::eV, 6.19998*CLHEP::eV };
   //G4double photEneSteel[] = { 1.56962*CLHEP::eV, 20.7*CLHEP::eV};
   G4double rIndexSteel[] = { 1.35, 1.35};
   G4double absSteel[] = { 1.0e-9*CLHEP::cm,  1.0e-9*CLHEP::cm};
   //G4double refSteel[] = {0.5, 0.5}; // low estimate. --> replaced by tunable teflon reflectivity value (whole structure is wrapped)
   //Introduce the reflectivity of the steel inner structure as a function of a tunable variable. Previous behavior can be replicated with a tuning factor of 0.5 (most definitely too low).
   G4double refSteel[] = {1.0*TEFLONRFF,1.0*TEFLONRFF}; // Introduce tuning parameter for impact of teflon wrapping on reflections
   G4double specularlobeSteel[] = {0.8, 0.8};   // ¯\_(ツ)_/¯
   G4double specularspikeSteel[] = {0.1, 0.1};
   G4double backscatterSteel[] = {0.01, 0.01};
   // remaining reflection type (lambertian) is the remainder from 1
   
   // see http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s02.html
   G4MaterialPropertiesTable *innerstructureMatProps = new G4MaterialPropertiesTable();
   innerstructureMatProps->AddProperty("RINDEX", photEneSteel, rIndexSteel, arrEntries);
//   innerstructureMatProps->AddProperty("ABSLENGTH",photEneSteel,absSteel,arrEntries);
   // total probability of reflection
   innerstructureMatProps->AddProperty("REFLECTIVITY",photEneSteel, refSteel,arrEntries);
   // selection of type of reflection
   innerstructureMatProps->AddProperty("SPECULARLOBECONSTANT",photEneSteel,specularlobeSteel,arrEntries);
   innerstructureMatProps->AddProperty("SPECULARSPIKECONSTANT",photEneSteel,specularspikeSteel,arrEntries);
   innerstructureMatProps->AddProperty("BACKSCATTERCONSTANT",photEneSteel,backscatterSteel,arrEntries);
   
   InnerStructureOpSurface = new G4OpticalSurface("SteelInnerStructureSurface");
   InnerStructureOpSurface->SetType(dielectric_metal);
   InnerStructureOpSurface->SetModel(unified);
   InnerStructureOpSurface->SetFinish(ground);
   InnerStructureOpSurface->SetSigmaAlpha(0.3);
   InnerStructureOpSurface->SetMaterialPropertiesTable(innerstructureMatProps);
   
   // reflectivity of tank liner
   // ===========================
   // re-use the properties from steel, only reflectivity is important?
   G4double LINERRFF = WCSimTuningParams->GetLinerrff();
   G4double refLiner[] = {1.00*LINERRFF, 1.00*LINERRFF}; // from datasheet // add possibility to tune reflectivity value a little
   G4double rIndexLiner[] = { 1.5, 1.5};
   // remaining reflection type (lambertian) is the remainder from 1
   
   // see http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s02.html
   G4MaterialPropertiesTable *linerSurfaceMatProps = new G4MaterialPropertiesTable();
   linerSurfaceMatProps->AddProperty("RINDEX", photEneSteel, rIndexLiner, arrEntries);
   linerSurfaceMatProps->AddProperty("ABSLENGTH",photEneSteel,absSteel,arrEntries);
   linerSurfaceMatProps->AddProperty("REFLECTIVITY",photEneSteel, refLiner,arrEntries);
   linerSurfaceMatProps->AddProperty("SPECULARLOBECONSTANT",photEneSteel,specularlobeSteel,arrEntries);
   linerSurfaceMatProps->AddProperty("SPECULARSPIKECONSTANT",photEneSteel,specularspikeSteel,arrEntries);
   linerSurfaceMatProps->AddProperty("BACKSCATTERCONSTANT",photEneSteel,backscatterSteel,arrEntries);
   
   LinerOpSurface = new G4OpticalSurface("LinerOpSurface");
   LinerOpSurface->SetType(dielectric_metal); // fine, reflection or absorption only.
   LinerOpSurface->SetModel(unified);
   LinerOpSurface->SetFinish(ground);
   LinerOpSurface->SetSigmaAlpha(0.1);
   LinerOpSurface->SetMaterialPropertiesTable(linerSurfaceMatProps);

   //ANNIE Holder are made out of polyethlene
   //Should be close to acrylic white as defined in ratpac simulation
   //Use refractive index for material 'acrylic_white' from ratpac
   G4double HOLDERRFF = WCSimTuningParams->GetHolderrff();
   const G4int arrEntriesHolder = 61;
   G4double refHolder[] = {1.00*HOLDERRFF,1.00*HOLDERRFF};
   G4double photEneHolder[] = {1.55*CLHEP::eV, 1.569620253*CLHEP::eV, 1.58974359*CLHEP::eV, 1.61038961*CLHEP::eV, 1.631578947*CLHEP::eV, 1.653333333*CLHEP::eV, 1.675675676*CLHEP::eV, 1.698630137*CLHEP::eV, 1.722222222*CLHEP::eV, 1.746478873*CLHEP::eV, 1.771428571*CLHEP::eV, 1.797101449*CLHEP::eV, 1.823529412*CLHEP::eV, 1.850746269*CLHEP::eV, 1.878787879*CLHEP::eV, 1.907692308*CLHEP::eV, 1.9375*CLHEP::eV, 1.968253968*CLHEP::eV, 2*CLHEP::eV, 2.032786885*CLHEP::eV, 2.066666667*CLHEP::eV, 2.101694915*CLHEP::eV, 2.137931034*CLHEP::eV, 2.175438596*CLHEP::eV, 2.214285714*CLHEP::eV, 2.254545455*CLHEP::eV, 2.296296296*CLHEP::eV, 2.339622642*CLHEP::eV, 2.384615385*CLHEP::eV, 2.431372549*CLHEP::eV, 2.48*CLHEP::eV, 2.530612245*CLHEP::eV, 2.583333333*CLHEP::eV, 2.638297872*CLHEP::eV, 2.695652174*CLHEP::eV, 2.755555556*CLHEP::eV, 2.818181818*CLHEP::eV, 2.88372093*CLHEP::eV, 2.952380952*CLHEP::eV, 3.024390244*CLHEP::eV, 3.1*CLHEP::eV, 3.179487179*CLHEP::eV, 3.263157895*CLHEP::eV, 3.351351351*CLHEP::eV, 3.444444444*CLHEP::eV, 3.542857143*CLHEP::eV, 3.647058824*CLHEP::eV, 3.757575758*CLHEP::eV, 3.875*CLHEP::eV, 4*CLHEP::eV, 4.133333333*CLHEP::eV, 4.275862069*CLHEP::eV, 4.428571429*CLHEP::eV, 4.592592593*CLHEP::eV, 4.769230769*CLHEP::eV, 4.96*CLHEP::eV, 5.166666667*CLHEP::eV, 5.391304348*CLHEP::eV, 5.636363636*CLHEP::eV, 5.904761905*CLHEP::eV, 6.2*CLHEP::eV};
//, 20.66666667*CLHEP::eV}; //60nm
   G4double rIndexHolder[] = {1.485, 1.485, 1.485, 1.485, 1.486, 1.486, 1.486, 1.486, 1.487, 1.487, 1.487, 1.488, 1.488, 1.488, 1.488, 1.489, 1.489, 1.49, 1.49, 1.49, 1.491, 1.491, 1.492, 1.492, 1.493, 1.493, 1.494, 1.494, 1.495, 1.496, 1.496, 1.497, 1.498, 1.499, 1.499, 1.5, 1.501, 1.502, 1.503, 1.505, 1.506, 1.507, 1.509, 1.51, 1.512, 1.514, 1.516, 1.519, 1.521, 1.524, 1.527, 1.531, 1.534, 1.539, 1.544, 1.55, 1.556, 1.564, 1.573, 1.584, 1.597};
//, 1.597}; //60nm

   G4MaterialPropertiesTable *holderSurfaceMatProps = new G4MaterialPropertiesTable();
   holderSurfaceMatProps->AddProperty("RINDEX", photEneHolder, rIndexHolder, arrEntriesHolder);
   holderSurfaceMatProps->AddProperty("ABSLENGTH",photEneSteel,absSteel,arrEntries);		//This should probably be something else, check (TODO)
   holderSurfaceMatProps->AddProperty("REFLECTIVITY",photEneSteel, refHolder,arrEntries);
   holderSurfaceMatProps->AddProperty("SPECULARLOBECONSTANT",photEneSteel,specularlobeSteel,arrEntries); // Do we need this?
   holderSurfaceMatProps->AddProperty("SPECULARSPIKECONSTANT",photEneSteel,specularspikeSteel,arrEntries); // Do we need this?
   holderSurfaceMatProps->AddProperty("BACKSCATTERCONSTANT",photEneSteel,backscatterSteel,arrEntries); // Do we need this?
   
   HolderOpSurface = new G4OpticalSurface("HolderOpSurface");
   HolderOpSurface->SetType(dielectric_metal); // fine, reflection or absorption only.
   HolderOpSurface->SetModel(unified);
   HolderOpSurface->SetFinish(ground);
   HolderOpSurface->SetSigmaAlpha(0.1);
   HolderOpSurface->SetMaterialPropertiesTable(holderSurfaceMatProps);

   //LUX & ETEL Holders are made of Schedule 80 PVT
   // Not completely black & has some kind of reflective surface
   // --> Reflectivity is assumed to be > 0.5
   // Assume same refractive index as for the ANNIE holders

   G4double LUXHOLDERRFF = WCSimTuningParams->GetHolderrffLUX();
   G4double refHolderLUX[] = {1.00*LUXHOLDERRFF,1.00*LUXHOLDERRFF};

   G4MaterialPropertiesTable *luxholderSurfaceMatProps= new G4MaterialPropertiesTable();
   luxholderSurfaceMatProps->AddProperty("RINDEX",photEneHolder, rIndexHolder, arrEntriesHolder);
   luxholderSurfaceMatProps->AddProperty("ABSLENGTH",photEneSteel,absSteel,arrEntries); //This should probably be something else, check
   luxholderSurfaceMatProps->AddProperty("REFLECTIVITY",photEneSteel, refHolderLUX, arrEntries);
   luxholderSurfaceMatProps->AddProperty("SPECULARLOBECONSTANT",photEneSteel,specularlobeSteel,arrEntries); //Do we need this?
   luxholderSurfaceMatProps->AddProperty("SPECULARSPIKECONSTANT",photEneSteel,specularspikeSteel,arrEntries); // Do we need this?
   luxholderSurfaceMatProps->AddProperty("BACKSCATTERCONSTANT",photEneSteel,backscatterSteel,arrEntries); //Do we need this?

   LUXHolderOpSurface = new G4OpticalSurface("LUXHolderOpSurface");
   LUXHolderOpSurface->SetType(dielectric_metal); //fine, reflection or absorption only
   LUXHolderOpSurface->SetModel(unified);
   LUXHolderOpSurface->SetFinish(ground);
   LUXHolderOpSurface->SetSigmaAlpha(0.1);
   LUXHolderOpSurface->SetMaterialPropertiesTable(luxholderSurfaceMatProps);

   //	------------- Surfaces --------------

   // Blacksheet
   G4MaterialPropertiesTable *myST1 = new G4MaterialPropertiesTable();
   myST1->AddProperty("RINDEX", ENERGY_water, RINDEX_blacksheet, NUMENTRIES_water);
   myST1->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT, NUM);
   myST1->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT, NUM);
   myST1->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT, NUM);
   myST1->AddProperty("REFLECTIVITY", ENERGY_water, REFLECTIVITY_blacksheet, NUMENTRIES_water);
   myST1->AddProperty("EFFICIENCY", ENERGY_water, EFFICIENCY_blacksheet, NUMENTRIES_water);
   OpWaterBSSurface->SetMaterialPropertiesTable(myST1);

   //Glass to Cathode surface inside PMTs
   G4MaterialPropertiesTable *myST2 = new G4MaterialPropertiesTable();
   myST2->AddProperty("RINDEX", PP, RINDEX_cathode, NUM);
   //   myST2->AddProperty("SPECULARLOBECONSTANT", PP, SPECULARLOBECONSTANT_glasscath, NUM);
   //   myST2->AddProperty("SPECULARSPIKECONSTANT", PP, SPECULARSPIKECONSTANT_glasscath, NUM);
   //   myST2->AddProperty("BACKSCATTERCONSTANT", PP, BACKSCATTERCONSTANT_glasscath, NUM);
   myST2->AddProperty("REFLECTIVITY", PP, REFLECTIVITY_glasscath, NUM);
   myST2->AddProperty("EFFICIENCY", PP, EFFICIENCY_glasscath, NUM);
   //myST2->AddProperty("ABSLENGTH", PP, abslength_paint , NUM);
   OpGlassCathodeSurface->SetMaterialPropertiesTable(myST2);

   G4MaterialPropertiesTable *myST2_R7081 = new G4MaterialPropertiesTable();
   myST2_R7081->AddProperty("RINDEX", PP, RINDEX_cathode, NUM);
   myST2_R7081->AddProperty("REFLECTIVITY",PP,REFLECTIVITY_glasscath_R7081, NUM);
   myST2_R7081->AddProperty("EFFICIENCY",PP,EFFICIENCY_glasscath,NUM);
   OpGlassCathodeSurface_R7081->SetMaterialPropertiesTable(myST2_R7081);

   //Tyvek - jl145
   G4MaterialPropertiesTable *myST3 = new G4MaterialPropertiesTable();
   myST3->AddProperty("RINDEX", PP, RINDEX_tyvek, NUM);
   myST3->AddProperty("SPECULARLOBECONSTANT", PP, TySPECULARLOBECONSTANT, NUM);
   myST3->AddProperty("SPECULARSPIKECONSTANT", PP, TySPECULARSPIKECONSTANT, NUM);
   myST3->AddProperty("BACKSCATTERCONSTANT", PP, TyBACKSCATTERCONSTANT, NUM);
   myST3->AddProperty("REFLECTIVITY", PP, TyREFLECTIVITY, NUM);
   myST3->AddProperty("EFFICIENCY", PP, EFFICIENCY_blacksheet, NUM);
   //use same efficiency as blacksheet, which is 0
   OpWaterTySurface->SetMaterialPropertiesTable(myST3);
   
   
   //--------------------------------------------------
   //  Polystyrene
   //--------------------------------------------------
   // arrays from /optical/wls example
/*   G4double photonEnergyArr[] =
   {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
    2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
    2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
    2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
    2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
    2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
    2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
    3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
    3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
    3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};
   const G4int nEntriesPhot = sizeof(photonEnergyArr)/sizeof(G4double);
   
  
   G4double refractiveIndexPS[] =
   { 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
     1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
     1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
     1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
     1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50};
   assert(sizeof(refractiveIndexPS) == sizeof(photonEnergyArr));
 
//   G4double absPS[] =
//   {2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
//    2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
//    2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
//    2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,
//    2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm,2.*cm};
   G4double absPS[] =
   {124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,
    124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,
    124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,
    124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,
    124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm,124.*cm};
   assert(sizeof(absPS) == sizeof(photonEnergyArr));
   
// changed as average abs length from mrdmodule.txt is 124cm!
// "The absorption length is the average distance traveled by a photon before being absorpted by the medium
 
  G4double scintilFast[] =
   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
   assert(sizeof(scintilFast) == sizeof(photonEnergyArr));

*/

  //alternative arrays of properties from LXe example
  const G4int nEntriesPhot = 4;
  G4double photonEnergyArr[] = {2.00*eV,2.87*eV,2.90*eV,3.47*eV}; 
  G4double refractiveIndexPS[] = { 1.5, 1.5, 1.5, 1.5};
  //G4double absPS[] = {124.*cm, 124.*cm, 124.*cm, 124.*cm};
  G4double absPS[] = {124.*cm, 124.*cm, 124.*cm, 124.*cm};
  G4double scintilFast[] = {0.00, 0.00, 1.00, 1.00};

   G4MaterialPropertiesTable* MPTPStyrene = new G4MaterialPropertiesTable();
   MPTPStyrene->AddProperty("RINDEX",photonEnergyArr,refractiveIndexPS,nEntriesPhot);
   MPTPStyrene->AddProperty("ABSLENGTH",photonEnergyArr,absPS,nEntriesPhot);
   MPTPStyrene->AddProperty("FASTCOMPONENT",photonEnergyArr, scintilFast,nEntriesPhot);
  // also available: slow component. not needed? Organic scintillators tend to have fast response. Need to also specify YIELDRATIO if
  // specifying both FAST and SLOW components. 
  MPTPStyrene->AddConstProperty("SCINTILLATIONYIELD", 10./keV);
  MPTPStyrene->AddConstProperty("RESOLUTIONSCALE",1.0);
  MPTPStyrene->AddConstProperty("FASTTIMECONSTANT", 10.*ns);
  Scinti->SetMaterialPropertiesTable(MPTPStyrene);
  Scinti->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
  
  /* if scintillation depends on particle type, see http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s02.html#sect.PhysProc.Photo */
  
  //--------------------------------------------------
  // Silicone (optical grease)
  //--------------------------------------------------
   G4double photonEnergyArrSil[] =
   {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
    2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
    2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
    2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
    2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
    2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
    2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
    3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
    3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
    3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};
   const G4int nEntriesPhotSil = sizeof(photonEnergyArrSil)/sizeof(G4double);
  
  G4double refractiveIndexSilicone[] =
  { 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
     1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
     1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
     1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46,
     1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46, 1.46};
  assert(sizeof(refractiveIndexSilicone) == sizeof(photonEnergyArrSil));
     
  G4double absSil[] =
  {20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
   20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
   20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
   20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
   20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m};
  assert(sizeof(absSil) == sizeof(photonEnergyArrSil));
  
  
  // Add entries into properties table
  G4MaterialPropertiesTable* mptSilicone = new G4MaterialPropertiesTable();
  mptSilicone->AddProperty("RINDEX",photonEnergyArrSil,refractiveIndexSilicone,nEntriesPhotSil);
  mptSilicone->AddProperty("ABSLENGTH",photonEnergyArrSil,absSil,nEntriesPhotSil);
  
  Silicone->SetMaterialPropertiesTable(mptSilicone);
  
  // =============================
  // DEFINE NCV liquid properties (from discussion with Eljen)
   
   const G4int NumEntriesNCV = 35;
   // Energy table
    G4double ScintNCV_Energy[NumEntriesNCV] = {1.2398E-6/380*GeV,1.2398E-6/385*GeV,1.2398E-6/390*GeV,1.2398E-6/395*GeV,1.2398E-6/400*GeV,1.2398E-6/405*GeV,1.2398E-6/410*GeV,1.2398E-6/415*GeV,1.2398E-6/420*GeV,1.2398E-6/425*GeV,1.2398E-6/430*GeV,1.2398E-6/435*GeV,1.2398E-6/440*GeV,1.2398E-6/445*GeV,1.2398E-6/450*GeV,1.2398E-6/455*GeV,1.2398E-6/460*GeV,1.2398E-6/465*GeV,1.2398E-6/470*GeV,1.2398E-6/475*GeV,1.2398E-6/480*GeV,1.2398E-6/485*GeV,1.2398E-6/490*GeV,1.2398E-6/495*GeV,1.2398E-6/500*GeV,1.2398E-6/505*GeV,1.2398E-6/510*GeV,1.2398E-6/515*GeV,1.2398E-6/520*GeV,1.2398E-6/525*GeV,1.2398E-6/530*GeV,1.2398E-6/535*GeV,1.2398E-6/540*GeV,1.2398E-6/545*GeV,1.2398E-6/550*GeV};
    
   // Reflexion index
   G4double RI_ScintNCV[NumEntriesNCV] =  {1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49};
   
   // Emission spectrum
   G4double ScintNCV_WB_LS[NumEntriesNCV] = {0.000,0.105,0.410,0.820,0.990,0.950,0.820,0.810,0.920,0.940,0.880,0.750,0.610,0.550,0.510,0.485,0.435,0.270,0.210,0.175,0.150,0.130,0.111,0.096,0.083,0.070,0.060,0.050,0.043,0.038,0.030,0.025,0.020,0.015,0.010};
   
   
   G4MaterialPropertiesTable* myMPT8 = new G4MaterialPropertiesTable();
 

   myMPT8->AddProperty("RINDEX", ENERGY_water, RINDEX1, NUMENTRIES_water);
   myMPT8->AddProperty("ABSLENGTH",ENERGY_water, ABSORPTION_water, NUMENTRIES_water);
   myMPT8->AddProperty("RAYLEIGH",ENERGY_water,RAYLEIGH_water,NUMENTRIES_water);

   myMPT8->AddProperty("FASTCOMPONENT",ScintNCV_Energy, ScintNCV_WB_LS,NumEntriesNCV);
   myMPT8->AddProperty("SLOWCOMPONENT",ScintNCV_Energy, ScintNCV_WB_LS,NumEntriesNCV);
   
   myMPT8->AddConstProperty("SCINTILLATIONYIELD",9570./MeV); // EJ335 is 55% of anthracene (which is 17400 per MeV)
   myMPT8->AddConstProperty("RESOLUTIONSCALE",.01);
   myMPT8->AddConstProperty("FASTTIMECONSTANT", 1.23*ns);
   myMPT8->AddConstProperty("SLOWTIMECONSTANT",9.26*ns);
   myMPT8->AddConstProperty("YIELDRATIO",1.0);
   
   NCVliquid->SetMaterialPropertiesTable(myMPT8);
   NCVliquid->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
  

}
