#include "WCSimDetectorConstruction.hh"
#include "WCSimDetectorMessenger.hh"
#include "WCSimTuningParameters.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"

#include "G4RunManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "WCSimDarkRateMessenger.hh"
#include "G4SolidStore.hh"
#include "G4GDMLParser.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeometryTolerance.hh"

std::map<int, G4Transform3D> WCSimDetectorConstruction::tubeIDMap;
std::map<int, G4Transform3D> WCSimDetectorConstruction::mrdtubeIDMap;
std::map<int, G4Transform3D> WCSimDetectorConstruction::facctubeIDMap;
std::map<int, G4Transform3D> WCSimDetectorConstruction::lappdIDMap;
//std::map<int, cyl_location>  WCSimDetectorConstruction::tubeCylLocation;
hash_map<std::string, int, hash<std::string> > WCSimDetectorConstruction::tubeLocationMap;
hash_map<std::string, int, hash<std::string> > WCSimDetectorConstruction::mrdtubeLocationMap;
hash_map<std::string, int, hash<std::string> > WCSimDetectorConstruction::facctubeLocationMap;
hash_map<std::string, int, hash<std::string> > WCSimDetectorConstruction::lappdLocationMap;

WCSimDetectorConstruction::WCSimDetectorConstruction(G4int DetConfig,WCSimTuningParameters* WCSimTuningPars):WCSimTuningParams(WCSimTuningPars), noRot(0), rotatedmatx(0), upmtx(0), downmtx(0), rightmtx(0), leftmtx(0), scintSurface_op(0), MPTmylarSurface(0), lgSurface_op(0), lgsurf_MPT(0)
{
	
  // Decide if (only for the case of !1kT detector) should be upright or horizontal
  isUpright = false;
  isEggShapedHyperK  = false;

  debugMode = false;

  myConfiguration = DetConfig;

  //-----------------------------------------------------
  // Create Materials
  //-----------------------------------------------------
    
  ConstructMaterials();

  //-----------------------------------------------------
  // Initialize things related to the tubeID
  //-----------------------------------------------------

  WCSimDetectorConstruction::tubeIDMap.clear();
  WCSimDetectorConstruction::mrdtubeIDMap.clear();
  WCSimDetectorConstruction::facctubeIDMap.clear();
  WCSimDetectorConstruction::lappdIDMap.clear();
  //WCSimDetectorConstruction::tubeCylLocation.clear();// (JF) Removed
  WCSimDetectorConstruction::tubeLocationMap.clear();
  WCSimDetectorConstruction::mrdtubeLocationMap.clear();
  WCSimDetectorConstruction::facctubeLocationMap.clear();
  WCSimDetectorConstruction::lappdLocationMap.clear();
  WCSimDetectorConstruction::PMTLogicalVolumes.clear();
  WCSimDetectorConstruction::LAPPDLogicalVolumes.clear();
  totalNumPMTs = 0;
  totalNumMrdPMTs = 0;
  totalNumFaccPMTs = 0;
  totalNumLAPPDs = 0;
  WCPMTExposeHeight= 0.;
  WCLAPPDExposeHeight= 0.;
  //-----------------------------------------------------
  // Set the default WC geometry.  This can be changed later.
  //-----------------------------------------------------

  //SetSuperKGeometry();
  //SetHyperKGeometry();
  //SetANNIEPhase1Geometry();
  //SetANNIEPhase2Geometry();
  //SetANNIEPhase2Geometryv2();
  //SetANNIEPhase2Geometryv3();
  //SetANNIEPhase2Geometryv4();
  //SetANNIEPhase2Geometryv5();
  //SetANNIEPhase2Geometryv6();
  SetANNIEPhase2Geometryv7();

  //----------------------------------------------------- 
  // Set whether or not Pi0-specific info is saved
  //-----------------------------------------------------

  SavePi0Info(false);
  
  //-----------------------------------------------------
  // Set whether or not neutron capture info is saved
  //-----------------------------------------------------

  SaveCaptureInfo(true);
  
  //-----------------------------------------------------
  // Set the default method for implementing the PMT QE
  //-----------------------------------------------------
  SetPMT_QE_Method(1);

  //default is to use collection efficiency
  SetPMT_Coll_Eff(1);
  SetLAPPD_QE_Method(1);
  SetLAPPD_Coll_Eff(1);
  // set default visualizer to OGLSX
  SetVis_Choice("OGLSX");

  //----------------------------------------------------- 
  // Make the detector messenger to allow changing geometry
  //-----------------------------------------------------

  messenger = new WCSimDetectorMessenger(this);
}

