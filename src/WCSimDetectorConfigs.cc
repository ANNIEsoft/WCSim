//  -*- mode:c++; tab-width:4;  -*-
#include "WCSimDetectorConstruction.hh"
#include "WCSimPMTObject.hh"
#include "WCSimLAPPDObject.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

/***********************************************************
 *
 * This file contains the setup functions for various 
 * detector configurations.  These can be set up by 
 * default in WCSimDetectorConstruction.cc or called
 * in mac files by adding them to WCSimDetectorMessenger.cc.
 *
 * Sourcefile for the WCSimDetectorConstruction class
 *
 ***********************************************************/

void WCSimDetectorConstruction::SetANNIEPhase1Geometry()
{
  isANNIE=true;
  WCDetectorName = "ANNIEp1";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCSimPMTObject * PMT = CreatePMTObject("PMT8inch", WCIDCollectionName);
 
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCAddGd = false;
  // TODO: conver these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;  	// 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;				// 13ft exactly (TSW blueprint) = 3.96m tall; hz is HALF height
  tankzoffset = 15.70*cm;		//15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  constructmrd = true;
  constructveto = true;
  
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  WCPosition = 0.;	//??
  
//  WCIDDiameter          = 1.77*m; 				// the shortest distance to the centre of a (hexagonal) cell wall;
//												// from blueprints, (1+1/sqrt(2))*40.81" = 69" or 1.77m
//  WCIDHeight            = 3.96*m; 				// full height
//  WCBarrelPMTOffset     = 0.0715*m; 			// offset of first barrel ring from tank caps
//  WCBarrelNumPMTHorizontal  = 16; 				// 2 PMTs per panel around an octagonal inner structure
//  WCBarrelNRings        = 5;					// 5 rings of 16 = 80 PMTs + 60 on each end cap = 200 total PMTs
//  WCPMTperCellHorizontal= 2;					// 
//  WCPMTperCellVertical  = 1;					// assume each row corresponds to a cell - significance of cells?
//  WCCapPMTSpacing       = 2*(WCPMTRadius+15.*mm);	// something like that looks about right
//  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;// 
//  WCBlackSheetThickness = 1.01*mm;				// liner is 40 mil. which is, of course, 40 milli inches. 
}

void WCSimDetectorConstruction::SetANNIEPhase2Geometry()
{
  isANNIE=true;
  WCDetectorName = "ANNIEp2";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  //disabling top veto is an option set in tuning_parameters.mac!
 
  WCSimPMTObject * PMT = CreatePMTObject("PMT8inch", WCIDCollectionName);
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
   
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();

  WCAddGd = true;
  // TODO: conver these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;  	// 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;				// 13ft exactly (TSW blueprint) = 3.96m tall; hy is HALF height
  tankzoffset = 15.70*cm;		//15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  constructmrd = true;
  constructveto = true;
  
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
  MRDPMTName = MRDPMT->GetPMTName();
  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
  MRDPMTRadius = MRDPMT->GetRadius();
  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
  FACCPMTName = FACCPMT->GetPMTName();
  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
  FACCPMTRadius = FACCPMT->GetRadius();
  
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  // from blueprints of inner structure diameter is 106.64", hexagonal side is 40.81", 100.57" from face-to-face
  // (note: OUTER dimensions, including steel bar width)
  WCIDDiameter          = 2.554*m; 				// 2x shortest distance to the centre of a (hexagonal) cell wall;
												// from blueprints, this is 100.57" = 255.4cm
  WCIDHeight            = 3.96*m; 				// full height
  WCBarrelPMTOffset     = 0.0715*m; 			// offset of first barrel ring from tank caps
  WCBarrelNumPMTHorizontal  = 32; 				// 2 PMTs per panel around an octagonal inner structure
  WCBarrelNRings        = 5;					// 5 rings of 16 = 80 PMTs + 60 on each end cap = 200 total PMTs
  WCPMTperCellHorizontal= 4;					// 
  WCPMTperCellVertical  = 1;					// assume each row corresponds to a cell - significance of cells?
  G4double CapPMTRadius = std::max(WCPMTRadius, WCLAPPDRadius);
  WCCapPMTSpacing       = 2*(CapPMTRadius+25.*mm);	// something like that looks about right    prev 15*mm
  // 15->20 produces 8x8 with lappds, but they overlap significantly.
  //WCCapEdgeLimit        = 4.9*WCCapPMTSpacing;	// breaks geometry... 
  WCCapEdgeLimit        = WCIDDiameter/2.0 - CapPMTRadius - 10*cm;    // added -5
  WCBlackSheetThickness = 1.01*mm;				// liner is 40 mil. which is, of course, 40 milli inches. 
  //WCBarrelNRingsLAPPD     = 2;
  WCBarrelLAPPDOffset     = 0.2*m; 			// offset of first barrel ring from tank caps
  WCCapLAPPDSpacing       = 2*(WCLAPPDRadius+100.*mm);
  //WCBarrelNumLAPPDHorizontal  = 1;  			// it should result to: 4 rings of 1 LAPPD = 4LAPPDs (?)
  WCLAPPDperCellHorizontal= 4;					// 
  WCLAPPDperCellVertical  = 1;					// assume each row corresponds to a cell - significance of cells?
  compressionfactor        = 0.9;				// how much to squeeze PMTs together on an octagon face FIXME
}

