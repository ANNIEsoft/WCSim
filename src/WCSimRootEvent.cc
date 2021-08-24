// Based on Root test Event.cxx
////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include <string>
#include <vector>

#include <TStopwatch.h>
#include "WCSimRootEvent.hh"

#ifndef REFLEX_DICTIONARY
ClassImp(WCSimRootCherenkovDigiHit)
ClassImp(WCSimRootCherenkovHit)
ClassImp(WCSimRootCherenkovHitTime)
ClassImp(WCSimRootTrack)
ClassImp(WCSimRootPi0)
ClassImp(WCSimRootEventHeader)
ClassImp(WCSimRootTrigger)
ClassImp(WCSimRootEvent)
#endif

//TClonesArray* WCSimRootTrigger::fgTracks = 0;
//
//TClonesArray* WCSimRootTrigger::fgCherenkovHits = 0;
//TClonesArray* WCSimRootTrigger::fgCherenkovHitTimes = 0;
//TClonesArray* WCSimRootTrigger::fgCherenkovDigiHits = 0;
//
//TClonesArray* WCSimRootTrigger::fgFVCherenkovHits = 0;
//TClonesArray* WCSimRootTrigger::fgFVCherenkovHitTimes = 0;
//TClonesArray* WCSimRootTrigger::fgFVCherenkovDigiHits = 0;
//
// 
//_____________________________________________________________________________

WCSimRootTrigger::WCSimRootTrigger()
{
  // Create an WCSimRootTrigger object.

  // WARNING : default constructor for ROOT : do not allocate memory
  // inside it or upon re-reading the object there will be a memory leak
  // TClonesArray of WCSimRootTracks
  fTracks = 0;
  fNtrack = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fCherenkovHits = 0;
  fCherenkovHitTimes = 0;
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;
  
  // TClonesArray of Captures
  fCaptures = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = 0;
  fNcherenkovdigihits = 0;
  fSumQ = 0;

  //Number of Cherenkov photons
  fCherenkov = 0;
  fCherenkovWCSim = 0;

  fTriggerType = kTriggerUndefined;
  fTriggerInfo.clear();
  
  fParentEvent = nullptr;
  
  IsZombie = true;
  
}

WCSimRootTrigger::WCSimRootTrigger(int Number,int Subevt)
{
  this->Initialize();
  fEvtHdr.Set(Number,0,0,Subevt);
}

//copy constructor --> only shallow copy of preallocated objects ??


void WCSimRootTrigger::Initialize() //actually allocate memory for things in here
{
  // Create an WCSimRootTrigger object.
  // When the constructor is invoked for the first time, the class static
  // variable fgTracks is 0 and the TClonesArray fgTracks is created.
  // Sim. for the other TClonesArray
  //TStopwatch* mystopw = new TStopwatch();

  // TClonesArray of WCSimRootTracks
  fTracks = new TClonesArray("WCSimRootTrack", 10000);
  fNtrack = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fCherenkovHits = new TClonesArray("WCSimRootCherenkovHit", 
				    10000);
  fCherenkovHitTimes = new TClonesArray("WCSimRootCherenkovHitTime", 
					10000);
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fCherenkovDigiHits = new TClonesArray("WCSimRootCherenkovDigiHit", 
				       10000);
  fNcherenkovdigihits = 0;
  fSumQ = 0;

  // Number of Cherenkov photons
  fCherenkov = 0;
  fCherenkovWCSim = 0;

  // TClonesArray of WCSimRootCaptures
  fCaptures = new TClonesArray("WCSimRootCapture", 100);
  fNcaptures = 0;

  fTriggerType = kTriggerUndefined;
  fTriggerInfo.clear();
  
  //  std::cout << " Time to allocate the TCAs :  Real = " << mystopw->RealTime() 
  //	    << " ; CPU = " << mystopw->CpuTime() << "\n";
  //delete mystopw;
  

  IsZombie = false; // the memory has been allocated
}

//_____________________________________________________________________________

