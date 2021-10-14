//  -*- mode:c++; tab-width:2;  -*-
#ifndef WCSimDetectorConstruction_H
#define WCSimDetectorConstruction_H 1

#include "WCSimPmtInfo.hh"
#include "WCSimPMTObject.hh"
#include "WCSimLAPPDInfo.hh"
#include "WCSimLAPPDObject.hh"
#include "WCSimRootOptions.hh"

#include "G4Transform3D.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VPhysicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "globals.hh"

#include <fstream>
#include <map>
#include <vector>
//#include <hash_map.h>
// warning : hash_map is not part of the standard
#include <ext/hash_map>

// **************SciBooNE integration
//#include "SBsimInputCard.hh"	--crossed out by marcus, try to simplify by extracting just mrd struct
#include "G4UserLimits.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4AssemblyVolume.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4Trd.hh"
#include "G4SystemOfUnits.hh"
#include "SBsimMRDDB.hh"
static const G4double INCH = 2.54*cm;
// *************/SciBooNE integration


using __gnu_cxx::hash;
using __gnu_cxx::hashtable;
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_multimap;

// (JF) We don't need this distinction for DUSEL
//enum cyl_location {endcap1,wall,endcap2};

class G4Box;
class G4Tubs;
class G4Material;
class G4LogicalVolume;
class G4AssemblyVolume;
class G4VPhysicalVolume;
class WCSimTuningParameters;
class WCSimDetectorMessenger;
class WCSimWCSD;

namespace __gnu_cxx  {
  template<> struct hash< std::string >
  {
    size_t operator()( const std::string& x ) const
    {
      return hash< const char* >()( x.c_str() );
    }
  };
}

class WCSimDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  WCSimDetectorConstruction(G4int DetConfig,WCSimTuningParameters* WCSimTuningPars);
  ~WCSimDetectorConstruction();

  void SaveOptionsToOutput(WCSimRootOptions * wcopt);

  G4VPhysicalVolume* Construct();

  // Related to the WC geometry
  void SetSuperKGeometry();
  void SuperK_20inchPMT_20perCent();
  void SuperK_20inchBandL_20perCent();
  void SuperK_12inchBandL_15perCent();
  void SuperK_20inchBandL_14perCent();
  void Cylinder_60x74_20inchBandL_14perCent();
  void Cylinder_60x74_20inchBandL_40perCent();
  void Cylinder_12inchHPD_15perCent();
  void SetHyperKGeometry();
  void CylinderGeometry();
  void UpdateGeometry();
  void SetANNIEPhase1Geometry();   // phase 1 geometry - just 60 PMTs on bottom, with NCV.
  void SetANNIEPhase2Geometry();   // phase 2 geometry - 60 PMTs on bottom, top, and 200 PMTs around barrel.
  void SetANNIEPhase2Geometryv2(); // phase 2 geometry - with realistic PMT types
  void SetANNIEPhase2Geometryv3(); // phase 2 geometry - later PMT layout
  void SetANNIEPhase2Geometryv4(); // phase 2 geometry -  alt later PMT layout
  void SetANNIEPhase2Geometryv5(); // phase 2 geometry -  alt later PMT layout
  void SetANNIEPhase2Geometryv6(); // phase 2 geometry -  entirely new Cylinder construction
  void SetANNIEPhase2Geometryv7(); // phase 2 geometry - read PMT positions from scan file
  
  G4int    GetTotalNumPmts(G4String key){
    if(std::find(WCTankCollectionNames.begin(), WCTankCollectionNames.end(), key)!=WCTankCollectionNames.end())
      return GetTubesInCollection(key).size();
    return -1;                                                                              // ✩
  }
  G4String GetTubeCollection(Int_t tubeID){return WCTubeCollectionMap.at(tubeID);}          // ✩
  std::vector<G4String> GetIDCollectionNames(){return WCTankCollectionNames;}               // ✩
  std::vector<Int_t> GetTubesInCollection(G4String CollectionName){                         // ✩
    if(TubeIdsByCollection.count(CollectionName)==0){
      G4cerr<<"WCSimDetectorConstruction::GetTubesInCollection could not find collection "
            <<CollectionName<<G4endl;
            return std::vector<Int_t>{};
    }
    return TubeIdsByCollection.at(CollectionName);
  }
  

  G4String GetDetectorName()      {return WCDetectorName;}
  G4double GetWaterTubeLength()   {return WCLength;}
  G4double GetWaterTubePosition() {return WCPosition;}
  G4double GetPMTSize()           {return WCPMTRadius;} // ⚠ not used
  G4String GetPMTName()           {return WCPMTName;}   // ⚠ not used
  G4int    GetMyConfiguration()   {return myConfiguration;}
  G4double GetGeo_Dm(G4int);
  G4int    GetTotalNumPmts() {return totalNumPMTs;}     // ⚠
  G4int    GetTotalNumMrdPmts() {return totalNumMrdPMTs;}
  G4int    GetTotalNumFaccPmts() {return totalNumFaccPMTs;}
  G4int    GetTotalNumLAPPDs() {return totalNumLAPPDs;}
  
  G4int    GetPMT_QE_Method(){return PMT_QE_Method;}
  G4double GetwaterTank_Length() {return waterTank_Length;} 
  G4int    UsePMT_Coll_Eff(){return PMT_Coll_Eff;}

	G4int    GetLAPPD_QE_Method(){return LAPPD_QE_Method;}
  G4int    UseLAPPD_Coll_Eff(){return LAPPD_Coll_Eff;}

  G4double GetPMTSize1() {return WCPMTSize;}
  G4double GetLAPPDSize1() {return WCLAPPDSize;}

  G4float GetPMTQE(G4String,G4float, G4int, G4float, G4float, G4float);
  G4float GetPMTCollectionEfficiency(G4float theta_angle, G4String CollectionName) { return GetPMTPointer(CollectionName)->GetCollectionEfficiency(theta_angle); };

	G4float GetLAPPDQE(G4String,G4float, G4int, G4float, G4float, G4float);
	G4float GetLAPPDCollectionEfficiency(G4float theta_angle, G4String CollectionName2) { return GetLAPPDPointer(CollectionName2)->GetCollectionEfficiency(theta_angle); };

  WCSimPMTObject *CreatePMTObject(G4String, G4String);

  std::map<G4String, WCSimPMTObject*>  CollectionNameMap;
 
  void SetPMTPointer(WCSimPMTObject* PMT, G4String CollectionName){
    if(CollectionNameMap.count(CollectionName)!=0){
      G4cerr<<"Repeated call to SetPMTPointer for collection "
            <<CollectionName<<"! Each collection should only have it's PMT type declared once!"<<G4endl;
      exit(1);
    } else {
      CollectionNameMap.emplace(CollectionName, PMT);
    }
  }

  WCSimPMTObject* GetPMTPointer(G4String CollectionName){
    WCSimPMTObject* PMTptr = nullptr;
    if(CollectionNameMap.count(CollectionName)==0){
      G4cerr << CollectionName << " is not a recognized hit collection. Exiting WCSim." << G4endl;
      exit(1);
    } else {
      PMTptr = CollectionNameMap.at(CollectionName);
    }
    if(PMTptr==nullptr){
      G4cerr<< "PMT pointer in CollectionNameMap for CollectionName "<<CollectionName<<" is null!"<<G4endl;
      exit(1);
    }
    return PMTptr;
  }
 
  G4ThreeVector GetWCOffset(){return WCOffset;}
	G4ThreeVector GetWCOffset2(){return WCOffset2;}

  //_________lappd ________

  WCSimLAPPDObject * CreateLAPPDObject(G4String, G4String);
  std::map<G4String, WCSimLAPPDObject*>  CollectionNameMap2;
  WCSimLAPPDObject * LAPPDptr;

  void SetLAPPDPointer(WCSimLAPPDObject* lappd, G4String CollectionName2){
    CollectionNameMap2[CollectionName2] = lappd;
  }
  WCSimLAPPDObject* GetLAPPDPointer(G4String CollectionName2){
    LAPPDptr = CollectionNameMap2[CollectionName2];
    if (LAPPDptr==NULL) {G4cout << CollectionName2 << " is not a recognized hit collection. Exiting WCSim." << G4endl; exit(1);}
    return LAPPDptr;
  }
 //______________________ 

  
  // Related to the WC tube ID
  static G4int GetTubeID(std::string tubeTag){return tubeLocationMap[tubeTag];}
  static G4Transform3D GetTubeTransform(int tubeNo){return tubeIDMap[tubeNo];}
  // MRD tube ID
  static G4int GetMrdTubeID(std::string tubeTag){return mrdtubeLocationMap[tubeTag];}
  static G4Transform3D GetMrdTubeTransform(int tubeNo){return mrdtubeIDMap[tubeNo];}
  // FACC tube ID
  static G4int GetFaccTubeID(std::string tubeTag){return facctubeLocationMap[tubeTag];}
  static G4Transform3D GetFaccTubeTransform(int tubeNo){return facctubeIDMap[tubeNo];}

  static G4int GetLAPPDID(std::string lappdTag){return lappdLocationMap[lappdTag];}
  static G4Transform3D GetlappdTransform(int lappdNo){return lappdIDMap[lappdNo];}

  // Related to Pi0 analysis
  G4bool SavePi0Info()              {return pi0Info_isSaved;}
  void   SavePi0Info(G4bool choice) {pi0Info_isSaved=choice;}
  
  // Related to neutron capture
  G4bool SaveCaptureInfo()              {return captureInfo_isSaved;}
  void   SaveCaptureInfo(G4bool choice) {captureInfo_isSaved=choice;}
  
  void   SetPMT_QE_Method(G4int choice){PMT_QE_Method = choice;}
  void   SetPMT_Coll_Eff(G4int choice){PMT_Coll_Eff = choice;}
	void   SetLAPPD_QE_Method(G4int choice){LAPPD_QE_Method = choice;}
  void   SetLAPPD_Coll_Eff(G4int choice){LAPPD_Coll_Eff = choice;}
  void   SetVis_Choice(G4String choice){Vis_Choice = choice;}
  G4String GetVis_Choice() {return Vis_Choice;}

  //Partition Length
  void SetwaterTank_Length(G4double length){waterTank_Length = length;}
  void SetWaterTubeLength(G4double length){WCLength = length;}
  // Geometry options
  void   SetIsUpright(G4bool choice) {isUpright = choice;}

  // *** Begin Egg-Shaped HyperK Geometry ***

  void   SetIsEggShapedHyperK(G4bool choice) {isEggShapedHyperK = choice;}
  G4bool GetIsEggShapedHyperK() {return isEggShapedHyperK;}

  void SetEggShapedHyperKGeometry();
  void SetEggShapedHyperKGeometry_withHPD();


  // *** End Egg-Shaped HyperK Geometry ***

    void SetCylinderHeight(G4double height){cylinderTank_Height = height; /*UpdateCylinderGeometry();*/}
    void SetCylinderDiameter(G4double diameter){cylinderTank_Diameter = diameter; /*UpdateCylinderGeometry();*/}
    void SetCylinderCoverage(G4double coverage){cylinderTank_Coverage = coverage; /*UpdateCylinderGeometry();*/}
    void SetCylinderPMTType(G4String pmtType){cylinderTank_PMTType = pmtType; /*UpdateCylinderGeometry();*/}

    void UpdateCylinderGeometry();

    void   SetIsCylinder(G4bool choice) {isCylinder = choice;}
    G4bool GetIsCylinder() {return isCylinder;}

  std::vector<WCSimPmtInfo*>* Get_Pmts() {return &fpmts;}
  std::vector<WCSimLAPPDInfo*>* Get_LAPPDs() {return &flappds;}

  G4String GetIDCollectionName(){return WCIDCollectionName;} // ⚠
	G4String GetIDCollectionName2(){return WCIDCollectionName2;}

    WCSimTuningParameters* Get_TuningParams(){return WCSimTuningParams;} 

