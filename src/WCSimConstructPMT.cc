//#ifndef __CONSTRUCT_PMT_VERBOSE__
//#define __CONSTRUCT_PMT_VERBOSE__ 1
//#endif

#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4SDManager.hh"
#include "WCSimWCSD.hh"
#include "WCSimPMTObject.hh"

#include "G4SystemOfUnits.hh"

//PMT logical volume construction.

WCSimDetectorConstruction::PMTMap_t WCSimDetectorConstruction::PMTLogicalVolumes;

G4LogicalVolume* WCSimDetectorConstruction::ConstructPMT(G4String PMTName, G4String CollectionName, G4String detectorElement)
{
  G4cout <<"Making PMTKey (PMTName="<<PMTName<<", CollectionName="<<CollectionName<<G4endl;
#ifdef __CONSTRUCT_PMT_VERBOSE__
  G4cout<<"Making PMTKey_t (PMTName="<<PMTName<<", CollectionName="<<CollectionName<<") "<<G4endl;
#endif
  PMTKey_t key(PMTName,CollectionName);
  PMTMap_t::iterator it = PMTLogicalVolumes.find(key);
  if (it != PMTLogicalVolumes.end()) {
#ifdef __CONSTRUCT_PMT_VERBOSE__
      G4cout<<"This key already exists in the PMTLogicalVolumes map. Restoring it."<<G4endl;
#endif
      //G4cout << "Restore PMT" << G4endl;
      return it->second;
  }
  //G4cout << "Create PMT" << G4endl;

#ifdef __CONSTRUCT_PMT_VERBOSE__
G4cout<<"Key not found; creating a new PMT logical volume"<<G4endl;
#endif

if (Vis_Choice == "RayTracer"){
    // Blue wireframe visual style
    // Used in the RayTracer visualizer
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
  WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 
}

else
   { // Gray wireframe visual style
    // used in OGLSX visualizer
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  WCPMTVisAtt->SetForceWireframe(true);}

  G4double expose;
  G4double radius;
  G4double glassThickness;
#ifdef __CONSTRUCT_PMT_VERBOSE__
  G4cout<<"Getting pointer to PMT object corresponding to CollectionName "<<CollectionName<<G4endl;
#endif
  WCSimPMTObject *PMT = GetPMTPointer(CollectionName);
#ifdef __CONSTRUCT_PMT_VERBOSE__
  G4cout<<"Retrieved PMT with name "<<PMT->GetPMTName()<<G4endl;
#endif
  expose = PMT->GetExposeHeight();
  radius = PMT->GetRadius();
  glassThickness = PMT->GetPMTGlassThickness();

  G4double sphereRadius = (expose*expose+ radius*radius)/(2*expose);
  G4double PMTOffset =  sphereRadius - expose;

  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is on the blacksheet, faces positive z-direction.
  
#ifdef __CONSTRUCT_PMT_VERBOSE__
G4cout<<"Making the geometry"<<G4endl;
#endif

  G4double PMTHolderZ[2] = {0, expose};
  G4double PMTHolderR[2] = {radius, radius};
  G4double PMTHolderr[2] = {0,0};
  G4Polycone* solidWCPMT = 
   new G4Polycone("WCPMT",                    
                  0.0*deg,
                  360.0*deg,
                  2,
                  PMTHolderZ,
                  PMTHolderr, // R Inner
                  PMTHolderR);// R Outer

  G4LogicalVolume* logicWCPMT =
    new G4LogicalVolume(    solidWCPMT,
                            G4Material::GetMaterial("Water"),
                            "WCPMT",
                            0,0,0);

if (Vis_Choice == "RayTracer"){
// Makes the volume containing the PMT visible, solid, and forces the auxiliary edges to be viewed.
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
  WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 

    logicWCPMT->SetVisAttributes(WCPMTVisAtt);}

else{
// Makes the volume containg the PMT invisible for normal visualization
    logicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);}

  //Need a volume to cut away excess behind blacksheet
  G4Box* solidCutOffTubs =
      new G4Box(    "cutOffTubs",
            sphereRadius+1.*cm,
            sphereRadius+1.*cm,
            PMTOffset);


  //Create PMT Interior
  G4Sphere* tmpSolidInteriorWCPMT =
      new G4Sphere(    "tmpInteriorWCPMT",
                       0.0*m,(sphereRadius-glassThickness),
                       0.0*deg,360.0*deg,
                       0.0*deg,90.0*deg);

  G4SubtractionSolid* solidInteriorWCPMT =
      new G4SubtractionSolid(    "InteriorWCPMT",
                    tmpSolidInteriorWCPMT,
                    solidCutOffTubs);

  // "Air" here is not true air, but a modified material
  // with n = 1 and a very short absorption length
  G4LogicalVolume* logicInteriorWCPMT =
    new G4LogicalVolume(    solidInteriorWCPMT,
                    G4Material::GetMaterial("Air"),
                    "InteriorWCPMT",
                    0,0,0);

  G4VPhysicalVolume* physiInteriorWCPMT =
      new G4PVPlacement(0,
                  G4ThreeVector(0, 0, -1.0*PMTOffset),
                  logicInteriorWCPMT,
                  "InteriorWCPMT",
                  logicWCPMT,
                  false,
                  0);