void WCSimDetectorConstruction::SetANNIEPhase2Geometryv2()
{
  // 24 x 10" PMTs on the top, arranged in 4 rows of 3 interleaved with 3 rows of 4
  // 24 x 11" PMTs on the bottom, arranged as above
  // 40 x 8" + 40 x 10" PMTs in the barrel:
  //   each ocatgon face consists of 7 rows, alternating 1 and 2 PMTs,
  //   with rows of diameter 8", 8", 10", 10", 10", 8", 10"
  // i.e. symmetrical about the centre line, except for the outermost ring, which is 10" at the bottom
  // but 8" at the top, and with greatest photosensor coverage in the middle of the barrel.
  // This uses 2 more 11" ETEL PMTs on the top than we have, 4 more 10" LUX PMTs on the bottom than we have,
  // and 5 fewer 10" watchboy PMTs in the barrel than we have
  // note that due to a bug, top and bottom caps were swapped. This has been left as-is (change zflip to fix)
  
  WCTankCollectionNames.clear();
  WCPMTNameMap.clear();
  WCPMTRadiusMap.clear();
  
  isANNIE=true;
  WCDetectorName = "ANNIEp2v2";
  
  // tank PMT collections - one for each type of PMT.
  WCIDCollectionName="WCIDCollectionNameIsUnused";  // some default.
  // the order in the maps is important due to hardcoded hacking in ConstructCylinder
  std::string WCIDCollectionName_R7081 = WCDetectorName +"-glassFaceWCPMT_R7081";
  std::string WCIDCollectionName_D784KFLB = WCDetectorName +"-glassFaceWCPMT_D784KFLB";
  std::string WCIDCollectionName_R5912HQE = WCDetectorName +"-glassFaceWCPMT_R5912HQE";
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081);     // 10" LUX/Watchboy
  WCTankCollectionNames.push_back(WCIDCollectionName_D784KFLB);  // 11" HQE LBNE
  WCTankCollectionNames.push_back(WCIDCollectionName_R5912HQE);  // 8"  HQE new
  
  // other detector element collections
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  
  // create the tank PMTs and get their info
  WCSimPMTObject* PMT_R7081 = CreatePMTObject("R7081", WCIDCollectionName_R7081);
  G4String WCPMTName_R7081 = PMT_R7081->GetPMTName();
  G4double WCPMTExposeHeight_R7081 = PMT_R7081->GetExposeHeight();
  G4double WCPMTRadius_R7081 = PMT_R7081->GetRadius();
  
  WCSimPMTObject* PMT_D784KFLB = CreatePMTObject("D784KFLB", WCIDCollectionName_D784KFLB);
  G4String WCPMTName_D784KFLB = PMT_D784KFLB->GetPMTName();
  G4double WCPMTExposeHeight_D784KFLB = PMT_D784KFLB->GetExposeHeight();
  G4double WCPMTRadius_D784KFLB = PMT_D784KFLB->GetRadius();
  
  WCSimPMTObject* PMT_R5912HQE = CreatePMTObject("R5912HQE", WCIDCollectionName_R5912HQE);
  G4String WCPMTName_R5912HQE = PMT_R5912HQE->GetPMTName();
  G4double WCPMTExposeHeight_R5912HQE = PMT_R5912HQE->GetExposeHeight();
  G4double WCPMTRadius_R5912HQE = PMT_R5912HQE->GetRadius();
  
  // store info in the maps
  WCPMTNameMap.emplace(WCIDCollectionName_R7081, WCPMTName_R7081);
  WCPMTNameMap.emplace(WCIDCollectionName_D784KFLB, WCPMTName_D784KFLB);
  WCPMTNameMap.emplace(WCIDCollectionName_R5912HQE, WCPMTName_R5912HQE);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081, WCPMTRadius_R7081);
  WCPMTRadiusMap.emplace(WCIDCollectionName_D784KFLB, WCPMTRadius_D784KFLB);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R5912HQE, WCPMTRadius_R5912HQE);
  WCPMTRadius = WCPMTRadius_D784KFLB;         // the LBNE PMTs are largest at 11"
  WCPMTExposeHeight = WCPMTRadius_D784KFLB;   // the largest expose height of barrel PMTs TODO << is it LBNE?
  
   
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();

  WCAddGd = true;
  // TODO: conver these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;  	// 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;				// 13ft exactly (TSW blueprint) = 3.96m tall; hy is HALF height
  tankzoffset = 15.70*cm;		//15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  GDMLInnerStructureFilename = "InnerStructure.gdml";
  addGDMLinnerstructure = false;
  constructmrd = true;
  constructveto = true;
  
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
  MRDPMTName = MRDPMT->GetPMTName();
  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
  MRDPMTRadius = MRDPMT->GetRadius();
  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
  FACCPMTName = FACCPMT->GetPMTName();
  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
  FACCPMTRadius = FACCPMT->GetRadius();
  
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  // from blueprints of inner structure diameter is 106.64", hexagonal side is 40.81", 100.57" from face-to-face
  // (note: OUTER dimensions, including steel bar width)
  WCIDDiameter             = 2.554*m;		// 2x shortest distance to the centre of a ocatgonal
											// cell wall- from blueprints, this is 100.57" = 255.4cm
  WCIDHeight               = 3.96*m;		// full height
  WCBarrelPMTOffset        = 0.0715*m;		// offset of first barrel ring from tank caps
  
  // NOTE: ANNIEp2v2 bypasses the use of multiple identical rings to fill the barrel
  // instead, there is just ONE main barrel ring, in which 5 rings of PMTs are placed manually
  // and TWO border rings, in each of which one ring of PMTs is manually placed
  WCBarrelNumPMTHorizontal = 16;			// this + WCPMTperCellHorizontal define num detector sides
  WCPMTperCellHorizontal   = 2;				// ^ for octagonal inner structure it must produce 8
  WCBarrelNRings           = 7;				// can't use replication of rings unless they're identical
  											// hack equal size barrel/border rings by setting height of ring
  											// according to real # rings, but overriding replication in
  											// ConstructCylinder
  WCPMTperCellVertical     = 5;				// There will be 3 + 4 = 7 actual rings, 
  											// but we need to also populate border rings
  											// 5 vertical PMTs placed within the one main barrel ring
  											// border rings have 1 vertical PMT per cell
  
  G4double CapPMTRadius    = WCPMTRadius_D784KFLB;		// 11" on top are largest cap PMTs
  WCCapPMTSpacing          = 2*(CapPMTRadius+25.*mm);
  WCCapEdgeLimit           = WCIDDiameter/2.0 - CapPMTRadius - 10*cm;
  WCBlackSheetThickness    = 1.01*mm;					// liner is 40 mil. = 40 milli inches. 
  
  // no LAPPDs in this configuration
  WCBarrelLAPPDOffset     = 0.*m; 
  WCCapLAPPDSpacing       = 0.*m;
  WCLAPPDperCellHorizontal= 0;
  WCLAPPDperCellVertical  = 0;
}

void WCSimDetectorConstruction::SetANNIEPhase2Geometryv3()
{
  // 20 x 10" LUX PMTs on the bottom, arranged in a cross with a 4x4 rectangle, extended on it's narrow
  // dimension by an extra 2 PMTs in the centre on either side
  // 19 x 11" LBNE PMTs on the top, arranged in a ring of 16, with 3 on the central hatch
  // 40 x 8" + 45 x 10" PMTs in the barrel:
  //   4 central rings of 2 PMTs each (one of each type)  = 4 x 2 x 8 = 64 PMTs (21 left)

      // top ring has one PMT per cell, alternating PMT type is not possible to implement
      // (cells in the ring must be the same), so top is... 10" PMTs
  //   2 rings of 1 PMTs each (alternating type) = 2 x 1 x 8 = 16 PMTs (5 x 10" left)
  //   --> put into ... the bottom ring i guess?
  // 
  // PMTs on hand:
  // Side: 45 x 10" (Watchboy, R7081) + 40 x 8" (new, HQE(?) R5912)
  // Bottom: 20 x 10” PMTs (LUX, R7081)
  // Top: 19 x 11” PMTs (LBNE, HQE D784KFLB)  // NB. UPDATED NUMBER, DOWN FROM 22
  
  WCTankCollectionNames.clear();
  WCPMTNameMap.clear();
  WCPMTRadiusMap.clear();
  
  isANNIE=true;
  WCDetectorName = "ANNIEp2v3";
  
  // tank PMT collections - one for each type of PMT.
  WCIDCollectionName="WCIDCollectionNameIsUnused";  // some default.
  // the order in the maps is important due to hardcoded hacking in ConstructCylinder
  std::string WCIDCollectionName_R7081 = WCDetectorName +"-glassFaceWCPMT_R7081";
  std::string WCIDCollectionName_D784KFLB = WCDetectorName +"-glassFaceWCPMT_D784KFLB";
  std::string WCIDCollectionName_R5912HQE = WCDetectorName +"-glassFaceWCPMT_R5912HQE";
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081);     // 10" LUX/Watchboy
  WCTankCollectionNames.push_back(WCIDCollectionName_D784KFLB);  // 11" HQE LBNE
  WCTankCollectionNames.push_back(WCIDCollectionName_R5912HQE);  // 8"  HQE new
  
  // other detector element collections
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  
  // create the tank PMTs and get their info
  WCSimPMTObject* PMT_R7081 = CreatePMTObject("R7081", WCIDCollectionName_R7081);
  G4String WCPMTName_R7081 = PMT_R7081->GetPMTName();
  G4double WCPMTExposeHeight_R7081 = PMT_R7081->GetExposeHeight();
  G4double WCPMTRadius_R7081 = PMT_R7081->GetRadius();
  
  WCSimPMTObject* PMT_D784KFLB = CreatePMTObject("D784KFLB", WCIDCollectionName_D784KFLB);
  G4String WCPMTName_D784KFLB = PMT_D784KFLB->GetPMTName();
  G4double WCPMTExposeHeight_D784KFLB = PMT_D784KFLB->GetExposeHeight();
  G4double WCPMTRadius_D784KFLB = PMT_D784KFLB->GetRadius();
  
  WCSimPMTObject* PMT_R5912HQE = CreatePMTObject("R5912HQE", WCIDCollectionName_R5912HQE);
  G4String WCPMTName_R5912HQE = PMT_R5912HQE->GetPMTName();
  G4double WCPMTExposeHeight_R5912HQE = PMT_R5912HQE->GetExposeHeight();
  G4double WCPMTRadius_R5912HQE = PMT_R5912HQE->GetRadius();
  
  // store info in the maps
  WCPMTNameMap.emplace(WCIDCollectionName_R7081, WCPMTName_R7081);
  WCPMTNameMap.emplace(WCIDCollectionName_D784KFLB, WCPMTName_D784KFLB);
  WCPMTNameMap.emplace(WCIDCollectionName_R5912HQE, WCPMTName_R5912HQE);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081, WCPMTRadius_R7081);
  WCPMTRadiusMap.emplace(WCIDCollectionName_D784KFLB, WCPMTRadius_D784KFLB);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R5912HQE, WCPMTRadius_R5912HQE);
  WCPMTRadius = WCPMTRadius_D784KFLB;             // the LBNE PMTs are largest at 11"
  WCPMTExposeHeight = WCPMTRadius_R7081;   // the largest expose height of barrel PMTs
  
   
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();

  WCAddGd = true;
  // TODO: convert these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;		// 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;				// 13ft exactly (TSW blueprint) = 3.96m tall; hy is HALF height
  tankzoffset = 15.70*cm;		//15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  GDMLInnerStructureFilename = "InnerStructure.gdml";
  addGDMLinnerstructure = false;
  constructmrd = true;
  constructveto = true;
  
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
  MRDPMTName = MRDPMT->GetPMTName();
  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
  MRDPMTRadius = MRDPMT->GetRadius();
  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
  FACCPMTName = FACCPMT->GetPMTName();
  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
  FACCPMTRadius = FACCPMT->GetRadius();
  
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  // from blueprints of inner structure diameter is 106.64", hexagonal side is 40.81", 100.57" from face-to-face
  // (note: OUTER dimensions, including steel bar width)
  WCIDDiameter             = 2.554*m;		// 2x shortest distance to the centre of a ocatgonal
											// cell wall- from blueprints, this is 100.57" = 255.4cm
  WCIDHeight               = 3.96*m;		// full height
  WCBarrelPMTOffset        = 0.0715*m;		// offset of first barrel ring from tank caps
  WCBarrelNumPMTHorizontal = 16;			// this + WCPMTperCellHorizontal define num detector sides
  WCPMTperCellHorizontal   = 2;				// ^ for octagonal inner structure it must produce 8
  WCPMTperCellVertical     = 1;				// 4 main rings with 1 PMT per cell
  WCBarrelNRings           = 6;				// 2 border rings with 1 PMT per cell
  
  G4double CapPMTRadius    = WCPMTRadius_D784KFLB;		// 11" on top are largest cap PMTs
  WCCapPMTSpacing          = 3.*(CapPMTRadius+25.*mm);
  WCCapEdgeLimit           = WCIDDiameter/2.0 - CapPMTRadius; // - 10*cm
  compressionfactor        = 0.9;			// how much to squeeze PMTs together on an octagon face
  capcompressionratio      = 0.7;			// how much cap PMTs are squeezed in one dir relative to the other
  WCCapPMTPosRadius        = 0.9*m;			// radius at which to position PMTs for the top cap
  WCCapPMTPosRadius2       = 0.3*m;			// radius at which to position PMTs for the hatch
  
  WCBlackSheetThickness    = 1.01*mm;					// liner is 40 mil. = 40 milli inches. 
  
  // no LAPPDs in this configuration, yet
  WCBarrelLAPPDOffset     = 0.*m; 
  WCCapLAPPDSpacing       = 0.*m;
  WCLAPPDperCellHorizontal= 0;
  WCLAPPDperCellVertical  = 0;
}