#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

void WCSimDetectorConstruction::UpdateGeometry()
{
 
  
  G4bool geomChanged = true;
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct(), geomChanged);
 
 }



WCSimDetectorConstruction::~WCSimDetectorConstruction(){
  for (unsigned int i=0;i<fpmts.size();i++){
    delete fpmts.at(i);
  }
  fpmts.clear();
  for (unsigned int i=0;i<fmrdpmts.size();i++){
    delete fmrdpmts.at(i);
  }
  fmrdpmts.clear();
    for (unsigned int i=0;i<ffaccpmts.size();i++){
    delete ffaccpmts.at(i);
  }
  ffaccpmts.clear();
  for (unsigned int i=0;i<flappds.size();i++){
    delete flappds.at(i);
  }
  flappds.clear();
  
  // MRD objects... 
  // rotation matrices
  if(noRot) delete noRot;
  if(rotatedmatx) delete rotatedmatx;
  if(upmtx) delete upmtx;
  if(downmtx) delete downmtx;
  if(rightmtx) delete rightmtx;
  if(leftmtx) delete leftmtx;
  
  // optical surfaces and materials properties tables
  if(scintSurface_op) delete scintSurface_op;
  if(MPTmylarSurface) delete MPTmylarSurface;
  if(lgSurface_op) delete lgSurface_op;
  if(lgsurf_MPT) delete lgsurf_MPT;
  
  // logical border surfaces
  for(auto surface : bordersurfaces){
    delete surface;
  }
  bordersurfaces.clear();
  
  // visualisation attributes
    for(auto visatt : mrdvisattributes){
    delete visatt;
  }
  mrdvisattributes.clear();
  
}

