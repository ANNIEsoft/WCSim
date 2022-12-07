#ifndef WCSim_RootEvent
#define WCSim_RootEvent

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//    WCSim_RootEvent                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include <string>
#include <vector>
#include <iostream>
#include "jhfNtuple.h"
#include "TH1D.h"
#include "TH2D.h"
//#include <map>
//#include "G4Transform3D.hh"

// #include "WCSimDetectorConstruction.hh"
#include "WCSimEnumerations.hh"

class TDirectory;
class WCSimRootEvent;

class WCSimRootTrack : public TObject {

private:

  // See jhfNtuple.h for the meaning of these data members:
  Int_t   fIpnu;        
  Int_t   fFlag;        
  Float_t fM;
  Float_t fP;       // start mom magnitude
  Float_t fE;       // start E (relativistic inc. mass^2)
  Float_t fP2;      // end mom magnitude
  Float_t fE2;      // end E (relativistic inc. mass^2)
  Int_t   fStartvol;
  Int_t   fStopvol;
  Float_t fDir[3];
  Float_t fPdir[3];  // start mom
  Float_t fPdir2[3]; // end mom
  Float_t fStop[3];
  Float_t fStart[3];
  Int_t fParenttype;
  Float_t fTime;   // start time
  Float_t fTime2;  // end time 
  Int_t fId;
  std::string fStartProcess;
  std::string fEndProcess;
  Float_t fTankExitPos[3];
  Double_t fTankExitE;
  Float_t fTankExitMom[3];

public:
  WCSimRootTrack() {}
  WCSimRootTrack(Int_t ipnu, 
		  Int_t flag, 
		  Float_t m, 
		  Float_t p, 
		  Float_t E, 
		  Float_t endP, 
		  Float_t endE, 
		  Int_t startvol, 
		  Int_t stopvol, 
		  Float_t dir[3], 
		  Float_t pdir[3], 
		  Float_t pdir2[3],
		  Float_t stop[3], 
		  Float_t start[3], 
		  Int_t parenttype,
		  Float_t time,
		  Float_t endtime,
		  Int_t id,
		  std::string sProcess,
		  std::string eProcess,
		  Float_t tankexitp[3],
		  Double_t tankexite,
		  Float_t tankexitmom[3]);
  
  virtual ~WCSimRootTrack() { }

  Int_t     GetIpnu() const { return fIpnu;}
  Int_t     GetFlag() const { return fFlag;}
  Float_t   GetM() const { return fM;}
  Float_t   GetP() const { return fP;}
  Float_t   GetE() const { return fE;}
  Float_t   GetEndE() const { return fE2;}
  Float_t   GetEndP() const { return fP2;}
  Int_t     GetStartvol() { return fStartvol;}
  Int_t     GetStopvol() { return fStopvol;}
  Float_t   GetDir(Int_t i=0) {return (i<3) ? fDir[i] : 0;} 
  Float_t   GetPdir(Int_t i=0) {return (i<3) ? fPdir[i] : 0;}
  Float_t   GetPdirEnd(Int_t i=0) {return (i<3) ? fPdir2[i] : 0;}
  Float_t   GetStop(Int_t i=0) {return (i<3) ? fStop[i] : 0;}
  Float_t   GetStart(Int_t i=0) {return (i<3) ? fStart[i] : 0;}
  Int_t     GetParenttype(/*Int_t i=0*/) {return fParenttype;}
  Float_t   GetTime() { return fTime;}
  Float_t   GetStopTime() { return fTime2;}
  Int_t     GetId(){return fId;}
  std::string GetStartProcess(){return fStartProcess;}
  std::string GetEndProcess(){return fEndProcess;}
  Float_t   GetTankExitPoint(Int_t i=0){return (i<3) ? fTankExitPos[i] : 0;}
  Double_t  GetTankExitE(){return fTankExitE;}
  Float_t   GetTankExitMom(Int_t i=0){return (i<3) ? fTankExitMom[i] : 0;}
  
  void Clear(Option_t *option ="");