WCSimRootTrigger::~WCSimRootTrigger()
{
  // now we must do a bunch a deleting stuff...
  //Destroys all the TClonesArray.. Let's see if Ren'e Brun is right...

  //TStopwatch* mystopw = new TStopwatch();

  //mystopw->Start();

  if (!IsZombie) {

    fTracks->Delete();
    fCherenkovHits->Delete();      
    fCherenkovHitTimes->Delete();   
    fCherenkovDigiHits->Delete();  
    fCaptures->Delete();
    
    delete   fTracks;            
    delete   fCherenkovHits;      
    delete   fCherenkovHitTimes;   
    delete   fCherenkovDigiHits; 
    delete   fCaptures;
  }
  if (hEventCher){
    delete hEventCher;
    hEventCher= nullptr;
  }
  //mystopw->Stop();

  //  std::cout << " Time to delete the TCAs :  Real = " << mystopw->RealTime() 
  //    << " ; CPU = " << mystopw->CpuTime() << "\n";

  //delete mystopw;
  //Clear("C");
}


//_____________________________________________________________________________

void WCSimRootTrigger::Clear(Option_t */*option*/)
{
  // To be filled in 
  // Filled in, by MF, 31/08/06  -> Keep all the alloc'ed memory but reset all
  // the indices to 0 in the TCAs.
  fNtrack = 0;

  // TClonesArray of WCSimRootCherenkovHits
  fNcherenkovhits = 0;
  fNcherenkovhittimes = 0;

  // TClonesArray of WCSimRootCherenkovDigiHits
  fNcherenkovdigihits = 0;
  fSumQ = 0;

  // Cherenkov photons
  fCherenkov = 0;
  fCherenkovWCSim = 0;

  // TClonesArray of WCSimRootCaptures
  fNcaptures = 0;

  // remove whatever's in the arrays
  // but don't deallocate the arrays themselves

  fTracks->Clear("C");
  fCherenkovHits->Delete();      
  fCherenkovHitTimes->Delete();   
  fCherenkovDigiHits->Delete();
  fCaptures->Delete();

  fTriggerType = kTriggerUndefined;
  fTriggerInfo.clear();

  IsZombie = false ; // we DO NOT deallocate the memory
}

//_____________________________________________________________________________

