/* vim:set noexpandtab tabstop=4 wrap */
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: WCLiteDetectorConstruction.cc,v 1.15 2006/06/29 17:54:17 gunter Exp $
// GEANT4 tag $Name: geant4-09-01-patch-03 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "WCSimDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4PVParameterised.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SDManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4ReflectionFactory.hh"
#include "G4SystemOfUnits.hh"

#include "WCSimWCSD.hh"
#include "WCSimPMTObject.hh"
#include "WCSimLAPPDObject.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4LogicalVolume* WCSimDetectorConstruction::ConstructTestTank()
{
	
	//============================================================
	//                  Define (Gd Loaded) Water
	//============================================================
	G4String watertype;
	if (WCAddGd){
		watertype = "Doped Water";
		G4cout<<"Tank is full of ***Doped Water***"<<G4endl;
	} else {
		watertype = "Water";
		G4cout<<"Tank is full of Refreshing Water"<<G4endl;
	}
	
	//============================================================
	//                  Create Experimental Hall
	//============================================================
	G4Box* expHall_box = new G4Box("Hall",5*m,5*m,5*m);
	G4LogicalVolume* expHall_log = new G4LogicalVolume(expHall_box,
	                                                   G4Material::GetMaterial("Air"),
	                                                   "Hall",
	                                                   0,0,0);
	
	//============================================================
	//                     Construct Tank
	//============================================================
	G4RotationMatrix* rotm = new G4RotationMatrix();
	G4LogicalVolume* waterTank_log;     // steel tank physical volume
	G4VPhysicalVolume* waterTank_phys;  // steel tank physical volume
	G4cout <<"Constructing LAPPD Test Tank with version " << WCDetectorName << G4endl;
	
	waterTank_log = ConstructANNIETestTank();
	
	waterTank_phys = new G4PVPlacement(rotm,
	                                   G4ThreeVector(0,0,0),
	                                   waterTank_log,
	                                   "waterTank",
	                                   expHall_log,
	                                   false,
	                                   0);
	
	//============================================================
	//                           Tank Liner                       
	//============================================================
	// Get logical and physical volumes of the water, within the steel barrel
	G4VPhysicalVolume* water_phys;      // water volume within it
	G4LogicalVolume* water_log;         // water logical volume
	int nDaughters = waterTank_log->GetNoDaughters();
	for (int iDaughter = 0; iDaughter < nDaughters; iDaughter++){
		G4VPhysicalVolume* nextdaughterphys = waterTank_log->GetDaughter(iDaughter);
		G4LogicalVolume* nextdaughterlog = nextdaughterphys->GetLogicalVolume();
		if(nextdaughterlog->GetName()=="WCBarrel"){
			water_log=nextdaughterlog;
			water_phys=nextdaughterphys;
			break;
		}
	}
	
	// place an optical blacksheet surface between the steel and water to kill reflections
	G4LogicalBorderSurface* LinerSurface_log = new G4LogicalBorderSurface("LinerSurface",
	                                                                      water_phys,
	                                                                      waterTank_phys,
	                                                                      OpWaterBSSurface);
	bordersurfaces.push_back(LinerSurface_log);
	
	// Return
	// ======
	expHall_log->SetVisAttributes (G4VisAttributes::Invisible); // set hall volume invisible
	return expHall_log;                                         // return a logical volume
	
}

G4LogicalVolume* WCSimDetectorConstruction::ConstructANNIETestTank()
{
	G4cout << "**** Building LAPPD Test Tank ****" << G4endl;
	G4cout << "ConstructANNIETestTank" << G4endl;
	
	debugMode = false;
	
	//-----------------------------------------------------
	//---------------------Steel Container----------------
	//-----------------------------------------------------
	
	G4double SteelThickness = 5.0*mm;
	
	G4Box* solidWC = new G4Box("WC",
	                            WCIDDiameter+SteelThickness,
	                            WCIDDiameter+SteelThickness,
	                            WCIDHeight  +SteelThickness);
	
	G4LogicalVolume* logicWC = new G4LogicalVolume(solidWC,
	                                               G4Material::GetMaterial("StainlessSteel"),
	                                               "WC",
	                                                0,0,0);
	
	G4VisAttributes* showColor = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logicWC->SetVisAttributes(showColor);
	
	//-----------------------------------------------------
	//---------------Water in the Tank-------------------
	//-----------------------------------------------------
	
	// Decide if adding Gd
	water = (WCAddGd) ? "Doped Water" : "Water";
	
	G4Box* solidWCBarrel = new G4Box("WCBarrel",
	                                  WCIDDiameter,
	                                  WCIDDiameter,
	                                  WCIDHeight);
	
	logicWCBarrel = new G4LogicalVolume(solidWCBarrel,
	                                    G4Material::GetMaterial(water),
	                                    "WCBarrel",
	                                    0,0,0);
	
	physiWCBarrel = new G4PVPlacement(0,
	                                  G4ThreeVector(0.,0.,0.),
	                                  logicWCBarrel,
	                                  "WCBarrel",
	                                  logicWC,
	                                  false,
	                                  0);
	
	//-----------------------------------------------------
	//--------------- The PMT and LAPPD -------------------
	//-----------------------------------------------------
	
	// make a dummy PMT so we don't end up with an empty vector somewhere where it's not handled properly
	// (may not be necessary)
	auto atankcollection = WCTankCollectionNames.front();
	G4String thepmtname = WCPMTNameMap.at(atankcollection);
	G4LogicalVolume* logicWCPMT= ConstructPMT(thepmtname, atankcollection, "tank");
	
	// Make the LAPPD
	G4LogicalVolume* logicWCLAPPD = ConstructLAPPD(WCLAPPDName, WCIDCollectionName2);
	
	// Make them visible
	G4VisAttributes* WClogic = new G4VisAttributes(G4Colour(0.4,0.0,0.8));
	WClogic->SetForceSolid(true);
	WClogic->SetForceAuxEdgeVisible(true);
	logicWCPMT->SetVisAttributes(WClogic);
	logicWCLAPPD->SetVisAttributes(WClogic);
	
	//////-------------------------------------------------
	//////----------- PMT and LAPPD placement -------------
	//////-------------------------------------------------
	
	// Place the PMT outside the tank so it doesn't block any light or create reflections
	G4RotationMatrix *pmt_rot = new G4RotationMatrix;
	G4ThreeVector PMTPosition(0,0,-WCIDHeight);
	G4VPhysicalVolume *physicalWCPMT = new G4PVPlacement(pmt_rot,            //its rotation
	                                                     PMTPosition,        //its position
	                                                     logicWCPMT,         //its logical volume
	                                                     "WCPMT",            //its name
	                                                     logicWCBarrel,      //its mother volume
	                                                     false,              //no boolean operations
	                                                     0,                  //ID for this PMT
	                                                     true);              //check overlaps
	
	// Place the LAPPD dead center. Sure, why not.
	G4ThreeVector LAPPDPosition(0,0,0);
	G4VPhysicalVolume* physiWCBarrelLAPPD = new G4PVPlacement(pmt_rot,       // its rotation
	                                                          LAPPDPosition, // its position
	                                                          logicWCLAPPD,  // its logical volume
	                                                          "WCLAPPD",     // its name
	                                                          logicWCBarrel, // its mother volume
	                                                          false,         // no boolean operations
	                                                          0,             // a unique copy number
	                                                          false);        // don't check overlaps
	
	return logicWC;
	
}

