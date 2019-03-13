#include <stdio.h>
#include "include/sabotage.h"
#include "include/smoke.h"
#include "include/fire.h"
#include "include/package.h"
#include "include/crowd.h"
#include "include/counter.h"
#include "include/track.h"

HANDLE sabotage = NULL;
HANDLE smoke    = NULL;
HANDLE fire     = NULL;
HANDLE package  = NULL;
HANDLE crowd    = NULL;
HANDLE counter  = NULL;
HANDLE track    = NULL;

TVAInitParams  params;
TVAPackageInit package_params;


#define _CHECK_MODULE_(module, name)\
	if (module == NULL)\
	{\
		printf("Cannot create module: %s\n", name);\
		result = VA_ERROR;\
		goto cleanup;\
	}\
	
	

int main(int argc, const char** argv)
{
	int result = VA_OK;
	sabotage = sabotageCreate(&params);
	_CHECK_MODULE_(sabotage, "SABOTAGE")
	
	smoke = smokeCreate(&params);
	_CHECK_MODULE_(smoke, "SMOKE")
	
	fire     = fireCreate(&params);
	_CHECK_MODULE_(fire, "FIRE")

	package  = packageCreate(&params, &package_params);
	_CHECK_MODULE_(package, "PACKAGE")

	crowd    = crowdCreate(&params);
	_CHECK_MODULE_(crowd, "CROWD")

	counter  = counterCreate(&params);
	_CHECK_MODULE_(counter, "COUNTER")

	track    = trackCreate(&params);
	_CHECK_MODULE_(track, "TRACK")

cleanup:
	if (sabotage != NULL)
		sabotageRelease(&sabotage);
	if (smoke != NULL)
		smokeRelease(&smoke);
	if (fire != NULL)
		fireRelease(&fire);
	if (package != NULL)
		packageRelease(&package);
	if (crowd != NULL)
		crowdRelease(&crowd);
	if (counter != NULL)
		counterRelease(&counter);
	if (track != NULL)
		trackRelease(&track);
	return result;
}
