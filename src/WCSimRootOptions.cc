////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <fstream>

#include "WCSimRootOptions.hh"

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimRootOptions)
#endif

using std::endl;
using std::cout;

//______________________________________________________________________________
WCSimRootOptions::WCSimRootOptions()
{
  // Create a WCSimRootOptions object.
}

//______________________________________________________________________________
void WCSimRootOptions::PopulateFileVersion()
{
  // Populate the WCSimVersion and CommitHash members
  
  // Retrieve the WCSimVersion number from file:
  std::string filepath = "WCSimVersion.txt"; // this file stores a double we can compare within code
  WCSimVersion = -1;
  std::ifstream fin(filepath.c_str());
  if(fin.is_open()){
    std::string tempstring;
    std::getline(fin,tempstring);
    WCSimVersion = atof(tempstring.c_str());
    fin.close();
  }
  if(WCSimVersion<0){
    std::cerr<<"Unable to read WCSim version file "<<filepath
          <<", please ensure the file exists and contains the current version number"<<std::endl;
    assert(false);
  }
  
  // we'll also try to retrieve the git commit hash.
  CommitHash = "";

  // This of course relies on us having git and a git repository
  std::string command = "which git >> /dev/null";
  int dont_have_git = system(command.c_str());  // returns 0 if we *do* have git
  if(dont_have_git) return;

  std::cout<<"Attempting to fetch git head commit hash"<<std::endl;
  // if we have a branch checked out then 'cat ${WCSIMDIR}/.git/HEAD' returns "ref: refs/heads/branchname"
  // the commit hash is then stored in ${WCSIMDIR}/.git/refs/heads/branchname
  // if we have a detached head, then 'cat ${WCSIMDIR}/.git/HEAD' returns the commit hash directly.
  command = std::string("WCSIMDIR='")+std::string(WCSIMDIR)+"' && "
       " nfields=$(cat ${WCSIMDIR}/.git/HEAD | awk '{ print NF; }'); "
       " if [ $nfields -eq 2 ]; then "
       "    cat ${WCSIMDIR}/.git/$(cat ${WCSIMDIR}/.git/HEAD | awk '{ print $NF; }') > CommitHash.txt;"
       " else "
       "    cat ${WCSIMDIR}/.git/HEAD > CommitHash.txt;"
       " fi";
  int gothash = system(command.c_str());
  // system call returns 0 if this worked and an error code otherwise.
  if(gothash==0){
    std::cout<<"Retrieved HEAD commit hash"<<std::endl;
    // read it in
    fin.open("CommitHash.txt");
    if(fin.is_open()){
      std::string tempstring="";
      std::getline(fin,tempstring);
      if(tempstring!=""){
        CommitHash=tempstring;
      } else {
        std::cerr<<"Git HEAD reported empty string!"<<std::endl;
      }
      fin.close();
    } else {
      std::cerr<<"Failed to open tempfile 'CommitHash.txt'"<<std::endl;
    }
  } else {
    std::cerr<<"failed to get git commit hash!"<<std::endl;
  }
  
  if(CommitHash!=""){
    std::cout<<"Current WCSim commit hash is: "<<CommitHash<<std::endl;
  } else {
    std::cerr<<"Unable to read WCSim commit hash\n"
             <<"Please ensure you built WCSim from a git repository"<<std::endl;
    //       <<"If working on the grid fetch the minimum code with e.g.\n"
    //       <<"`git clone --depth 1 --single-branch -b annie https://github.com/ANNIEsoft/WCSim.git`"<<std::endl;
    // Hmm, this could cause grid jobs to fail if someone got their code as a zip file,
    // but if you're running on the grid it's probably all the more important your outputs
    // are properly git tagged. So we will fail.
    assert(false);
  }
  
  // we can also warn if there are any outstanding changes, as in this case
  // the corresponding commit may not be an accurate representation of the code
  command = std::string("(WCSIMDIR='") + std::string(WCSIMDIR) + "' && "
            " cd ${WCSIMDIR}/ && rm -f gitstatusstring.txt && "
            " git diff HEAD > gitstatusstring.txt && "
            " if [ -s gitstatusstring.txt ]; then /bin/false; fi )";
  int any_changes = system(command.c_str());
  if(any_changes){
    std::cerr<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "<<std::endl;
    std::cerr<<"WARNING: THERE ARE UNCOMMITTED CHANGES TO THE SOURCE FILES"<<std::endl;
    std::cerr<<"    WCSimRootOptions::CommitHash WILL NOT BE ACCURATE!"<<std::endl;
    std::cerr<<"       PLEASE COMMIT YOUR CHANGES AND REBUILD"<<std::endl;
    std::cerr<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "<<std::endl;
  }
}

