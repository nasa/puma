#ifndef GENERATE_TPMS_H
#define GENERATE_TPMS_H

#include "generate.h"
#include "tpmsinput.h"
#include "mp_volumefraction.h"
#include "mp_volumefractionhelper.h"
#include "prng_engine.h"
#include "vector.h"
#include "workspace.h"

#include <string>


namespace puma {

/*! Generate TPMS geometry
\param grayWS the output workspace
\param input the TPMSInput details for the generation
\return true if successful, false if it failing
*/
bool generateTPMS(Workspace *grayWS, TPMSInput input);
}


class Generate_TPMS : public Generate
{
public:

    Generate_TPMS(puma::Workspace *sphereWS, TPMSInput input);
    bool generate();


private:

    TPMSInput input;
    puma::Workspace *tpmsWS;

    bool TPMSGenerationHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};



#endif // GENERATE_TPMS_H

