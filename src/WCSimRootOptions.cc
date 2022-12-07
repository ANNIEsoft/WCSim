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
  
  // we'll also retrieve the git commit hash
  filepath = "CommitHash.txt"; // this file stores the hash of the current commit
  CommitHash = "";
  fin.open(filepath.c_str());
  if(fin.is_open()){
    std::getline(fin,CommitHash);
    fin.close();
    std::cout<<"Initial CommitHash.txt contents: "<<CommitHash<<std::endl;
  } else {
    std::cerr<<"No CommitHash.txt file found"<<std::endl;
  }
  // But requiring the user to keep this file up-to-date is potentially risky.
  // We can try to update this file to the current HEAD straight from the git files.
  // These automatically track the current commit, so are more likely to be up to date.
  // try to see if we have the git repository and update the file if we do
  //  (it get stripped out before submitting to the grid, so we may not have the git directory)
  std::cout<<"Attempting to update CommitHash.txt based on git head"<<std::endl;
  std::string command = std::string("WCSIMDIR='")+std::string(WCSIMDIR)+"' && "
       " nfields=$(cat ${WCSIMDIR}/.git/HEAD | awk '{ print NF; }'); "
       " if [ $nfields -eq 2 ]; then "
       "    cat ${WCSIMDIR}/.git/$(cat ${WCSIMDIR}/.git/HEAD | awk '{ print $NF; }') > CommitHashUpdate.txt;"
       " else "
       "    cat ${WCSIMDIR}/.git/HEAD > CommitHashUpdate.txt;"
       " fi";
  int fileupdated = system(command.c_str());
  // if we have a branch checked out then 'cat ${WCSIMDIR}/.git/HEAD' returns "ref: refs/heads/branchname"
  // the commit hash is then stored in ${WCSIMDIR}/.git/refs/heads/branchname 
  // if we have a detached head, then 'cat ${WCSIMDIR}/.git/HEAD' returns the commit hash directly.
  // 0 if this worked (i.e. if we could access the file in .git), not 0 otherwise.
  if(fileupdated==0){
    std::cout<<"Retrieved updated commit hash based on current HEAD"<<std::endl;
    // re-read the file with the updated hash
    fin.open("CommitHashUpdate.txt");
    if(fin.is_open()){
      std::string tempstring="";
      std::getline(fin,tempstring);
      if(tempstring!=""){
        CommitHash=tempstring;
        command = std::string("WCSIMDIR='") + std::string(WCSIMDIR) +"' && "
                  " cat CommitHashUpdate.txt > CommitHash.txt &&"
                  " cp CommitHash.txt ${WCSIMDIR}/CommitHash.txt";
        fileupdated = system(command.c_str());
        if(fileupdated==0) std::cout<<"Updated CommitHash.txt"<<std::endl;
        else std::cout<<"Will use updated commit hash, but could not update CommitHash.txt file"<<std::endl;
      } else {
        std::cout<<"Git HEAD reported empty string; skipping CommitHash.txt update"<<std::endl;
      }
      fin.close();
    }
  } else {
    //std::cerr<<"Update failed"<<std::endl;
  }
  
  // if either method succeeded, we should have a commit hash now:
  if(CommitHash!=""){
    std::cout<<"Current WCSim commit hash is: "<<CommitHash<<std::endl;
  } else {
    std::cerr<<"Unable to read WCSim commit hash file "<<filepath
          <<", please ensure the file exists and contains the current commit hash"<<std::endl;
    assert(false);
  }
  
  // we could also, for completeness, check if there are any outstanding changes:
  command = "which git >> /dev/null";
  int dont_have_git = system(command.c_str());  // returns 0 if we *do* have git
  if(dont_have_git) return; // can't do anything more without git
  //command  = "(cd ${WCSIMDIR}/ && git diff --exit-code > /dev/null )";
  //int unstaged_changes = system(command.c_str());
  //command  = "(cd ${WCSIMDIR}/ && git diff --cached --exit-code > /dev/null )";
  //int staged_changes = system(command.c_str());
  //command = "(cd ${WCSIMDIR}/ && rm -f gitstatusstring.txt && git status -uno --porcelain > gitstatusstring.txt && [ -s gitstatusstring.txt ] && rm gitstatusstring.txt)";
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
