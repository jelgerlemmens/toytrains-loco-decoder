#include "Structs.h"
#include "UserConfig.h"
#include <NmraDcc.h>

#ifndef CONFVARCONTROLLERH
#define CONFVARCONTROLLERH
// see https://www.nmra.org/sites/default/files/standards/sandrp/pdf/s-9.2.2_decoder_cvs_2012.07.pdf

class ConfVarController
{
private:
    NmraDcc& DccInst;   // reference to the shared NmraDcc instance — avoids copying a non-trivial object
public:
    ConfVarController(NmraDcc& dcc);
    ~ConfVarController();
    void   InitConfVars(int decoderAddress);  // returns void — previously declared int but never returned a value
    CVPair GetCVPair(uint8_t CV_ID);
    int    GetCVPairValue(uint8_t CV_ID);
    void   StoreCvPairValue(CVPair* cv_pair);
};

#endif