//______________________________________________________________________________
WCSimRootOptions::~WCSimRootOptions()
{
}

//______________________________________________________________________________
void WCSimRootOptions::Print(Option_t *) const
{
  cout
    << "WCSim version (git commit):"<<CommitHash << endl
    << "Detector construction:" << endl
    << "\tDetectorName: " << DetectorName << endl
    << "\tSavePi0: " << SavePi0 << endl
    << "\tPMTQEMethod: " << PMTQEMethod << endl
    << "\tPMTCollEff: " << PMTCollEff << endl
    << "Dark Noise options:" << endl
    << "\tPMTDarkRate: " << PMTDarkRate << " kHz" << endl
    << "\tConvRate: " << ConvRate << " kHz" << endl
    << "\tDarkHigh: " << DarkHigh << " ns" << endl
    << "\tDarkLow: " << DarkLow << " ns" << endl
    << "\tDarkWindow: " << DarkWindow << " ns" << endl
    << "\tDarkMode: " << DarkMode << endl
    << "Digitizer options:" << endl
    << "\tDigitizerClassName: " << DigitizerClassName << endl
    << "\tDigitizerDeadTime: " << DigitizerDeadTime << " ns" << endl
    << "\tDigitizerIntegrationWindow: " << DigitizerIntegrationWindow << " ns" << endl
    << "\tExtendDigitizerIntegrationWindow: " << ExtendDigitizerIntegrationWindow << endl
    << "Trigger options:" << endl
    << "\tTriggerClassName: " << TriggerClassName << endl
    << "\tMultiDigitsPerTrigger: " << MultiDigitsPerTrigger << endl
    << "NDigits-style trigger options:" << endl
    << "\tNDigitsThreshold: " << NDigitsThreshold << " digitized hits" << endl
    << "\tNDigitsWindow: " << NDigitsWindow << " ns" << endl
    << "\tNDigitsAdjustForNoise: " << NDigitsAdjustForNoise << endl
    << "\tNDigitsPreTriggerWindow: " << NDigitsPreTriggerWindow << " ns" << endl
    << "\tNDigitsPostTriggerWindow: " << NDigitsPostTriggerWindow << " ns" << endl
    << "\tPromptTriggerEnabled: " << enablePromptTrigger << endl
    << "\tPromptTriggerWindow: " << promptPostTriggerWindow << " ns" << endl
    << "Save failures trigger options:" << endl
    << "\tSaveFailuresMode: " << SaveFailuresMode << endl
    << "\tSaveFailuresTime: " << SaveFailuresTime << " ns" << endl
    << "\tSaveFailuresPreTriggerWindow: " << SaveFailuresPreTriggerWindow << " ns" << endl
    << "\tSaveFailuresPostTriggerWindow: " << SaveFailuresPostTriggerWindow << " ns" << endl
    << "Tuning parameters:" << endl
    << "\tRayff: " << Rayff << endl
    << "\tBsrff: " << Bsrff << endl
    << "\tAbwff: " << Abwff << endl
    << "\tRgcff: " << Rgcff << endl
    << "\tRgcffR7081: " << RgcffR7081 << endl
    << "\tMieff: " << Mieff << endl
    << "\tTeflonrff: " << Teflonrff << endl
    << "\tLinerrff: " << Linerrff << endl
    << "\tHolderrff: " << Holderrff << endl
    << "\tHolderrffLUX: " << HolderrffLUX << endl
    << "\tHolder: " << Holder << endl
    << "\tQERatio: "<< QERatio << endl
    << "\tQERatioWB: "<< QERatioWB << endl
    << "\tPMTWiseQE: " << PMTWiseQE << endl
    << "\tTvspacing: " << Tvspacing << endl
    << "\tTopveto: " << Topveto << endl
    << "Physics List Factory:" << endl
    << "\tPhysicsListName: " << PhysicsListName << endl
    << "WCSimPrimaryGeneratorAction" << endl
    << "\tVectorFileName: " << VectorFileName << endl
    << "\tGeneratorType: " << GeneratorType << endl
    << "WCSimPrimaryGeneratorAction" << endl
    << "\tRandomSeed: " << RandomSeed << endl
    << "\tRandomGenerator: " << WCSimEnumerations::EnumAsString(RandomGenerator) << endl
    << endl;
}