void WCSimDetectorConstruction::SetANNIEPhase2Geometryv4()
{
  // 20 x 10" LUX PMTs on the bottom, arranged in a cross with a 4x4 rectangle, extended on it's narrow
  // dimension by an extra 2 PMTs in the centre on either side
  // 19 x 11" LBNE PMTs on the top, arranged in a ring of 16, with 3 on the central hatch
  // 40 x 8" + 45 x 10" PMTs in the barrel:
  //   ring config:
  //   10", 10"
  //    8",  8"
  //   10", 10"
  //    8",  8"
  //   10",  8"   << one asymmetrical ring
  //   10", 10"
  // 
  // Since rings are not all identical, must override replication as per ANNIEp2v2
  // PMTs on hand:
  // Side: 45 x 10" (Watchboy, R7081) + 40 x 8" (new, HQE(?) R5912)
  // Bottom: 20 x 10” PMTs (LUX, R7081)
  // Top: 19 x 11” PMTs (LBNE, HQE D784KFLB)  // NB. UPDATED NUMBER, DOWN FROM 22
  
  WCTankCollectionNames.clear();
  WCPMTNameMap.clear();
  WCPMTRadiusMap.clear();
  
  isANNIE=true;
  WCDetectorName = "ANNIEp2v4";
  
  // tank PMT collections - one for each type of PMT.
  WCIDCollectionName="WCIDCollectionNameIsUnused";  // some default.
  // the order in the maps is important due to hardcoded hacking in ConstructCylinder
  std::string WCIDCollectionName_R7081 = WCDetectorName +"-glassFaceWCPMT_R7081";
  std::string WCIDCollectionName_D784KFLB = WCDetectorName +"-glassFaceWCPMT_D784KFLB";
  std::string WCIDCollectionName_R5912HQE = WCDetectorName +"-glassFaceWCPMT_R5912HQE";
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081);     // 10" LUX/Watchboy
  WCTankCollectionNames.push_back(WCIDCollectionName_D784KFLB);  // 11" HQE LBNE
  WCTankCollectionNames.push_back(WCIDCollectionName_R5912HQE);  // 8"  HQE new
  
  // other detector element collections
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  
  // create the tank PMTs and get their info
  WCSimPMTObject* PMT_R7081 = CreatePMTObject("R7081", WCIDCollectionName_R7081);
  G4String WCPMTName_R7081 = PMT_R7081->GetPMTName();
  G4double WCPMTExposeHeight_R7081 = PMT_R7081->GetExposeHeight();
  G4double WCPMTRadius_R7081 = PMT_R7081->GetRadius();
  
  WCSimPMTObject* PMT_D784KFLB = CreatePMTObject("D784KFLB", WCIDCollectionName_D784KFLB);
  G4String WCPMTName_D784KFLB = PMT_D784KFLB->GetPMTName();
  G4double WCPMTExposeHeight_D784KFLB = PMT_D784KFLB->GetExposeHeight();
  G4double WCPMTRadius_D784KFLB = PMT_D784KFLB->GetRadius();
  
  WCSimPMTObject* PMT_R5912HQE = CreatePMTObject("R5912HQE", WCIDCollectionName_R5912HQE);
  G4String WCPMTName_R5912HQE = PMT_R5912HQE->GetPMTName();
  G4double WCPMTExposeHeight_R5912HQE = PMT_R5912HQE->GetExposeHeight();
  G4double WCPMTRadius_R5912HQE = PMT_R5912HQE->GetRadius();
  
  // store info in the maps
  WCPMTNameMap.emplace(WCIDCollectionName_R7081, WCPMTName_R7081);
  WCPMTNameMap.emplace(WCIDCollectionName_D784KFLB, WCPMTName_D784KFLB);
  WCPMTNameMap.emplace(WCIDCollectionName_R5912HQE, WCPMTName_R5912HQE);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081, WCPMTRadius_R7081);
  WCPMTRadiusMap.emplace(WCIDCollectionName_D784KFLB, WCPMTRadius_D784KFLB);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R5912HQE, WCPMTRadius_R5912HQE);
  WCPMTRadius = WCPMTRadius_D784KFLB;             // the LBNE PMTs are largest at 11"
  WCPMTExposeHeight = WCPMTRadius_R7081;   // the largest expose height of barrel PMTs
  
   
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();

  WCAddGd = true;
  // TODO: convert these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;		// 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;				// 13ft exactly (TSW blueprint) = 3.96m tall; hy is HALF height
  tankzoffset = 15.70*cm;		//15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  GDMLInnerStructureFilename = "InnerStructure.gdml";
  addGDMLinnerstructure = false;
  constructmrd = true;
  constructveto = true;
  
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
  MRDPMTName = MRDPMT->GetPMTName();
  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
  MRDPMTRadius = MRDPMT->GetRadius();
  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
  FACCPMTName = FACCPMT->GetPMTName();
  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
  FACCPMTRadius = FACCPMT->GetRadius();
  
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  // from blueprints of inner structure diameter is 106.64", hexagonal side is 40.81", 100.57" from face-to-face
  // (note: OUTER dimensions, including steel bar width)
  WCIDDiameter             = 2.554*m;		// 2x shortest distance to the centre of a ocatgonal
											// cell wall- from blueprints, this is 100.57" = 255.4cm
  WCIDHeight               = 3.96*m;		// full height
  WCBarrelPMTOffset        = 0.0715*m;		// offset of first barrel ring from tank caps
  
  // NOTE: ANNIEp2v2 bypasses the use of multiple identical rings to fill the barrel
  // instead, there is just ONE main barrel ring, in which 5 rings of PMTs are placed manually
  // and TWO border rings, in each of which one ring of PMTs is manually placed
  WCBarrelNumPMTHorizontal = 16;			// this + WCPMTperCellHorizontal define num detector sides
  WCPMTperCellHorizontal   = 2;				// ^ for octagonal inner structure it must produce 8
  WCBarrelNRings           = 6;				// can't use replication of rings unless they're identical
  											// hack equal size barrel/border rings by setting height of ring
  											// according to real # rings, but overriding replication in
  											// ConstructCylinder
  WCPMTperCellVertical     = 4;				// There will be 6 actual rings, 
  											// but we need to also populate border rings
  											// 4 vertical PMTs placed within the one main barrel ring
  											// border rings have 1 vertical PMT per cell
  
  G4double CapPMTRadius    = WCPMTRadius_D784KFLB;		// 11" on top are largest cap PMTs
  WCCapPMTSpacing          = 3.*(CapPMTRadius+25.*mm);
  WCCapEdgeLimit           = WCIDDiameter/2.0 - CapPMTRadius; // - 10*cm
  compressionfactor        = 0.9;			// how much to squeeze PMTs together on an octagon face
  capcompressionratio      = 0.7;			// how much cap PMTs are squeezed in one dir relative to the other
  WCCapPMTPosRadius        = 0.9*m;			// radius at which to position PMTs for the top cap
  WCCapPMTPosRadius2       = 0.3*m;			// radius at which to position PMTs for the hatch
  
  WCBlackSheetThickness    = 1.01*mm;		// liner is 40 mil. = 40 milli inches. 
  
  // 3 rings of LAPPDs at octagon corners
  WCBarrelLAPPDOffset     = 0.*m; 
  WCCapLAPPDSpacing       = 0.*m;
  WCLAPPDperCellHorizontal= 1;
  WCLAPPDperCellVertical  = 3;
  
}

