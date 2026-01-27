#include "WCSimWCLAPPD.hh"
//#include "WCSimLAPPDResponse.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimLAPPDInfo.hh"
#include "WCSimLAPPDObject.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "TObject.h"
#include "TFile.h"
#include "TH1.h"
#include <vector>
#include "TF1.h"

#include <iostream>
#include <cmath>
// for memset
#include <cstring>
#include <string>
#include <random>
#include <map>
#include <vector>
#include <set>
#include <utility>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <iterator>
#include <exception>

//This class is an edited copy of LAPPDresponse.hh 

extern "C" void skrn1pe_(float* );
//extern "C" void rn1pe_(float* ); // 1Kton

//ClassImp(WCSimWCLAPPD)

TFile WCSimWCLAPPD::tf("pulsecharacteristics.root","READ");
int WCSimWCLAPPD::lappobjectcounter=0;

WCSimWCLAPPD::WCSimWCLAPPD(G4String name,
                                   WCSimDetectorConstruction* myDetector)
 :G4VDigitizerModule(name)
{
  lappobjectcounter++;
  G4String colName = "WCRawLAPPDSignalCollection"; //this is the post-noise hit collection for the WC-Check EventAction
  this->myDetector = myDetector;
  collectionName.push_back(colName);
  DigiHitMapLAPPD.clear();
 
 
  // the shape of a typical pulse 
  _templatepulse = (TH1D*) tf.Get("templatepulse");
  // variations in the peak signal on the central strip
  _PHD = (TH1D*) tf.Get("PHD");

  // charge spreading of a pulse in the transverse direction (in mm)
  // as a function of nearness to strip center. The charge tends to 
  // spread more in the transverse direction if the centroid of the
  // signal is between two striplines
   _pulsewidth = (TH1D*) tf.Get("pulsewidth");

  // structure to store the pulses, count them, and organize them by channel
  _pulseCluster = new WCSimLAPPDpulseCluster();

  // random numbers for generating noise
  mrand = new TRandom3();
  
}

WCSimWCLAPPD::~WCSimWCLAPPD(){
  lappobjectcounter--;
  if(lappobjectcounter==0){ tf.Close(); }
  
  delete _pulseCluster;
  _pulseCluster=0;
  
  delete mrand;
  mrand=0;
}

std::map<int,double> stripno_peak; std::map<int,double> stripno_time; 
std::map<int,double> stripno_lefttime; std::map<int,double> stripno_righttime;