G4VPhysicalVolume* WCSimDetectorConstruction::Construct()
{  
  G4GeometryManager::GetInstance()->OpenGeometry();
  // having issues with StepTooSmall not being set - step sizes are ~1e-9, which *is* larger 
  // than the kCarTolerance/2 (5e-10m), but StepTooSmall isn't set for some reason...
  // Try calling SetWorldMaximumExtent to have it calculated from World size. 
  // This value must be setBEFORE ANY GEOMETRY is instantiated.
  // Seems to work - kCarTolerance is now much smaller, no more errors.
  if(isANNIE){
    // let's try setting the tolerance to try to get rid of geometry errors
    // this takes the world extent - set in WCSimDetectorConstruction as expHallLength
    G4GeometryManager::GetInstance()->SetWorldMaximumExtent(3.*WCLength);
    G4cout << "Computed tolerance = "
           << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm << " mm" << G4endl;
  }

  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalBorderSurface::CleanSurfaceTable();
  G4LogicalSkinSurface::CleanSurfaceTable();
  WCSimDetectorConstruction::PMTLogicalVolumes.clear();
  WCSimDetectorConstruction::LAPPDLogicalVolumes.clear();

  totalNumPMTs = 0;
  totalNumMrdPMTs = 0;
  totalNumFaccPMTs = 0;
  totalNumLAPPDs = 0;  
  
  //-----------------------------------------------------
  // Create Logical Volumes
  //-----------------------------------------------------

  // First create the logical volumes of the sub detectors.  After they are 
  // created their size will be used to make the world volume.
  // Note the order is important because they rearrange themselves depending
  // on their size and detector ordering.

  G4LogicalVolume* logicWCBox;
  // Select between egg-shaped HyperK and cylinder
  if (isEggShapedHyperK) logicWCBox = ConstructEggShapedHyperK();
  else if (isANNIE) logicWCBox = ConstructANNIE(); // returns a 5x5x5m box MatryoshkaMother
  else logicWCBox = ConstructCylinder();
  G4cout << " WCLength       = " << WCLength/CLHEP::m << " m"<< G4endl;

  //-------------------------------

  // Now make the detector Hall.  The lengths of the subdectors 
  // were set above.

  G4double expHallLength = 3.*WCLength; //jl145 - extra space to simulate cosmic muons more easily

  G4cout << " expHallLength = " << expHallLength / CLHEP::m << G4endl;
  G4double expHallHalfLength = 0.5*expHallLength;

  G4Box* solidExpHall = new G4Box("expHall",
				  expHallHalfLength,
				  expHallHalfLength,
				  expHallHalfLength);
  
  G4LogicalVolume* logicExpHall = 
    new G4LogicalVolume(solidExpHall,
			G4Material::GetMaterial("Vacuum"),
			"expHall",
			0,0,0);

  // Now set the visualization attributes of the logical volumes.

  //   logicWCBox->SetVisAttributes(G4VisAttributes::Invisible);
  logicExpHall->SetVisAttributes(G4VisAttributes::Invisible);

  //-----------------------------------------------------
  // Create and place the physical Volumes
  //-----------------------------------------------------
  // Experimental Hall
  G4VPhysicalVolume* physiExpHall = 
    new G4PVPlacement(0,G4ThreeVector(),
  		      logicExpHall,
  		      "expHall",
  		      0,false,0,true);

  // Water Cherenkov Detector (WC) mother volume
  // WC Box, nice to turn on for x and y views to provide a frame:

	  //G4RotationMatrix* rotationMatrix = new G4RotationMatrix;
	  //rotationMatrix->rotateX(90.*deg);
	  //rotationMatrix->rotateZ(90.*deg);

  G4ThreeVector genPosition = G4ThreeVector(0., 0., WCPosition);
  G4VPhysicalVolume* physiWCBox = 
    new G4PVPlacement(0,
		      genPosition,
		      logicWCBox,
		      "WCBox",
		      logicExpHall,
		      false,
		      0);

  // Reset the tubeID and tubeLocation maps before refiling them
  tubeIDMap.clear();
  mrdtubeIDMap.clear();
  facctubeIDMap.clear();
  lappdIDMap.clear();
  tubeLocationMap.clear();
  mrdtubeLocationMap.clear();
  facctubeLocationMap.clear();
  lappdLocationMap.clear();


  // Traverse and print the geometry Tree
  
  //  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
  //	   &WCSimDetectorConstruction::PrintGeometryTree) ;
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
	           &WCSimDetectorConstruction::DescribeAndRegisterPMT) ;
  
  TraverseReplicas(physiWCBox, 0, G4Transform3D(), 
		   &WCSimDetectorConstruction::GetWCGeom) ;
  DumpGeometryTableToFile();
  
  for(auto apmt : WCTubeCollectionMap){
    int tubeid = apmt.first;
    G4String collectionname = apmt.second;
    if(TubeIdsByCollection.count(collectionname)==0){
      TubeIdsByCollection.emplace(collectionname,std::vector<int>{tubeid});
    } else {
      TubeIdsByCollection.at(collectionname).push_back(tubeid);
    }
  }
  
  //G4cout<<"Writing GDML output file"<<G4endl;
  //G4String GDMLOutFilename = "anniegeomv3.gdml";
  //G4GDMLParser parser;  // Write GDML file
  //parser.Write(GDMLOutFilename, logicExpHall);
  //G4cout<<"GDML file "<<GDMLOutFilename<<" written"<<G4endl;
  
  // Return the pointer to the physical experimental hall
  return physiExpHall;
  
  
}

