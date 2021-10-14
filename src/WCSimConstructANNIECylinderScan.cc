//	-*- mode:c++; tab-width:4;	-*-
#include "WCSimDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4IntersectionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVReplica.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4AssemblyVolume.hh"
#include "G4SubtractionSolid.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalSurface.hh"
#include "G4UserLimits.hh"
#include "G4ReflectionFactory.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "WCSimTuningParameters.hh" //jl145

#include <fstream>

/***********************************************************
 *
 * This file containts the functions which construct a 
 * cylindrical WC detector.	It used by both the SK and 
 * LBNE WC detector modes.	It is called in the Construct()
 * method in WCSimDetectorConstruction.cc.
 *
 * Sourcefile for the WCSimDetectorConstruction class
 *
 * This modified file (ConstructANNIECylinderScan) is designed 
 * explicitly for constructing the ANNIE detector, with 
 * the PMT positions extracted from laser scan data. 
 ***********************************************************/

G4LogicalVolume* WCSimDetectorConstruction::ConstructANNIECylinderScan()
{
	G4cout << "**** Building Cylindrical Detector ****" << G4endl;
	G4cout << "ConstructANNIECylinderScan" << G4endl;

	debugMode = false;
	
	//-----------------------------------------------------
	//---------------------Steel Barrel-------------------- 
	//-----------------------------------------------------
	
	totalAngle = 2.0*pi*rad;
	dPhi = totalAngle/ WCBarrelRingNPhi;
	
	WCIDRadius = WCIDDiameter/2.;
	WCLength = WCIDHeight;
	
	G4double WChalfheightexcess, WCradialexcess;
	// tank is 7GA (7-gauge steel) = (0.1793"normal sheet steel or) 0.1875" stainless steel
	// ^ gauge represents the thickness of 1 ounce of copper rolled out to an area of 1 square foot
	// = 4.7625mm thick 'A36' steel (density 7,800 kg/m3 )
	WChalfheightexcess=4.7625*mm; 
	WCradialexcess=4.7625*mm;
	
	G4Tubs* solidWC = new G4Tubs("WC",
								 0.0*m,
								 WCRadius+WCradialexcess, 
								 0.5*WCLength+WChalfheightexcess,
								 0.*deg,
								 360.*deg);
	
	G4LogicalVolume* logicWC;
	logicWC = new G4LogicalVolume(solidWC,
								  G4Material::GetMaterial("StainlessSteel"),
								  "WC",
								  0,0,0);
	
	G4VisAttributes* showColor = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logicWC->SetVisAttributes(showColor);
	
	//logicWC->SetVisAttributes(G4VisAttributes::Invisible); //amb79
	
	//-----------------------------------------------------
	//---------------Water in the Barrel-------------------
	//-----------------------------------------------------
	
	// Decide if adding Gd
	water = (WCAddGd) ? "Doped Water" : "Water";
	
	G4Tubs* solidWCBarrel = new G4Tubs("WCBarrel",
										0.0*m,
										WCRadius,
										0.5*WCLength,
										0.*deg,
										360.*deg);
	
	logicWCBarrel = 
		new G4LogicalVolume(solidWCBarrel,
			G4Material::GetMaterial(water),
			"WCBarrel",
			0,0,0);
	
	physiWCBarrel = 
		new G4PVPlacement(0,
						  G4ThreeVector(0.,0.,0.),
						  logicWCBarrel,
						  "WCBarrel",
						  logicWC,
						  false,
						  0);
	
	// This volume needs to made invisible to view the blacksheet and PMTs with RayTracer
	if (Vis_Choice == "RayTracer"){
		logicWCBarrel->SetVisAttributes(G4VisAttributes::Invisible);
	} else {
		//{if(!debugMode)
		//logicWCBarrel->SetVisAttributes(G4VisAttributes::Invisible);} 
	}
	
	//-----------------------------------------------------------
	// The Blacksheet, a daughter of the cells containing PMTs,
	// and also some other volumes to make the edges light tight
	//-----------------------------------------------------------
	
	G4double BlackSheetRadius = WCIDRadius + 3*cm;

	G4double annulusBlackSheetRmax[2] = {(BlackSheetRadius+WCBlackSheetThickness),
										  BlackSheetRadius+WCBlackSheetThickness};
	G4double annulusBlackSheetRmin[2] = {(BlackSheetRadius),
										  BlackSheetRadius};
	mainAnnulusHeight = WCIDHeight -2.*WCBarrelPMTOffset;
	mainAnnulusHeight += 30*cm;	//WCBlackSheet needs to extend to above ETEL holders and below LUX housings to match reality
	G4double mainAnnulusZ[2] = {-mainAnnulusHeight/2., mainAnnulusHeight/2};
	
	G4Polyhedra* solidWCBarrelCellBlackSheet = new G4Polyhedra("WCBarrelCellBlackSheet",
																WCBarrelCellStartPhi, // phi start
																totalAngle,           //total phi
																WCBarrelRingNPhi,     //NPhi-gon
																2,
																mainAnnulusZ,
																annulusBlackSheetRmin,
																annulusBlackSheetRmax);
	
	logicWCBarrelCellBlackSheet =
		new G4LogicalVolume(solidWCBarrelCellBlackSheet,
							G4Material::GetMaterial("Blacksheet"),
							"WCBarrelCellBlackSheet",
							0,0,0);
	
	G4VPhysicalVolume* physiWCBarrelCellBlackSheet =
		new G4PVPlacement(0,
						  G4ThreeVector(0.,0.,InnerStructureCentreOffset-5*cm),
						  logicWCBarrelCellBlackSheet,
						  "WCBarrelCellBlackSheet",
						  logicWCBarrel,
						  false,
						  0,true);
	G4cout<<"Constructed barrel cell blacksheet with radius "<<WCIDRadius<<" to "<<(WCIDRadius+WCBlackSheetThickness)<<G4endl;
	WCCylInfo[2]=BlackSheetRadius/10.;
	
	G4LogicalBorderSurface * WaterBSBarrelCellSurface 
		= new G4LogicalBorderSurface("WaterBSBarrelCellSurface",
									 physiWCBarrel,
									 physiWCBarrelCellBlackSheet,
									 OpWaterBSSurface);
	
	// Change made here to have the if statement contain the !debugmode to be consistent
	// This code gives the Blacksheet its color. 
	
	if(Vis_Choice == "RayTracer"){
		G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
			= new G4VisAttributes(G4Colour(0.2,0.9,0.2)); // green color
		WCBarrelBlackSheetCellVisAtt->SetForceSolid(true); // force the object to be visualized with a surface
		WCBarrelBlackSheetCellVisAtt->SetForceAuxEdgeVisible(true); // force auxiliary edges to be shown
		if(!debugMode)
			logicWCBarrelCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
		else
			logicWCBarrelCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
	} else {
		G4VisAttributes* WCBarrelBlackSheetCellVisAtt 
			= new G4VisAttributes(G4Colour(0.2,0.9,0.2));
		if(!debugMode)
			logicWCBarrelCellBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
		else
			logicWCBarrelCellBlackSheet->SetVisAttributes(WCBarrelBlackSheetCellVisAtt);
	}
	
	//-----------------------------------------------------
	//--------------------- The PMT -----------------------
	//-----------------------------------------------------
	
	G4LogicalVolume* logicWCPMT;
	std::vector<G4LogicalVolume*> logicWCPMTs;
	for(auto atankcollection : WCTankCollectionNames){
		G4String thepmtname = WCPMTNameMap.at(atankcollection);
		logicWCPMT= ConstructPMT(thepmtname, atankcollection, "tank");
		logicWCPMTs.push_back(logicWCPMT);
	}
	G4LogicalVolume* logicWCLAPPD=0;
	logicWCLAPPD = ConstructLAPPD(WCLAPPDName, WCIDCollectionName2);
	
	G4VisAttributes* WClogic 
			= new G4VisAttributes(G4Colour(0.4,0.0,0.8));
	WClogic->SetForceSolid(true);
	WClogic->SetForceAuxEdgeVisible(true);
	
	//logicWCPMT->SetVisAttributes(WClogic);
	//TODO: Re-enable Invisible attribute for PMTs after testing
	for(auto alogicWCPMT : logicWCPMTs) alogicWCPMT->SetVisAttributes(G4VisAttributes::Invisible);
	if(logicWCLAPPD) logicWCLAPPD->SetVisAttributes(G4VisAttributes::Invisible);
	
	//////--------------------------------------------------------------
	//////----------- Barrel + Top + Bottom PMT placement --------------
	//////--------------------------------------------------------------

	// Rotate in a way that the y axis points upwards (into the sky)
	G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
	WCPMTRotation->rotateY(90.*deg);
	
	// Create rotation matrices for the orientations of the PMTs
	std::vector<G4RotationMatrix*> pmt_rotation_matrices;
	// Bottom PMTs have panel number 0
	G4RotationMatrix *WCBottomCapRotation = new G4RotationMatrix();
	pmt_rotation_matrices.push_back(WCBottomCapRotation);	
	// Barrel PMTs have panel numbers 1-8
	for(int facei=0; facei<WCBarrelRingNPhi; facei++){
		G4RotationMatrix* WCPMTRotationNext = new G4RotationMatrix(*WCPMTRotation);
		WCPMTRotationNext->rotateX((dPhi*facei)-67.5*deg+180*deg);
		pmt_rotation_matrices.push_back(WCPMTRotationNext);
	}
	// Top PMTs have panel number 9
	G4RotationMatrix *WCTopCapRotation = new G4RotationMatrix();
	WCTopCapRotation->rotateY(180.*deg);
	pmt_rotation_matrices.push_back(WCTopCapRotation);

	G4cout <<"Size of pmt_rotation_matrices: "<<pmt_rotation_matrices.size()<<G4endl;

	// Read in file with PMT positions, create PMTs
	std::ifstream pmt_position_file("PMTPositions_Scan.txt");
	std::string next_pmt;
	double pmt_x, pmt_y, pmt_z, pmt_dirx, pmt_diry, pmt_dirz;
	double pmt_x_shift, pmt_y_shift, pmt_z_shift;
	int panel_nr, pmt_type;
	int PMTID;
	while (!pmt_position_file.eof()){
		pmt_position_file >> PMTID >> panel_nr >> pmt_x >> pmt_y >> pmt_z >> pmt_dirx >> pmt_diry >> pmt_dirz >> pmt_type;
		if (pmt_position_file.eof()) break;
		//G4cout << "Read in PMT "<<PMTID<<", panel nr: "<<panel_nr<<", Position ("<<pmt_x<<","<<pmt_y<<","<<pmt_z<<"), PMT type: "<<pmt_type<<G4endl;
		G4LogicalVolume *logicWCPMT = logicWCPMTs.at(pmt_type);
		G4RotationMatrix *pmt_rot = pmt_rotation_matrices.at(panel_nr);
		pmt_x_shift = pmt_x*cm;
		pmt_y_shift = (168.1-pmt_z)*cm;
		pmt_z_shift = ((pmt_y+14.45))*cm;
		//pmt_z_shift = ((pmt_y+14.45)-InnerStructureCentreOffset/10.)*cm;
		//G4cout <<"Edited PMT position ("<<pmt_x_shift<<","<<pmt_y_shift<<","<<pmt_z_shift<<")"<<G4endl;
		G4ThreeVector PMTPosition(pmt_x_shift,pmt_y_shift,pmt_z_shift);
		G4VPhysicalVolume *physicalWCPMT = new G4PVPlacement(pmt_rot,	//its rotation
															PMTPosition,		//its position
															logicWCPMT,			//its logical volume
															"WCPMT",			//its name
															logicWCBarrel,		//its mother volume
															false,				//no boolean operations
															PMTID,				//ID for this PMT (=channelkey in data)
															true);				//check overlaps*/
	}
	pmt_position_file.close();

	// Leave LAPPD placement in for now (?)
	
	//////-------------------------------------------------
	//////----------- Barrel LAPPD placement --------------
	//////-------------------------------------------------

	G4RotationMatrix* WCLAPPDRotation = new G4RotationMatrix;
	WCLAPPDRotation->rotateY(90.*deg);
	
	G4cout <<"Barrel LAPPD placement"<<G4endl;
	for(int facei=0; facei<WCBarrelRingNPhi; facei++){
		// rotate the LAPPD to point to tank centre. We need a new rotation matrix for each G4VPhysicalVolume
		G4RotationMatrix* WCLAPPDRotationNext = new G4RotationMatrix(*WCLAPPDRotation);
		WCLAPPDRotationNext->rotateX((dPhi*facei)-90.*deg);
		
		// position LAPPD on corner of the inner strucutre
		// Note PMTs use double facei=0.5, using int facei=0 accounts for the relative rotation
		G4double CellCentreX = WCIDRadius * sin(dPhi*facei);
		G4double CellCentreY = WCIDRadius * cos(dPhi*facei);
		
		double verticalSpacingLAPPD	= mainAnnulusHeight/(WCLAPPDperCellVertical+1);
		
		for(G4double j = 0; j < WCLAPPDperCellVertical; j++){	// num LAPPD cols in the central ring
		
		G4ThreeVector LAPPDPosition = G4ThreeVector(CellCentreX,
													CellCentreY,
													-mainAnnulusHeight/2.+(j+1.)*verticalSpacingLAPPD);
		
		G4VPhysicalVolume* physiWCBarrelLAPPD =
		new G4PVPlacement(WCLAPPDRotationNext,                      // its rotation
							LAPPDPosition,                          // its position
							logicWCLAPPD,                           // its logical volume
							"WCLAPPD",                              // its name
							logicWCBarrel,                          // its mother volume
							false,                                  // no boolean operations
							(int)(j+facei*WCLAPPDperCellVertical),  // a unique copy number
							false);                                 // don't check overlaps
		}
	}
	
	//--------------------------------------------------------------
	//---------------------------- CAPS ----------------------------
	//--------------------------------------------------------------

	// Leave the ConstructANNIECaps functions inside to create the black sheet in the top and bottom planes
	// Change the name: ConstructANNIECaps -> ConstructANNIECapsSheet to avoid conflicts with version in WCSimConstructANNIECylinder.cc

	ConstructANNIECapsSheet(-1);
	ConstructANNIECapsSheet(1);

	//--------------------------------------------------------------
	//--------------------ANNIE PMT holders ------------------------
	//--------------------------------------------------------------

	//If desired, construct ANNIE PMT holders
	//Geometrical properties of the holders are roughly extracted from the laser scan file (edges not clearly visible)

	G4bool HOLDER = WCSimTuningParams->GetHolder();
	G4cout <<"HOLDER variable: "<<HOLDER<<G4endl;
	G4double bsrff = WCSimTuningParams->GetBsrff();
	G4cout <<"Bsrff: "<<bsrff<<G4endl;

	if (HOLDER){
		ConstructANNIEHolders();
		ConstructLUXETELHolders();
	}
	
	
	return logicWC;
	
}