void WCSimWCLAPPD::AddSinglePhotonTrace(double trans, double para, double time)
{
  // Draw a random value for the peak signal peak
  double peak = _PHD->GetRandom(); 
  //
  //G4cout<<"______ peak= "<<peak<<G4endl;
  
  // find nearest strip
  int neareststripnum = this->FindStripNumber(trans);
  
  // calculate distance from nearest strip center
  double offcenter = fabs(trans - (this->StripCoordinate(neareststripnum))) ; //trans - striptrans;

  // width of the charge sharing
  double thesigma =  _pulsewidth->Interpolate(offcenter);

  //G4cout<<"nearest stripnum: "<<neareststripnum<<" off center: "<<offcenter<<" thesigma "<<thesigma<<G4endl;

  TF1 theChargeSpread = TF1("theChargeSpread","gaus",-100,100);
  theChargeSpread.SetParameter(0,peak);
  theChargeSpread.SetParameter(1,0.0);
  theChargeSpread.SetParameter(2,thesigma);
 
  // calculate distances and times in the parallel direction
  double leftdistance = fabs(-114.554 - para); // annode is 229.108 mm in parallel direction
  double rightdistance = fabs(114.554 - para);

  if(leftdistance+rightdistance!=229.108) G4cout<<"WHAT!? "<<(leftdistance+rightdistance)<<G4endl;

  double lefttime = leftdistance/(0.53*(0.299792458)); // 53% speed of light (picoseconds per mm) on transmission lines
  double righttime = rightdistance/(0.53*(0.299792458)); // 53% speed of light (picoseconds per mm) on transmission lines

  //G4cout<<leftdistance<<" "<<rightdistance<<" "<<lefttime<<" "<<righttime<<G4endl;

  //loop over five-strip cluster about the central strip
  for(int i=0; i<5; i++){

    int wstrip = (neareststripnum-2)+i;
    double wtrans = this->StripCoordinate(wstrip);
    double wspeak = theChargeSpread.Eval(trans-wtrans);

    //signal has to be larger than 0.5 mV
    if( (wspeak>0.5) && (wstrip>0) && (wstrip<31) ) {
   
      //G4cout<<"which strip "<<wstrip<<" peakvalue "<<wspeak<<G4endl;
      //G4cout<<"time= "<<time<<" lefttime: "<<lefttime<<" righttime: "<<righttime<<G4endl;
      WCSimLAPPDpulse* pulse = new WCSimLAPPDpulse(time,lefttime,righttime,wspeak,wstrip);
      _pulseCluster->AddPulse(pulse);
      //G4cout<<"pulsetime: "<<pulse->Getpulsetime()<<" peak= "<<pulse->Getpeakvalue()<<" stripnum= "<<pulse->Getstripnum()<<G4endl;
      stripno_peak.insert(std::pair<int,double> (pulse->Getstripnum(), wspeak) );
      stripno_time.insert(std::pair<int,double> (pulse->Getstripnum(), time) );
      stripno_lefttime.insert(std::pair<int,double> (pulse->Getstripnum(), lefttime) );
      stripno_righttime.insert(std::pair<int,double> (pulse->Getstripnum(), righttime) );
    }
  }   
  //G4cout<<"---> Done Adding Pulse"<<G4endl;
 }
   
TH1D WCSimWCLAPPD::GetTrace(int CHnumber, int parity, double starttime, double samplesize, int numsamples, double thenoise)
{
  //G4cout<<"======== IN GetTrace ======"<<G4endl;
  // parameters for the histogram of the scope trace
  double lowend = (starttime-(samplesize/2.));
  double upend = lowend + samplesize*((double)numsamples);
  TString tracename;
  tracename += "trace_";
  tracename += CHnumber;
  tracename += "_";
  if(parity==1) tracename+="right";
  else tracename+="left";
  //create said histogram
  TH1D trace(tracename,tracename,numsamples,lowend,upend);
  //if there are no pulses on the strip, just generate white noise
  if(_pulseCluster->GetNPulsesStrip(CHnumber)==0) {
        for(int j=0; j<numsamples; j++){

          double mnoise = thenoise*(mrand->Rndm()-0.5);
          trace.SetBinContent(j+1, mnoise);
	  //G4cout<<"___ --- mnoise= "<<mnoise<<G4endl;
         }
  } else{
  //if there are pulses on the strip, loop over the N pulses on that strip
   for(int k=0; k<(_pulseCluster->GetNPulsesStrip(CHnumber)); k++){
     //looks up the index number for pulse "k" on strip "CHnumber"
     int wPulse = _pulseCluster->GetPulseNum(CHnumber,k);
     //gets the pulse with that index number
     WCSimLAPPDpulse* mpulse = _pulseCluster->GetPulse(wPulse);
     //peak value of the signal on that strip
     double peakv = mpulse->Getpeakvalue();
     //arrival time of the pulse
     double ptime = mpulse->Getpulsetime();
     //transit time of the pulse along the strip
     double stime;
     //looking at the signal to the left (parity=-1) or right (parity=1)?
     if(parity<0) stime = mpulse->Getlefttime();
     else stime = mpulse->Getrighttime();

     //sum the pulse arrival time with transit time on the strip to determine
     //when the signal will arrive 
     double tottime = ptime + stime;

     //loop over number of samples
     for(int j=0; j<numsamples; j++){

      //get the global time when each sample is acquired
      double bcent = trace.GetBinCenter(j+1);
      double mbincontent=0.0;
      double mnoise=0.0;

      //only add the noise on ONCE
      if(k==0) mnoise = thenoise*(mrand->Rndm()-0.5);
      mbincontent+=mnoise;
        
      //if the sample time actually falls in the window for when the pulse
      //should arrive, evaluate the pulse value at that sample point
     if( (bcent > tottime) && (bcent< tottime+3000) ) mbincontent+=(peakv*(_templatepulse->Interpolate(bcent-tottime)));
     //add this on to the contributions to the trace from previous pulses
     double obincontent = trace.GetBinContent(j+1);
     trace.SetBinContent(j+1,obincontent+mbincontent);

   }
  }
 }
 return trace;
}