WCSimLAPPDObject *WCSimDetectorConstruction::CreateLAPPDObject(G4String LAPPDType, G4String CollectionName2)
{
  if (LAPPDType == "lappd"){
     WCSimLAPPDObject* lappd = new LAPPD;
     WCSimDetectorConstruction::SetLAPPDPointer(lappd, CollectionName2);
      return lappd;
  }
}

WCSimPMTObject *WCSimDetectorConstruction::CreatePMTObject(G4String PMTType, G4String CollectionName)
{
  if (PMTType == "PMT20inch"){
     WCSimPMTObject* PMT = new PMT20inch;
     WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
      return PMT;
  }
  else if (PMTType == "PMT8inch"){
    WCSimPMTObject* PMT = new PMT8inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT10inch"){
    WCSimPMTObject* PMT = new PMT10inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT10inchHQE"){
    WCSimPMTObject* PMT = new PMT10inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT12inchHQE"){
    WCSimPMTObject* PMT = new PMT12inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "HPD20inchHQE"){
    WCSimPMTObject* PMT = new HPD20inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "HPD12inchHQE"){
    WCSimPMTObject* PMT = new HPD12inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "BoxandLine20inchHQE"){
    WCSimPMTObject* PMT = new BoxandLine20inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "BoxandLine12inchHQE"){
    WCSimPMTObject* PMT = new BoxandLine12inchHQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "FlatFacedPMT2inch"){
    WCSimPMTObject* PMT = new FlatFacedPMT2inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "FlatFacedPMT4inch"){
    WCSimPMTObject* PMT = new FlatFacedPMT4inch;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "PMT1cm"){
    WCSimPMTObject* PMT = new PMT1cm;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "R7081"){
    WCSimPMTObject* PMT = new PMT_R7081;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "D784KFLB"){
    WCSimPMTObject* PMT = new PMT_D784KFLB;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "R5912HQE"){
    WCSimPMTObject* PMT = new PMT_R5912HQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }
  else if (PMTType == "R7081HQE"){
    WCSimPMTObject* PMT = new PMT_R7081HQE;
    WCSimDetectorConstruction::SetPMTPointer(PMT, CollectionName);
    return PMT;
  }

  else { G4cout << PMTType << " is not a recognized PMT Type. Exiting WCSim." << G4endl; exit(1);}
}

void WCSimDetectorConstruction::UpdateCylinderGeometry()
{
  WCSimPMTObject * PMT = CreatePMTObject(cylinderTank_PMTType, WCIDCollectionName);
  WCPMTName           = PMT->GetPMTName();
  WCPMTExposeHeight   = PMT->GetExposeHeight();
  WCPMTRadius         = PMT->GetRadius();
  WCIDDiameter          = cylinderTank_Diameter;
  WCIDHeight            = cylinderTank_Height;
  WCBarrelPMTOffset     = WCPMTRadius; //offset from vertical
  WCPMTPercentCoverage  = cylinderTank_Coverage;
  WCBarrelNumPMTHorizontal = round(WCIDDiameter*sqrt(pi*WCPMTPercentCoverage)/(10.0*WCPMTRadius));
  WCBarrelNRings           = round(((WCBarrelNumPMTHorizontal*((WCIDHeight-2*WCBarrelPMTOffset)/(pi*WCIDDiameter)))
                                    /WCPMTperCellVertical));
  WCCapPMTSpacing       = (pi*WCIDDiameter/WCBarrelNumPMTHorizontal); // distance between centers of top and bottom pmts
  WCCapEdgeLimit        = WCIDDiameter/2.0 - WCPMTRadius;
  G4cout << "Cylinder height " << cylinderTank_Height << "mm, diameter " << cylinderTank_Diameter << "mm, coverage "
         << cylinderTank_Coverage << "% with " << cylinderTank_PMTType << "." << G4endl;
}

void WCSimDetectorConstruction::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetDetectorName(WCDetectorName);
  wcopt->SetSavePi0(pi0Info_isSaved);
  wcopt->SetPMTQEMethod(PMT_QE_Method);
  wcopt->SetPMTCollEff(PMT_Coll_Eff);
}