void WCSimDetectorConstruction::SetANNIEPhase2Geometryv5()
{
  // 20 x 10" LUX PMTs on the bottom, arranged in a cross with a 4x4 rectangle, extended on it's narrow
  // dimension by an extra 2 PMTs in the centre on either side
  // 19 x 11" LBNE PMTs on the top, arranged in a ring of 16, with 3 on the central hatch
  // 30 x 10" HQE + 45 x 10" PMTs in the barrel:
  //   ring config:
  //  WB,  N/A   << border ring
  //  WB,   WB
  // HQE,  HQE
  // HQE,  HQE
  //  WB,   WB
  // N/A,   WB   << border ring
  // 
  // Since rings are not all identical, must override replication as per ANNIEp2v2
  // above design uses 32 x 10" HQE and 48 x WB - must remove 2 HQE and 3 WB PMTs
  
  // PMTs on hand:
  // Side: 45 x 10" (Watchboy, R7081) + 30 x 10" (new R7081HQE)
  // Bottom: 20 x 10” PMTs (LUX, R7081)
  // Top: 19 x 11” PMTs (LBNE, HQE D784KFLB)  // NB. UPDATED NUMBER, DOWN FROM 22
  
  WCTankCollectionNames.clear();
  WCPMTNameMap.clear();
  WCPMTRadiusMap.clear();
  
  isANNIE=true;
  WCDetectorName = "ANNIEp2v5";
  
  // tank PMT collections - one for each type of PMT.
  WCIDCollectionName="WCIDCollectionNameIsUnused";  // some default.
  // the order in the maps is important due to hardcoded hacking in ConstructCylinder
  std::string WCIDCollectionName_R7081 = WCDetectorName +"-glassFaceWCPMT_R7081";
  std::string WCIDCollectionName_D784KFLB = WCDetectorName +"-glassFaceWCPMT_D784KFLB";
  std::string WCIDCollectionName_R7081HQE = WCDetectorName +"-glassFaceWCPMT_R7081HQE";
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081);     // 10" LUX/Watchboy
  WCTankCollectionNames.push_back(WCIDCollectionName_D784KFLB);  // 11" HQE LBNE
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081HQE);  // new 10" HQE
  
  // other detector element collections
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  
  // create the tank PMTs and get their info
  WCSimPMTObject* PMT_R7081 = CreatePMTObject("R7081", WCIDCollectionName_R7081);
  G4String WCPMTName_R7081 = PMT_R7081->GetPMTName();
  G4double WCPMTExposeHeight_R7081 = PMT_R7081->GetExposeHeight();
  G4double WCPMTRadius_R7081 = PMT_R7081->GetRadius();
  
  WCSimPMTObject* PMT_D784KFLB = CreatePMTObject("D784KFLB", WCIDCollectionName_D784KFLB);
  G4String WCPMTName_D784KFLB = PMT_D784KFLB->GetPMTName();
  G4double WCPMTExposeHeight_D784KFLB = PMT_D784KFLB->GetExposeHeight();
  G4double WCPMTRadius_D784KFLB = PMT_D784KFLB->GetRadius();
  
  WCSimPMTObject* PMT_R7081HQE = CreatePMTObject("PMT10inchHQE", WCIDCollectionName_R7081HQE);
  G4String WCPMTName_R7081HQE = PMT_R7081HQE->GetPMTName();
  G4double WCPMTExposeHeight_R7081HQE = PMT_R7081HQE->GetExposeHeight();
  G4double WCPMTRadius_R7081HQE = PMT_R7081HQE->GetRadius();
  
  // store info in the maps
  WCPMTNameMap.emplace(WCIDCollectionName_R7081, WCPMTName_R7081);
  WCPMTNameMap.emplace(WCIDCollectionName_D784KFLB, WCPMTName_D784KFLB);
  WCPMTNameMap.emplace(WCIDCollectionName_R7081HQE, WCPMTName_R7081HQE);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081, WCPMTRadius_R7081);
  WCPMTRadiusMap.emplace(WCIDCollectionName_D784KFLB, WCPMTRadius_D784KFLB);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081HQE, WCPMTRadius_R7081HQE);
  WCPMTRadius = WCPMTRadius_D784KFLB;             // the LBNE PMTs are largest at 11"
  WCPMTExposeHeight = WCPMTRadius_R7081;   // the largest expose height of barrel PMTs
  
   
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();

  WCAddGd = true;
  // TODO: convert these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;		// 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;				// 13ft exactly (TSW blueprint) = 3.96m tall; hy is HALF height
  tankzoffset = 15.70*cm;		//15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  GDMLInnerStructureFilename = "PHASE2_INNER_STRUCTURE.gdml";
  addGDMLinnerstructure = true;
  doOverlapCheck = true;		// check overlaps when adding inner structure
  constructmrd = false;
  constructveto = false;
  
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
  MRDPMTName = MRDPMT->GetPMTName();
  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
  MRDPMTRadius = MRDPMT->GetRadius();
  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
  FACCPMTName = FACCPMT->GetPMTName();
  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
  FACCPMTRadius = FACCPMT->GetRadius();
  
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  // from blueprints of inner structure diameter is 106.64", hexagonal side is 40.81", 100.57" from face-to-face
  // (note: OUTER dimensions, including steel bar width)
  WCIDDiameter             = 2.554*m;		// 2x shortest distance to the centre of a ocatgonal
											// cell wall- from blueprints, this is 100.57" = 255.4cm
  WCIDHeight               = 3.96*m;		// full height
  WCBarrelPMTOffset        = 0.5715*m;		// offset of first barrel ring from tank caps
  
  // NOTE: ANNIEp2v2 bypasses the use of multiple identical rings to fill the barrel
  // instead, there is just ONE main barrel ring, in which 5 rings of PMTs are placed manually
  // and TWO border rings, in each of which one ring of PMTs is manually placed
  WCBarrelNumPMTHorizontal = 16;			// this + WCPMTperCellHorizontal define num detector sides
  WCPMTperCellHorizontal   = 2;				// ^ for octagonal inner structure it must produce 8
  WCBarrelNRings           = 6;				// can't use replication of rings unless they're identical
  											// hack equal size barrel/border rings by setting height of ring
  											// according to real # rings, but overriding replication in
  											// ConstructCylinder
  WCPMTperCellVertical     = 4;				// There will be 6 actual rings, 
  											// but we need to also populate border rings
  											// 4 vertical PMTs placed within the one main barrel ring
  											// border rings have 1 vertical PMT per cell
  
  numhatchpmts = 4;
  G4double CapPMTRadius    = WCPMTRadius_D784KFLB;		// 11" on top are largest cap PMTs
  WCCapPMTSpacing          = 3.*(CapPMTRadius+25.*mm);
  WCCapEdgeLimit           = WCIDDiameter/2.0 - CapPMTRadius; // - 10*cm
  WCCapPMTOffset           = 0.35*m;		// offset of the cap PMTs into the tank centre.
  WCCapTopPMTOffset        = 0.15*m;		// additional offset for the top cap PMTs, the offsets are different
  WCBorderBarrelTopPMTOffset = 0.2*m;		// top border ring PMTs need pushing down
  compressionfactor        = 0.62;			// how much to squeeze PMTs together on an octagon face
  capcompressionratio      = 0.6;			// how much cap PMTs are squeezed in one dir relative to the other
  WCCapPMTPosRadius        = 0.9*m;			// radius at which to position PMTs for the top cap
  WCCapPMTPosRadius2       = 0.3*m;			// radius at which to position PMTs for the hatch
  
  WCBlackSheetThickness    = 1.01*mm;		// liner is 40 mil. = 40 milli inches. 
  
  // 3 rings of LAPPDs at octagon corners
  WCBarrelLAPPDOffset     = 0.*m; 
  WCCapLAPPDSpacing       = 0.*m;
  WCLAPPDperCellHorizontal= 1;
  WCLAPPDperCellVertical  = 3;
  barrelcompressionfactor = 0.9;
  barrelbordercompressionfactor = 1.; // 0.8
  
}