private:

  // Tuning parameters

  WCSimTuningParameters* WCSimTuningParams;

  // Sensitive Detectors. We declare the pointers here because we need
  // to check their state if we change the geometry, otherwise will segfault
  // between events!
  WCSimWCSD* aWCPMT;
  
  WCSimWCSD* aWCLAPPD;

  //Water, Blacksheet surface
  G4OpticalSurface * OpWaterBSSurface;

  //Glass, Cathode surface in PMTs

  G4OpticalSurface * OpGlassCathodeSurface;
  G4OpticalSurface * OpGlassCathodeSurface_R7081;

  //Tyvek surface - jl145
  G4OpticalSurface * OpWaterTySurface;
  
  //Inner structure surface
  G4OpticalSurface * InnerStructureOpSurface;
  
  // Tank liner surface
  G4OpticalSurface * LinerOpSurface;

  // ANNIE Holder surface
  G4OpticalSurface * HolderOpSurface;

  // LUX / ETEL Holder surface
  G4OpticalSurface * LUXHolderOpSurface;

  // The messenger we use to change the geometry.

  WCSimDetectorMessenger* messenger;

  // The Construction routines
  G4LogicalVolume* ConstructCylinder();
  G4LogicalVolume* ConstructANNIECylinder();
  G4LogicalVolume* ConstructANNIECylinderScan();
  G4LogicalVolume* ConstructPMT(G4String,G4String, G4String detectorElement="tank");
  G4LogicalVolume* ConstructFlatFacedPMT(G4String PMTName, G4String CollectionName, G4String detectorElement="mrd");
  G4LogicalVolume* ConstructLAPPD(G4String,G4String);

  G4LogicalVolume* ConstructCaps(G4int zflip);
  void ConstructANNIECaps(G4int zflip);
  void ConstructANNIECapsSheet(G4int zflip); //Only create the black sheet for top and bottom planes, no PMT placement

  void ConstructANNIEHolders(); //Construct ANNIE PMT holders
  void ConstructLUXETELHolders(); //Construct LUX + ETEL PMT holders (top + bottom PMTs)

  void  ConstructMaterials();

  G4LogicalVolume* logicWCBarrel;
  G4VPhysicalVolume* physiWCBarrel;
  G4LogicalVolume* logicWCBarrelCellBlackSheet;
  G4LogicalVolume* logicWCTowerBlackSheet;
  G4double capAssemblyHeight;

  G4bool WCAddGd;

  // Code for traversing the geometry and assigning tubeIDs.

  // First make a typedef for the pointer to the member fcn.  The
  // syntax is too wacked out to be using all over.

  typedef void (WCSimDetectorConstruction::*DescriptionFcnPtr)
    (G4VPhysicalVolume*, int, int, const G4Transform3D&);

  // Now Funcs for traversing the geometry
  void TraverseReplicas(G4VPhysicalVolume*, int, const G4Transform3D&,
			DescriptionFcnPtr);

  void DescribeAndDescendGeometry(G4VPhysicalVolume*, int, int, 
				  const G4Transform3D&, DescriptionFcnPtr);

  // Functions that the traversal routines call or we use to manipulate the
  // data we accumulate.
  void DumpGeometryTableToFile();

  void PrintGeometryTree(G4VPhysicalVolume*, int, int, const G4Transform3D&);
  void DescribeAndRegisterPMT(G4VPhysicalVolume*, int, int, 
			      const G4Transform3D&);
  void DescribeAndRegisterPMT_1KT(G4VPhysicalVolume*, int, int, 
				  const G4Transform3D&);
  void GetWCGeom(G4VPhysicalVolume*, int, int, 
			      const G4Transform3D&);

  //---Volume lengths

  // These are shared between the different member functions 
  // constructWC, constructFGD, constructlArD, constuctMRD
  // toggle between FGD(0) and lArD(1)
  // toggle between lArD readout types
  // toggle between MRDScint and MRDk2k

  G4bool pi0Info_isSaved;
  G4bool captureInfo_isSaved;


  // XQ 08/17/10
  //   PMT_QE_Method == 1
  //   Only use it in the stacking function (no WLS)
  //   PMT_QE_Method == 2
  //   Use Part of it in the stacking function (MAX QE)
  //   Put the rest of it in the sensitive detector according to QE/Max_QE
  //   PMT_QE_Method == 3
  //   Put all of it in the sensitive detector according to QE
  //   Good for low energy photons
  G4int PMT_QE_Method;
	G4int LAPPD_QE_Method;

  //XQ 03/31/11
  // 0 to not use collection efficiency
  // 1 to use
  G4int PMT_Coll_Eff;
	G4int LAPPD_Coll_Eff;

  //NP 06/17/15
  // "OGLSX" for classic visualization
  // "RayTracer" for RayTracer visualization
  G4String Vis_Choice;
  

  G4double WCLength;

  G4double WCPosition;
  
  // Hit collection name parameters
  G4String WCDetectorName;
  G4String WCIDCollectionName; // ⚠
  G4String WCODCollectionName;
	G4String WCIDCollectionName2;
  std::map<G4String, G4String> WCPMTNameMap;                 // ✩
  std::map<G4String, G4double> WCPMTRadiusMap;               // ✩
  std::map<G4String, G4double> WCPMTExposeHeightMap;         // ✩
  std::map<G4String, std::vector<int> > TubeIdsByCollection; // ✩ Store all tubeIDs in each collection
  std::map<Int_t, G4String> WCTubeCollectionMap;             // ✩ Map a tubeID to the collection it's in
  std::vector<G4String> WCTankCollectionNames;               // ✩

  // WC PMT parameters
  G4String WCPMTName; // ⚠
  typedef std::pair<G4String, G4String> PMTKey_t;
  typedef std::map<PMTKey_t, G4LogicalVolume*> PMTMap_t;

  static PMTMap_t PMTLogicalVolumes;

  //LAPPD 
  G4String WCLAPPDName;
  typedef std::pair<G4String, G4String> LAPPDKey_t;
  typedef std::map<LAPPDKey_t, G4LogicalVolume*> LAPPDMap_t;
  static LAPPDMap_t LAPPDLogicalVolumes;

  // WC geometry parameters

  G4double WCPMTRadius;       // ⚠
  G4double WCPMTExposeHeight; // ⚠
  G4double WCBarrelPMTOffset;

  G4double WCLAPPDRadius;
  G4double WCLAPPDExposeHeight;
  G4double WCBarrelLAPPDOffset;
  G4double WCIDDiameter;
  G4double WCLAPPDSliderThickness;

  G4double WCCapLength;
  G4double WCBackODLength;
  G4double WCFrontODLength;
  G4double WCIDHeight;

  G4double WCBarrelRingRadius;

  G4double WCBarrelRingNPhi;
  G4double WCBarrelNRings;
  G4double WCPMTperCellHorizontal;
  G4double WCPMTperCellVertical;
  // added for ANNIE
  G4double WCBarrelCellNPhiGon;
  G4double WCBarrelCelldPhi;
  G4double WCBarrelCellStartPhi;

  G4double WCPMTPercentCoverage;

  G4double WCBarrelNumPMTHorizontal;
  G4double WCCapPMTSpacing;

  G4double WCBarrelRingNPhi2;
  G4double WCBarrelNRingsLAPPD;
  G4double WCLAPPDPercentCoverage;
  G4double WCLAPPDperCellHorizontal;
  G4double WCLAPPDperCellVertical;
  G4double WCBarrelNumLAPPDHorizontal;
  
  G4double WCCapLAPPDSpacing;
  G4double WCCapEdgeWidth;//jh TODO: not used
  
  G4double WCCapEdgeLimit;
  G4double WCBlackSheetThickness;