  ClassDef(WCSimRootTrack,1)  
};


//////////////////////////////////////////////////////////////////////////


class WCSimRootCherenkovHit : public TObject {

private:
  Int_t fTubeID;
  Int_t fTotalPe[2];

public:
  WCSimRootCherenkovHit() {}
  WCSimRootCherenkovHit(Int_t tubeID,
			Int_t totalPe[2]);

  virtual ~WCSimRootCherenkovHit() { }

  Int_t GetTubeID()       const { return fTubeID;}
  Int_t GetTotalPe(int i) const { return (i<2) ? fTotalPe[i]: 0;}

  ClassDef(WCSimRootCherenkovHit,1)  
};

class WCSimRootCherenkovHitTime : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Float_t fTruetime;
  Int_t   fPrimaryParentID;

public:
  WCSimRootCherenkovHitTime() {}
  WCSimRootCherenkovHitTime(Float_t truetime,
			    Int_t   primaryParentID);
  virtual ~WCSimRootCherenkovHitTime() { }

  Float_t   GetTruetime() { return fTruetime;}
  Int_t     GetParentID() { return fPrimaryParentID;}

  ClassDef(WCSimRootCherenkovHitTime,1)  
};


//////////////////////////////////////////////////////////////////////////


class WCSimRootCherenkovDigiHit : public TObject {

private:
  // See jhfNtuple.h for the meaning of these data members:
  Float_t fQ;
  Float_t fT;
  Int_t fTubeId;
  std::vector<int> fPhotonIds;

public:
  WCSimRootCherenkovDigiHit() {}
  WCSimRootCherenkovDigiHit(Float_t q, Float_t t, Int_t tubeid, std::vector<int> photon_ids);

  virtual ~WCSimRootCherenkovDigiHit() { }

  Float_t     GetQ() const { return fQ;}
  Float_t     GetT() const { return fT;}
  Int_t       GetTubeId() const { return fTubeId;}
  std::vector<int> GetPhotonIds() const { return fPhotonIds; }

  ClassDef(WCSimRootCherenkovDigiHit,2)  
};


//////////////////////////////////////////////////////////////////////////

class WCSimRootEventHeader {

private:
  Int_t   fEvtNum;
  Int_t   fRun;
  Int_t   fDate;
  Int_t   fSubEvtNumber;
  
  TString fDirtFileName;
  TString fGenieFileName;
  Int_t   fDirtEntryNum;
  Int_t   fGenieEntryNum;

public:
  WCSimRootEventHeader() : fEvtNum(0), fRun(0), fDate(0), fSubEvtNumber(1), fDirtFileName(""), fGenieFileName(""), fDirtEntryNum(-1), fGenieEntryNum(-1){ }
   virtual ~WCSimRootEventHeader() { }
  void   Set(Int_t i, Int_t r, Int_t d, Int_t s=1) { fEvtNum = i; fRun = r; fDate = d; fSubEvtNumber = s;}
  void   SetDate(Int_t d) { fDate=d; }
  void   SetDirtFileName(TString namein){ fDirtFileName = namein; }
  void   SetGenieFileName(TString namein){ fGenieFileName = namein; }
  void   SetDirtEntryNum(Int_t numin){ fDirtEntryNum = numin; }
  void   SetGenieEntryNum(Int_t numin){ fGenieEntryNum = numin; }
  
   Int_t  GetEvtNum() const { return fEvtNum; }
   Int_t  GetRun() const { return fRun; }
   Int_t  GetDate() const { return fDate; }
   Int_t GetSubEvtNumber() const { return fSubEvtNumber;}
   TString GetDirtFileName() const { return fDirtFileName; }
   TString GetGenieFileName() const { return fGenieFileName; }
   Int_t GetDirtEntryNum() const { return fDirtEntryNum; }
   Int_t GetGenieEntryNum() const { return fGenieEntryNum; }
  

