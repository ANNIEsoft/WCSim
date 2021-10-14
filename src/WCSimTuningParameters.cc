// Tuning parameters

#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"


WCSimTuningParameters::WCSimTuningParameters()
{

 TuningMessenger = new WCSimTuningMessenger(this);
 // Default values
 rayff=0.75;
 bsrff=2.50;
 abwff=1.30;
 rgcff=0.32;
 rgcffr7081=0.32;
 mieff=0.0;

 //ANNIE-default values
 teflonrff=1.00;
 holderrff=1.00;
 holderrfflux=1.00;
 linerrff=1.00;
 holder=false;
 QEratio=1.00;
 QEratioWB=1.00;
 PMTWiseQE=false;

 //jl145 - For Top Veto
 tvspacing = 100.0;
 topveto = false;

}

WCSimTuningParameters::~WCSimTuningParameters()
{
  delete TuningMessenger;
  TuningMessenger = 0;
}

void WCSimTuningParameters::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetRayff(rayff);
  wcopt->SetBsrff(bsrff);
  wcopt->SetAbwff(abwff);
  wcopt->SetRgcff(rgcff);
  wcopt->SetRgcffR7081(rgcffr7081);
  wcopt->SetMieff(mieff);
  wcopt->SetTeflonrff(teflonrff);
  wcopt->SetHolderrff(holderrff);
  wcopt->SetHolderrffLUX(holderrfflux);
  wcopt->SetLinerrff(linerrff);
  wcopt->SetHolder(holder);
  wcopt->SetQERatio(QEratio);
  wcopt->SetQERatioWB(QEratioWB);
  wcopt->SetPMTWiseQE(PMTWiseQE);
  wcopt->SetTvspacing(tvspacing);
  wcopt->SetTopveto(topveto);
}
