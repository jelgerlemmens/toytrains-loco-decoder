#include "ConfVarController.h"

ConfVarController::ConfVarController(NmraDcc& dcc) : DccInst(dcc)
{
}

ConfVarController::~ConfVarController()
{
}

// NOTE: setCV() writes directly to EEPROM storage
void ConfVarController::InitConfVars(int decoderAddress)
{
    // NMRA CV 1 (See S-9.2.2): primary address
    DccInst.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, decoderAddress);

    // CV 29: standard short-address mode with 28/128 speed steps
    // bit 0 = direction of travel (0 = normal), also sets FL(f) and FL(r)
    DccInst.setCV(CV_29_CONFIG, CV29_F0_LOCATION);

    #ifdef DEBUG
        SERIALINTERFACE.println("CV init complete... ");
        SERIALINTERFACE.print("CV 1 (BINARY): ");
        SERIALINTERFACE.println(DccInst.getCV(1), BIN);
        SERIALINTERFACE.print("CV 29 (BINARY): ");
        SERIALINTERFACE.println(DccInst.getCV(29), BIN);
    #endif
}

CVPair ConfVarController::GetCVPair(uint8_t CV_ID)
{
    CVPair cvpair;
    cvpair.CV_ID = CV_ID;
    cvpair.Value = DccInst.getCV(CV_ID);
    return cvpair;
}

int ConfVarController::GetCVPairValue(uint8_t CV_ID)
{
    #ifdef DEBUG
        SERIALINTERFACE.print("Q CVpair ID: ");
        SERIALINTERFACE.println(CV_ID, DEC);
        SERIALINTERFACE.println(DccInst.getCV(CV_ID), DEC);
    #endif
    return DccInst.getCV(CV_ID);
}

void ConfVarController::StoreCvPairValue(CVPair* cv_pair)
{
    DccInst.setCV(cv_pair->CV_ID, cv_pair->Value);
}