// raise scope of derived parameters
  G4double WCIDRadius;
  G4double totalAngle;
  G4double dPhi;
  G4double barrelCellHeight;
  G4double mainAnnulusHeight;
  G4double innerAnnulusRadius;
  G4double outerAnnulusRadius;
  G4String water;

  G4double totalAngle2;
  G4double dPhi2;
  G4double barrelCellHeight2;
  G4double mainAnnulusHeight2;

 //for 1kt
  G4double WCDiameter;
  G4double WCRadius;
  G4double WCBarrelPMTRadius;
  G4double WCBarrelRingdPhi;
  G4double WCBarrelCellLength;
  G4double WCCapNCell;
  G4double WCBarrelLength;
  
  // for annie
  G4int PMTcounter;                  // ANNIEp2v6 needs this available in ConstructANNIECaps 
  G4double WCCapPMTOffset;           // offset of the cap PMTs toward the centre of the tank.
  G4double WCBorderBarrelTopPMTOffset; // more fudging for ANNIE
  G4int numhatchpmts;
  G4bool constructmrd;
  G4bool constructveto;
  G4double compressionfactor;        // ratio to squeeze PMTs together on an octagon face, relative to full width
  G4double capcompressionratio;      // aspect ratio of PMT spacing on bottom cap
  G4double capcentrebarwidth;        // cap PMT offset due to width of central bar
  G4double WCCapPMTPosRadius;        // radius at which to position top cap PMTs, for some ANNIE designs
  G4double WCCapPMTPosRadius2;       // radius at which to position PMTs on the hatch
  G4double barrelcompressionfactor;  // how much to squeeze barrel PMTs from caps, ANNIE PMTs fill only central ~60%
  G4double barrelbordercompressionfactor;  // ANNIE border ring sizes also need tuning
  G4double InnerStructureCentreOffset;

  G4double WCCapTopPMTOffset;        // offsets are different for ANNIE
  G4double WCCapBottomPMTOffset;     // 

  // amb79: to universally make changes in structure and geometry
  bool isUpright;

  // *** Begin egg-shaped HyperK Geometry ***

    void MatchWCSimAndEggShapedHyperK();
    G4LogicalVolume* ConstructEggShapedHyperK();

    G4Material* FindMaterial(G4String);

    G4VSolid* ConstructHalf(G4double, G4double);

    G4LogicalVolume* ConstructRadialPMT(G4bool,
                                        G4double, G4double,
                                        G4double, G4double,
                                        G4double, G4double);

    G4LogicalVolume* ConstructEndWallPMT();

    G4LogicalVolume* ConstructCeilingPMT(G4bool,
                                         G4double, G4double,
                                         G4double, G4double);

    G4bool isEggShapedHyperK;

    G4double waterTank_TopR;
    G4double waterTank_BotR;
    G4double waterTank_Height;
    G4double waterTank_UpperA;
    G4double waterTank_LowerB;
    G4double waterTank_Length;

    G4double innerPMT_TopR;
    G4double innerPMT_BotR;
    G4double innerPMT_TopW;
    G4double innerPMT_BotW;
    G4double innerPMT_Height;
    G4double innerPMT_Radius;
    G4double innerPMT_Expose;
    G4double innerPMT_Rpitch;
    G4double innerPMT_Apitch;

    G4double outerPMT_TopR;
    G4double outerPMT_BotR;
    G4double outerPMT_TopW;
    G4double outerPMT_BotW;
    G4double outerPMT_Height;
    G4double outerPMT_Radius;
    G4double outerPMT_Expose;
    G4String outerPMT_Name;
    G4double outerPMT_TopRpitch;
    G4double outerPMT_BotRpitch;
    G4double outerPMT_Apitch;

    G4double blackSheetThickness;
	  G4double innerLAPPD_Height;
    G4double innerLAPPD_Radius;
    G4double innerLAPPD_Expose;
	  G4double outerLAPPD_Height;
    G4double outerLAPPD_Radius;
    G4double outerLAPPD_Expose;
	  G4String outerLAPPD_Name;

    G4int innerPMT_TopN;
    G4int innerPMT_BotN;

    G4bool checkOverlaps;
    G4LogicalVolume* waterTankLV;

    G4int PMTCopyNo;
    G4int wallSlabCopyNo;

  // *** End egg-shaped HyperK Geometry ***


    G4double cylinderTank_Height;
    G4double cylinderTank_Diameter;
    G4double cylinderTank_Coverage;
    G4String cylinderTank_PMTType;
    G4bool isCylinder;


    // amb79: debug to display all parts
  bool debugMode;

  // Variables related to the geometry

  std::ofstream geoFile;   // File for text output

  G4int totalNumPMTs;       // ⚠ The number of PMTs for this configuration     
  G4double WCCylInfo[3];    // Info for the geometry tree: radius & length or mail box, length, width and depth
  G4double WCPMTSize;       // ⚠ Info for the geometry tree: pmt size
  G4ThreeVector WCOffset;   // Info for the geometry tree: WC center offset

  G4int totalNumLAPPDs; 
  G4double WCLAPPDSize;  
  G4ThreeVector WCOffset2;   // Info for the geometry tree: WC center offset
 
  // Tube map information

  static std::map<int, G4Transform3D> tubeIDMap;
