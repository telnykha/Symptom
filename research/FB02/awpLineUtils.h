#ifndef _awp_lineutils_h_ 
#define _awp_lineutils_h_ 

#include "_awpipl.h"

AWPRESULT awpGetLineLength(awpImage* image, awpPoint p1, awpPoint p2, AWPINT* lenngth);
AWPRESULT awpGetLineData(awpImage* image, awpPoint p1, awpPoint p2, AWPDOUBLE* data);

#endif //_awp_lineutils_h_ 