//--------------------------------------------------------------------
//------------------------- Construct Caps ---------------------------
//--------------------------------------------------------------------

void WCSimDetectorConstruction::ConstructANNIECapsSheet(G4int zflip)
{
	//--------------------------------------------------------------------
	// ---------------------Add cap blacksheet----------------------------
	// -------------------------------------------------------------------
	
	G4double BlackSheetRadius = WCIDRadius + 3*cm;

	G4double capBlackSheetZ[2] = {-WCBlackSheetThickness*zflip, 0.};
	G4double capBlackSheetRmin[2] = {0., 0.};
	G4double capBlackSheetRmax[2] = {BlackSheetRadius+WCBlackSheetThickness, 
									 BlackSheetRadius+WCBlackSheetThickness};
	G4VSolid* solidWCCapBlackSheet;
	if(WCBarrelRingNPhi*WCPMTperCellHorizontal == WCBarrelNumPMTHorizontal){
		solidWCCapBlackSheet
			= new G4Polyhedra("WCCapBlackSheet",  // name
							  0.*deg,             // phi start
							  totalAngle,         // total phi
							  WCBarrelRingNPhi,   // NPhi-gon
							  2,                  // z-planes
							  capBlackSheetZ,     // position of the Z planes
							  capBlackSheetRmin,  // min radius at the z planes
							  capBlackSheetRmax   // max radius at the Z planes
							  );
		// G4cout << *solidWCCapBlackSheet << G4endl;
	}
	
	G4LogicalVolume* logicWCCapBlackSheet;
	G4SubtractionSolid* solidWCCapBlackSheetHole = (G4SubtractionSolid*) solidWCCapBlackSheet;
	G4bool HOLDER = WCSimTuningParams->GetHolder();
	if (HOLDER && zflip == 1){
		//Select only ETEL + LUX PMTs and propagate their position up-/downwards to get central holder position
		std::ifstream pmt_position_file("PMTPositions_Scan.txt");
		std::string next_pmt;
		double pmt_x, pmt_y, pmt_z, pmt_dirx, pmt_diry, pmt_dirz;
		double hole_x, hole_y, hole_z;
		int panel_nr, pmt_type;
		int HolderID;
		while (!pmt_position_file.eof()){
			pmt_position_file >> HolderID >> panel_nr >> pmt_x >> pmt_y >> pmt_z >> pmt_dirx >> pmt_diry >> pmt_dirz >> pmt_type;
			if (pmt_position_file.eof()) break;
			if (fabs(pmt_diry+1.) < 0.00001) {       //select only ETEL PMTs for the holders (pointing downwards)

			hole_x = pmt_x*cm;
			hole_y = (168.1-pmt_z)*cm;
			hole_z = ((pmt_y+14.45))*cm;

			G4Tubs *WCCap_Hole = new G4Tubs("WCCap_Hole",0.0*cm,8.414*cm,WCBlackSheetThickness+0.1*cm,0*deg,360*deg);

			//Create combined logical volume of the Box + Tube to get holder with hole (Subtraction Solid)

			solidWCCapBlackSheetHole = new G4SubtractionSolid("WCCapBlackSheetHole",
											solidWCCapBlackSheetHole,
											WCCap_Hole,
											0,
											G4ThreeVector(hole_x,hole_y,0.));


			}

		}
		pmt_position_file.close();

	    logicWCCapBlackSheet =
		new G4LogicalVolume(solidWCCapBlackSheetHole,
							G4Material::GetMaterial("Blacksheet"),
							"WCCapBlackSheet",
							0,0,0);

	} else {

	  logicWCCapBlackSheet =
		new G4LogicalVolume(solidWCCapBlackSheet,
							G4Material::GetMaterial("Blacksheet"),
							"WCCapBlackSheet",
							0,0,0);

	}
	
	capAssemblyHeight = mainAnnulusHeight/2+1*mm+WCBlackSheetThickness;
	
	G4double AssemblyHeight = capAssemblyHeight*zflip + InnerStructureCentreOffset - 5*cm;


	G4VPhysicalVolume* physiWCCapBlackSheet =
		new G4PVPlacement(0,
						  G4ThreeVector(0.,0.,AssemblyHeight),
						  logicWCCapBlackSheet,
						  "WCCapBlackSheet",
						  logicWCBarrel,
						  false,
						  0,
						  true);

	G4cout<<"constructed cap blacksheet at height "<<AssemblyHeight<<G4endl;
	WCCylInfo[(zflip>0)]=(AssemblyHeight)/10.;

	G4LogicalBorderSurface * WaterBSBottomCapSurface 
			= new G4LogicalBorderSurface("WaterBSCapPolySurface",
										 physiWCBarrel,physiWCCapBlackSheet,
										 OpWaterBSSurface);
	
	G4VisAttributes* WCCapBlackSheetVisAtt 
			= new G4VisAttributes(G4Colour(0.9,0.2,0.2));
	
	// used for OGLSX
	if (Vis_Choice == "OGLSX"){
		G4VisAttributes* WCCapBlackSheetVisAtt 
			= new G4VisAttributes(G4Colour(0.9,0.2,0.2));
		if(!debugMode)
			logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
		else
			logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
	}
	// used for RayTracer (makes the caps blacksheet yellow)
	if (Vis_Choice == "RayTracer"){
		
		G4VisAttributes* WCCapBlackSheetVisAtt 
		= new G4VisAttributes(G4Colour(1.0,1.0,0.0));
		
		if(!debugMode)
				//logicWCCapBlackSheet->SetVisAttributes(G4VisAttributes::Invisible);
				logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
				// Use first line if you want to make the blacksheet on the caps invisible to view through
			else
				logicWCCapBlackSheet->SetVisAttributes(WCCapBlackSheetVisAtt);
	}
	
}