//Strip coordinate in y direction (i.e.vertical to Detector Radius) is used as trans
int WCSimWCLAPPD::FindStripNumber(double trans){

  double newtrans = trans + 101.6; //LAPPD side/2 = 101.6 mm
  // the first and last strips have a different width
  int stripnum=-1;
  if(newtrans<5.765) stripnum = 1;
  if(newtrans>197.435) stripnum = 30;

  double stripdouble;
  if(stripnum==-1){      
   // divide the 28 remaining strips into the remaining area 
   double stripdouble = 28.0*((newtrans-5.765)/(203.2 - 11.53));
   stripnum = 2 + floor(stripdouble);
  }
  return stripnum;
}


double WCSimWCLAPPD::StripCoordinate(int stripnumber){

  double coor = -55555.;
  // the first and last strips have a different width
  if(stripnumber==1) coor = (2.31-101.6);
  if(stripnumber==30) coor = (101.6-2.31);

  if( stripnumber>1 && stripnumber<30 ){
  // remaining 28 strips have the same spacing
  coor= (5.765-101.6+3.455) + (stripnumber-2)*6.91;
  }
 return coor;
}

G4double WCSimWCLAPPD::rn1pe(){
  G4String WCIDCollectionName = myDetector->GetIDCollectionName2();
  WCSimLAPPDObject * LAPPD;
  LAPPD = myDetector->GetLAPPDPointer(WCIDCollectionName);
  G4int i;
  G4double random = G4UniformRand();
  G4double random2 = G4UniformRand(); 
  G4float *qpe0;
  qpe0 = LAPPD->Getqpe();

  for(i = 0; i < 501; i++){
    
    if (random <= *(qpe0+i)) break;
  }
  if(i==500)
    random = G4UniformRand();
  
  return (G4double(i-50) + random2)/22.83;
  
}

void WCSimWCLAPPD::Digitize()
{
  //G4cout<<"..........I'm in digitizing step for LAPPDs.........."<<G4endl;
  DigitsCollection = new WCSimWCDigitsCollection ("WCDigitizedCollectionLAPPD",collectionName[0]);
  G4String WCIDCollectionName = myDetector->GetIDCollectionName2();
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
 
  // Get the Associated Hit collection IDs
  G4int WCHCID = DigiMan->GetHitsCollectionID(WCIDCollectionName);

  // The Hits collection
  WCSimWCHitsCollection* WCHClappd =
    (WCSimWCHitsCollection*)(DigiMan->GetHitsCollection(WCHCID));

  if (WCHClappd) {
    //G4cout<<" WCHCID= "<<WCHCID<<G4endl;
    MakePeCorrection_lappd(WCHClappd);
  }

  StoreDigiCollection(DigitsCollection);

}


