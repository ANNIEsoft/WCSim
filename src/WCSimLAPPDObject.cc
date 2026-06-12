#include "WCSimWCLAPPD.hh"
#include "WCSimLAPPDObject.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"
#include "WCSimLAPPDInfo.hh"
#include "G4SystemOfUnits.hh"

#include <vector>
// for memset
#include <cstring>


////////////////////////////////////////////////////////////////////////////////////////////////
// LAPPD Base Class

G4float  WCSimLAPPDObject::GetCollectionEfficiency(float angle)
{
    return Interpolate_func(angle, 10, GetCollectionEfficiencyAngle(), GetCollectionEfficiencyArray())/100.;
}

G4float WCSimLAPPDObject::Interpolate_func(G4float x, G4int ncount, G4float *angle, G4float *quantity){
  // linear interpolate the quantity function versus angle                                                                                                                        
  if (x < *angle || x >=*(angle+ncount-1)){
    return 0;
  }else{
    for (Int_t i=0;i!=ncount;i++){
      if (x>=*(angle+i) && x < *(angle+i+1)){
        return (x-*(angle+i))/(*(angle+i+1)-*(angle+i))* (*(quantity+i+1)) + (*(angle+i+1)-x)/(*(angle+i+1)-*(angle+i)) * (*(quantity+i));
      }
    }
  }

  // Error Condition
  G4cerr << "Interpolation failure." << G4endl;
  assert(false);
  return -999.;
}


// By default, collection efficiency is binned in 10-degree angular bins from 0 to 90
// This can be overridden by setting GetCE in the derived class
G4float* WCSimLAPPDObject::GetCollectionEfficiencyAngle(){
  static G4float angle[10] = { 0., 10., 20., 30., 40., 50., 60., 70., 80., 90.};
  return angle;
}


