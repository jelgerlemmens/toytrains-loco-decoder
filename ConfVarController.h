#include "Structs.h"
#include "UserConfig.h"
#include <NmraDcc.h>

#ifndef CONFVARCONTROLLERH
#define CONFVARCONTROLLERH
#define CVSSIZE 65 //see https://www.nmra.org/sites/default/files/standards/sandrp/pdf/s-9.2.2_decoder_cvs_2012.07.pdf

class ConfVarController
{
private:
    NmraDcc DccInst;
public:
    CVPair CVs [CVSSIZE] = {};
    ConfVarController(NmraDcc dcc);
    ~ConfVarController();
    int InitConfVars(int DECODERADRESS);
    CVPair GetCVPair(int CV_ID);
    int GetCVPairValue(int CV_ID);
    void StoreCvPairValue(CVPair *cv_pair);
};

#endif