void WCSimWCLAPPD::MakePeCorrection_lappd(WCSimWCHitsCollection* WCHClappd)
{ 
  //Get the LAPPD info for hit time smearing
  G4String WCIDCollectionName = myDetector->GetIDCollectionName2();
  WCSimLAPPDObject * LAPPD = myDetector->GetLAPPDPointer(WCIDCollectionName);
  //G4cout<<"____WCIDCollectionName from WCSimWCLAPPD: "<<WCIDCollectionName<<G4endl;
  //G4cout<<"WCHClappd->entries()= "<<WCHClappd->entries()<<G4endl;

  for (G4int i=0; i < WCHClappd->entries(); i++)
    {
      G4int  lappd        = (*WCHClappd)[i]->GetTubeID();
      G4double peSmeared = 0.0;
      double time_LAPPD, time_true;
      G4ThreeVector hitPos2;

      //G4cout<<"Total Pes: "<<(*WCHClappd)[i]->GetTotalPe()<<G4endl;
      WCSimWCHit* aHit = (*WCHClappd)[i]; //(*mymrdCollection)[hitnum];
      G4ThreeVector hitPos = aHit->GetPos();
      double hitPosx=hitPos.x();
      double hitPosy=hitPos.y();
      double hitPosz=hitPos.z();
      //G4cout<<"LAPPD= "<<lappd<<" hitPosx= "<<hitPosx<<" hitPosy= "<<hitPosy<<" hitPosz= "<<hitPosz<<G4endl;

      for (G4int ip =0; ip < (*WCHClappd)[i]->GetTotalPe(); ip++){
	try{
	  time_true = (*WCHClappd)[i]->GetTime(ip);
	}
	catch (...){
	  G4cout<<"Exception in WCSimWCLAPPD::MakePeCorrection_lappd call of WCSimWCHit::GetTime"<<G4endl;
	  assert(false);
	}
	peSmeared = rn1pe(); 
	int parent_id = (*WCHClappd)[i]->GetPrimaryParentID(ip);
  int directparent_id = (*WCHClappd)[i]->GetDirectParentID(ip);
	//G4cout<<"------- ip= "<<ip<<" time_true= "<<time_true<<" parent_id= "<<parent_id<<G4endl;

	//apply time smearing
	float Q = (peSmeared > 0.5) ? peSmeared : 0.5;
	time_LAPPD = time_true + LAPPD->HitTimeSmearing(Q);
	//G4cout<<"---- from WCSimWCLAPPD: lappd= "<<lappd<<" time_LAPPD= "<<time_LAPPD<<G4endl;
        
	double strip_coorx = ((*WCHClappd)[i]->GetStripPosition(ip).x()); 
	double strip_coory = ((*WCHClappd)[i]->GetStripPosition(ip).y()); 
	//G4cout<<"GetStripPosition= "<<(*WCHClappd)[i]->GetStripPosition(ip)<<" strip_coorx= "<<strip_coorx<<" strip_coory= "<<strip_coory<<G4endl;

	//-------- Get a random strip number and find its coordinate --------
	//WCSimLAPPDResponse* lappdres = new WCSimLAPPDResponse();
	/*double coor2 = 101.6-5.766; 
	int stripuse;
	int random0 = rand()%(30-1 + 1) + 1;
	G4cout<<"random0= "<<random0<<G4endl;
	stripuse= int(random0);
	double coor = StripCoordinate(stripuse);
	// test function to get the transverse coordinate (mm) for a given strip number
	G4cout<<"strip number: "<<stripuse<<" stripcoordinate: "<<coor<<G4endl;  	
	// test function to get the strip number for a given transverse coordinate (mm)	
	int sno0 = FindStripNumber(coor);
	G4cout<<"strip number for: " <<coor<<" "<<sno0<<G4endl;
        G4cout<<"--------------"<<G4endl;*/
	int sno = FindStripNumber(strip_coory);
        //G4cout<<"strip number for: " <<strip_coory<<" "<<sno<<G4endl;

	// add a photon hit to the LAPPDresponse class
	// AddSinglePhotonTrace(position_transverse_tostrips(mm), position_parallel_tostrips(mm), global_time(psec))
	//AddSinglePhotonTrace(coor2-20.0, 50, 1000);
	//G4cout<<"_______ for check_________****"<<G4endl;
        AddSinglePhotonTrace(strip_coory, strip_coorx, time_LAPPD);
/*
        for(std::map<int,double>::iterator m1=stripno_peak.begin(); m1!=stripno_peak.end(); ++m1){
           G4cout<<"map____"<<(m1)->first<<","<<(m1)->second<<G4endl;
        }
        for(std::map<int,double>::iterator m1=stripno_time.begin(); m1!=stripno_time.end(); ++m1){
           G4cout<<"maptime____"<<(m1)->first<<","<<(m1)->second<<G4endl;
        }
        for(std::map<int,double>::iterator m3=stripno_lefttime.begin(); m3!=stripno_lefttime.end(); ++m3){
           G4cout<<"maplefttime____"<<(m3)->first<<","<<(m3)->second<<G4endl;
        }
        for(std::map<int,double>::iterator m4=stripno_righttime.begin(); m4!=stripno_righttime.end(); ++m4){
           G4cout<<"maprighttime____"<<(m4)->first<<","<<(m4)->second<<G4endl;
        }
*/
	// Get the cluster storing all of the pulses on the LAPPD
	/*LAPPDpulseCluster* mclust = mlappd->GetPulseCluster();
	cout<<"pulses for string: "<<sno<<mclust->GetPulse(sno)<<endl;

	int wPulse = _pulseCluster->GetPulseNum(CHnumber,k);
        //gets the pulse with that index number
        WCSimLAPPDpulse* mpulse = _pulseCluster->GetPulse(wPulse);
        //peak value of the signal on that strip
        double peakv = mpulse->Getpeakvalue();
        //arrival time of the pulse
        double ptime = mpulse->Getpulsetime();*/
        /*for(int i=0; i<30; i++){
	   // loop over 30 strips, query the cluster of pulses - how many hits (if any) are on the strip
	   cout<<"channel "<<i+1<<" number of hits: "<<mclust->GetNPulsesStrip(i+1)<<endl;
	}*/
	//--------------
	//G4cout<<"----------before digitisation........"<<G4endl;
	if ( DigiHitMapLAPPD[lappd] == 0) {
	  WCSimWCDigi* Digi = new WCSimWCDigi();
	  Digi->SetLogicalVolume((*WCHClappd)[0]->GetLogicalVolume());
	  Digi->AddPe(time_LAPPD);	
	  Digi->SetLAPPDID(lappd);
	  Digi->SetPe(ip,peSmeared);
	  Digi->SetTime(ip,time_LAPPD);
	  Digi->SetPreSmearTime(ip,time_true);
	  Digi->SetPrimaryParentID(ip,parent_id);
    Digi->SetDirectParentID(ip,directparent_id);
          Digi->SetStripNo(ip,sno);
          Digi->SetNeighStripNo(ip,stripno_peak);
          Digi->SetNeighStripTime(ip,stripno_time);
          Digi->SetNeighStripLeftTime(ip,stripno_lefttime);
          Digi->SetNeighStripRightTime(ip,stripno_righttime);
	  DigiHitMapLAPPD[lappd] = DigitsCollection->insert(Digi);
	}	
	else {
	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->AddPe(time_LAPPD);
	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetLogicalVolume((*WCHClappd)[0]->GetLogicalVolume());
	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetLAPPDID(lappd);
	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetPe(ip,peSmeared);
	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetTime(ip,time_LAPPD);
	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetPreSmearTime(ip,time_true);
	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetPrimaryParentID(ip,parent_id);
    (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetDirectParentID(ip,directparent_id);
 	  (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetStripNo(ip,sno);
          (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetNeighStripNo(ip,stripno_peak);
          (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetNeighStripTime(ip,stripno_time);
          (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetNeighStripLeftTime(ip,stripno_lefttime);
          (*DigitsCollection)[DigiHitMapLAPPD[lappd]-1]->SetNeighStripRightTime(ip,stripno_righttime);
	}
      stripno_peak.clear();	
      stripno_time.clear();
      stripno_lefttime.clear();
      stripno_righttime.clear();
      } // Loop over hits in each LAPPD
    }// Loop over LAPPDs
}

