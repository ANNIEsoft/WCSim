
#include "G4UImanager.hh"
#include "DrawMessage.hh"


void DrawMessage::Error(G4String file_name, G4String parameter)
{
	G4cout<<"============= ERROR!! ============="<<G4endl;
	G4cout<<"==  "<<    file_name                <<G4endl;
	G4cout<<"=                                  "<<G4endl;
	G4cout<<"                                   "<<G4endl;
	G4cout<<"            << check >>            "<<G4endl;
	G4cout<<"       "<<parameter                 <<G4endl;
	G4cout<<"                                   "<<G4endl;
	G4cout<<"                                  ="<<G4endl;
	G4cout<<"                                 =="<<G4endl;
	G4cout<<"============= ERROR!! ============="<<G4endl;
	exit(1);
}

G4String DrawMessage::DefDuct(G4int duct)
{
	G4String duct_str = "";
	switch(duct){
		case 1:
			duct_str = "2013B0025 (Al)";
			break;
		case 2:
			duct_str = "2014B0124 (Al+LiF)";
			break;
	}
	return duct_str;
}

G4String DrawMessage::DefParticle(G4int parti)
{
	G4String par_str = "";
	switch(parti){
		case 11:
			par_str = "G4ParticleGun (1 gamma emission)";
			break;
		case 12:
			par_str = "G4ParticleGun (2 gamma emission)";
			break;
		case 2:
			par_str = "  G4GeneralParticleSource (GPS) ";
			break;
	}
	return par_str;
}

G4String DrawMessage::DefTarget(G4int target, G4int model, G4int capture, G4int cascade)
{
	G4String tar_str = "";
	G4String mod_str = "";
	G4String cap_str = "";
	G4String cas_str = "";
	G4String str = "";
	switch(target){
		case 1:
			tar_str = "Gd Target ";
			break;
		case 2:
			tar_str = "Cl Target ";
			break;
		case 3:
			tar_str = "Eu Source ";
			break;
		case 4:
			tar_str = "Co,Na,Cs Source ";
			break;
		case 5:
			tar_str = "None Target ";
			break;
	}

	switch(model){
		case 1:
			mod_str = "(ggarnet model) ";		
			break;
		case 2:
			mod_str = "(glg4sim model) ";		
			break;
		case 3:
			mod_str = "(geant4 default) ";		
			break;
	}
	
	switch(capture){
		case 1:
			cap_str = "Natural ";		
			break;
		case 2:
			cap_str = "Enrich 157";		
			break;
	
		case 3:
			cap_str = "Enrich 155";		
			break;
	}
	
	switch(cascade){
		case 1:
			cas_str = "comparison ";		
			break;
		case 2:
			cas_str = "discrete ";		
			break;
		case 3:
			cas_str = "continuum ";		
			break;
	}

	if(target==1&&model==3){
		str = tar_str+mod_str;
	}else if(target==1&&model!=3){
		str = cap_str+tar_str+mod_str+cas_str;
	}else if(target!=1){
		str = tar_str;
	}	

	return str;
}

G4String DrawMessage::DefVETO(G4int veto)
{
	G4String par_str = "";
	switch(veto){
		case 1:
			par_str = "BGO veto detectors are working!!";
			break;
		case 2:
			par_str = "BGO veto detectors are NOT working!!";
			break;
	}
	return par_str;
}