if (Vis_Choice == "RayTracer"){
// Adding color and forcing the inner portion of the PMT's to be viewed
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
  WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 

  logicInteriorWCPMT->SetVisAttributes(WCPMTVisAtt);}

else {
// Making the inner portion of the detector invisible for OGLSX visualization
  logicInteriorWCPMT->SetVisAttributes(G4VisAttributes::Invisible);}


  //Create PMT Glass Face
  G4Sphere* tmpGlassFaceWCPMT =
      new G4Sphere(    "tmpGlassFaceWCPMT",
                       (sphereRadius-glassThickness),
                       sphereRadius,
                       0.0*deg,360.0*deg,
                       0.0*deg,90.0*deg);
  
  G4SubtractionSolid* solidGlassFaceWCPMT =
      new G4SubtractionSolid(    CollectionName,
                                 tmpGlassFaceWCPMT,
                                 solidCutOffTubs); 

  G4LogicalVolume *logicGlassFaceWCPMT;
  if (PMTName == "R7081"){
    logicGlassFaceWCPMT = new G4LogicalVolume(solidGlassFaceWCPMT,
				G4Material::GetMaterial("GlassR7081"),
				CollectionName,
				0,0,0);
  } else {
   logicGlassFaceWCPMT = new G4LogicalVolume(    solidGlassFaceWCPMT,
                            G4Material::GetMaterial("Glass"),
                            CollectionName,
                            0,0,0);
  }

  G4VPhysicalVolume* physiGlassFaceWCPMT =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, -1.0*PMTOffset),
                        logicGlassFaceWCPMT,
                        CollectionName,
                        logicWCPMT,
                        false,
                        0,
                        checkOverlaps);

// For either visualization type, logicGlassFaceWCPMT will either be visible or invisible depending on which
// line is commented at the end of the respective if statements

  if (Vis_Choice == "OGLSX")
   { // Gray wireframe visual style
    // used in OGLSX visualizer
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  WCPMTVisAtt->SetForceWireframe(true);
  //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
  logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);}

  if (Vis_Choice == "RayTracer"){
    // Blue wireframe visual style
    // Used in the RayTracer visualizer
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  WCPMTVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
  WCPMTVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown 
  //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

  logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);}

  else
   { // Gray wireframe visual style
    // used in OGLSX visualizer
  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  WCPMTVisAtt->SetForceWireframe(true);
  //logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
  logicGlassFaceWCPMT->SetVisAttributes(WCPMTVisAtt);}

#ifdef __CONSTRUCT_PMT_VERBOSE__
  G4cout<<"Finished making the geometry"<<G4endl;
#endif
  // Instantiate a new sensitive detector 
  // and register this sensitive detector volume with the SD Manager. 
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String SDName = "/WCSim/";
  SDName += CollectionName;

#ifdef __CONSTRUCT_PMT_VERBOSE__
  G4cout<<"Searching for sensitive detector "<<SDName<<G4endl;
#endif
  // If there is no such sensitive detector with that SDName yet,
  // make a new one
  if( ! SDman->FindSensitiveDetector(SDName, false) ) {
#ifdef __CONSTRUCT_PMT_VERBOSE__
    G4cout<<"Sensitive detector not found. Making aWCPMT = new WCSimWCSD("<<CollectionName<<", "<<SDName
          <<", "<<"{DetectorConstruction}"<<", "<<detectorElement<<"), and adding to the SD manager"<<G4endl;
#endif
    aWCPMT = new WCSimWCSD(CollectionName,SDName,this,detectorElement );
    SDman->AddNewDetector( aWCPMT );
  }

  logicGlassFaceWCPMT->SetSensitiveDetector( aWCPMT );

  PMTLogicalVolumes[key] = logicWCPMT;

#ifdef __CONSTRUCT_PMT_VERBOSE__
  G4cout<<"Adding optical surfaces to cathode"<<G4endl;
#endif
  //Add Logical Border Surface
    if (PMTName == "R7081"){
  new G4LogicalBorderSurface("GlassCathodeSurface_R7081",
                             physiGlassFaceWCPMT,
                             physiInteriorWCPMT,
                             OpGlassCathodeSurface_R7081);
} else {
  new G4LogicalBorderSurface("GlassCathodeSurface",
                             physiGlassFaceWCPMT,
                             physiInteriorWCPMT,
                             OpGlassCathodeSurface);
}
#ifdef __CONSTRUCT_PMT_VERBOSE__
  G4cout<<"returning logical volume"<<G4endl;
#endif
  return logicWCPMT;
}
