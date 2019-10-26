#include "awpLineUtils.h"
AWPRESULT awpGetLineLength(awpImage* image, awpPoint p1, awpPoint p2, AWPINT* length)
{
	AWPWORD t, distance;
	AWPSHORT incx, incy;

	AWPWORD startx = p1.X;
	AWPWORD starty = p1.Y;
	AWPWORD endx = p2.X;
	AWPWORD endy = p2.Y;
	AWPINT xerr = 0, yerr = 0, delta_x, delta_y;
	AWPRESULT  res = AWP_OK;


	if (image == NULL || length == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
	
	_CHECK_RESULT_((res = awpCheckImage(image)))
	if (p1.X > image->sSizeX || p1.Y > image->sSizeY ||
		p2.X > image->sSizeX || p2.Y > image->sSizeY)
		_ERROR_EXIT_RES_(AWP_BADARG)


	delta_x = endx - startx;
	delta_y = endy - starty;

	if (delta_x > 0) incx = 1;
	else if (delta_x == 0) incx = 0;
	else incx = -1;

	if (delta_y > 0) incy = 1;
	else if (delta_y == 0) incy = 0;
	else incy = -1;

	delta_x = abs(delta_x);
	delta_y = abs(delta_y);
	if (delta_x > delta_y) distance = delta_x;
	else distance = delta_y;
	*length = 0;
	for (t = 0; t <= distance + 1; t++)
	{
		if (startx < image->sSizeX && starty < image->sSizeY)
		{
			(*length)++;
		}
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > (AWPINT)distance){ xerr -= (AWPINT)distance; startx += (AWPDWORD)incx; }
		if (yerr > (AWPINT)distance){ yerr -= (AWPINT)distance; starty += (AWPDWORD)incy; }
	}

CLEANUP:
	return res;
}

AWPRESULT awpGetLineData(awpImage* image, awpPoint p1, awpPoint p2, AWPDOUBLE* data)
{
	AWPWORD t, distance;
	AWPSHORT incx, incy;
	AWPINT c;
	AWPWORD startx = p1.X;
	AWPWORD starty = p1.Y;
	AWPWORD endx = p2.X;
	AWPWORD endy = p2.Y;
	AWPINT xerr = 0, yerr = 0, delta_x, delta_y;
	AWPRESULT  res = AWP_OK;

	if (image == NULL || data == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
	//printf("image is not null.\n");
	_CHECK_RESULT_((res = awpCheckImage(image)))
	if (p1.X > image->sSizeX || p1.Y > image->sSizeY ||
		p2.X > image->sSizeX || p2.Y > image->sSizeY)
		_ERROR_EXIT_RES_(AWP_BADARG)

	delta_x = endx - startx;
	delta_y = endy - starty;

	if (delta_x > 0) incx = 1;
	else if (delta_x == 0) incx = 0;
	else incx = -1;

	if (delta_y > 0) incy = 1;
	else if (delta_y == 0) incy = 0;
	else incy = -1;

	delta_x = abs(delta_x);
	delta_y = abs(delta_y);
	if (delta_x > delta_y) distance = delta_x;
	else distance = delta_y;
	c = 0;
	for (t = 0; t <= distance + 1; t++)
	{
		if (startx < image->sSizeX && starty < image->sSizeY)
		{

			switch (image->dwType)
			{
			case AWP_BYTE:
				data[c] = (AWPDOUBLE)_PIXEL(image, startx, starty, 0, AWPBYTE);
				break;
			case AWP_SHORT:
				data[c] = (AWPDOUBLE)_PIXEL(image, startx, starty, 0, AWPSHORT);
				break;
			case AWP_FLOAT:
				data[c] = (AWPDOUBLE)_PIXEL(image, startx, starty, 0, AWPFLOAT);
				break;
			case AWP_DOUBLE:
				data[c] = (AWPDOUBLE)_PIXEL(image, startx, starty, 0, AWPDOUBLE);
				break;
			}
			c++;
		}
		xerr += delta_x;
		yerr += delta_y;
		if (xerr >(AWPINT)distance){ xerr -= (AWPINT)distance; startx += (AWPDWORD)incx; }
		if (yerr >(AWPINT)distance){ yerr -= (AWPINT)distance; starty += (AWPDWORD)incy; }
	}
	//printf("awpGetLineData DONE.\n");
CLEANUP:
	return res;
}