void WCSimDetectorConstruction::ConstructANNIEHolders(){

	G4cout <<"Construct ANNIE Holders"<<G4endl;

	//Tried to get rough dimensions of the ANNIE holder from the laser scan file
	//Not really sure about the thickness, assume 2cm thickness for now (should not be super important)
	//G4Box *ANNIEHolder_Box = new G4Box("ANNIEHolder_Box",10.5*cm,17.75*cm,1.*cm);
	G4Box *ANNIEHolder_Box = new G4Box("ANNIEHolder_Box",10.5*cm,16.75*cm,1.*cm); // Make holder slightly less wide to prevent geometry overlaps
	G4Tubs *ANNIEHolder_Tube = new G4Tubs("ANNIEHolder_Tube",0.0*cm,6.0*cm,1.1*cm,0*deg,360*deg);

	//Create combined logical volume of the Box + Tube to get holder with hole (Subtraction Solid)

	G4SubtractionSolid *solidANNIEHolder = new G4SubtractionSolid("ANNIEHolder",
																ANNIEHolder_Box,
																ANNIEHolder_Tube,
																0,
																G4ThreeVector(0.,0.,0.));

	//Check the material of the ANNIE holders somewhere!
	//ANNIE holders should be made out of polyethylene
	//Assume white acrylic since it should be similar
	G4LogicalVolume *logANNIEHolder = new G4LogicalVolume(solidANNIEHolder,
			G4Material::GetMaterial("Acrylic"),
			"WCANNIEHolder",
			0,0,0);

	//G4double dist_pmt_holder = 10.84;		//Holder is 20cm away from the front face of the ANNIE PMTs, WCSim center is 9.16cm away from front --> 10.84cm distance
	G4double dist_pmt_holder = 9.84;		//Reduce distance by 1cm to prevent geometry overlaps

	//Read in PMT positions again, and project position of holder positions from the PMT positions

	// Create rotation matrices for the orientations of the PMTs
	std::vector<G4RotationMatrix*> holder_rotation_matrices;
	// Bottom PMTs have panel number 0
	G4RotationMatrix *WCBottomCapRotation = new G4RotationMatrix();
	holder_rotation_matrices.push_back(WCBottomCapRotation);
	G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;
	WCPMTRotation->rotateY(90.*deg);	
	// Barrel PMTs have panel numbers 1-8
	for(int facei=0; facei<WCBarrelRingNPhi; facei++){
		G4RotationMatrix* WCPMTRotationNext = new G4RotationMatrix(*WCPMTRotation);
		WCPMTRotationNext->rotateX((dPhi*facei)-67.5*deg+180*deg);
		holder_rotation_matrices.push_back(WCPMTRotationNext);
	}
	// Top PMTs have panel number 9
	G4RotationMatrix *WCTopCapRotation = new G4RotationMatrix();
	WCTopCapRotation->rotateY(180.*deg);
	holder_rotation_matrices.push_back(WCTopCapRotation);

	//Select only ANNIE PMTs and propagate their position outwards to get central holder position
	std::ifstream pmt_position_file("PMTPositions_Scan.txt");
	std::string next_pmt;
	double pmt_x, pmt_y, pmt_z, pmt_dirx, pmt_diry, pmt_dirz;
	double holder_x, holder_y, holder_z;
	int panel_nr, pmt_type;
	int HolderID;
	while (!pmt_position_file.eof()){
		pmt_position_file >> HolderID >> panel_nr >> pmt_x >> pmt_y >> pmt_z >> pmt_dirx >> pmt_diry >> pmt_dirz >> pmt_type;
		if (pmt_position_file.eof()) break;
		//G4cout << "Read in PMT "<<HolderID<<", panel nr: "<<panel_nr<<", Position ("<<pmt_x<<","<<pmt_y<<","<<pmt_z<<"), PMT type: "<<pmt_type<<G4endl;
		
		if (pmt_type == 2){	//select only ANNIE (Hamamatsu 8inch) PMTs for the holders
		G4RotationMatrix *holder_rot = holder_rotation_matrices.at(panel_nr);

		//Shift the PMT position outwards
		pmt_x -= (pmt_dirx*dist_pmt_holder);
		pmt_y -= (pmt_diry*dist_pmt_holder);
		pmt_z -= (pmt_dirz*dist_pmt_holder);

		holder_x = pmt_x*cm;
		holder_y = (168.1-pmt_z)*cm;
		holder_z = ((pmt_y+14.45))*cm;
		//pmt_z_shift = ((pmt_y+14.45)-InnerStructureCentreOffset/10.)*cm;
		//G4cout <<"Edited Holder position ("<<holder_x<<","<<holder_y<<","<<holder_z<<")"<<G4endl;
		
		G4ThreeVector HolderPosition(holder_x,holder_y,holder_z);
		G4VPhysicalVolume *physicalHolder = new G4PVPlacement(holder_rot,	//its rotation
															HolderPosition,		//its position
															logANNIEHolder,			//its logical volume
															"ANNIE-Holder",			//its name
															logicWCBarrel,		//its mother volume
															false,				//no boolean operations
															HolderID,				//ID for this PMT (=channelkey in data)
															true);				//check overlaps*/
		G4LogicalBorderSurface* ANNIEHolderSurface
                = new G4LogicalBorderSurface("ANNIEHolderSurface",
                                                                         physiWCBarrel,
                                                                         physicalHolder,
                                                                         HolderOpSurface);
		}
	}
	pmt_position_file.close();

}