   ClassDef(WCSimRootEventHeader,2)  //WCSimRootEvent Header
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootPi0 : public TObject {
  // this is a class used specifically for Pi0 events

private:
  Float_t fPi0Vtx[3];
  Int_t   fGammaID[2];
  Float_t fGammaE[2];
  Float_t fGammaVtx[2][3];

public:
  WCSimRootPi0() {}

  virtual ~WCSimRootPi0() {}

  void Set(Float_t pi0Vtx[3],
	   Int_t   gammaID[2],
	   Float_t gammaE[2],
	   Float_t gammaVtx[2][3]);

  Float_t  GetPi0Vtx(int i)           const { return (i<3) ? fPi0Vtx[i]: 0;}
  Int_t    GetGammaID(int i)          const { return (i<2) ? fGammaID[i]: 0;}
  Float_t  GetGammaE(int i)           const { return (i<2) ? fGammaE[i]: 0;}
  Float_t  GetGammaVtx(int i, int j)  const { return fGammaVtx[i][j];}

  ClassDef(WCSimRootPi0,1)
};

//////////////////////////////////////////////////////////////////////////

class WCSimRootCaptureGamma : public TObject {

private:
  Int_t   fID;
  Float_t fEnergy;
  Float_t fDir[3];

public:
  WCSimRootCaptureGamma() {}
  WCSimRootCaptureGamma(Int_t id,
                        Float_t energy,
                        Float_t dir[3]
  );

  virtual ~WCSimRootCaptureGamma() {}

  Int_t    GetID()           const { return fID;}
  Float_t  GetE()            const { return fEnergy;}
  Float_t  GetDir(int i)     const { return (i<3) ? fDir[i]: 0;}

  ClassDef(WCSimRootCaptureGamma,1)
};

//////////////////////////////////////////////////////////////////////////


class WCSimRootCapture : public TObject {
// this is a class used specifically for neutron capture events

private:
   Int_t          fCaptureParent;
   Float_t        fCaptureVtx[3];
   Int_t          fNGamma;
   Float_t        fTotalGammaE;
   Float_t        fCaptureT;
   Int_t          fCaptureNucleus;
   TClonesArray * fGammas;
   bool           IsZombie;

public:
   WCSimRootCapture() {
             fGammas = 0;
             IsZombie = true;
           }
   WCSimRootCapture(Int_t captureParent);

   virtual ~WCSimRootCapture();

   void SetInfo(Float_t captureVtx[3],
                Float_t captureT,
                Int_t   captureNucleus
   );

   void AddGamma(Int_t   gammaID,
                 Float_t gammaE,
                 Float_t gammaDir[3]
   );

   Int_t                   GetCaptureParent()   const { return fCaptureParent;}
   Float_t                 GetCaptureVtx(int i) const { return (i<3) ? fCaptureVtx[i]: 0;}
   Int_t                   GetNGamma()          const { return fNGamma;}
   Float_t                 GetTotalGammaE()     const { return fTotalGammaE;}
   Float_t                 GetCaptureT()        const { return fCaptureT;}
   Int_t                   GetCaptureNucleus()  const { return fCaptureNucleus;}
   TClonesArray           *GetGammas()          const { return fGammas;}

   ClassDef(WCSimRootCapture,1)
};

////////////////////////////////////////////////////////////////////////////

class WCSimRootTrigger : public TObject {

private:
  WCSimRootEventHeader    fEvtHdr;  // The header
  WCSimRootEvent*         fParentEvent; //! a link back to the parent, set when trigger is retrieved
  // See jhfNtuple.h for the meaning of these data members:
  Int_t                fMode;
  Int_t                fNvtxs;
  Int_t                fVtxsvol[MAX_N_PRIMARIES];
  Float_t              fVtxs[MAX_N_PRIMARIES][3];
  Int_t                fVecRecNumber;       // "info event" number in inputvectorfile 
  Int_t                fJmu;
  Int_t                fJp;

  WCSimRootPi0        fPi0;                // Pi0 info (default = not used)

  TClonesArray        *fCaptures;            // Neutron capture info (default = not used)
  Int_t                fNcaptures;           // Number of tracks in the array

