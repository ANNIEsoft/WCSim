#ifndef WCSim_RootOptions
#define WCSim_RootOptions

//////////////////////////////////////////////////////////////////////////
//                                                                      
// WCSim_RootOptions                                                      
//                                                                      
// This class contains run option information
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include <string>

#include "WCSimEnumerations.hh"

class TDirectory;
using std::string;

//////////////////////////////////////////////////////////////////////////

class WCSimRootOptions : public TObject {

public:

  WCSimRootOptions();
  virtual ~WCSimRootOptions();
  void Print(Option_t *option = "") const;
  void PopulateFileVersion(); // helper function to populate these members

  //General sets
  void SetCommitHash(string iCommitHash){CommitHash=iCommitHash;}
  void SetWCSimVersion(double iWCSimVersion){WCSimVersion=iWCSimVersion;}
  //General gets
  double GetWCSimVersion(){return WCSimVersion;}
  string GetCommitHash(){return CommitHash;}
  //WCSimDetector* gets
  void SetDetectorName(string iDetectorName) {DetectorName = iDetectorName;}
  void SetSavePi0(bool iSavePi0) {SavePi0 = iSavePi0;}
  void SetPMTQEMethod(bool iPMTQEMethod) {PMTQEMethod = iPMTQEMethod;}
  void SetPMTCollEff(bool iPMTCollEff) {PMTCollEff = iPMTCollEff;}
  //WCSimDetector* sets
  string GetDetectorName() {return DetectorName;}
  bool   GetSavePi0() {return SavePi0;}
  int    GetPMTQEMethod() {return PMTQEMethod;}
  int    GetPMTCollEff() {return PMTCollEff;}
  //WCSimWCAddDarkNoise sets
  void SetPMTDarkRate(double iPMTDarkRate) {PMTDarkRate = iPMTDarkRate;}
  void SetConvRate(double iConvRate) {ConvRate = iConvRate;}
  void SetDarkHigh(double iDarkHigh) {DarkHigh = iDarkHigh;}
  void SetDarkLow(double iDarkLow) {DarkLow = iDarkLow;}
  void SetDarkWindow(double iDarkWindow) {DarkWindow = iDarkWindow;}
  void SetDarkMode(int iDarkMode) {DarkMode = iDarkMode;}
  //WCSimWCAddDarkNoise gets
  double GetPMTDarkRate() {return PMTDarkRate;}
  double GetConvRate() {return ConvRate;}
  double GetDarkHigh() {return DarkHigh;}
  double GetDarkLow() {return DarkLow;}
  double GetDarkWindow() {return DarkWindow;}
  int    GetDarkMode() {return DarkMode;}
  //WCSimWCDigitizer* sets
  void SetDigitizerClassName(string iDigitizerClassName) {DigitizerClassName = iDigitizerClassName;}
  void SetDigitizerDeadTime(int iDigitizerDeadTime) {DigitizerDeadTime = iDigitizerDeadTime;}
  void SetDigitizerIntegrationWindow(int iDigitizerIntegrationWindow) {DigitizerIntegrationWindow = iDigitizerIntegrationWindow;}
  void SetExtendIntegrationWindow(bool iExtendDigitizerIntegrationWindow) {ExtendDigitizerIntegrationWindow = iExtendDigitizerIntegrationWindow;}
  void SetDoPhotonIntegration(bool iDoPhotonIntegration) {DoPhotonIntegration = iDoPhotonIntegration;}
  //WCSimWCDigitizer* gets
  string GetDigitizerClassName() {return DigitizerClassName;}
  int    GetDigitizerDeadTime() {return DigitizerDeadTime;}
  int    GetDigitizerIntegrationWindow() {return DigitizerIntegrationWindow;}
  bool   GetExtendDigitizerIntegrationWindow() {return ExtendDigitizerIntegrationWindow;}
  bool   GetDoPhotonIntegration() {return DoPhotonIntegration;}
  //WCSimWCTrigger* sets
  void SetTriggerClassName(string itriggerClassName) {TriggerClassName = itriggerClassName;};
  void SetMultiDigitsPerTrigger(bool imultiDigitsPerTrigger) {MultiDigitsPerTrigger = imultiDigitsPerTrigger;};
  //ndigits
  void SetNDigitsThreshold(int indigitsThreshold) {NDigitsThreshold = indigitsThreshold;};
  void SetNDigitsWindow(int indigitsWindow) {NDigitsWindow = indigitsWindow;};
  void SetNDigitsAdjustForNoise(bool indigitsAdjustForNoise) {NDigitsAdjustForNoise = indigitsAdjustForNoise;};
  void SetNDigitsPreTriggerWindow(int indigitsPreTriggerWindow) {NDigitsPreTriggerWindow = indigitsPreTriggerWindow;};
  void SetNDigitsPostTriggerWindow(int indigitsPostTriggerWindow) {NDigitsPostTriggerWindow = indigitsPostTriggerWindow;};
  //prompt
  void SetPromptTriggerEnabled(bool enablePromptTriggerin){ enablePromptTrigger = enablePromptTriggerin;}
  void SetPromptPreTriggerWindow(int promptPreTriggerWindowin){ promptPreTriggerWindow = promptPreTriggerWindowin;}
  void SetPromptPostTriggerWindow(int promptPostTriggerWindowin){ promptPostTriggerWindow = promptPostTriggerWindowin;}
  //savefailures
  void SetSaveFailuresMode(int isaveFailuresMode) {SaveFailuresMode = isaveFailuresMode;};
  void SetSaveFailuresTime(double isaveFailuresTime) {SaveFailuresTime = isaveFailuresTime;};
  void SetSaveFailuresPreTriggerWindow(int isaveFailuresPreTriggerWindow) {SaveFailuresPreTriggerWindow = isaveFailuresPreTriggerWindow;};
  void SetSaveFailuresPostTriggerWindow(int isaveFailuresPostTriggerWindow) {SaveFailuresPostTriggerWindow = isaveFailuresPostTriggerWindow;};
  //WCSimWCTrigger* gets
  string GetTriggerClassName() {return TriggerClassName;}
  bool   GetMultiDigitsPerTrigger() {return MultiDigitsPerTrigger;}
  //ndigits
  int    GetNDigitsThreshold() {return NDigitsThreshold;}
  int    GetNDigitsWindow() {return NDigitsWindow;}
  bool   GetNDigitsAdjustForNoise() {return NDigitsAdjustForNoise;}
  int    GetNDigitsPreTriggerWindow() {return NDigitsPreTriggerWindow;}
  int    GetNDigitsPostTriggerWindow() {return NDigitsPostTriggerWindow;}
  //prompt
  bool   GetPromptTriggerEnabled(){ return enablePromptTrigger;}
  int    GetPromptPreTriggerWindow(){ return promptPreTriggerWindow;}
  int    GetPromptPostTriggerWindow(){ return promptPostTriggerWindow;}
  //savefailures
  int    GetSaveFailuresMode() {return SaveFailuresMode;}
  double GetSaveFailuresTime() {return SaveFailuresTime;}
  int    GetSaveFailuresPreTriggerWindow() {return SaveFailuresPreTriggerWindow;}
  int    GetSaveFailuresPostTriggerWindow() {return SaveFailuresPostTriggerWindow;}
  //WCSimTuningParameters sets
  void SetRayff(double iRayff) {Rayff = iRayff;}
  void SetBsrff(double iBsrff) {Bsrff = iBsrff;}
  void SetAbwff(double iAbwff) {Abwff = iAbwff;}
  void SetRgcff(double iRgcff) {Rgcff = iRgcff;}
  void SetRgcffR7081(double iRgcffR7081) {RgcffR7081 = iRgcffR7081;}
  void SetMieff(double iMieff) {Mieff = iMieff;}
  void SetTeflonrff(double iTeflonrff) {Teflonrff = iTeflonrff;}
  void SetHolderrff(double iHolderrff) {Holderrff = iHolderrff;}
  void SetHolderrffLUX(double iHolderrffLUX) {HolderrffLUX = iHolderrffLUX;}
  void SetLinerrff(double iLinerrff) {Linerrff = iLinerrff;}
  void SetHolder(bool iHolder) {Holder = iHolder;}
  void SetQERatio(double iQERatio) {QERatio = iQERatio;}
  void SetQERatioWB(double iQERatioWB) {QERatioWB = iQERatioWB;}
  void SetPMTWiseQE(bool iPMTWiseQE) {PMTWiseQE = iPMTWiseQE;}
  void SetTvspacing(double iTvspacing) {Tvspacing = iTvspacing;}
  void SetTopveto(bool iTopveto) {Topveto = iTopveto;}
  //WCSimTuningParameters gets
  double GetRayff() {return Rayff;}
  double GetBsrff() {return Bsrff;}
  double GetAbwff() {return Abwff;}
  double GetRgcff() {return Rgcff;}
  double GetRgcffR7081() {return RgcffR7081;}
  double GetMieff() {return Mieff;}
  double GetTeflonrff() {return Teflonrff;}
  double GetLinerrff() {return Linerrff;}
  double GetHolderrff() {return Holderrff;}
  double GetHolderrffLUX() {return HolderrffLUX;}
  bool GetHolder() {return Holder;}
  double GetQERatio() {return QERatio;}
  double GetQERatioWB() {return QERatioWB;}
  double GetPMTWiseQE() {return PMTWiseQE;}
  double GetTvspacing() {return Tvspacing;}
  bool   GetTopveto() {return Topveto;}
  //WCSimPhysicsListFactory sets
  void SetPhysicsListName(string iPhysicsListName) {PhysicsListName = iPhysicsListName;}
  //WCSimPhysicsListFactory gets
  string GetPhysicsListName() {return PhysicsListName;}
  //WCSimPrimaryGeneratorAction sets
  void SetVectorFileName(string iVectorFileName) {VectorFileName = iVectorFileName;}
  void SetGeneratorType(string iGeneratorType) {GeneratorType = iGeneratorType;}
  //WCSimPrimaryGeneratorAction gets
  string GetVectorFileName() {return VectorFileName;}
  string GetGeneratorType() {return GeneratorType;}
  //WCSimRandomParameters sets
  void SetRandomSeed(int iRandomSeed) {RandomSeed = iRandomSeed;}
  void SetRandomGenerator(WCSimRandomGenerator_t iRandomGenerator) {RandomGenerator = iRandomGenerator;}
  //WCSimRandomParameters gets
  int                    GetRandomSeed() {return RandomSeed;}
  WCSimRandomGenerator_t GetRandomGenerator() {return RandomGenerator;}

private:
  //WCSim General
  string CommitHash;  // git tag of this commit
  double WCSimVersion;
  //WCSimDetector*
  string DetectorName;
  bool   SavePi0;
  int    PMTQEMethod;
  int    PMTCollEff;
  