void WCSimDetectorConstruction::SetANNIEPhase2Geometryv6()
{
  
  // 20 x 10" LUX PMTs on the bottom, arranged in a cross with a 4x4 rectangle, extended on it's narrow
  // dimension by an extra 2 PMTs in the centre on either side
  // 19 x 11" LBNE PMTs on the top, arranged in a ring of 16, with 3 on the central hatch
  //  8",  8"                                     == 16*8"
  //  WB,  8" << 3 faces //  WB,  WB << 5 faces   == 3*8", 13*WB
  //  WB,  WM << 2 faces //  8",  WB << 6 faces   == 2*WM, 6*8", 8*WB
  //  WM,  WB                                     == 8*WM,       8*WB
  //  WB,  WB                                     == 16 * WB
  //  8",  8" (missing one)                       == 15*8"
  // 
  //                                              == 10*WM, (16+3+6+15=40 8"), (13+8+8+16=45 WB) OK!
  // 
  // border rings:       31 *  8" HQE                         ->  9 remaining  8" HQE
  // central rings:      10 * 10" HQE +  6 * 8" HQE + 16 * WB ->  3 remaining  8" HQE + 29 remaining WB
  // intermediate rings:  3 *  8" HQE + 29 * WB               ->  none remaining 
  // 
  // inner structure has 16*6 = 96 holes
  // PMTs we should have:
  // 40 new hamamatsu 8"
  // 45 WB 10" tubes
  // 10 WM 10" HQE tubes << new, for barrel
  // Total: 
  // 95 PMTs
  // --> only one hole!
  // 
  // PMTs on hand:
  // Side: 45 x 10" (Watchboy, R7081) + 40 x 8" (new, HQE R5912) + 10 x 10" HQE (Watchman, R7081HQE)
  // Bottom: 20 x 10” PMTs (LUX, R7081)
  // Top: 19 x 11” PMTs (LBNE, HQE D784KFLB)  // NB. UPDATED NUMBER, DOWN FROM 22
  
  WCPosition=0.;//Set the WC tube offset to zero ???
  
  WCTankCollectionNames.clear();
  WCPMTNameMap.clear();
  WCPMTRadiusMap.clear();
  
  isANNIE=true;
  WCDetectorName = "ANNIEp2v6";
  
  // tank PMT collections - one for each type of PMT.
  WCIDCollectionName="WCIDCollectionNameIsUnused";  // some default.
  // the order in the maps is important due to hardcoded hacking in ConstructCylinder
  std::string WCIDCollectionName_R7081 = WCDetectorName +"-glassFaceWCPMT_R7081";
  std::string WCIDCollectionName_D784KFLB = WCDetectorName +"-glassFaceWCPMT_D784KFLB";
  std::string WCIDCollectionName_R5912HQE = WCDetectorName +"-glassFaceWCPMT_R5912HQE";
  std::string WCIDCollectionName_R7081HQE = WCDetectorName +"-glassFaceWCPMT_R7081HQE";
  std::string WCODCollectionName_EMI9954KB = WCDetectorName +"-glassFaceWCPMT_EMI9954KB"; // 2" OD PMTs
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081);     // 10" LUX/Watchboy
  WCTankCollectionNames.push_back(WCIDCollectionName_D784KFLB);  // 11" HQE LBNE
  WCTankCollectionNames.push_back(WCIDCollectionName_R5912HQE);  //  8" HQE new
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081HQE);  // 10" HQE Watchman
  WCTankCollectionNames.push_back(WCODCollectionName_EMI9954KB); //  2" PMT, for now use same as MRD
  
  // other detector element collections
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  
  // create the tank PMTs and get their info
  WCSimPMTObject* PMT_R7081 = CreatePMTObject("R7081", WCIDCollectionName_R7081);
  G4String WCPMTName_R7081 = PMT_R7081->GetPMTName();
  G4double WCPMTExposeHeight_R7081 = PMT_R7081->GetExposeHeight();
  G4double WCPMTRadius_R7081 = PMT_R7081->GetRadius();
  
  WCSimPMTObject* PMT_D784KFLB = CreatePMTObject("D784KFLB", WCIDCollectionName_D784KFLB);
  G4String WCPMTName_D784KFLB = PMT_D784KFLB->GetPMTName();
  G4double WCPMTExposeHeight_D784KFLB = PMT_D784KFLB->GetExposeHeight();
  G4double WCPMTRadius_D784KFLB = PMT_D784KFLB->GetRadius();
  
  WCSimPMTObject* PMT_R5912HQE = CreatePMTObject("R5912HQE", WCIDCollectionName_R5912HQE);
  G4String WCPMTName_R5912HQE = PMT_R5912HQE->GetPMTName();
  G4double WCPMTExposeHeight_R5912HQE = PMT_R5912HQE->GetExposeHeight();
  G4double WCPMTRadius_R5912HQE = PMT_R5912HQE->GetRadius();
  
  WCSimPMTObject* PMT_R7081HQE = CreatePMTObject("R7081HQE", WCIDCollectionName_R7081HQE);
  G4String WCPMTName_R7081HQE = PMT_R7081HQE->GetPMTName();
  G4double WCPMTExposeHeight_R7081HQE = PMT_R7081HQE->GetExposeHeight();
  G4double WCPMTRadius_R7081HQE = PMT_R7081HQE->GetRadius();
  
  WCSimPMTObject* PMT_EMI9954KB = CreatePMTObject("FlatFacedPMT2inch",WCODCollectionName_EMI9954KB);
  G4String WCPMTName_EMI9954KB = PMT_EMI9954KB->GetPMTName();
  G4double WCPMTExposeHeight_EMI9954KB = PMT_EMI9954KB->GetExposeHeight();
  G4double WCPMTRadius_EMI9954KB = PMT_EMI9954KB->GetRadius();
  
  // store info in the maps
  // names
  WCPMTNameMap.emplace(WCIDCollectionName_R7081, WCPMTName_R7081);
  WCPMTNameMap.emplace(WCIDCollectionName_D784KFLB, WCPMTName_D784KFLB);
  WCPMTNameMap.emplace(WCIDCollectionName_R5912HQE, WCPMTName_R5912HQE);
  WCPMTNameMap.emplace(WCIDCollectionName_R7081HQE, WCPMTName_R7081HQE);
  WCPMTNameMap.emplace(WCODCollectionName_EMI9954KB, WCPMTName_EMI9954KB);
  // radii
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081, WCPMTRadius_R7081);
  WCPMTRadiusMap.emplace(WCIDCollectionName_D784KFLB, WCPMTRadius_D784KFLB);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R5912HQE, WCPMTRadius_R5912HQE);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081HQE, WCPMTRadius_R7081HQE);
  WCPMTRadiusMap.emplace(WCODCollectionName_EMI9954KB, WCPMTRadius_EMI9954KB);
  // expose heights
  // these are used in ConstructANNIECylinder to account for different inner radii due to different holders
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_R7081,     20.*cm); // (just fudge factors)
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_D784KFLB,   0.*cm);
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_R5912HQE,  10.*cm);
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_R7081HQE,  20.*cm);
  WCPMTExposeHeightMap.emplace(WCODCollectionName_EMI9954KB,  10.*cm);
  
  WCPMTRadius = WCPMTRadius_D784KFLB;             // the LBNE PMTs are largest at 11"
  WCPMTExposeHeight = WCPMTRadius_R7081;          // the largest expose height of barrel PMTs
  
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();
  
  WCAddGd = true;
  // TODO: convert these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;		// 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;				// 13ft exactly (TSW blueprint) = 3.96m tall; hy is HALF height
  tankzoffset = 15.70*cm;		//15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  GDMLInnerStructureFilename = "PHASE2_INNER_STRUCTURE.gdml";
  addGDMLinnerstructure = true;
  doOverlapCheck = true;   // check overlaps when adding inner structure
  constructmrd = true;     // not optional without further work, except for visualization
  constructveto = true;    // not optional without further work, except for visualization
  
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
  MRDPMTName = MRDPMT->GetPMTName();
  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
  MRDPMTRadius = MRDPMT->GetRadius();
  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
  FACCPMTName = FACCPMT->GetPMTName();
  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
  FACCPMTRadius = FACCPMT->GetRadius();
  
  // PMT rings and faces counts
  WCBarrelNumPMTHorizontal = 16;			// this + WCPMTperCellHorizontal define num detector sides
  WCPMTperCellHorizontal   = 2;				// ^ for octagonal inner structure it must produce 8
  WCBarrelNRings           = 6;				// N/A
  WCBarrelRingNPhi         = 8;				// number of faces in the inner structure
  numhatchpmts = 4;
  
  // Offsets and positioning
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  WCIDHeight               = 3.96*m;		// full height
  WCIDDiameter             = 2.554*m;		// 2x shortest distance to the centre of a ocatgonal
											// cell wall- from blueprints, this is 100.57" = 255.4cm
											// from blueprints of inner structure diameter is 106.64",
											// hexagonal side is 40.81", 100.57" from face-to-face
											// (note: OUTER dimensions, including steel bar width)
  WCBarrelPMTOffset        = 0.415*m;			// offset of first barrel ring from tank caps  0.4 -> .34?
  WCBlackSheetThickness    = 1.01*mm;		// liner is 40 mil. = 40 milli inches. 
  InnerStructureCentreOffset = -5.6*cm;		// centre of inner structure is closer to the ground
  
  // cap positioning
  G4double CapPMTRadius    = WCPMTRadius_D784KFLB;				// 11" on top are largest cap PMTs
  WCCapPMTSpacing          = 3.*(CapPMTRadius) + 4*cm;			// wide separation of bottom PMTs
  capcompressionratio      = 0.62;			// relative spacing of bottom PMTs in x relative to in y
  capcentrebarwidth        = 4*cm;			// additional offset of bottom PMTs from centre due to central bar
  WCCapEdgeLimit           = WCIDDiameter/2.0 - CapPMTRadius;	//
  WCCapPMTPosRadius        = 0.85*m;		// radius at which to position PMTs for the top cap
  											// TODO this could probably have a squeeze in pairs
  WCCapPMTPosRadius2       = 0.3*m;			// radius at which to position PMTs for the hatch
  WCCapTopPMTOffset        = -0.1*cm;		//
  WCCapBottomPMTOffset     = 0.2*cm;		//
  
  // barrel positioning
  barrelcompressionfactor  = 0.82;			// barrel rings do not span full tank height
  compressionfactor        = 0.62;			// how much to squeeze PMTs together on an octagon face
  
  // LAPPDs at octagon corners
  WCLAPPDperCellHorizontal= 1;				// 1 LAPPD on each octagon face (corner)
  WCLAPPDperCellVertical  = 3;				// 3 rings of LAPPDs
  WCLAPPDSliderThickness  = 20*cm;			// offset of the LAPPD from the inner structure corner
  
}