  Int_t                fNpar;               // Number of particles
  Int_t                fNtrack;             // Number of tracks in the array
  TClonesArray         *fTracks;            //-> Array of WCSimRootTracks 

  Int_t                fNumTubesHit;        // Number of tubes hit
  Int_t                fNcherenkovhits;      // Number of hits in the array
  TClonesArray         *fCherenkovHits;      //-> Array of WCSimRootCherenkovHits

  Int_t			fCherenkov;		//Number of Cherenkov photons
  Int_t			fCherenkovWCSim;		//Number of Cherenkov photons in typical WCSim range (200nm-790nm)

  Int_t                fCherenkovHitCounter;
  Int_t                fNcherenkovhittimes;      // Number of hits in the array
  TClonesArray         *fCherenkovHitTimes;      //-> Array of WCSimRootCherenkovHits

  Int_t                fNumDigitizedTubes;  // Number of digitized tubes
  Int_t                fNcherenkovdigihits;  // Number of digihits in the array
  Float_t              fSumQ;
  TClonesArray         *fCherenkovDigiHits;  //-> Array of WCSimRootCherenkovDigiHit's

  TriggerType_t        fTriggerType;         // Trigger algorithm that created this trigger
  std::vector<Float_t> fTriggerInfo;         // Information about how it passed the trigger (e.g. how many hits in the NDigits window)

  TH1D *hEventCher = nullptr;
  TH1D *hEventCherWCSim = nullptr;
  TH2D *hEventCherXZ = nullptr;
  TH2D *hEventCherYZ = nullptr;

  bool IsZombie;

public:
  WCSimRootTrigger();
  WCSimRootTrigger(int, int);
  virtual ~WCSimRootTrigger();
  
  void Initialize();

  void          Clear(Option_t *option ="");
  static void   Reset(Option_t *option ="");
  void Print(int verbosity=0, int maxprimariestoprint=10, int maxtrackstoprint=10, int maxdigitstoprint=10, int maxphotonsperdigittoprint=5, int maxphotonstoprint=5);

  void          SetHeader(Int_t i, Int_t run, Int_t date,Int_t subevtn=1);
  void          SetTriggerInfo(TriggerType_t trigger_type, std::vector<Float_t> trigger_info);
  bool          IsASubEvent() {  return (fEvtHdr.GetSubEvtNumber()>=1); }
  void          SetMode(Int_t i) {fMode = i;}
  void          SetNvtxs(Int_t i) {fNvtxs = i;}
  void          SetVtxvol(Int_t i) {fVtxsvol[0] = i;}
  void          SetVtxsvol(Int_t i, Int_t v) {fVtxsvol[i] = v;}
  void          SetVtx(Int_t i, Float_t f) {fNvtxs=1; fVtxs[0][i] = ( (i<3) ? f : 0);}
  void          SetVtxs(Int_t n, Int_t i, Float_t f) {fVtxs[n][i]= ( (i<3) ? f : 0);}
  void          SetVecRecNumber(Int_t i) {fVecRecNumber = i;}
  void          SetJmu(Int_t i) {fJmu = i;}
  void          SetJp(Int_t i) {fJp = i;}
  void          SetNpar(Int_t i) {fNpar = i;}
  void          SetNumTubesHit(Int_t i) {fNumTubesHit = i;}
  void          SetSumQ(Float_t x){fSumQ = x;}
  void          SetNumDigitizedTubes(Int_t i) {fNumDigitizedTubes = i;}
  void          SetPi0Info(Float_t pi0Vtx[3], 
                           Int_t   gammaID[2], 
                           Float_t gammaE[2],
                           Float_t gammaVtx[2][3]);
  void          SetCaptureParticle(Int_t parent,
                                   Int_t ipnu,
                                   Float_t time,
                                   Float_t vtx[3],
                                   Float_t dir[3],
                                   Float_t energy,
                                   Int_t id);
  void          SetParentEvent(WCSimRootEvent* parenteventin){ fParentEvent = parenteventin; }
  void		SetNCherenkov(Int_t n_Cher) { fCherenkov = n_Cher; }
  void		SetNCherenkovWCSim(Int_t n_CherWCSim) { fCherenkovWCSim = n_CherWCSim; }
  void          SetHCher(TH1D* h){
    if (hEventCher){
      delete hEventCher;
      hEventCher = nullptr;
    }
    hEventCher = (TH1D*) h->Clone();
  }
  TH1D* GetHCher() { return hEventCher;}
  void          SetHCherWCSim(TH1D* h){
    if (hEventCherWCSim){
      delete hEventCherWCSim;
      hEventCherWCSim = nullptr;
    }
    hEventCherWCSim = (TH1D*) h->Clone();
  }
  TH1D* GetHCherWCSim() { return hEventCherWCSim;}
  void          SetHCherXZ(TH2D* h){
    if (hEventCherXZ){
      delete hEventCherXZ;
      hEventCherXZ = nullptr;
    }
    hEventCherXZ = (TH2D*) h->Clone();
  }
  TH2D* GetHCherXZ() { return hEventCherXZ;}
  void          SetHCherYZ(TH2D* h){
    if (hEventCherYZ){
      delete hEventCherYZ;
      hEventCherYZ = nullptr;
    }
    hEventCherYZ = (TH2D*) h->Clone();
  }
  TH2D* GetHCherYZ() { return hEventCherYZ;}