  //WCSimWCAddDarkNoise
  double PMTDarkRate; // kHz
  double ConvRate; // kHz
  double DarkHigh; // ns
  double DarkLow; // ns
  double DarkWindow; // ns
  int    DarkMode;

  //WCSimWCDigitizer*
  string DigitizerClassName;
  int    DigitizerDeadTime; // ns
  int    DigitizerIntegrationWindow; // ns
  bool   ExtendDigitizerIntegrationWindow;
  bool   DoPhotonIntegration;

  //WCSimWCTrigger*
  string TriggerClassName;
  bool   MultiDigitsPerTrigger;
  //ndigits
  int    NDigitsThreshold; // digitized hits
  int    NDigitsWindow; // ns
  bool   NDigitsAdjustForNoise;
  int    NDigitsPreTriggerWindow; // ns
  int    NDigitsPostTriggerWindow; // ns
  //prompt
  bool   enablePromptTrigger;
  int    promptPreTriggerWindow;  // ns
  int    promptPostTriggerWindow; // ns
  //savefailures
  int    SaveFailuresMode;
  double SaveFailuresTime; // ns
  int    SaveFailuresPreTriggerWindow; // ns
  int    SaveFailuresPostTriggerWindow; // ns

  //WCSimTuningParameters
  double Rayff;
  double Bsrff;
  double Abwff;
  double Rgcff;
  double RgcffR7081;
  double Mieff;
  double Teflonrff;
  double Linerrff;
  double Holderrff;
  double HolderrffLUX;
  bool Holder;
  double QERatio;
  double QERatioWB;
  bool PMTWiseQE;
  double Tvspacing;
  bool   Topveto;

  //WCSimPhysicsListFactory
  string PhysicsListName;

  //WCSimPrimaryGeneratorAction
  string VectorFileName;
  string GeneratorType;

  //WCSimRandomParameters
  int                    RandomSeed;
  WCSimRandomGenerator_t RandomGenerator;
  
  ClassDef(WCSimRootOptions,1)  //WCSimRootEvent structure
};


#endif