//  static std::map<int, cyl_location> tubeCylLocation;
  static hash_map<std::string, int, hash<std::string> >  tubeLocationMap; 
 
  static std::map<int, G4Transform3D> lappdIDMap;
  static hash_map<std::string, int, hash<std::string> >  lappdLocationMap;

  // Variables related to configuration

  G4int myConfiguration;   // Detector Config Parameter
  G4double innerradius;
 
  std::vector<WCSimPmtInfo*> fpmts;
  std::vector<WCSimLAPPDInfo*> flappds;
  
  // MRD & Veto variables
  // ====================================================
  public:
  G4LogicalVolume* ConstructANNIE();
  void DefineMRD(G4PVPlacement* expHall);
  void DefineANNIEdimensions();
  void AddANNIEPhase1PMTs(G4LogicalVolume* waterTank_log);
  void AddANNIEPhase2PMTs(G4LogicalVolume* waterTank_log);
  void ConstructMRD(G4LogicalVolume* expHall_log, G4VPhysicalVolume* expHall_phys);
  void ConstructVETO(G4LogicalVolume* expHall_log, G4VPhysicalVolume* expHall_phys);
  void ConstructNCV(G4LogicalVolume* waterTank_log);
  int GetLayerNum(int copyNo, int* paddlenuminlayer, bool* ishpaddle);
  int GetNumPaddlesInLayer(int layernum);
  void ComputePaddleTransformation (const G4int copyNo, G4VPhysicalVolume* physVol);
  void ComputeTaperTransformation (const G4int copyNo, G4VPhysicalVolume* physVol, G4int selector, G4bool additionaloffset);
  void ComputeSteelTransformation (const G4int copyNo, G4VPhysicalVolume* physVol);
  void ComputeVetoPaddleTransformation (const G4int copyNo, G4VPhysicalVolume* physVol, G4int selector);
  void PlacePaddles(G4LogicalVolume* totMRD_log);
  void PlaceTapers(G4LogicalVolume* totMRD_log);
  void PlaceLGs(G4LogicalVolume* totMRD_log);
  void PlaceMRDSDSurfs(G4LogicalVolume* totMRD_log);
  void PlaceMRDPMTs(G4LogicalVolume* totMRD_log);
  void PlaceMylarOnMRDPaddles(G4VPhysicalVolume* expHall_phys, G4VPhysicalVolume* totMRD_phys);
  void PlaceSteels(G4LogicalVolume* totMRD_log);
  void makeAlu(G4AssemblyVolume* totMRD);
  void MakeAluSciBooNE(G4LogicalVolume* totMRD_log);
  void PlaceVetoPaddles(G4LogicalVolume* totVeto_log);
  void PlaceVetoLGs(G4LogicalVolume* totVeto_log);
  void PlaceVetoSDsurfs(G4LogicalVolume* totVeto_log);
  void PlaceVetoPMTs(G4LogicalVolume* totVeto_log);
  void PlaceMylarOnFACCPaddles(G4VPhysicalVolume* expHall_phys, G4VPhysicalVolume* totVeto_phys);
  G4String GetMRDPMTName()			  {return MRDPMTName;}
  G4double GetMRDPMTRadius()      {return MRDPMTRadius;}
  G4String GetFACCPMTName()			  {return FACCPMTName;}
  G4double GetFACCPMTRadius()     {return FACCPMTRadius;}
  G4String GetMRDCollectionName(){return WCMRDCollectionName;}
  G4String GetFACCCollectionName(){return WCFACCCollectionName;}
  G4bool GetIsANNIE(){return isANNIE;}