  WCSimRootEventHeader *GetHeader()               {return &fEvtHdr; }
  WCSimRootPi0       *GetPi0Info()                 {return &fPi0; }
  Int_t               GetMode()               const {return fMode;}
  Int_t               GetVtxvol()             const {return fVtxsvol[0];}
  Float_t             GetVtx(Int_t i=0)             {return (i<3) ? fVtxs[0][i]: 0;}
  Int_t               GetNvtxs()             const {return fNvtxs;}
  Int_t               GetVtxsvol(Int_t i)             const {return fVtxsvol[i];}
  Float_t             GetVtxs(Int_t n, Int_t i=0)             {return (i<3) ? fVtxs[n][i]: 0;}
  Int_t               GetVecRecNumber()       const {return fVecRecNumber;}
  Int_t               GetJmu()                const {return fJmu;}
  Int_t               GetJp()                 const {return fJp;}
  Int_t               GetNpar()               const {return fNpar;}
  Int_t               GetNumTubesHit()        const {return fNumTubesHit;}
  Int_t               GetNumDigiTubesHit()    const {return fNumDigitizedTubes;}
  Int_t               GetNtrack()             const {return fNtrack; }
  Int_t               GetNcherenkovhits()     const {return fNcherenkovhits; }
  Int_t               GetNcherenkovhittimes() const {return fNcherenkovhittimes;}
  Int_t               GetNcherenkovdigihits() const {return fNcherenkovdigihits;}
  Float_t             GetSumQ()               const { return fSumQ;}
  TriggerType_t       GetTriggerType()        const { return fTriggerType;}
  std::vector<Float_t> GetTriggerInfo()        const { return fTriggerInfo;}
  WCSimRootEvent*     GetParentEvent()        const {return fParentEvent;}
  Int_t 		GetNCherenkov()		const { return fCherenkov;}
  Int_t 		GetNCherenkovWCSim()		const { return fCherenkovWCSim;}

  WCSimRootTrack         *AddTrack(Int_t ipnu, 
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
				   Float_t TankExitPoint[3],
				   Double_t TankExitE,
				   Float_t TankExitMom[3]);

  TClonesArray        *GetTracks() const {return fTracks;}
  
  Int_t               GetNcaptures()          const {return fNcaptures; }

  WCSimRootCherenkovHit   *AddCherenkovHit(Int_t                tubeID,
					  std::vector<Float_t> truetime,
					  std::vector<Int_t>   primParID);
  TClonesArray        *GetCherenkovHits() const {return fCherenkovHits;}
  TClonesArray        *GetCherenkovHitTimes() const {return fCherenkovHitTimes;}

