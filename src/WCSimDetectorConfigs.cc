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
  
  // no LAPPDs in this configuration, yet
  WCBarrelLAPPDOffset     = 0.*m; 
  WCCapLAPPDSpacing       = 0.*m;
  WCLAPPDperCellHorizontal= 0;
  WCLAPPDperCellVertical  = 0;
  
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



