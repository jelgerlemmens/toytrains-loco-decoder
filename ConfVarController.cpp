#include "ConfVarController.h"

ConfVarController::ConfVarController(NmraDcc dcc)
{
  DccInst = dcc;
}

ConfVarController::~ConfVarController()
{
  
}
//NOTE: the "setCV function will write the CV's to eeprom storage!"
int ConfVarController::InitConfVars(int DECODERADRESS)
{
  //fill configuration vars list
  //NMRA CV 1 (See S-9.2.2 doc) primary adress, CV as defined in the DCC lib
  DccInst.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, DECODERADRESS);\
    
  //CV 29 : using standard "short"-address with 28/128 speeds steps, as defined in the DCC lib 
  //bit 0 sets direction of travel 0 = normal 1 = inverted, also sets FL(f) and FL(r)
  DccInst.setCV(CV_29_CONFIG, CV29_F0_LOCATION);

  #ifdef DEBUG
    SERIALINTERFACE.println("CV init complete... ");
    SERIALINTERFACE.print("CV 1 (BINARY): ");
    SERIALINTERFACE.println(DccInst.getCV(1), BIN);
    SERIALINTERFACE.print("CV 29 (BINARY): ");
    SERIALINTERFACE.println(DccInst.getCV(29), BIN);
  #endif

}

CVPair ConfVarController::GetCVPair(int CV_ID)
{    
  CVPair cvpair;
  cvpair.CV_ID = CV_ID;
  cvpair.Value = DccInst.getCV(CV_ID);
  return cvpair;
}

int ConfVarController::GetCVPairValue(int CV_ID)
{
  #ifdef DEBUG
    SERIALINTERFACE.print("Q CVpair ID: ");
    SERIALINTERFACE.println(CV_ID, DEC);
    SERIALINTERFACE.println(DccInst.getCV(CV_ID), DEC);
  #endif
  return DccInst.getCV(CV_ID);
}

void ConfVarController::StoreCvPairValue(CVPair *cv_pair){
  DccInst.setCV(cv_pair->CV_ID,cv_pair->Value);  
}

 

    