// By default, each PMT has 100% collection efficiency at all angles
// This can be overridden by setting GetCE in the derived class
G4float* WCSimLAPPDObject::GetCollectionEfficiencyArray(){
  static G4float CE[10] = { 100., 100., 100., 100., 100., 100., 100., 100., 100., 100.};
  // CollectionEfficiency before modification on 2015-03-27 (Different from SKDetSim)
  // static G4float CE[10]={100,100,99,95,90,85,80,69,35,13}; 

  // 0.05 height
  //static G4float CE[10] = {83.08139999999999,86.38524450659465,89.77418831538222,92.6474904290534,95.61177509118487,98.17723228980405,99.52376371657738,93.51187976305238,76.20134983323001,0.0};
  // 0.05 height + 1
  //static G4float CE[10] = {84.0814, 87.38524450659465, 90.77418831538222, 93.6474904290534, 96.61177509118487, 99.17723228980405, 100.52376371657739, 94.51187976305238, 77.20134983323001, 0.0};

  //static G4float CE[10] = {0, 0, 0, 96.61177509118487, 99.17723228980405, 100.52376371657739, 94.51187976305238, 77.20134983323001, 0.0};
  // ~ 5cm active
  //static G4float CE[10] = {100, 100, 100, 100, 100, 0,0,0,0,0};
  // ~ 1cm active
  // ?
  
  
  
  // 0.0092456 height
  //static G4float CE[10] = {83.08139999999999, 83.69232049540342, 84.34244850736174, 85.08174680488179, 85.98863042107332, 87.21047141188019, 89.08244041464535, 91.57234059392873, 97.78303362808366, 0.0};
  //To enable the collection efficiency parameter, the user must set `/WCSim/PMTCollEff on` in the macro.

  return CE;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// LAPPDs inch (dimensions not added yet)

LAPPD::LAPPD() {}
LAPPD::~LAPPD(){}

G4String LAPPD::GetLAPPDName() {G4String LAPPDName = "lappd_v1"; return LAPPDName;}
G4double LAPPD::GetExposeHeight() {return .0092456*CLHEP::m;} // z dimension: LAPPD half height
G4double LAPPD::GetRadius() {return  .1015*CLHEP::m;} //here the radius is x,y for the lappd (active area)
G4double LAPPD::GetLAPPDGlassThickness() {return 0.001375*CLHEP::m;} //half the glass thickness becuse it will be substracted twice
float LAPPD::HitTimeSmearing(float Q) {
  float timingConstant = 10.0; 
  float timingResolution = 0.33 + sqrt(timingConstant/Q); 
  // looking at SK's jitter function for 20" tubes
  if (timingResolution < 0.58) timingResolution=0.58;
  float Smearing_factor = G4RandGauss::shoot(0.0,timingResolution);
  return Smearing_factor;

}

G4float* LAPPD::Getqpe()
   {
  static G4float qpe0[501]= {
    // 1
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 0.000129, 0.000754, 0.004060, 0.028471,
    // 2
    0.068449, 0.115679, 0.164646, 0.203466, 0.235631,
    0.262351, 0.282064, 0.303341, 0.320618, 0.338317,
    0.357825, 0.371980, 0.385820, 0.398838, 0.413595,
    0.428590, 0.444387, 0.461685, 0.482383, 0.502369,
    0.520779, 0.540011, 0.559293, 0.579354, 0.599337,
    0.619580, 0.639859, 0.659807, 0.679810, 0.699620,
    0.718792, 0.737382, 0.755309, 0.772042, 0.788232,
    0.803316, 0.817861, 0.831148, 0.844339, 0.855532,
    0.866693, 0.876604, 0.886067, 0.894473, 0.902150,
    0.909515, 0.915983, 0.922050, 0.927418, 0.932492,
    // 3
    0.936951, 0.940941, 0.944660, 0.948004, 0.951090,
    0.953833, 0.956576, 0.958886, 0.961134, 0.963116,
    0.964930, 0.966562, 0.968008, 0.969424, 0.970687,
    0.971783, 0.972867, 0.973903, 0.974906, 0.975784,
    0.976632, 0.977438, 0.978190, 0.978891, 0.979543,
    0.980124, 0.980666, 0.981255, 0.981770, 0.982227,
    0.982701, 0.983146, 0.983566, 0.983975, 0.984357,
    0.984713, 0.985094, 0.985404, 0.985739, 0.986049,
    0.986339, 0.986630, 0.986922, 0.987176, 0.987431,
    0.987655, 0.987922, 0.988173, 0.988414, 0.988639,
    // 4
    0.988856, 0.989065, 0.989273, 0.989475, 0.989662,
    0.989828, 0.990007, 0.990172, 0.990327, 0.990497,
    0.990645, 0.990797, 0.990981, 0.991135, 0.991272,
    0.991413, 0.991550, 0.991673, 0.991805, 0.991928,
    0.992063, 0.992173, 0.992296, 0.992406, 0.992514,
    0.992632, 0.992733, 0.992837, 0.992954, 0.993046,
    0.993148, 0.993246, 0.993354, 0.993458, 0.993549,
    0.993656, 0.993744, 0.993836, 0.993936, 0.994033,
    0.994134, 0.994222, 0.994307, 0.994413, 0.994495,
    0.994572, 0.994659, 0.994739, 0.994816, 0.994886,
    // 5
    0.994970, 0.995032, 0.995110, 0.995178, 0.995250,
    0.995321, 0.995383, 0.995464, 0.995532, 0.995609,
    0.995674, 0.995750, 0.995821, 0.995889, 0.995952,
    0.996010, 0.996071, 0.996153, 0.996218, 0.996283,
    0.996335, 0.996384, 0.996431, 0.996484, 0.996537,
    0.996597, 0.996655, 0.996701, 0.996745, 0.996802,
    0.996860, 0.996917, 0.996962, 0.997014, 0.997079,
    0.997114, 0.997165, 0.997204, 0.997250, 0.997295,
    0.997335, 0.997379, 0.997418, 0.997454, 0.997488,
    0.997530, 0.997573, 0.997606, 0.997648, 0.997685,
    // 6
    0.997725, 0.997762, 0.997795, 0.997835, 0.997866,
    0.997898, 0.997941, 0.997966, 0.997997, 0.998039,
    0.998065, 0.998104, 0.998128, 0.998153, 0.998179,
    0.998205, 0.998223, 0.998254, 0.998293, 0.998319,
    0.998346, 0.998374, 0.998397, 0.998414, 0.998432,
    0.998456, 0.998482, 0.998511, 0.998532, 0.998553,
    0.998571, 0.998594, 0.998614, 0.998638, 0.998669,
    0.998693, 0.998715, 0.998743, 0.998762, 0.998793,
    0.998812, 0.998834, 0.998857, 0.998872, 0.998888,
    0.998904, 0.998926, 0.998946, 0.998963, 0.998983,
    // 7
    0.999007, 0.999027, 0.999044, 0.999064, 0.999079,
    0.999096, 0.999120, 0.999133, 0.999152, 0.999160,
    0.999174, 0.999188, 0.999206, 0.999221, 0.999234,
    0.999248, 0.999263, 0.999276, 0.999286, 0.999300,
    0.999313, 0.999321, 0.999331, 0.999347, 0.999356,
    0.999369, 0.999381, 0.999394, 0.999402, 0.999415,
    0.999427, 0.999433, 0.999446, 0.999458, 0.999472,
    0.999484, 0.999499, 0.999513, 0.999522, 0.999532,
    0.999540, 0.999550, 0.999559, 0.999567, 0.999574,
    0.999588, 0.999599, 0.999613, 0.999618, 0.999627,
    // 8
    0.999635, 0.999639, 0.999652, 0.999662, 0.999667,
    0.999671, 0.999678, 0.999682, 0.999688, 0.999693,
    0.999698, 0.999701, 0.999706, 0.999711, 0.999718,
    0.999722, 0.999727, 0.999732, 0.999737, 0.999740,
    0.999746, 0.999750, 0.999754, 0.999763, 0.999766,
    0.999769, 0.999774, 0.999780, 0.999784, 0.999788,
    0.999796, 0.999803, 0.999807, 0.999809, 0.999815,
    0.999820, 0.999827, 0.999830, 0.999833, 0.999833,
    0.999836, 0.999839, 0.999842, 0.999845, 0.999850,
    0.999853, 0.999857, 0.999860, 0.999865, 0.999870,
    // 9
    0.999873, 0.999877, 0.999880, 0.999882, 0.999883,
    0.999886, 0.999888, 0.999889, 0.999895, 0.999896,
    0.999897, 0.999901, 0.999902, 0.999905, 0.999907,
    0.999907, 0.999909, 0.999911, 0.999911, 0.999912,
    0.999913, 0.999914, 0.999917, 0.999919, 0.999921,
    0.999923, 0.999927, 0.999929, 0.999931, 0.999933,
    0.999936, 0.999942, 0.999942, 0.999944, 0.999947,
    0.999947, 0.999948, 0.999949, 0.999952, 0.999955,
    0.999957, 0.999957, 0.999961, 0.999962, 0.999963,
    0.999963, 0.999963, 0.999964, 0.999965, 0.999965,
    // 10
    0.999965, 0.999965, 0.999966, 0.999968, 0.999969,
    0.999971, 0.999972, 0.999972, 0.999973, 0.999975,
    0.999975, 0.999975, 0.999975, 0.999975, 0.999975,
    0.999975, 0.999979, 0.999979, 0.999980, 0.999982,
    0.999983, 0.999985, 0.999986, 0.999987, 0.999987,
    0.999988, 0.999989, 0.999989, 0.999989, 0.999989,
    0.999990, 0.999990, 0.999992, 0.999993, 0.999994,
    0.999994, 0.999994, 0.999994, 0.999994, 0.999995,
    0.999995, 0.999995, 0.999996, 0.999996, 0.999996,
    0.999996, 0.999998, 0.999999, 1.000000, 1.000000,
    // Dummy element for noticing if the loop reached the end of the array
    0.0 
  };
   return qpe0;
  }

//data for QE for LAPPDs was obtained: http://indico.cern.ch/event/432527/contributions/1071935/attachments/1319657/1979729/Pilot_Production_of_LAPPD_-_Aug_5_2016_FINAL_V5.0_08-03-2016.pdf - Minot, ICHEP 2016
//data exist from ~363. - 628. nm [everyhting else uses PMTs QE]
G4float* LAPPD::GetQEWavelength(){
  // wavelength bin for 19 Celsius and RT
  //static G4float wavelength_value[20] = { 280., 300., 320., 340., 360., 380., 400., 420., 440., 460., 480., 500., 520., 540., 560., 580., 600., 620., 640., 660.};

  // wavelength bin for LAPPD 25 for 40
  static G4float wavelength_value[20] = {220., 240., 260., 280., 300., 320., 340., 360., 380., 400., 420., 440., 460., 480., 500., 520., 540., 560., 580., 600.};
  //static G4float wavelength_value[20] = {280., 300., 320., 340., 360., 380., 400., 420., 440., 460., 480., 500., 520., 540., 560., 580., 600.,  620., 640., 660.};

  return wavelength_value;
}

G4float* LAPPD::GetQE(){  
// new data from LAPPD 25 for 40
static G4float QE[20] = {0.02937435, 0.06062565, 0.09187696 , 0.12312826  ,  0.15437956  , 0.18471379  , 0.21641814 , 0.2428098   , 0.24469183 , 0.22234098  ,  0.207122    ,0.19884722  ,  0.17932031  ,  0.14470496  ,0.1113269  ,   0.09028386  ,  0.07672619  ,0.06275337  , 0.05160584 ,  0.03901694};
//

// new QE start from 280ns
//static G4float QE[20] = {0.12312826  ,  0.15437956  , 0.18471379  , 0.21641814 , 0.2428098   , 0.24469183 , 0.22234098  ,  0.207122    ,0.19884722  ,  0.17932031  ,  0.14470496  ,0.1113269  ,   0.09028386  ,  0.07672619  ,0.06275337  , 0.05160584 ,  0.03901694, 0.03, 0.02, 0.01};

// estimate QE @ 19 Celsius:  //the value @ 340nm cannot be 0.169 as for PMTs so I insert: 0.10!!
//static G4float QE[20] = { 0.00, .0139, .0854, .10, .12000, .10060, .09129, .09369, .09750, .09284, .07616, .05902, .05200, .04707, .04226, .03543, .03038, .02456, .00158, 0.00};

// estimate QE @ RT:  
//static G4float QE[20] = { 0.00, .0139, .0854, .169, .22629, .18517, .15956, .16345, .16189, .14715, .11301, .08431, .07112, .06724, .06103, .05405, .04551, .03776, .00158, 0.00};

//static G4float QE[20] = { 1., 1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,};


return QE;
}
G4float LAPPD::GetmaxQE(){

  const G4float maxQE = 0.25; //for LAPPDs //0.211; if for PMTs

  return maxQE;
}

// Should be actual PMT Dark Rate, not effective dark rate in detector including other LE noise
G4float LAPPD::GetDarkRate(){
  /* From e-mail discussion with A.Konaka and S.Nakayama:
   * SK-I: 4.2 kHz 
   * SK-IV:5.7 kHz, both before electronics threshold in skdetsim
   * Measured DN with 0.25 pe threshold:
   * SK-I: 3.4 kHz  (2003 SK-NIM: 3 kHz. A.Konaka: "2kHz with hot PMTs removed?") 
   * SK-IV: 4.5 kHz (higher due to FRP)
   * ToDo: investigate after updating electronics routing, whether to change value to 3.4 kHz
   */

  const G4float rate = 4.2*CLHEP::kilohertz;   //SKI value set in SKDETSim. 
  return rate;
}

// Convert dark noise frequency to one before applying threshold of 0.25 pe, as that is what
// will be simulated (WCSimWCDigitizer::AddPMTDarkRate)
G4float LAPPD::GetDarkRateConversionFactor(){
  const G4float factor = 1.367;
  return factor;
}