void WCSimDetectorConstruction::SetANNIEPhase2Geometryv7()
{
  
  // v7: Read exact positions from scan position file
  // Omit 2-inch PMTs (do not exist)
  // 20 x 10" LUX PMTs on the bottom, arranged in a cross with a 4x4 rectangle, extended on it's narrow
  // dimension by an extra 2 PMTs in the centre on either side
  // 19 x 11" LBNE PMTs on the top, arranged in a ring of 16, with 3 on the central hatch
  //  8",  8"                                     == 16*8"
  //  WB,  8" << 3 faces //  WB,  WB << 5 faces   == 3*8", 13*WB
  //  WB,  WM << 2 faces //  8",  WB << 6 faces   == 2*WM, 6*8", 8*WB
  //  WM,  WB                                     == 8*WM,       8*WB
  //  WB,  WB                                     == 16 * WB
  //  8",  8" (missing one)                       == 15*8"
  // 
  //                                              == 10*WM, (16+3+6+15=40 8"), (13+8+8+16=45 WB) OK! 

  
  WCPosition=0.;//Set the WC tube offset to zero ???
  
  WCTankCollectionNames.clear();
  WCPMTNameMap.clear();
  WCPMTRadiusMap.clear();
  
  isANNIE=true;
  WCDetectorName = "ANNIEp2v7";
  
  // tank PMT collections - one for each type of PMT.
  WCIDCollectionName="WCIDCollectionNameIsUnused";  // some default.
  // the order in the maps is important due to hardcoded hacking in ConstructCylinder
  std::string WCIDCollectionName_R7081 = WCDetectorName +"-glassFaceWCPMT_R7081";
  std::string WCIDCollectionName_D784KFLB = WCDetectorName +"-glassFaceWCPMT_D784KFLB";
  std::string WCIDCollectionName_R5912HQE = WCDetectorName +"-glassFaceWCPMT_R5912HQE";
  std::string WCIDCollectionName_R7081HQE = WCDetectorName +"-glassFaceWCPMT_R7081HQE";
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081);     // 10" LUX/Watchboy
  WCTankCollectionNames.push_back(WCIDCollectionName_D784KFLB);  // 11" HQE LBNE
  WCTankCollectionNames.push_back(WCIDCollectionName_R5912HQE);  //  8" HQE new
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081HQE);  // 10" HQE Watchman
  
  // other detector element collections
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  
  // create the tank PMTs and get their info
  WCSimPMTObject* PMT_R7081 = CreatePMTObject("R7081", WCIDCollectionName_R7081);
  G4String WCPMTName_R7081 = PMT_R7081->GetPMTName();
  G4double WCPMTExposeHeight_R7081 = PMT_R7081->GetExposeHeight();
  G4double WCPMTRadius_R7081 = PMT_R7081->GetRadius();
  
  WCSimPMTObject* PMT_D784KFLB = CreatePMTObject("D784KFLB", WCIDCollectionName_D784KFLB);
  G4String WCPMTName_D784KFLB = PMT_D784KFLB->GetPMTName();
  G4double WCPMTExposeHeight_D784KFLB = PMT_D784KFLB->GetExposeHeight();
  G4double WCPMTRadius_D784KFLB = PMT_D784KFLB->GetRadius();
  
  WCSimPMTObject* PMT_R5912HQE = CreatePMTObject("R5912HQE", WCIDCollectionName_R5912HQE);
  G4String WCPMTName_R5912HQE = PMT_R5912HQE->GetPMTName();
  G4double WCPMTExposeHeight_R5912HQE = PMT_R5912HQE->GetExposeHeight();
  G4double WCPMTRadius_R5912HQE = PMT_R5912HQE->GetRadius();
  
  WCSimPMTObject* PMT_R7081HQE = CreatePMTObject("R7081HQE", WCIDCollectionName_R7081HQE);
  G4String WCPMTName_R7081HQE = PMT_R7081HQE->GetPMTName();
  G4double WCPMTExposeHeight_R7081HQE = PMT_R7081HQE->GetExposeHeight();
  G4double WCPMTRadius_R7081HQE = PMT_R7081HQE->GetRadius();
  
  // store info in the maps
  // names
  WCPMTNameMap.emplace(WCIDCollectionName_R7081, WCPMTName_R7081);
  WCPMTNameMap.emplace(WCIDCollectionName_D784KFLB, WCPMTName_D784KFLB);
  WCPMTNameMap.emplace(WCIDCollectionName_R5912HQE, WCPMTName_R5912HQE);
  WCPMTNameMap.emplace(WCIDCollectionName_R7081HQE, WCPMTName_R7081HQE);
  // radii
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081, WCPMTRadius_R7081);
  WCPMTRadiusMap.emplace(WCIDCollectionName_D784KFLB, WCPMTRadius_D784KFLB);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R5912HQE, WCPMTRadius_R5912HQE);
  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081HQE, WCPMTRadius_R7081HQE);
  // expose heights
  // these are used in ConstructANNIECylinder to account for different inner radii due to different holders
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_R7081,     20.*cm); // (just fudge factors)
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_D784KFLB,   0.*cm);
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_R5912HQE,  10.*cm);
  WCPMTExposeHeightMap.emplace(WCIDCollectionName_R7081HQE,  20.*cm);
  
  WCPMTRadius = WCPMTRadius_D784KFLB;             // the LBNE PMTs are largest at 11"
  WCPMTExposeHeight = WCPMTRadius_R7081;          // the largest expose height of barrel PMTs
  
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();
  
  WCAddGd = true;
  // TODO: convert these with the ones below, and add in other constants from MRD definition etc.
  tankouterRadius= 1.524*m;   // 120" exactly (TSW blueprint) = 3.048m diameter
  tankhy = 1.98*m;        // 13ft exactly (TSW blueprint) = 3.96m tall; hy is HALF height
  tankzoffset = 15.70*cm;   //15.70*cm
  tankyoffset = 144.64875*mm;
  expHall_x = 50*m;
  expHall_y = expHall_z = 500*m;
  GDMLFilename = "annie_v04.gdml";
  GDMLInnerStructureFilename = "PHASE2_INNER_STRUCTURE.gdml";
  addGDMLinnerstructure = true;
  doOverlapCheck = true;   // check overlaps when adding inner structure
  constructmrd = true;     // not optional without further work, except for visualization
  constructveto = true;    // not optional without further work, except for visualization
  
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
  MRDPMTName = MRDPMT->GetPMTName();
  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
  MRDPMTRadius = MRDPMT->GetRadius();
  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
  FACCPMTName = FACCPMT->GetPMTName();
  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
  FACCPMTRadius = FACCPMT->GetRadius();
  
  //-----------------------------------------------------------------------------------------------------------------------------------
  // Leave all of the following definitions, although not sure whether they are strictly needed when we read the positions from a file
  //-----------------------------------------------------------------------------------------------------------------------------------

  // PMT rings and faces counts
  WCBarrelNumPMTHorizontal = 16;      // this + WCPMTperCellHorizontal define num detector sides
  WCPMTperCellHorizontal   = 2;       // ^ for octagonal inner structure it must produce 8
  WCBarrelNRings           = 6;       // N/A
  WCBarrelRingNPhi         = 8;       // number of faces in the inner structure
  numhatchpmts = 4;
  
  // Offsets and positioning
  WCLength = tankhy;
  WCRadius = tankouterRadius;
  WCIDHeight               = 3.96*m;    // full height
  WCIDDiameter             = 2.554*m;   // 2x shortest distance to the centre of a ocatgonal
                      // cell wall- from blueprints, this is 100.57" = 255.4cm
                      // from blueprints of inner structure diameter is 106.64",
                      // hexagonal side is 40.81", 100.57" from face-to-face
                      // (note: OUTER dimensions, including steel bar width)
  WCBarrelPMTOffset        = 0.415*m;     // offset of first barrel ring from tank caps  0.4 -> .34?
  WCBlackSheetThickness    = 1.01*mm;   // liner is 40 mil. = 40 milli inches. 
  InnerStructureCentreOffset = -5.6*cm;   // centre of inner structure is closer to the ground
  
  // cap positioning
  G4double CapPMTRadius    = WCPMTRadius_D784KFLB;        // 11" on top are largest cap PMTs
  WCCapPMTSpacing          = 3.*(CapPMTRadius) + 4*cm;      // wide separation of bottom PMTs
  capcompressionratio      = 0.62;      // relative spacing of bottom PMTs in x relative to in y
  capcentrebarwidth        = 4*cm;      // additional offset of bottom PMTs from centre due to central bar
  WCCapEdgeLimit           = WCIDDiameter/2.0 - CapPMTRadius; //
  WCCapPMTPosRadius        = 0.85*m;    // radius at which to position PMTs for the top cap
                        // TODO this could probably have a squeeze in pairs
  WCCapPMTPosRadius2       = 0.3*m;     // radius at which to position PMTs for the hatch
  WCCapTopPMTOffset        = -0.1*cm;   //
  WCCapBottomPMTOffset     = 0.2*cm;    //
  
  // barrel positioning
  barrelcompressionfactor  = 0.82;      // barrel rings do not span full tank height
  compressionfactor        = 0.62;      // how much to squeeze PMTs together on an octagon face
  
  // LAPPDs at octagon corners
  WCLAPPDperCellHorizontal= 1;        // 1 LAPPD on each octagon face (corner)
  WCLAPPDperCellVertical  = 3;        // 3 rings of LAPPDs
}