  WCSimRootCherenkovDigiHit   *AddCherenkovDigiHit(Float_t q, 
						   Float_t t, 
						   Int_t tubeid,
						   std::vector<int> photon_ids);
//  WCSimRootCherenkovDigiHit   *AddCherenkovDigiHit(Float_t q, 
//						  Float_t t, 
//						  Int_t tubeid,
 //                                                 Float_t sumq);

  TClonesArray            *GetCherenkovDigiHits() const {return fCherenkovDigiHits;}
  TClonesArray            *GetCaptures() const {return fCaptures;}

  ClassDef(WCSimRootTrigger,2) //WCSimRootEvent structure
};


class WCSimRootEvent : public TObject {
public:
  WCSimRootEvent();
  virtual ~WCSimRootEvent();

  void          Clear(Option_t *option ="");
  static void   Reset(Option_t *option ="");
  Int_t GetCurrentIndex() { return Current;}
  void Print(int verbosity=0, int maxtriggerstoprint=10, int maxprimariestoprint=10, int maxtrackstoprint=10, int maxdigitstoprint=10, int maxphotonsperdigittoprint=5, int maxphotonstoprint=5);

  //  WCSimRootTrigger* GetTrigger(int number) { return fEventList[number];}
  WCSimRootTrigger* GetTrigger(int number) {
    if(number>=fEventList->GetEntriesFast()){
      std::cout<<"WCSimRootEvent::GetTrigger("<<number<<") - no such trigger"<<std::endl;
      return nullptr;
    }
    WCSimRootTrigger* thetrigger = (WCSimRootTrigger*) (*fEventList)[number];
    thetrigger->SetParentEvent(this);
    return thetrigger;
  }

  Int_t GetNumberOfEvents() const { return fEventList->GetEntriesFast();}
  Int_t GetNumberOfSubEvents() const { return (fEventList->GetEntriesFast()-1);}
  bool HasSubEvents() { return  (fEventList->GetEntriesFast() > 1); } 

  //Int_t GetNumberOfEvents() const { return fEventList.size();}
  //Int_t GetNumberOfSubEvents() const { return (fEventList.size()-1);}

  //void AddSubEvent() { fEventList.push_back(new WCSimRootTrigger()); }
  void AddSubEvent() { 
    // be sure not to call the default constructor BUT the actual one
    WCSimRootTrigger* tmp = dynamic_cast<WCSimRootTrigger*>( (*fEventList)[0] );
    int num = tmp->GetHeader()->GetEvtNum();
    ++Current; 
    if ( Current > 9 ) fEventList->Expand(150);
    fEventList->AddAt(new WCSimRootTrigger(num,Current),Current);
  }
  
  /*  void ReInitialize() { // need to remove all subevents at the end, or they just get added anyway...
    std::vector<WCSimRootTrigger*>::iterator  iter = fEventList.begin();
    ++iter; // do not delete the first event --> regular beaviour for this program ?
  */
  void Initialize();

  void ReInitialize() { // need to remove all subevents at the end, or they just get added anyway...
    Current = 0;
    if(fEventList==nullptr) return;
    for ( int i = fEventList->GetLast() ; i>=1 ; i--) {
           // std::cout << "removing element # " << i << "...";
      WCSimRootTrigger* tmp = 
	dynamic_cast<WCSimRootTrigger*>(fEventList->RemoveAt(i));
      //std::cout<<"deleting tmp "<<tmp<<std::endl;
      delete tmp;
      //std::cout <<"done !\n";
    }
    WCSimRootTrigger* tmp = dynamic_cast<WCSimRootTrigger*>( (*fEventList)[0]);
    tmp->Clear();
  }

private:
  //std::vector<WCSimRootTrigger*> fEventList;
  TObjArray* fEventList;
  Int_t Current;                      //!               means transient, not writable to file
  ClassDef(WCSimRootEvent,1)

};


#endif