//  private:

  G4bool isANNIE;
  G4String GDMLFilename;
  G4String GDMLInnerStructureFilename;
  G4bool addGDMLinnerstructure;
  G4double tankouterRadius;
  G4double tankhy;
  G4double tankzoffset;
  G4double tankyoffset;
  G4double expHall_x;
  G4double expHall_y;
  G4double expHall_z;
  G4bool doOverlapCheck;
  
  G4String WCMRDCollectionName;
  G4String WCFACCCollectionName;
  G4String MRDPMTName;
  G4double MRDPMTExposeHeight;
  G4double MRDPMTRadius;
  G4String FACCPMTName;
  G4double FACCPMTExposeHeight;
  G4double FACCPMTRadius;
  WCSimWCSD* afacc;
  WCSimWCSD* aMRDPMT;
  
  // info for map of PMT locations
  G4int totalNumMrdPMTs;
  G4int totalNumFaccPMTs;
  static std::map<int, G4Transform3D> mrdtubeIDMap; 
  static std::map<int, G4Transform3D> facctubeIDMap;
  static hash_map<std::string, int, hash<std::string> > mrdtubeLocationMap;
  static hash_map<std::string, int, hash<std::string> > facctubeLocationMap; 
  std::vector<WCSimPmtInfo*> fmrdpmts, ffaccpmts;
  std::vector<WCSimPmtInfo*>* Get_MrdPmts() {return &fmrdpmts;}
  std::vector<WCSimPmtInfo*>* Get_FaccPmts() {return &ffaccpmts;}
  
	G4double Xposition, Yposition, Zposition;		// used for positioning parameterisations.
	G4int numpaddlesperpanelh;									// paddles per h scintillator panel
	G4int numpaddlesperpanelv;									// paddles per v scintillator panel
	std::vector<int> numpaddlesperpanelvv;			// not all the same in refurbished MRD
	G4int nummrdpmts;														// total number of mrd paddles
	G4int numpanels;														// scintillator panels
	G4int numhpanels;														// horizontal scintillator panels
	G4int numvpanels;														// vertical scintillator panels
	G4int numrpcs;															// rpc panels
	G4int numplates;														// steel plates
	G4int numalustructs;												// number of supporting structs. We may be dropping one as we have fewer scintillators?
	G4int numvetopaddles;												// number of scintillator paddles in the FACC; 13 panels in 2 layers
	G4int vetopaddlesperpanel;									// number of scintillator paddles in each FACC panel

	G4double scintbordergap;										// gap between each scintillator (cm) (border to account for cladding etc)
	G4double steelscintgap;											// gap between steel and scintillator
	G4double scintalugap;												// gap between scintillator and alu struct
	G4double alusteelgap; 											// gap between alu struct and subsequent steel of next layer
	G4double layergap;													// total gaps of layers

	G4double steelfullxlen;
	G4double steelfullylen;
	G4double steelfullzlen;

	G4double scintfullxlen;
	G4double scintfullxlen2;
	G4double scintfullzlen;
	G4double scintfullzlen2;
	G4double scinthfullylen;
	G4double scintvfullylen;

	G4double scinttapfullwidth; 								// width of the tapering part of scintillator paddles at the narrow end
	G4double scinttapfullheight; 								// z length of tapering part of scint paddles.

	G4double scintlgfullwidth; 									// tapered light guides at narrow end
	G4double scintlgfullheight;

	G4double alufullxlen;												// outer thicknesses - total frame dims are about those of the steel plate
	G4double alufullylen;												// 
	G4double alufullzlen;												// 
	G4double alufullxthickness;									// 
	G4double alufullythickness;
	G4double windowwidth;												// (full length - 4 beams) / 3 windows
	G4double windowheight;
	
	G4double mrdpmtfullheight;									// full length of MRD PMTs

	G4double mrdZlen; 

	G4double vetopaddlefullxlen;
	G4double vetopaddlefullylen;
	G4double vetopaddlefullzlen;
	G4double vetolayer2offset;
	G4double vetopaddlegap;
	G4double nothickness;
	
	G4double vetolgfullxlen;								// width of the veto light guides at narrow end
	G4double vetolgfullylen;
	
	G4double vetopmtfullheight;

	G4double vetoZlen;
	G4double vetolayerthickness;

	G4double maxwidth;
	G4double maxheight;
	G4double mrdZoffset;

	// Define solids 
	//==============  
	// G4Box* variableName = new G4Box("SolidName", x_halflength, y_halflength, z_halflength);
	// G4Trd("SolidName", x_halflength1, x_halflength2, y_halflength1, y_halflength2, z_halflength); 
	// 2 x and y dimensions given - define dims of the cross-sections at the two z ends. 

	G4Box* sciMRDhpaddle_box;										// Paddles - h 
	G4Box* sciMRDvpaddle_box;										// and v
	G4Box* sciMRDvpaddle_box2;									// kTeV version
	G4Trd* mrdScintTap_box;											// Paddle Tapered ends

	G4Trd* mrdLG_box;														// Tapered Light Guides
	G4Trd* mrdLG_box2;													// KTeV version
	G4Box* mrdSurface_box;											// little box for surface to detect photons

	G4Box* steelMRDplate_box;										// Steel plates

	G4Box* aluMRDstruc_box;											// The alu support structure is roughly the external size of the steel plates...
	G4Box* aluMRDwindow_box;										// with a 3x3 grid of ~even sized holes  

	G4Box* totMRD_box;

	G4Box* vetoPaddle_box;
	G4Box* vetoSurface_box;
	G4Box* totVeto_box;
	G4Trd* vetoLG_box;
	
	
	// Define logical volumes 
	//=======================
	// G4LogicalVolume* variableName = new G4LogicalVolume(solidVariableName, Material, "logicalVolName");
	// Can't do this outside a function - the materials aren't defined yet.

	G4LogicalVolume* hpaddle_log;
	G4LogicalVolume* vpaddle_log;
	G4LogicalVolume* vpaddle_log2;
	G4LogicalVolume* taper_log;
	G4LogicalVolume* lg_log;
	G4LogicalVolume* lg_log2;
	G4LogicalVolume* steel_log;
	G4LogicalVolume* mrdsdsurf_log;

	G4LogicalVolume* vetoPaddle_log;
	G4LogicalVolume* vetol2Paddle_log;
	G4LogicalVolume* vetoSurface_log;
	G4LogicalVolume* vetolg_log;
	
	G4LogicalVolume* logicMRDPMT;
	G4LogicalVolume* logicFACCPMT;
	
	G4OpticalSurface* scintSurface_op;	// mylar reflective surface
	G4MaterialPropertiesTable* MPTmylarSurface;
	G4OpticalSurface* lgSurface_op;			// optical surface with efficiency of detection defined, for boundary detection on ends of LGs
	G4MaterialPropertiesTable* lgsurf_MPT;

	// Physical Volumes
	// ================
	// to place optical surface between tapers and LGs for optical detection we need pointers to the physical volumes
	// declare containers for the pointers here, they'll be filled by the placement function.
	std::vector<G4VPhysicalVolume*> paddles_phys;
	std::vector<G4VPhysicalVolume*> tapers_phys;
	std::vector<G4VPhysicalVolume*> lgs_phys;
	std::vector<G4VPhysicalVolume*> mrdsdsurfs_phys;
	std::vector<G4VPhysicalVolume*> vetopaddles_phys;
	std::vector<G4VPhysicalVolume*> vetosurfaces_phys;
	std::vector<G4VPhysicalVolume*> vetolgs_phys;
	std::vector<G4LogicalBorderSurface*> bordersurfaces;

	// Define rotation matrices 
	//=========================
	// rotated and unrotated scintillator paddles. Could do away with one by changing dims but hey.
	G4RotationMatrix* noRot;										// null rotation pointer
	G4RotationMatrix* rotatedmatx;							// horizontal config for scint paddles

	// Note trapezium narrows along z axis, so need to define a rotation of 90deg about the y(??)-axis to bring taper into the x-y plane. 
	G4RotationMatrix* upmtx;
	G4RotationMatrix* downmtx;
	G4RotationMatrix* rightmtx;
	G4RotationMatrix* leftmtx;

	// Define Visualisation Attributes 
	//================================
	std::vector<G4VisAttributes*> mrdvisattributes;
	G4VisAttributes* scinthatts;
	G4VisAttributes* scintvatts;
	G4VisAttributes* steelatts;
	G4VisAttributes* scinttapatts;
	G4VisAttributes* scintlgatts;
	G4VisAttributes* senssurfatts;
	
  // End MRD & Veto variables
  // ====================================================
  
  public:
    // ****************SciBooNE integration
    int startindex;
    G4bool useadditionaloffset;
    SBsimMRDDB*     mrddb;
    inline SBsimMRDDB* GetMRDDB() {return mrddb;};
    
    G4Material *Air, *Al, *Fe, *Pb;
    G4Material *Steel, *Scinti, *Lucite;
    G4Material *ECScinti, *MRDIron;
    G4Material *Plywood;
    
		// for MRD
		G4VSolid *MRDIron_Solid[12];
		G4VSolid *MRDVScinti_Solid, *MRDHScinti_Solid;
		G4VSolid *MRDTScinti_Solid;
		G4VSolid *MRDLG_Solid;

		G4LogicalVolume *MRDIron_LV[12];
		G4LogicalVolume *MRDVScinti_LV, *MRDHScinti_LV;
		G4LogicalVolume *MRDTScinti_LV;
		G4LogicalVolume *MRDLG_LV;

		// for MRD Al support
		G4SubtractionSolid *MRDAlV1_Solid;
		G4SubtractionSolid *MRDAlV2_Solid;
		G4SubtractionSolid *MRDAlV3_Solid;
		G4SubtractionSolid *MRDAlV4_Solid;
		G4SubtractionSolid *MRDAlV5_Solid;
		G4SubtractionSolid *MRDAlH1_Solid;
		G4SubtractionSolid *MRDAlH2_Solid;
		G4SubtractionSolid *MRDAlH3_Solid;

		G4VSolid *MRDAlV1_Outer;
		G4VSolid *MRDAlV2_Outer;
		G4VSolid *MRDAlV3_Outer;
		G4VSolid *MRDAlV4_Outer;
		G4VSolid *MRDAlV5_Outer;
		G4VSolid *MRDAlH1_Outer;
		G4VSolid *MRDAlH2_Outer;
		G4VSolid *MRDAlH3_Outer;

		G4VSolid *MRDAlV1_Inner;
		G4VSolid *MRDAlV2_Inner;
		G4VSolid *MRDAlV3_Inner;
		G4VSolid *MRDAlV4_Inner;
		G4VSolid *MRDAlV5_Inner;
		G4VSolid *MRDAlH1_Inner;
		G4VSolid *MRDAlH2_Inner;
		G4VSolid *MRDAlH3_Inner;

		G4LogicalVolume *MRDAlV1_LV;
		G4LogicalVolume *MRDAlV2_LV;
		G4LogicalVolume *MRDAlV3_LV;
		G4LogicalVolume *MRDAlV4_LV;
		G4LogicalVolume *MRDAlV5_LV;
		G4LogicalVolume *MRDAlH1_LV;
		G4LogicalVolume *MRDAlH2_LV;
		G4LogicalVolume *MRDAlH3_LV;

		G4AssemblyVolume * MRDAlSupportV;
		G4AssemblyVolume * MRDAlSupportH;
    // *********/SciBooNE integration
  
};

#endif