void WCSimDetectorConstruction::SetANNIETestTankGeometry()
{
  
  // a steel box of half width and depth WCIDDiameter, 
  // lined with blacksheet, with a single LAPPD placed at the origin.
  // a dummy PMT is placed below the tank at (0,0,-WCIDHeight) just in case
  // having no PMTs in the simulation causes problems.
  
  // Offsets and positioning
  WCLength = 1.*m;                   // world extend is 3*WCLength in all dimensions
  WCIDHeight = 2.*m;                 // the 'tank' will be a steel box of half height WCIDHeight...
  WCIDDiameter = 2.*m;               // ... and half-width and half-depth WCIDDiameter
  WCPosition=0.;                     // global z offset of the tank
  
  isANNIE=true;                      // totally lost track of what this does, but we're annie.
  isLAPPDTestBox=true;               // use our test box geometry constructor
  WCDetectorName = "LAPPDTestTank";  // arbitrary
  
  // geometry features
  WCAddGd = false;                   // for now, let's use pure water.
  
  // don't these are relevant, i think i removed the calling code from this geomtetry's constructor
  addGDMLinnerstructure = false;     // add the inner structure by reading stl file
  doOverlapCheck = false;            // check overlaps when adding inner structure
  constructmrd = false;              // not optional without further work, except for visualization
  constructveto = false;             // not optional without further work, except for visualization
  
  // collections of PMT properties, one entry per PMT type.
  WCTankCollectionNames.clear();
  WCPMTNameMap.clear();
  WCPMTRadiusMap.clear();
  
  // old style global PMT collection name, when there's only one type of PMT.
  // leave it as this for annie's multiple PMT type support.
  WCIDCollectionName="WCIDCollectionNameIsUnused";
  
  // added later, a vector of collection names, one entry for each type of PMT.
  // we'll make one entry, just so it's not empty, in case that causes problems.
  std::string WCIDCollectionName_R7081 = WCDetectorName +"-glassFaceWCPMT_R7081";
  WCTankCollectionNames.push_back(WCIDCollectionName_R7081);
  
  // other hit collections
  // we're only interested in the LAPPD one, but there are almost certainly places in the code
  // that expect the other hit collections to exist, so make them anyway.
  WCMRDCollectionName = WCDetectorName +"-glassFaceWCPMT_MRD";
  WCFACCCollectionName = WCDetectorName +"-glassFaceWCPMT_FACC";
  WCIDCollectionName2 = WCDetectorName +"-glassFaceWCONLYLAPPDS";
  
  // create the tank PMT type we're going to use for the dummy PMT
  WCSimPMTObject* PMT_R7081 = CreatePMTObject("R7081", WCIDCollectionName_R7081);
  
  // create the LAPPD
  WCSimLAPPDObject * lappd = CreateLAPPDObject("lappd", WCIDCollectionName2);
  WCLAPPDName = lappd->GetLAPPDName();
  WCLAPPDExposeHeight = lappd->GetExposeHeight();
  WCLAPPDRadius = lappd->GetRadius();
  
  // transfer PMT info into some other maps used by some geometry constructors.
  // At the least we need the name
  G4String WCPMTName_R7081 = PMT_R7081->GetPMTName();
  WCPMTNameMap.emplace(WCIDCollectionName_R7081, WCPMTName_R7081);
  
  // the rest of the info is only used in some constructors
//  G4double WCPMTRadius_R7081 = PMT_R7081->GetRadius();
//  WCPMTRadiusMap.emplace(WCIDCollectionName_R7081, WCPMTRadius_R7081);
//  G4double WCPMTExposeHeight_R7081 = PMT_R7081->GetExposeHeight();
//  WCPMTExposeHeightMap.emplace(WCIDCollectionName_R7081, WCPMTExposeHeight_R7081);
//  
//  // a couple more general parameters used in some old geometry constructors...
//  WCPMTRadius = WCPMTRadius_R7081;
//  WCPMTExposeHeight = WCPMTExposeHeight_R7081;
//  
  // create MRD and FACC PMT Logical volumes and get their info
  // again the objects need to exist to instantiate the underlying hit collections
  WCSimPMTObject* MRDPMT = CreatePMTObject("FlatFacedPMT2inch",WCMRDCollectionName);
//  MRDPMTName = MRDPMT->GetPMTName();
//  MRDPMTExposeHeight = MRDPMT->GetExposeHeight();
//  MRDPMTRadius = MRDPMT->GetRadius();
//  
  WCSimPMTObject* FACCPMT = CreatePMTObject("FlatFacedPMT2inch",WCFACCCollectionName);
//  FACCPMTName = FACCPMT->GetPMTName();
//  FACCPMTExposeHeight = FACCPMT->GetExposeHeight();
//  FACCPMTRadius = FACCPMT->GetRadius();
  
}


