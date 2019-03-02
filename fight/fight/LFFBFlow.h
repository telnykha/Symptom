#ifndef _lf_fbflow_h_
#define _lf_fbflow_h_
#include "awpipl.h"

AWPRESULT awpcvFBFlow(awpImage* image1, awpImage* image2, awpImage* flow);
AWPRESULT awpcvFBFlow2(awpImage* image1, awpImage* image2, awpImage* module, awpImage* angle);
AWPRESULT awpcvDrawFlow(awpImage* src, awpImage* module, awpImage* angle);

#endif //_lf_fbflow_h_