void WCSimDetectorConstruction::ConstructLUXETELHolders(){

 	G4cout <<"Construct LUX/ETEL Holders"<<G4endl;

	//Dimensions for LUX holders taken from this presentation:
	//https://annie-docdb.fnal.gov/cgi-bin/sso/RetrieveFile?docid=654&filename=LUXPMTs_Phone_2017-04-13_FISCHER.pdf&version=1
	//Thickness: 3/4" (1.905cm, half-height: ~0.95cm), Width: 6" (15.24cm), Length: 2x7"+6" = 20" (50.8cm)
	//Hole has a diameter of 6.625" (r=8.41375cm)
	//

	//Dimensions for ETEL holders are assumed to be the same, no reference though	

	//G4Box *LUXHolder_Box = new G4Box("ANNIEHolder_Box",7.62*cm,25.4*cm,0.95*cm);
	G4Box *LUXHolder_Box = new G4Box("ANNIEHolder_Box",7.62*cm,21.4*cm,0.95*cm); //Make holders smaller to prevent geometry overlaps
	//G4Tubs *LUXHolder_Tube = new G4Tubs("LUXHolder_Tube",0.0*cm,8.41375*cm,0.95*cm,0*deg,360*deg);
	//G4Tubs *LUXHolder_Tube = new G4Tubs("LUXHolder_Tube",0.0*cm,8.41375*cm,2.74*cm,0*deg,360*deg);
	//Distance between holder and PMT: 5.5cm (-> 2.75cm) + 18 cm housing height (->9cm): 11.75cm
	G4Tubs *LUXHolder_Tube = new G4Tubs("LUXHolder_Tube",0.0*cm,8.41375*cm,11.74*cm,0*deg,360*deg);

	//Create combined logical volume of the Box + Tube to get holder with hole (Subtraction Solid)

	G4UnionSolid *solidLUXHolder = new G4UnionSolid("LUXHolder",
											LUXHolder_Box,
											LUXHolder_Tube,
											0,
											G4ThreeVector(0.,0.,-6.25*cm));


	//LUX & ETEL holders are made of Schedule 80 PVC --> Use PVC as material
	G4LogicalVolume *logLUXHolder = new G4LogicalVolume(solidLUXHolder,
			G4Material::GetMaterial("PVC"),
			"WCLUXHolder",
			0,0,0);

	//G4double dist_pmt_holder_lux = 6.0;		//LUX center is 11.7cm from glass front surface total distance glass surface-wings = 17.7cm, dist = 6.0cm
	G4double dist_pmt_holder_lux = 5.5;		//Slightly reduce distance from 6 to 5.5 cm to prevent geometry overlaps

	//G4Tubs *ETELHolder_Tube = new G4Tubs("ETELHolder_Tube",0.0*cm,8.41375*cm,5.62*cm,0*deg,360*deg);
	//Distance ETEL holder /PMT: 11.25cm (->5.625cm) + 18cm housing height (->9cm): 14.625cm
	G4Tubs *ETELHolder_Tube = new G4Tubs("ETELHolder_Tube",0.0*cm,8.41375*cm,14.62*cm,0*deg,360*deg);
	G4UnionSolid *solidETELHolder = new G4UnionSolid("ETELHolder",
										LUXHolder_Box,
										ETELHolder_Tube,
										0,
										G4ThreeVector(0.,0.,3.375*cm));

	G4LogicalVolume *logETELHolder = new G4LogicalVolume(solidETELHolder,
			G4Material::GetMaterial("PVC"),
			"WCETELHolder",
			0,0,0);

	

	//G4double dist_pmt_holder_etel = 7.25;	//ETEL center is 11.8cm from glass front surface, total distance from glass surface to wings is 19.05cm (7.5") -> dist = 19.05cm-11.8cm = 7.25cm
	G4double dist_pmt_holder_etel = 11.25;	//Try to get ETEL wings above the top part of the Inner Structure--> increase distance


	//Create Rotation matrix for PMT holders
	G4RotationMatrix* WCPMTRotation = new G4RotationMatrix;

	//Select only ETEL + LUX PMTs and propagate their position up-/downwards to get central holder position
	std::ifstream pmt_position_file("PMTPositions_Scan.txt");
 	std::string next_pmt;
 	double pmt_x, pmt_y, pmt_z, pmt_dirx, pmt_diry, pmt_dirz;
 	double holder_x, holder_y, holder_z;
 	int panel_nr, pmt_type;
 	int HolderID;
 	while (!pmt_position_file.eof()){
 		pmt_position_file >> HolderID >> panel_nr >> pmt_x >> pmt_y >> pmt_z >> pmt_dirx >> pmt_diry >> pmt_dirz >> pmt_type;
 		if (pmt_position_file.eof()) break;
 		//G4cout << "Read in PMT "<<HolderID<<", panel nr: "<<panel_nr<<", Position ("<<pmt_x<<","<<pmt_y<<","<<pmt_z<<"), PMT type: "<<pmt_type<<G4endl;

 		if (fabs(pmt_diry-1.) < 0.00001) {	//select only LUX PMTs for the holders (pointing upwards)
 			//G4RotationMatrix *holder_rot = holder_rotation_matrices.at(panel_nr);
//Shift the PMT position outwards

			G4RotationMatrix *holder_rot = new G4RotationMatrix(*WCPMTRotation);
			holder_rot->rotateZ((45+90)*deg);
			pmt_x -= (pmt_dirx*dist_pmt_holder_lux);
 			pmt_y -= (pmt_diry*dist_pmt_holder_lux);
 			pmt_z -= (pmt_dirz*dist_pmt_holder_lux);

 			holder_x = pmt_x*cm;
 			holder_y = (168.1-pmt_z)*cm;
 			holder_z = ((pmt_y+14.45))*cm;

			//G4cout <<"Edited LUX Holder position ("<<holder_x<<","<<holder_y<<","<<holder_z<<")"<<G4endl;

 			G4ThreeVector HolderPosition(holder_x,holder_y,holder_z);
 			G4VPhysicalVolume *physicalHolder = new G4PVPlacement(holder_rot,	//its rotation
								HolderPosition,			//its position
 								logLUXHolder,			//its logical volume
 								"LUX-Holder",			//its name
 								logicWCBarrel,			//its mother volume
 								false,				//no boolean operations
 								HolderID,			//ID for this PMT (=channelkey in data)
 								true);				//check overlaps*/

 			G4LogicalBorderSurface* LUXHolderSurface = new G4LogicalBorderSurface("LUXHolderSurface",
                                                                          physiWCBarrel,
                                                                          physicalHolder,
                                                                          LUXHolderOpSurface);

 		} else if (fabs(pmt_diry+1.) < 0.00001) {       //select only ETEL PMTs for the holders (pointing downwards)
			

			pmt_x -= (pmt_dirx*dist_pmt_holder_etel);
 			pmt_y -= (pmt_diry*dist_pmt_holder_etel);
 			pmt_z -= (pmt_dirz*dist_pmt_holder_etel);

 			holder_x = pmt_x*cm;
 			holder_y = (168.1-pmt_z)*cm;
 			holder_z = ((pmt_y+14.45))*cm;

			//G4cout <<"Edited ETEL Holder position ("<<holder_x<<","<<holder_y<<","<<holder_z<<")"<<G4endl;

			G4RotationMatrix *holder_rot = new G4RotationMatrix(*WCPMTRotation);
			double phi = atan2(holder_x,holder_y);
			phi = (phi > 0)? phi : 2*pi+phi;
			//There are 8 different rotations for the top PMT holders, depending on the phi positions of the PMTs
			for (int i_phi = 0; i_phi < 8; i_phi++){
				double lower_phi = i_phi*pi/4.-pi/8.;
				double upper_phi = i_phi*pi/4.+pi/8.;
				if (lower_phi <= phi && phi <= upper_phi) holder_rot->rotateZ((i_phi*45)*deg);
				else {
					lower_phi += 2*pi;
					upper_phi += 2*pi;
					if (lower_phi <= phi && phi <= upper_phi) holder_rot->rotateZ((i_phi*45)*deg);
				}
			}
			//4 holder in the inner ring are rotated by 90degrees w.r.t. the holders in the outer ring
			if (sqrt(holder_x*holder_x+holder_y*holder_y)<320.) holder_rot->rotateZ(90*deg);
 			G4ThreeVector HolderPosition(holder_x,holder_y,holder_z);
 			G4VPhysicalVolume *physicalHolder = new G4PVPlacement(holder_rot,	//its rotation
								HolderPosition,			//its position
 								logETELHolder,			//its logical volume
 								"ETEL-Holder",			//its name
 								logicWCBarrel,			//its mother volume
 								false,				//no boolean operations
 								HolderID,			//ID for this PMT (=channelkey in data)
 								true);				//check overlaps*/

 			G4LogicalBorderSurface* ETELHolderSurface = new G4LogicalBorderSurface("ETELHolderSurface",
                                                                          physiWCBarrel,
                                                                          physicalHolder,
                                                                          LUXHolderOpSurface);
		}
			
 	}
 	pmt_position_file.close();


}