void WCSimRootTrigger::Reset(Option_t */*option*/)
{
// Static function to reset all static objects for this event
// To be filled in
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetHeader(Int_t i, 
				  Int_t run, 
				  Int_t date,Int_t subevent)
{
  // Set the header values
  fEvtHdr.Set(i, run, date,subevent);
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetTriggerInfo(TriggerType_t trigger_type,
				      std::vector<Float_t> trigger_info)
{
  fTriggerType = trigger_type;
  fTriggerInfo = trigger_info;
}

//_____________________________________________________________________________

void WCSimRootTrigger::SetPi0Info(Float_t pi0Vtx[3], 
				 Int_t   gammaID[2], 
				 Float_t gammaE[2],
				 Float_t gammaVtx[2][3])
{
  fPi0.Set(pi0Vtx, 
	   gammaID, 
	   gammaE,
	   gammaVtx);
}

//_____________________________________________________________________________

void WCSimRootPi0::Set(Float_t pi0Vtx[3], 
			Int_t   gammaID[2], 
			Float_t gammaE[2],
			Float_t gammaVtx[2][3])
{
  for (int i=0;i<2;i++)
  {
    fGammaID[i] = gammaID[i];
    fGammaE[i]  = gammaE[i];
  }

  for (int j=0;j<3;j++)
  {
    fPi0Vtx[j]      = pi0Vtx[j];
    fGammaVtx[0][j] = gammaVtx[0][j];
    fGammaVtx[1][j] = gammaVtx[1][j];
  }
}

//_____________________________________________________________________________

WCSimRootTrack *WCSimRootTrigger::AddTrack(Int_t ipnu, 
					   Int_t flag, 
					   Float_t m, 
					   Float_t p, 
					   Float_t E, 
					   Float_t p2,
					   Float_t E2,
					   Int_t startvol, 
					   Int_t stopvol, 
					   Float_t dir[3], 
					   Float_t pdir[3], 
					   Float_t pdir2[3], 
					   Float_t stop[3], 
					   Float_t start[3],
					   Int_t parenttype,
					   Float_t time,
					   Float_t time2,
					   Int_t id,
					   std::string sProcess,
					   std::string eProcess,
					   Float_t tankexitp[3],
					   Double_t tankexite,
					   Float_t tankexitmom[3])
{
  // Add a new WCSimRootTrack to the list of tracks for this event.
  // To avoid calling the very time consuming operator new for each track,
  // the standard but not well know C++ operator "new with placement"
  // is called. If tracks[i] is 0, a new Track object will be created
  // otherwise the previous Track[i] will be overwritten.

  TClonesArray &tracks = *fTracks;
  WCSimRootTrack *track = 
    new(tracks[fNtrack++]) WCSimRootTrack(ipnu,
					   flag,
					   m,
					   p,
					   E,
					   p2,
					   E2,
					   startvol,
					   stopvol,
					   dir,
					   pdir,
					   pdir2,
					   stop,
					   start,
					   parenttype,
					   time,
					   time2,
					   id,
					   sProcess,
					   eProcess,
					   tankexitp,
					   tankexite,
					   tankexitmom);

  return track;
}

//_____________________________________________________________________________
void WCSimRootTrigger::Print(int verbosity, int maxprimariestoprint, int maxtrackstoprint, int maxdigitstoprint, int maxphotonsperdigittoprint, int maxphotonstoprint){

  // get the first trigger of this event, required for info on cherenkov hits
  WCSimRootTrigger* firsttrig=fParentEvent->GetTrigger(0);
  
  std::cout<<"Trigger time : "<<fEvtHdr.GetDate()<<std::endl;
  std::cout<<"Trigger type : "<<WCSimEnumerations::EnumAsString(fTriggerType)<<std::endl;
  std::cout<<"Number Primary Vertices : "<<fNvtxs<<std::endl;
  // loop over primary vertices. Usually just 1. Delayed triggers may report 0, so coerce to 1. 
  for(int primaryi=0; primaryi<std::max(1,std::min(maxprimariestoprint,fNvtxs)); primaryi++){
    std::cout<<"Primary vertex "<<primaryi<<" was at ("
        <<fVtxs[primaryi][0]<<", "<<fVtxs[primaryi][1]<<", "<<fVtxs[primaryi][2]<<")"<<std::endl;
  }
  
  //int numphotons = firsttrig->GetCherenkovHits()->GetEntries();  //<< applicable to entire event, not trigger
  // quick manual count of number of photons in digits within this trigger
  int numphotons = 0;
  for(int digiti=0; digiti<fNcherenkovdigihits; digiti++){
    WCSimRootCherenkovDigiHit* digihit=(WCSimRootCherenkovDigiHit*)(fCherenkovDigiHits->At(digiti));
    numphotons += digihit->GetPhotonIds().size();
  }
  std::cout<<"Num Tracks: "<<fNtrack<<std::endl
      <<"Num Digits: "<<fNcherenkovdigihits<<std::endl
      <<"Num Photons: " <<numphotons<<std::endl;
  
  // scan through the truth tracks
  for(int tracki=0; tracki<std::min(maxtrackstoprint,fNtrack); tracki++){
    WCSimRootTrack* nextrack = (WCSimRootTrack*)(fTracks->At(tracki));
    //Int_t     GetIpnu()             pdg
    //Int_t     GetFlag()             -1: neutrino primary, -2: neutrino target, 0: other
    //Float_t   GetM()                mass
    //Float_t   GetP()                momentum magnitude
    //Float_t   GetE()                energy (inc rest mass^2)
    //Float_t   GetEndE()             energy on stopping of particle tracking
    //Float_t   GetEndP()             momentum on stopping of particle tracking
    //Int_t     GetStartvol()         starting volume: 10 is tank, 20 is facc, 30 is mrd
    //Int_t     GetStopvol()          stopping volume: but these may not be set.
    //Float_t   GetDir(Int_t i=0)     momentum unit vector
    //Float_t   GetPdir(Int_t i=0)    momentum vector
    //Float_t   GetPdirEnd(Int_t i=0) direction vector on stop tracking
    //Float_t   GetStop(Int_t i=0)    stopping vertex x,y,z for i=0-2, in cm
    //Float_t   GetStart(Int_t i=0)   starting vertex x,y,z for i=0-2, in cm
    //Int_t     GetParenttype()       parent pdg, 0 for primary.
    //Float_t   GetTime()             trj->GetGlobalTime(); starting time of particle
    //Float_t   GetStopTime()
    //Int_t     GetId()               wcsim trackid
    //Float_t   GetTankExitPos(Int_t i=0) exit pos, in cm
    //Double_t  GetTankExitE()        relativistic energy at exit pos
    //Float_t   GetTankExitMom(Int_t i=0) 3-momentum at exit pos
    //GetFlag=-1; neutrino.           This should be the neutrino. Only stop vertex is stored.
    //GetFlag=-2; primary target.     This is the target nucleus, or if none, a proton. Only stop vertex stored.
    //GetFlag= 0; any other track.    ALL PRIMARY TRACKS ALSO GET STORED HERE.
    std::cout<<"    Track "<<tracki<<"{"
        <<  " Flag: "<<nextrack->GetFlag()
        <<" | PDG: "<<nextrack->GetIpnu()
        <<" | ParentPDG: "<<nextrack->GetParenttype()
        <<" | sProc: "<<nextrack->GetStartProcess()
        <<" | eProc: "<<nextrack->GetEndProcess();
    if(verbosity){
      std::cout<<" | Start E: "<<nextrack->GetE()
          <<" | End E: "<<nextrack->GetEndE()
          <<" | Start Vtx: ("<<nextrack->GetTime()<<", "
          <<nextrack->GetStart(0)<<", "<<nextrack->GetStart(1)<<", "<<nextrack->GetStart(2)<<")"
          <<" | End Vtx: ("<<nextrack->GetStopTime()<<", "
          <<nextrack->GetStop(0)<<", "<<nextrack->GetStop(1)<<", "<<nextrack->GetStop(2)<<")";
    }
    std::cout<<" }"<<std::endl;
  }
  
  // loop over digits
  for(int digiti=0; digiti<std::min(maxdigitstoprint,fNcherenkovdigihits); digiti++){
    WCSimRootCherenkovDigiHit* digihit=(WCSimRootCherenkovDigiHit*)(fCherenkovDigiHits->At(digiti));
    std::vector<int> truephotonindices = digihit->GetPhotonIds();
    std::cout<<"      digit "<<digiti<<" at time "<<digihit->GetT()<<"ns has charge "<<digihit->GetQ()
        <<" from "<<truephotonindices.size()<<" true photons"<<std::endl;
    // loop over the photons
    for(int photoni=0; photoni<std::min(maxphotonsperdigittoprint,(int)truephotonindices.size()); photoni++){
      int thephotonsid = truephotonindices.at(photoni);
      WCSimRootCherenkovHitTime *thehittimeobject = 
        (WCSimRootCherenkovHitTime*)firsttrig->GetCherenkovHitTimes()->At(thephotonsid);
      Int_t thephotonsparenttrackid = thehittimeobject->GetParentID();
     std::cout<<"        digit "<<digiti<<", photon "<<photoni<<" has truetime "<<thehittimeobject->GetTruetime()<<std::endl;
    }
  }
  
}

//_____________________________________________________________________________

WCSimRootTrack::WCSimRootTrack(Int_t ipnu, 
				 Int_t flag, 
				 Float_t m, 
				 Float_t p, 
				 Float_t E, 
				 Float_t p2, 
				 Float_t E2, 
				 Int_t startvol, 
				 Int_t stopvol, 
				 Float_t dir[3], 
				 Float_t pdir[3], 
				 Float_t pdir2[3], 
				 Float_t stop[3], 
				 Float_t start[3],
				 Int_t parenttype,
				 Float_t time, 
				 Float_t time2,
				 Int_t id,
				 std::string sProcess,
				 std::string eProcess,
				 Float_t tankexitp[3],
				 Double_t tankexite,
				 Float_t tankexitmom[3])
{

  // Create a WCSimRootTrack object and fill it with stuff

  fIpnu = ipnu;
  fFlag = flag;
  fM = m;
  fP = p;
  fE = E;
  fP2 = p2;
  fE2 = E2;
  fStartvol = startvol;
  fStopvol = stopvol;
  int i;
  for (i=0;i<3;i++)
  {
    fDir[i] = dir[i]; 
    fPdir[i] = pdir[i]; 
    fPdir2[i] = pdir2[i]; 
    fStop[i] = stop[i]; 
    fStart[i] = start[i];
    fTankExitPos[i] = tankexitp[i];
    fTankExitMom[i] = tankexitmom[i];
  }
  fParenttype = parenttype;
  fTime = time;
  fTime2 = time2;
  fId = id;
  fStartProcess = sProcess;
  fEndProcess = eProcess;
  fTankExitE = tankexite;
}

//_____________________________________________________________________________
void WCSimRootTrack::Clear(Option_t* /*o*/){
  // must clear any datatypes that could potentially allocate memory
  fStartProcess.clear();
  fEndProcess.clear();
}

//_____________________________________________________________________________

WCSimRootCherenkovHit *WCSimRootTrigger::AddCherenkovHit(Int_t tubeID,std::vector<Float_t> truetime,std::vector<Int_t> primParID)
{
  // Add a new Cherenkov hit to the list of Cherenkov hits
  TClonesArray &cherenkovhittimes = *fCherenkovHitTimes;

  for (unsigned int i =0;i<truetime.size();i++)
  {
    fCherenkovHitCounter++;

    WCSimRootCherenkovHitTime *cherenkovhittime = 
      new(cherenkovhittimes[fNcherenkovhittimes++]) WCSimRootCherenkovHitTime(truetime[i],primParID[i]);
  }

  Int_t WC_Index[2];
  WC_Index[0] = fNcherenkovhittimes-truetime.size(); //fCherenkovHitCounter-truetime.size();
  WC_Index[1] = truetime.size();

  TClonesArray &cherenkovhits = *fCherenkovHits;
 
  WCSimRootCherenkovHit *cherenkovhit
    = new(cherenkovhits[fNcherenkovhits++]) WCSimRootCherenkovHit(tubeID,
								  WC_Index);

  return cherenkovhit;
}
//_____________________________________________________________________________

WCSimRootCherenkovHit::WCSimRootCherenkovHit(Int_t tubeID,
					     Int_t totalPe[2])
{
  // Create a WCSimRootCherenkovHitIndex object and fill it with stuff

  fTubeID     = tubeID;
  fTotalPe[0] = totalPe[0];
  fTotalPe[1] = totalPe[1];
}

WCSimRootCherenkovHitTime::WCSimRootCherenkovHitTime(Float_t truetime,
						     Int_t primParID)
{
  // Create a WCSimRootCherenkovHit object and fill it with stuff
    fTruetime        = truetime; 
    fPrimaryParentID = primParID;
}

//_____________________________________________________________________________

WCSimRootCherenkovDigiHit *WCSimRootTrigger::AddCherenkovDigiHit(Float_t q, 
								 Float_t t, 
								 Int_t tubeid,
								 std::vector<int> photon_ids)
{
  // Add a new digitized hit to the list of digitized hits
  TClonesArray &cherenkovdigihits = *fCherenkovDigiHits;
  WCSimRootCherenkovDigiHit *cherenkovdigihit = 
    new(cherenkovdigihits[fNcherenkovdigihits++]) WCSimRootCherenkovDigiHit(q, 
									    t, 
									    tubeid,
									    photon_ids);
 
  return cherenkovdigihit;
}
//_____________________________________________________________________________

WCSimRootCherenkovDigiHit::WCSimRootCherenkovDigiHit(Float_t q, 
						     Float_t t, 
						     Int_t tubeid,
						     std::vector<int> photon_ids)
{
  // Create a WCSimRootCherenkovDigiHit object and fill it with stuff

  fQ = q;
  fT = t;
  fTubeId = tubeid;
  fPhotonIds = photon_ids;
}

//_____________________________________________________________________________
void WCSimRootTrigger::SetCaptureParticle(Int_t parent,
                                          Int_t ipnu,
                                          Float_t time,
                                          Float_t vtx[3],
                                          Float_t dir[3],
                                          Float_t energy,
                                          Int_t id)
{
    WCSimRootCapture * capture = 0;
    for(int i = 0; i<fCaptures->GetEntriesFast(); i++){
        if(((WCSimRootCapture*)fCaptures->At(i))->GetCaptureParent() == parent)
            capture = (WCSimRootCapture*)fCaptures->At(i);
    }
    if(capture == 0) {
        TClonesArray &captures = *fCaptures;
        capture = new(captures[fNcaptures++]) WCSimRootCapture(parent);
    }
    if(ipnu==22) capture->AddGamma(id, energy, dir);
    else capture->SetInfo(vtx, time, ipnu);
}


//_____________________________________________________________________________
WCSimRootCapture::WCSimRootCapture(Int_t captureParent)
{
    fCaptureParent = captureParent;
    fNGamma = 0;
    fTotalGammaE = 0;
    fGammas = new TClonesArray("WCSimRootCaptureGamma", 10);
    IsZombie=false;
}

//_____________________________________________________________________________

WCSimRootCapture::~WCSimRootCapture()
{
    if(!IsZombie) {
        fGammas->Delete();
        delete fGammas;
    }
}

//_____________________________________________________________________________

void WCSimRootCapture::SetInfo(Float_t captureVtx[3],
                               Float_t captureT,
                               Int_t   captureNucleus)
{
    for (int i=0;i<3;i++) fCaptureVtx[i] = captureVtx[i];
    fCaptureT = captureT;
    fCaptureNucleus = captureNucleus;
}

//_____________________________________________________________________________

void WCSimRootCapture::AddGamma(Int_t   gammaID,
                                Float_t gammaE,
                                Float_t gammaDir[3])
{
    TClonesArray &gammas = *fGammas;
    new(gammas[fNGamma]) WCSimRootCaptureGamma(gammaID, gammaE, gammaDir);
    fTotalGammaE += gammaE;
    fNGamma++;
}

//_____________________________________________________________________________

WCSimRootCaptureGamma::WCSimRootCaptureGamma(Int_t id,
                                             Float_t energy,
                                             Float_t *dir) {
    fID = id;
    fEnergy = energy;
    for(int i=0;i<3;i++) fDir[i] = dir[i];
}

//_____________________________________________________________________________

// M Fechner, august 2006

WCSimRootEvent::WCSimRootEvent()
{
  // default constructor : nothing happens
  //fEventList.push_back(new WCSimRootTrigger() ); //at least one event 
  // this is standard root practise for streaming ROOT objtecs : if memory is alloc'ed here,
  // it will be lost
  fEventList = 0;
  Current = 0;
}

void WCSimRootEvent::Initialize()
{
  fEventList = new TObjArray(10,0); // very rarely more than 10 subevents...
  fEventList->AddAt(new WCSimRootTrigger(0,0),0);
  Current = 0;
}


WCSimRootEvent::~WCSimRootEvent()
{
  if (fEventList != 0) {
    for (int i = 0 ; i < fEventList->GetEntriesFast() ; i++) {
      delete (*fEventList)[i];
    }
    delete fEventList;
  }
  //  std::vector<WCSimRootTrigger*>::iterator  iter = fEventList.begin();
  //for ( ; iter != fEventList.end() ; ++iter) delete (*iter);
  //Clear("");
}

void WCSimRootEvent::Clear(Option_t* /*o*/)
{
  //nothing for now
}

void WCSimRootEvent::Reset(Option_t* /*o*/)
{
  //nothing for now
}

void WCSimRootEvent::Print(int verbosity, int maxtriggerstoprint, int maxprimariestoprint, int maxtrackstoprint, int maxdigitstoprint, int maxphotonsperdigittoprint, int maxphotonstoprint){
  int numtriggers = fEventList->GetEntriesFast();
  std::cout<<"This entry had "<<numtriggers<<" triggers"<<std::endl;
  for(int trigi=0; trigi<std::min(maxtriggerstoprint,numtriggers); trigi++){
    WCSimRootTrigger* nexttrigger = this->GetTrigger(trigi);
    nexttrigger->Print(verbosity, maxprimariestoprint, maxtrackstoprint, maxdigitstoprint, maxphotonsperdigittoprint, maxphotonstoprint);
  }
}