void WCSimDetectorConstruction::SetSuperKGeometry()
{
  WCDetectorName = "SuperK";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", WCIDCollectionName);
 
  WCPMTName = PMT->GetPMTName();
  WCPMTExposeHeight = PMT->GetExposeHeight();
  WCPMTRadius = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCBarrelNumPMTHorizontal  = 150; 
  WCBarrelNRings        = 17.;
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCCapPMTSpacing       = 0.707*m; // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = 16.9*m;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

// Note: the actual coverage is 20.27%
void WCSimDetectorConstruction::SuperK_20inchPMT_20perCent()
{
  WCDetectorName = "SuperK_20inchPMT_20perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCPMTPercentCoverage  = 20.27;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}


// Note: the actual coverage is 20.27%
void WCSimDetectorConstruction::SuperK_20inchBandL_20perCent()
{
  WCDetectorName = "SuperK_20inchBandL_20perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3; 
  WCPMTPercentCoverage  = 20.27;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}


// Note: the actual coverage is 14.59%
void WCSimDetectorConstruction::SuperK_12inchBandL_15perCent()
{
  WCDetectorName = "SuperK_12inchBandL_15perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine12inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 14.59;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}


// Note: the actual coverage is 13.51%
void WCSimDetectorConstruction::SuperK_20inchBandL_14perCent()
{
  WCDetectorName = "SuperK_20inchBandL_14perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 33.6815*m; //16.900*2*cos(2*pi*rad/75)*m; //inner detector diameter
  WCIDHeight            = 36.200*m; //"" "" height
  WCBarrelPMTOffset     = 0.0715*m; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 13.51;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::Cylinder_60x74_20inchBandL_14perCent()
{ 
  WCDetectorName = "Cylinder_60x74_20inchBandL_14perCent()";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 13.51;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::Cylinder_60x74_20inchBandL_40perCent()
{ 
  WCDetectorName = "Cylinder_60x74_20inchBandL_40perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 74.0*m;
  WCIDHeight            = 60.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::Cylinder_12inchHPD_15perCent()
{ 
  WCDetectorName = "Cylinder_12inchHPD_15perCent";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  // cylindrical detector with a height of 100m and a diameter of 69m 
  // with 12" HPD and 14.59% photocoverage
  WCSimPMTObject * PMT = CreatePMTObject("HPD12inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 69.0*m;
  WCIDHeight            = 100.0*m;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 14.59;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::SetHyperKGeometry()
{
  WCDetectorName = "HyperK";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCSimPMTObject * PMT = CreatePMTObject("BoxandLine20inchHQE", WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = 70.8*m; // = 74m - 2*(60cm ID wall + 1m OD)
  WCIDHeight            = 54.8*m; // = 60m - 2*(60cm ID wall + 2m OD)
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTperCellHorizontal= 4;
  WCPMTperCellVertical  = 3;
  WCPMTPercentCoverage  = 40.0;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                      /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = false;
}

void WCSimDetectorConstruction::SetEggShapedHyperKGeometry()
{
  WCDetectorName = "EggShapedHyperK";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCODCollectionName = WCDetectorName + "-glassFaceWCPMT_OD"; 
  WCSimPMTObject * PMT = CreatePMTObject("PMT20inch", WCIDCollectionName);
  WCSimPMTObject * outerPMT = CreatePMTObject("PMT8inch", WCODCollectionName);
  WCPMTName = PMT->GetPMTName();
  innerPMT_Expose = PMT->GetExposeHeight();
  innerPMT_Radius = PMT->GetRadius();
  waterTank_TopR   = 32000.*mm;
  waterTank_BotR   = 30000.*mm;
  waterTank_Height = 48000.*mm;
  waterTank_UpperA =  8000.*mm;
  waterTank_LowerB =  6000.*mm;
  waterTank_Length = 49500.*mm;

  innerPMT_TopR     = 29095.*mm; 
  innerPMT_BotR     = 27095.*mm;
  innerPMT_TopW     = 12038.*mm;
  innerPMT_BotW     = 11004.*mm;
  innerPMT_Height   = 21095.*mm;
  innerPMT_Rpitch   =   990.*mm;
  innerPMT_Apitch   =   990.*mm;

  outerPMT_Name = outerPMT->GetPMTName();
  outerPMT_Expose = outerPMT->GetExposeHeight();
  outerPMT_Radius = outerPMT->GetRadius();
  outerPMT_TopR      = innerPMT_TopR + 900.*mm;
  outerPMT_BotR      = innerPMT_BotR + 900.*mm;
  outerPMT_TopW      = 12394.*mm;
  outerPMT_BotW      = 11319.*mm;
  outerPMT_Height    = innerPMT_Height + 900.*mm;
  outerPMT_TopRpitch = 3. * innerPMT_Rpitch * (outerPMT_TopR/innerPMT_TopR);
  outerPMT_BotRpitch = 3. * innerPMT_Rpitch * (outerPMT_BotR/innerPMT_BotR);
  outerPMT_Apitch    = 2. * innerPMT_Apitch;

  blackSheetThickness = 20.*mm;

  innerPMT_TopN = 0;
  innerPMT_BotN = 0;

  isEggShapedHyperK = true; // Tell DetectorConstruction to build egg-shaped HK geometry

  MatchWCSimAndEggShapedHyperK();
}

void WCSimDetectorConstruction::SetEggShapedHyperKGeometry_withHPD()
{
  WCDetectorName = "EggShapedHyperK_withHPD";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  WCODCollectionName = WCDetectorName + "-glassFaceWCPMT_OD";
  WCSimPMTObject * PMT = CreatePMTObject("HPD20inchHQE", WCIDCollectionName);
  WCSimPMTObject * outerPMT = CreatePMTObject("PMT8inch", WCODCollectionName);
  WCPMTName = PMT->GetPMTName();
  innerPMT_Expose = PMT->GetExposeHeight();
  innerPMT_Radius = PMT->GetRadius();
  waterTank_TopR   = 32000.*mm;
  waterTank_BotR   = 30000.*mm;
  waterTank_Height = 48000.*mm;
  waterTank_UpperA =  8000.*mm;
  waterTank_LowerB =  6000.*mm;
  waterTank_Length = 49500.*mm;

  innerPMT_TopR     = 29095.*mm;
  innerPMT_BotR     = 27095.*mm;
  innerPMT_TopW     = 12038.*mm;
  innerPMT_BotW     = 11004.*mm;
  innerPMT_Height   = 21095.*mm;
  innerPMT_Rpitch   =   990.*mm;
  innerPMT_Apitch   =   990.*mm;


  outerPMT_Expose = outerPMT->GetExposeHeight();
  outerPMT_Radius = outerPMT->GetRadius();
  outerPMT_TopR      = innerPMT_TopR + 900.*mm;
  outerPMT_BotR      = innerPMT_BotR + 900.*mm;
  outerPMT_TopW      = 12394.*mm;
  outerPMT_BotW      = 11319.*mm;
  outerPMT_Height    = innerPMT_Height + 900.*mm;
  outerPMT_TopRpitch = 3. * innerPMT_Rpitch * (outerPMT_TopR/innerPMT_TopR);
  outerPMT_BotRpitch = 3. * innerPMT_Rpitch * (outerPMT_BotR/innerPMT_BotR);
  outerPMT_Apitch    = 2. * innerPMT_Apitch;

  blackSheetThickness = 20.*mm;

  innerPMT_TopN = 0;
  innerPMT_BotN = 0;

  isEggShapedHyperK = true; // Tell DetectorConstruction to build egg-shaped HK geometry

  MatchWCSimAndEggShapedHyperK();
}

void WCSimDetectorConstruction::CylinderGeometry()
{
  WCDetectorName = "Cylinder";
  WCIDCollectionName = WCDetectorName +"-glassFaceWCPMT";
  cylinderTank_PMTType = "HPD12inchHQE";
  cylinderTank_Diameter          = 8*m;
  cylinderTank_Height            = 14*m;
  cylinderTank_Coverage  = 40.;
  WCPMTperCellHorizontal= 1;
  WCPMTperCellVertical  = 1;
  WCBlackSheetThickness = 2.0*cm;
  WCAddGd               = true;
  UpdateCylinderGeometry();
}


/**
 * Transfer egg-shaped HK variables needed elsewhere
 * to their generic WC equivalents.
 * This should be included in all egg-shaped HK configurations.
 */
void WCSimDetectorConstruction::MatchWCSimAndEggShapedHyperK()
{
  WCLength = waterTank_Length;
  WCPosition = 0.;
  WCPMTRadius = innerPMT_Radius;
}



