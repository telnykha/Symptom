// internal API 

#ifndef __counter_h_
#define __counter_h_

class CCounter;
class TLFTCounter;
class TLFFlowCounter;

typedef struct
{
	int size;
	int options;
	CCounter*		 c_counter; // sigle door counter 
	TLFTCounter*	 t_counter; // trajectory counter
	TLFFlowCounter*  f_counter; // flow counter 
}TheCounter;


#endif //__counter_h_