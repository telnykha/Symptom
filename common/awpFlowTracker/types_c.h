/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __OPENCV_CORE_TYPES_H__
#define __OPENCV_CORE_TYPES_H__

#if !defined _CRT_SECURE_NO_DEPRECATE && defined _MSC_VER
#  if _MSC_VER > 1300
#    define _CRT_SECURE_NO_DEPRECATE /* to avoid multiple Visual Studio 2005 warnings */
#  endif
#endif


#ifndef SKIP_INCLUDES

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#if !defined _MSC_VER && !defined __BORLANDC__
#  include <stdint.h>
#endif

#if defined __ICL
#  define CV_ICC   __ICL
#elif defined __ICC
#  define CV_ICC   __ICC
#elif defined __ECL
#  define CV_ICC   __ECL
#elif defined __ECC
#  define CV_ICC   __ECC
#elif defined __INTEL_COMPILER
#  define CV_ICC   __INTEL_COMPILER
#endif

#if defined CV_ICC && !defined CV_ENABLE_UNROLLED
#  define CV_ENABLE_UNROLLED 0
#else
#  define CV_ENABLE_UNROLLED 1
#endif

#if (defined _M_X64 && defined _MSC_VER && _MSC_VER >= 1400) || (__GNUC__ >= 4 && defined __x86_64__)
#  if defined WIN32
#    include <intrin.h>
#  endif
#  if defined __SSE2__ || !defined __GNUC__
#    include <emmintrin.h>
#  endif
#endif

#if defined __BORLANDC__
#  include <fastmath.h>
#else
#  include <math.h>
#endif

#ifdef HAVE_IPL
#  ifndef __IPL_H__
#    if defined WIN32 || defined _WIN32
#      include <ipl.h>
#    else
#      include <ipl/ipl.h>
#    endif
#  endif
#elif defined __IPL_H__
#  define HAVE_IPL
#endif

#endif // SKIP_INCLUDES

#if defined WIN32 || defined _WIN32
#  define CV_CDECL __cdecl
#  define CV_STDCALL __stdcall
#else
#  define CV_CDECL
#  define CV_STDCALL
#endif

#ifndef CV_EXTERN_C
#  ifdef __cplusplus
#    define CV_EXTERN_C extern "C"
#    define CV_DEFAULT(val) = val
#  else
#    define CV_EXTERN_C
#    define CV_DEFAULT(val)
#  endif
#endif

#ifndef CV_EXTERN_C_FUNCPTR
#  ifdef __cplusplus
#    define CV_EXTERN_C_FUNCPTR(x) extern "C" { typedef x; }
#  else
#    define CV_EXTERN_C_FUNCPTR(x) typedef x
#  endif
#endif

#ifndef CV_INLINE
#  if defined __cplusplus
#    define CV_INLINE inline
#  elif (defined WIN32 || defined _WIN32 || defined WINCE) && !defined __GNUC__
#    define CV_INLINE __inline
#  else
#    define CV_INLINE static
#  endif
#endif /* CV_INLINE */

#if (defined WIN32 || defined _WIN32 || defined WINCE) && defined CVAPI_EXPORTS
#  define CV_EXPORTS __declspec(dllexport)
#else
#  define CV_EXPORTS
#endif

#ifndef CVAPI
#  define CVAPI(rettype) CV_EXTERN_C CV_EXPORTS rettype CV_CDECL
#endif

#if defined _MSC_VER || defined __BORLANDC__
   typedef __int64 int64;
   typedef unsigned __int64 uint64;
#  define CV_BIG_INT(n)   n##I64
#  define CV_BIG_UINT(n)  n##UI64
#else
   typedef int64_t int64;
   typedef uint64_t uint64;
#  define CV_BIG_INT(n)   n##LL
#  define CV_BIG_UINT(n)  n##ULL
#endif

#ifndef HAVE_IPL
   typedef unsigned char uchar;
   typedef unsigned short ushort;
#endif

typedef signed char schar;

/* special informative macros for wrapper generators */
#define CV_CARRAY(counter)
#define CV_CUSTOM_CARRAY(args)
#define CV_EXPORTS_W CV_EXPORTS
#define CV_EXPORTS_W_SIMPLE CV_EXPORTS
#define CV_EXPORTS_AS(synonym) CV_EXPORTS
#define CV_EXPORTS_W_MAP CV_EXPORTS
#define CV_IN_OUT
#define CV_OUT
#define CV_PROP
#define CV_PROP_RW
#define CV_WRAP
#define CV_WRAP_AS(synonym)
#define CV_WRAP_DEFAULT(value)

/* CvArr* is used to pass arbitrary
 * array-like data structures
 * into functions where the particular
 * array type is recognized at runtime:
 */
typedef void CvArr;

typedef union Cv32suf
{
    int i;
    unsigned u;
    float f;
}
Cv32suf;

typedef union Cv64suf
{
    int64 i;
    uint64 u;
    double f;
}
Cv64suf;

typedef int CVStatus;

enum {
 CV_StsOk=                       0,  /* everithing is ok                */
 CV_StsBackTrace=               -1,  /* pseudo error for back trace     */
 CV_StsError=                   -2,  /* unknown /unspecified error      */
 CV_StsInternal=                -3,  /* internal error (bad state)      */
 CV_StsNoMem=                   -4,  /* insufficient memory             */
 CV_StsBadArg=                  -5,  /* function arg/param is bad       */
 CV_StsBadFunc=                 -6,  /* unsupported function            */
 CV_StsNoConv=                  -7,  /* iter. didn't converge           */
 CV_StsAutoTrace=               -8,  /* tracing                         */
 CV_HeaderIsNull=               -9,  /* image header is NULL            */
 CV_BadImageSize=              -10, /* image size is invalid           */
 CV_BadOffset=                 -11, /* offset is invalid               */
 CV_BadDataPtr=                -12, /**/
 CV_BadStep=                   -13, /**/
 CV_BadModelOrChSeq=           -14, /**/
 CV_BadNumChannels=            -15, /**/
 CV_BadNumChannel1U=           -16, /**/
 CV_BadDepth=                  -17, /**/
 CV_BadAlphaChannel=           -18, /**/
 CV_BadOrder=                  -19, /**/
 CV_BadOrigin=                 -20, /**/
 CV_BadAlign=                  -21, /**/
 CV_BadCallBack=               -22, /**/
 CV_BadTileSize=               -23, /**/
 CV_BadCOI=                    -24, /**/
 CV_BadROISize=                -25, /**/
 CV_MaskIsTiled=               -26, /**/
 CV_StsNullPtr=                -27, /* null pointer */
 CV_StsVecLengthErr=           -28, /* incorrect vector length */
 CV_StsFilterStructContentErr= -29, /* incorr. filter structure content */
 CV_StsKernelStructContentErr= -30, /* incorr. transform kernel content */
 CV_StsFilterOffsetErr=        -31, /* incorrect filter ofset value */
 CV_StsBadSize=                -201, /* the input/output structure size is incorrect  */
 CV_StsDivByZero=              -202, /* division by zero */
 CV_StsInplaceNotSupported=    -203, /* in-place operation is not supported */
 CV_StsObjectNotFound=         -204, /* request can't be completed */
 CV_StsUnmatchedFormats=       -205, /* formats of input/output arrays differ */
 CV_StsBadFlag=                -206, /* flag is wrong or not supported */
 CV_StsBadPoint=               -207, /* bad CvPoint */
 CV_StsBadMask=                -208, /* bad format of mask (neither 8uC1 nor 8sC1)*/
 CV_StsUnmatchedSizes=         -209, /* sizes of input/output structures do not match */
 CV_StsUnsupportedFormat=      -210, /* the data format/type is not supported by the function*/
 CV_StsOutOfRange=             -211, /* some of parameters are out of range */
 CV_StsParseError=             -212, /* invalid syntax/structure of the parsed file */
 CV_StsNotImplemented=         -213, /* the requested function/feature is not implemented */
 CV_StsBadMemBlock=            -214, /* an allocated block has been corrupted */
 CV_StsAssert=                 -215, /* assertion failed */
 CV_GpuNotSupported=           -216,
 CV_GpuApiCallError=           -217,
 CV_OpenGlNotSupported=        -218,
 CV_OpenGlApiCallError=        -219
};

/****************************************************************************************\
*                             Common macros and inline functions                         *
\****************************************************************************************/

#ifdef HAVE_TEGRA_OPTIMIZATION
#  include "tegra_round.hpp"
#endif

#define CV_PI   3.1415926535897932384626433832795
#define CV_LOG2 0.69314718055994530941723212145818

#define CV_SWAP(a,b,t) ((t) = (a), (a) = (b), (b) = (t))

#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

/* min & max without jumps */
#define  CV_IMIN(a, b)  ((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))

#define  CV_IMAX(a, b)  ((a) ^ (((a)^(b)) & (((a) > (b)) - 1)))

/* absolute value without jumps */
#ifndef __cplusplus
#  define  CV_IABS(a)     (((a) ^ ((a) < 0 ? -1 : 0)) - ((a) < 0 ? -1 : 0))
#else
#  define  CV_IABS(a)     abs(a)
#endif
#define  CV_CMP(a,b)    (((a) > (b)) - ((a) < (b)))
#define  CV_SIGN(a)     CV_CMP((a),0)

CV_INLINE  int  cvRound( double value )
{
#if (defined _MSC_VER && defined _M_X64) || (defined __GNUC__ && defined __x86_64__ && defined __SSE2__ && !defined __APPLE__)
    __m128d t = _mm_set_sd( value );
    return _mm_cvtsd_si32(t);
#elif defined _MSC_VER && defined _M_IX86
    int t;
    __asm
    {
        fld value;
        fistp t;
    }
    return t;
#elif defined _MSC_VER && defined _M_ARM && defined HAVE_TEGRA_OPTIMIZATION
    TEGRA_ROUND(value);
#elif defined HAVE_LRINT || defined CV_ICC || defined __GNUC__
#  ifdef HAVE_TEGRA_OPTIMIZATION
    TEGRA_ROUND(value);
#  else
    return (int)lrint(value);
#  endif
#else
    double intpart, fractpart;
    fractpart = modf(value, &intpart);
    if ((fabs(fractpart) != 0.5) || ((((int)intpart) % 2) != 0))
        return (int)(value + (value >= 0 ? 0.5 : -0.5));
    else
        return (int)intpart;
#endif
}

#if defined __SSE2__ || (defined _M_IX86_FP && 2 == _M_IX86_FP)
#  include "emmintrin.h"
#endif

CV_INLINE  int  cvFloor( double value )
{
#if defined _MSC_VER && defined _M_X64 || (defined __GNUC__ && defined __SSE2__ && !defined __APPLE__)
    __m128d t = _mm_set_sd( value );
    int i = _mm_cvtsd_si32(t);
    return i - _mm_movemask_pd(_mm_cmplt_sd(t, _mm_cvtsi32_sd(t,i)));
#elif defined __GNUC__
    int i = (int)value;
    return i - (i > value);
#else
    int i = cvRound(value);
    float diff = (float)(value - i);
    return i - (diff < 0);
#endif
}


CV_INLINE  int  cvCeil( double value )
{
#if defined _MSC_VER && defined _M_X64 || (defined __GNUC__ && defined __SSE2__&& !defined __APPLE__)
    __m128d t = _mm_set_sd( value );
    int i = _mm_cvtsd_si32(t);
    return i + _mm_movemask_pd(_mm_cmplt_sd(_mm_cvtsi32_sd(t,i), t));
#elif defined __GNUC__
    int i = (int)value;
    return i + (i < value);
#else
    int i = cvRound(value);
    float diff = (float)(i - value);
    return i + (diff < 0);
#endif
}

#define cvInvSqrt(value) ((float)(1./sqrt(value)))
#define cvSqrt(value)  ((float)sqrt(value))

CV_INLINE int cvIsNaN( double value )
{
    Cv64suf ieee754;
    ieee754.f = value;
    return ((unsigned)(ieee754.u >> 32) & 0x7fffffff) +
           ((unsigned)ieee754.u != 0) > 0x7ff00000;
}


CV_INLINE int cvIsInf( double value )
{
    Cv64suf ieee754;
    ieee754.f = value;
    return ((unsigned)(ieee754.u >> 32) & 0x7fffffff) == 0x7ff00000 &&
           (unsigned)ieee754.u == 0;
}


/*************** Random number generation *******************/

typedef uint64 CvRNG;

#define CV_RNG_COEFF 4164903690U

CV_INLINE CvRNG cvRNG( int64 seed CV_DEFAULT(-1))
{
    CvRNG rng = seed ? (uint64)seed : (uint64)(int64)-1;
    return rng;
}

/* Return random 32-bit unsigned integer: */
CV_INLINE unsigned cvRandInt( CvRNG* rng )
{
    uint64 temp = *rng;
    temp = (uint64)(unsigned)temp*CV_RNG_COEFF + (temp >> 32);
    *rng = temp;
    return (unsigned)temp;
}

/* Returns random floating-point number between 0 and 1: */
CV_INLINE double cvRandReal( CvRNG* rng )
{
    return cvRandInt(rng)*2.3283064365386962890625e-10 /* 2^-32 */;
}

/****************************************************************************************\
*                                  Image type (IplImage)                                 *
\****************************************************************************************/

#ifndef HAVE_IPL

/*
 * The following definitions (until #endif)
 * is an extract from IPL headers.
 * Copyright (c) 1995 Intel Corporation.
 */
#define IPL_DEPTH_SIGN 0x80000000

#define IPL_DEPTH_1U     1
#define IPL_DEPTH_8U     8
#define IPL_DEPTH_16U   16
#define IPL_DEPTH_32F   32

#define IPL_DEPTH_8S  (IPL_DEPTH_SIGN| 8)
#define IPL_DEPTH_16S (IPL_DEPTH_SIGN|16)
#define IPL_DEPTH_32S (IPL_DEPTH_SIGN|32)

#define IPL_DATA_ORDER_PIXEL  0
#define IPL_DATA_ORDER_PLANE  1

#define IPL_ORIGIN_TL 0
#define IPL_ORIGIN_BL 1

#define IPL_ALIGN_4BYTES   4
#define IPL_ALIGN_8BYTES   8
#define IPL_ALIGN_16BYTES 16
#define IPL_ALIGN_32BYTES 32

#define IPL_ALIGN_DWORD   IPL_ALIGN_4BYTES
#define IPL_ALIGN_QWORD   IPL_ALIGN_8BYTES

#define IPL_BORDER_CONSTANT   0
#define IPL_BORDER_REPLICATE  1
#define IPL_BORDER_REFLECT    2
#define IPL_BORDER_WRAP       3

typedef struct _IplImage
{
    int  nSize;             /* sizeof(IplImage) */
    int  ID;                /* version (=0)*/
    int  nChannels;         /* Most of OpenCV functions support 1,2,3 or 4 channels */
    int  alphaChannel;      /* Ignored by OpenCV */
    int  depth;             /* Pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                               IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported.  */
    char colorModel[4];     /* Ignored by OpenCV */
    char channelSeq[4];     /* ditto */
    int  dataOrder;         /* 0 - interleaved color channels, 1 - separate color channels.
                               cvCreateImage can only create interleaved images */
    int  origin;            /* 0 - top-left origin,
                               1 - bottom-left origin (Windows bitmaps style).  */
    int  align;             /* Alignment of image rows (4 or 8).
                               OpenCV ignores it and uses widthStep instead.    */
    int  width;             /* Image width in pixels.                           */
    int  height;            /* Image height in pixels.                          */
    struct _IplROI *roi;    /* Image ROI. If NULL, the whole image is selected. */
    struct _IplImage *maskROI;      /* Must be NULL. */
    void  *imageId;                 /* "           " */
    struct _IplTileInfo *tileInfo;  /* "           " */
    int  imageSize;         /* Image data size in bytes
                               (==image->height*image->widthStep
                               in case of interleaved data)*/
    char *imageData;        /* Pointer to aligned image data.         */
    int  widthStep;         /* Size of aligned image row in bytes.    */
    int  BorderMode[4];     /* Ignored by OpenCV.                     */
    int  BorderConst[4];    /* Ditto.                                 */
    char *imageDataOrigin;  /* Pointer to very origin of image data
                               (not necessarily aligned) -
                               needed for correct deallocation */
}
IplImage;

typedef struct _IplTileInfo IplTileInfo;

typedef struct _IplROI
{
    int  coi; /* 0 - no COI (all channels are selected), 1 - 0th channel is selected ...*/
    int  xOffset;
    int  yOffset;
    int  width;
    int  height;
}
IplROI;

typedef struct _IplConvKernel
{
    int  nCols;
    int  nRows;
    int  anchorX;
    int  anchorY;
    int *values;
    int  nShiftR;
}
IplConvKernel;

typedef struct _IplConvKernelFP
{
    int  nCols;
    int  nRows;
    int  anchorX;
    int  anchorY;
    float *values;
}
IplConvKernelFP;

#define IPL_IMAGE_HEADER 1
#define IPL_IMAGE_DATA   2
#define IPL_IMAGE_ROI    4

#endif/*HAVE_IPL*/

/* extra border mode */
#define IPL_BORDER_REFLECT_101    4
#define IPL_BORDER_TRANSPARENT    5

#define IPL_IMAGE_MAGIC_VAL  ((int)sizeof(IplImage))
#define CV_TYPE_NAME_IMAGE "opencv-image"

#define CV_IS_IMAGE_HDR(img) \
    ((img) != NULL && ((const IplImage*)(img))->nSize == sizeof(IplImage))

#define CV_IS_IMAGE(img) \
    (CV_IS_IMAGE_HDR(img) && ((IplImage*)img)->imageData != NULL)

/* for storing double-precision
   floating point data in IplImage's */
#define IPL_DEPTH_64F  64

/* get reference to pixel at (col,row),
   for multi-channel images (col) should be multiplied by number of channels */
#define CV_IMAGE_ELEM( image, elemtype, row, col )       \
    (((elemtype*)((image)->imageData + (image)->widthStep*(row)))[(col)])

/****************************************************************************************\
*                                  Matrix type (CvMat)                                   *
\****************************************************************************************/

#define CV_CN_MAX     512
#define CV_CN_SHIFT   3
#define CV_DEPTH_MAX  (1 << CV_CN_SHIFT)

#define CV_8U   0
#define CV_8S   1
#define CV_16U  2
#define CV_16S  3
#define CV_32S  4
#define CV_32F  5
#define CV_64F  6
#define CV_USRTYPE1 7

#define CV_MAT_DEPTH_MASK       (CV_DEPTH_MAX - 1)
#define CV_MAT_DEPTH(flags)     ((flags) & CV_MAT_DEPTH_MASK)

#define CV_MAKETYPE(depth,cn) (CV_MAT_DEPTH(depth) + (((cn)-1) << CV_CN_SHIFT))
#define CV_MAKE_TYPE CV_MAKETYPE

#define CV_8UC1 CV_MAKETYPE(CV_8U,1)
#define CV_8UC2 CV_MAKETYPE(CV_8U,2)
#define CV_8UC3 CV_MAKETYPE(CV_8U,3)
#define CV_8UC4 CV_MAKETYPE(CV_8U,4)
#define CV_8UC(n) CV_MAKETYPE(CV_8U,(n))

#define CV_8SC1 CV_MAKETYPE(CV_8S,1)
#define CV_8SC2 CV_MAKETYPE(CV_8S,2)
#define CV_8SC3 CV_MAKETYPE(CV_8S,3)
#define CV_8SC4 CV_MAKETYPE(CV_8S,4)
#define CV_8SC(n) CV_MAKETYPE(CV_8S,(n))

#define CV_16UC1 CV_MAKETYPE(CV_16U,1)
#define CV_16UC2 CV_MAKETYPE(CV_16U,2)
#define CV_16UC3 CV_MAKETYPE(CV_16U,3)
#define CV_16UC4 CV_MAKETYPE(CV_16U,4)
#define CV_16UC(n) CV_MAKETYPE(CV_16U,(n))

#define CV_16SC1 CV_MAKETYPE(CV_16S,1)
#define CV_16SC2 CV_MAKETYPE(CV_16S,2)
#define CV_16SC3 CV_MAKETYPE(CV_16S,3)
#define CV_16SC4 CV_MAKETYPE(CV_16S,4)
#define CV_16SC(n) CV_MAKETYPE(CV_16S,(n))

#define CV_32SC1 CV_MAKETYPE(CV_32S,1)
#define CV_32SC2 CV_MAKETYPE(CV_32S,2)
#define CV_32SC3 CV_MAKETYPE(CV_32S,3)
#define CV_32SC4 CV_MAKETYPE(CV_32S,4)
#define CV_32SC(n) CV_MAKETYPE(CV_32S,(n))

#define CV_32FC1 CV_MAKETYPE(CV_32F,1)
#define CV_32FC2 CV_MAKETYPE(CV_32F,2)
#define CV_32FC3 CV_MAKETYPE(CV_32F,3)
#define CV_32FC4 CV_MAKETYPE(CV_32F,4)
#define CV_32FC(n) CV_MAKETYPE(CV_32F,(n))

#define CV_64FC1 CV_MAKETYPE(CV_64F,1)
#define CV_64FC2 CV_MAKETYPE(CV_64F,2)
#define CV_64FC3 CV_MAKETYPE(CV_64F,3)
#define CV_64FC4 CV_MAKETYPE(CV_64F,4)
#define CV_64FC(n) CV_MAKETYPE(CV_64F,(n))

#define CV_AUTO_STEP  0x7fffffff
#define CV_WHOLE_ARR  cvSlice( 0, 0x3fffffff )

#define CV_MAT_CN_MASK          ((CV_CN_MAX - 1) << CV_CN_SHIFT)
#define CV_MAT_CN(flags)        ((((flags) & CV_MAT_CN_MASK) >> CV_CN_SHIFT) + 1)
#define CV_MAT_TYPE_MASK        (CV_DEPTH_MAX*CV_CN_MAX - 1)
#define CV_MAT_TYPE(flags)      ((flags) & CV_MAT_TYPE_MASK)
#define CV_MAT_CONT_FLAG_SHIFT  14
#define CV_MAT_CONT_FLAG        (1 << CV_MAT_CONT_FLAG_SHIFT)
#define CV_IS_MAT_CONT(flags)   ((flags) & CV_MAT_CONT_FLAG)
#define CV_IS_CONT_MAT          CV_IS_MAT_CONT
#define CV_SUBMAT_FLAG_SHIFT    15
#define CV_SUBMAT_FLAG          (1 << CV_SUBMAT_FLAG_SHIFT)
#define CV_IS_SUBMAT(flags)     ((flags) & CV_MAT_SUBMAT_FLAG)

#define CV_MAGIC_MASK       0xFFFF0000
#define CV_MAT_MAGIC_VAL    0x42420000
#define CV_TYPE_NAME_MAT    "opencv-matrix"

typedef struct CvMat
{
    int type;
    int step;

    /* for internal use only */
    int* refcount;
    int hdr_refcount;

    union
    {
        uchar* ptr;
        short* s;
        int* i;
        float* fl;
        double* db;
    } data;

#ifdef __cplusplus
    union
    {
        int rows;
        int height;
    };

    union
    {
        int cols;
        int width;
    };
#else
    int rows;
    int cols;
#endif

}
CvMat;


#define CV_IS_MAT_HDR(mat) \
    ((mat) != NULL && \
    (((const CvMat*)(mat))->type & CV_MAGIC_MASK) == CV_MAT_MAGIC_VAL && \
    ((const CvMat*)(mat))->cols > 0 && ((const CvMat*)(mat))->rows > 0)

#define CV_IS_MAT_HDR_Z(mat) \
    ((mat) != NULL && \
    (((const CvMat*)(mat))->type & CV_MAGIC_MASK) == CV_MAT_MAGIC_VAL && \
    ((const CvMat*)(mat))->cols >= 0 && ((const CvMat*)(mat))->rows >= 0)

#define CV_IS_MAT(mat) \
    (CV_IS_MAT_HDR(mat) && ((const CvMat*)(mat))->data.ptr != NULL)

#define CV_IS_MASK_ARR(mat) \
    (((mat)->type & (CV_MAT_TYPE_MASK & ~CV_8SC1)) == 0)

#define CV_ARE_TYPES_EQ(mat1, mat2) \
    ((((mat1)->type ^ (mat2)->type) & CV_MAT_TYPE_MASK) == 0)

#define CV_ARE_CNS_EQ(mat1, mat2) \
    ((((mat1)->type ^ (mat2)->type) & CV_MAT_CN_MASK) == 0)

#define CV_ARE_DEPTHS_EQ(mat1, mat2) \
    ((((mat1)->type ^ (mat2)->type) & CV_MAT_DEPTH_MASK) == 0)

#define CV_ARE_SIZES_EQ(mat1, mat2) \
    ((mat1)->rows == (mat2)->rows && (mat1)->cols == (mat2)->cols)

#define CV_IS_MAT_CONST(mat)  \
    (((mat)->rows|(mat)->cols) == 1)

/* Size of each channel item,
   0x124489 = 1000 0100 0100 0010 0010 0001 0001 ~ array of sizeof(arr_type_elem) */
#define CV_ELEM_SIZE1(type) \
    ((((sizeof(size_t)<<28)|0x8442211) >> CV_MAT_DEPTH(type)*4) & 15)

/* 0x3a50 = 11 10 10 01 01 00 00 ~ array of log2(sizeof(arr_type_elem)) */
#define CV_ELEM_SIZE(type) \
    (CV_MAT_CN(type) << ((((sizeof(size_t)/4+1)*16384|0x3a50) >> CV_MAT_DEPTH(type)*2) & 3))

#define IPL2CV_DEPTH(depth) \
    ((((CV_8U)+(CV_16U<<4)+(CV_32F<<8)+(CV_64F<<16)+(CV_8S<<20)+ \
    (CV_16S<<24)+(CV_32S<<28)) >> ((((depth) & 0xF0) >> 2) + \
    (((depth) & IPL_DEPTH_SIGN) ? 20 : 0))) & 15)

/* Inline constructor. No data is allocated internally!!!
 * (Use together with cvCreateData, or use cvCreateMat instead to
 * get a matrix with allocated data):
 */
CV_INLINE CvMat cvMat( int rows, int cols, int type, void* data CV_DEFAULT(NULL))
{
    CvMat m;

    assert( (unsigned)CV_MAT_DEPTH(type) <= CV_64F );
    type = CV_MAT_TYPE(type);
    m.type = CV_MAT_MAGIC_VAL | CV_MAT_CONT_FLAG | type;
    m.cols = cols;
    m.rows = rows;
    m.step = m.cols*CV_ELEM_SIZE(type);
    m.data.ptr = (uchar*)data;
    m.refcount = NULL;
    m.hdr_refcount = 0;

    return m;
}


#define CV_MAT_ELEM_PTR_FAST( mat, row, col, pix_size )  \
    (assert( (unsigned)(row) < (unsigned)(mat).rows &&   \
             (unsigned)(col) < (unsigned)(mat).cols ),   \
     (mat).data.ptr + (size_t)(mat).step*(row) + (pix_size)*(col))

#define CV_MAT_ELEM_PTR( mat, row, col )                 \
    CV_MAT_ELEM_PTR_FAST( mat, row, col, CV_ELEM_SIZE((mat).type) )

#define CV_MAT_ELEM( mat, elemtype, row, col )           \
    (*(elemtype*)CV_MAT_ELEM_PTR_FAST( mat, row, col, sizeof(elemtype)))


CV_INLINE  double  cvmGet( const CvMat* mat, int row, int col )
{
    int type;

    type = CV_MAT_TYPE(mat->type);
    assert( (unsigned)row < (unsigned)mat->rows &&
            (unsigned)col < (unsigned)mat->cols );

    if( type == CV_32FC1 )
        return ((float*)(mat->data.ptr + (size_t)mat->step*row))[col];
    else
    {
        assert( type == CV_64FC1 );
        return ((double*)(mat->data.ptr + (size_t)mat->step*row))[col];
    }
}


CV_INLINE  void  cvmSet( CvMat* mat, int row, int col, double value )
{
    int type;
    type = CV_MAT_TYPE(mat->type);
    assert( (unsigned)row < (unsigned)mat->rows &&
            (unsigned)col < (unsigned)mat->cols );

    if( type == CV_32FC1 )
        ((float*)(mat->data.ptr + (size_t)mat->step*row))[col] = (float)value;
    else
    {
        assert( type == CV_64FC1 );
        ((double*)(mat->data.ptr + (size_t)mat->step*row))[col] = (double)value;
    }
}


CV_INLINE int cvIplDepth( int type )
{
    int depth = CV_MAT_DEPTH(type);
    return CV_ELEM_SIZE1(depth)*8 | (depth == CV_8S || depth == CV_16S ||
           depth == CV_32S ? IPL_DEPTH_SIGN : 0);
}


/****************************************************************************************\
*                       Multi-dimensional dense array (CvMatND)                          *
\****************************************************************************************/

#define CV_MATND_MAGIC_VAL    0x42430000
#define CV_TYPE_NAME_MATND    "opencv-nd-matrix"

#define CV_MAX_DIM            32
#define CV_MAX_DIM_HEAP       1024

typedef struct CvMatND
{
    int type;
    int dims;

    int* refcount;
    int hdr_refcount;

    union
    {
        uchar* ptr;
        float* fl;
        double* db;
        int* i;
        short* s;
    } data;

    struct
    {
        int size;
        int step;
    }
    dim[CV_MAX_DIM];
}
CvMatND;

#define CV_IS_MATND_HDR(mat) \
    ((mat) != NULL && (((const CvMatND*)(mat))->type & CV_MAGIC_MASK) == CV_MATND_MAGIC_VAL)

#define CV_IS_MATND(mat) \
    (CV_IS_MATND_HDR(mat) && ((const CvMatND*)(mat))->data.ptr != NULL)


/****************************************************************************************\
*                      Multi-dimensional sparse array (CvSparseMat)                      *
\****************************************************************************************/

#define CV_SPARSE_MAT_MAGIC_VAL    0x42440000
#define CV_TYPE_NAME_SPARSE_MAT    "opencv-sparse-matrix"

struct CvSet;

typedef struct CvSparseMat
{
    int type;
    int dims;
    int* refcount;
    int hdr_refcount;

    struct CvSet* heap;
    void** hashtable;
    int hashsize;
    int valoffset;
    int idxoffset;
    int size[CV_MAX_DIM];
}
CvSparseMat;

#define CV_IS_SPARSE_MAT_HDR(mat) \
    ((mat) != NULL && \
    (((const CvSparseMat*)(mat))->type & CV_MAGIC_MASK) == CV_SPARSE_MAT_MAGIC_VAL)

#define CV_IS_SPARSE_MAT(mat) \
    CV_IS_SPARSE_MAT_HDR(mat)

/**************** iteration through a sparse array *****************/

typedef struct CvSparseNode
{
    unsigned hashval;
    struct CvSparseNode* next;
}
CvSparseNode;

typedef struct CvSparseMatIterator
{
    CvSparseMat* mat;
    CvSparseNode* node;
    int curidx;
}
CvSparseMatIterator;

#define CV_NODE_VAL(mat,node)   ((void*)((uchar*)(node) + (mat)->valoffset))
#define CV_NODE_IDX(mat,node)   ((int*)((uchar*)(node) + (mat)->idxoffset))

/****************************************************************************************\
*                                         Histogram                                      *
\****************************************************************************************/

typedef int CvHistType;

#define CV_HIST_MAGIC_VAL     0x42450000
#define CV_HIST_UNIFORM_FLAG  (1 << 10)

/* indicates whether bin ranges are set already or not */
#define CV_HIST_RANGES_FLAG   (1 << 11)

#define CV_HIST_ARRAY         0
#define CV_HIST_SPARSE        1
#define CV_HIST_TREE          CV_HIST_SPARSE

/* should be used as a parameter only,
   it turns to CV_HIST_UNIFORM_FLAG of hist->type */
#define CV_HIST_UNIFORM       1

typedef struct CvHistogram
{
    int     type;
    CvArr*  bins;
    float   thresh[CV_MAX_DIM][2];  /* For uniform histograms.                      */
    float** thresh2;                /* For non-uniform histograms.                  */
    CvMatND mat;                    /* Embedded matrix header for array histograms. */
}
CvHistogram;

#define CV_IS_HIST( hist ) \
    ((hist) != NULL  && \
     (((CvHistogram*)(hist))->type & CV_MAGIC_MASK) == CV_HIST_MAGIC_VAL && \
     (hist)->bins != NULL)

#define CV_IS_UNIFORM_HIST( hist ) \
    (((hist)->type & CV_HIST_UNIFORM_FLAG) != 0)

#define CV_IS_SPARSE_HIST( hist ) \
    CV_IS_SPARSE_MAT((hist)->bins)

#define CV_HIST_HAS_RANGES( hist ) \
    (((hist)->type & CV_HIST_RANGES_FLAG) != 0)

/****************************************************************************************\
*                      Other supplementary data type definitions                         *
\****************************************************************************************/

/*************************************** CvRect *****************************************/

typedef struct CvRect
{
    int x;
    int y;
    int width;
    int height;
}
CvRect;

CV_INLINE  CvRect  cvRect( int x, int y, int width, int height )
{
    CvRect r;

    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;

    return r;
}


CV_INLINE  IplROI  cvRectToROI( CvRect rect, int coi )
{
    IplROI roi;
    roi.xOffset = rect.x;
    roi.yOffset = rect.y;
    roi.width = rect.width;
    roi.height = rect.height;
    roi.coi = coi;

    return roi;
}


CV_INLINE  CvRect  cvROIToRect( IplROI roi )
{
    return cvRect( roi.xOffset, roi.yOffset, roi.width, roi.height );
}

/*********************************** CvTermCriteria *************************************/

#define CV_TERMCRIT_ITER    1
#define CV_TERMCRIT_NUMBER  CV_TERMCRIT_ITER
#define CV_TERMCRIT_EPS     2

typedef struct CvTermCriteria
{
    int    type;  /* may be combination of
                     CV_TERMCRIT_ITER
                     CV_TERMCRIT_EPS */
    int    max_iter;
    double epsilon;
}
CvTermCriteria;

CV_INLINE  CvTermCriteria  cvTermCriteria( int type, int max_iter, double epsilon )
{
    CvTermCriteria t;

    t.type = type;
    t.max_iter = max_iter;
    t.epsilon = (float)epsilon;

    return t;
}


/******************************* CvPoint and variants ***********************************/

typedef struct CvPoint
{
    int x;
    int y;
}
CvPoint;


CV_INLINE  CvPoint  cvPoint( int x, int y )
{
    CvPoint p;

    p.x = x;
    p.y = y;

    return p;
}


typedef struct CvPoint2D32f
{
    float x;
    float y;
}
CvPoint2D32f;


CV_INLINE  CvPoint2D32f  cvPoint2D32f( double x, double y )
{
    CvPoint2D32f p;

    p.x = (float)x;
    p.y = (float)y;

    return p;
}


CV_INLINE  CvPoint2D32f  cvPointTo32f( CvPoint point )
{
    return cvPoint2D32f( (float)point.x, (float)point.y );
}


CV_INLINE  CvPoint  cvPointFrom32f( CvPoint2D32f point )
{
    CvPoint ipt;
    ipt.x = cvRound(point.x);
    ipt.y = cvRound(point.y);

    return ipt;
}


typedef struct CvPoint3D32f
{
    float x;
    float y;
    float z;
}
CvPoint3D32f;


CV_INLINE  CvPoint3D32f  cvPoint3D32f( double x, double y, double z )
{
    CvPoint3D32f p;

    p.x = (float)x;
    p.y = (float)y;
    p.z = (float)z;

    return p;
}


typedef struct CvPoint2D64f
{
    double x;
    double y;
}
CvPoint2D64f;


CV_INLINE  CvPoint2D64f  cvPoint2D64f( double x, double y )
{
    CvPoint2D64f p;

    p.x = x;
    p.y = y;

    return p;
}


typedef struct CvPoint3D64f
{
    double x;
    double y;
    double z;
}
CvPoint3D64f;


CV_INLINE  CvPoint3D64f  cvPoint3D64f( double x, double y, double z )
{
    CvPoint3D64f p;

    p.x = x;
    p.y = y;
    p.z = z;

    return p;
}


/******************************** CvSize's & CvBox **************************************/

typedef struct CvSize
{
    int width;
    int height;
}
CvSize;

CV_INLINE  CvSize  cvSize( int width, int height )
{
    CvSize s;

    s.width = width;
    s.height = height;

    return s;
}

typedef struct CvSize2D32f
{
    float width;
    float height;
}
CvSize2D32f;


CV_INLINE  CvSize2D32f  cvSize2D32f( double width, double height )
{
    CvSize2D32f s;

    s.width = (float)width;
    s.height = (float)height;

    return s;
}

typedef struct CvBox2D
{
    CvPoint2D32f center;  /* Center of the box.                          */
    CvSize2D32f  size;    /* Box width and length.                       */
    float angle;          /* Angle between the horizontal axis           */
                          /* and the first side (i.e. length) in degrees */
}
CvBox2D;


/* Line iterator state: */
typedef struct CvLineIterator
{
    /* Pointer to the current point: */
    uchar* ptr;

    /* Bresenham algorithm state: */
    int  err;
    int  plus_delta;
    int  minus_delta;
    int  plus_step;
    int  minus_step;
}
CvLineIterator;



/************************************* CvSlice ******************************************/

typedef struct CvSlice
{
    int  start_index, end_index;
}
CvSlice;

CV_INLINE  CvSlice  cvSlice( int start, int end )
{
    CvSlice slice;
    slice.start_index = start;
    slice.end_index = end;

    return slice;
}

#define CV_WHOLE_SEQ_END_INDEX 0x3fffffff
#define CV_WHOLE_SEQ  cvSlice(0, CV_WHOLE_SEQ_END_INDEX)


/************************************* CvScalar *****************************************/

typedef struct CvScalar
{
    double val[4];
}
CvScalar;

CV_INLINE  CvScalar  cvScalar( double val0, double val1 CV_DEFAULT(0),
                               double val2 CV_DEFAULT(0), double val3 CV_DEFAULT(0))
{
    CvScalar scalar;
    scalar.val[0] = val0; scalar.val[1] = val1;
    scalar.val[2] = val2; scalar.val[3] = val3;
    return scalar;
}


CV_INLINE  CvScalar  cvRealScalar( double val0 )
{
    CvScalar scalar;
    scalar.val[0] = val0;
    scalar.val[1] = scalar.val[2] = scalar.val[3] = 0;
    return scalar;
}

CV_INLINE  CvScalar  cvScalarAll( double val0123 )
{
    CvScalar scalar;
    scalar.val[0] = val0123;
    scalar.val[1] = val0123;
    scalar.val[2] = val0123;
    scalar.val[3] = val0123;
    return scalar;
}

/****************************************************************************************\
*                                   Dynamic Data structures                              *
\****************************************************************************************/

/******************************** Memory storage ****************************************/

typedef struct CvMemBlock
{
    struct CvMemBlock*  prev;
    struct CvMemBlock*  next;
}
CvMemBlock;

#define CV_STORAGE_MAGIC_VAL    0x42890000

typedef struct CvMemStorage
{
    int signature;
    CvMemBlock* bottom;           /* First allocated block.                   */
    CvMemBlock* top;              /* Current memory block - top of the stack. */
    struct  CvMemStorage* parent; /* We get new blocks from parent as needed. */
    int block_size;               /* Block size.                              */
    int free_space;               /* Remaining free space in current block.   */
}
CvMemStorage;

#define CV_IS_STORAGE(storage)  \
    ((storage) != NULL &&       \
    (((CvMemStorage*)(storage))->signature & CV_MAGIC_MASK) == CV_STORAGE_MAGIC_VAL)


typedef struct CvMemStoragePos
{
    CvMemBlock* top;
    int free_space;
}
CvMemStoragePos;


/*********************************** Sequence *******************************************/

typedef struct CvSeqBlock
{
    struct CvSeqBlock*  prev; /* Previous sequence block.                   */
    struct CvSeqBlock*  next; /* Next sequence block.                       */
  int    start_index;         /* Index of the first element in the block +  */
                              /* sequence->first->start_index.              */
    int    count;             /* Number of elements in the block.           */
    schar* data;              /* Pointer to the first element of the block. */
}
CvSeqBlock;


#define CV_TREE_NODE_FIELDS(node_type)                               \
    int       flags;             /* Miscellaneous flags.     */      \
    int       header_size;       /* Size of sequence header. */      \
    struct    node_type* h_prev; /* Previous sequence.       */      \
    struct    node_type* h_next; /* Next sequence.           */      \
    struct    node_type* v_prev; /* 2nd previous sequence.   */      \
    struct    node_type* v_next  /* 2nd next sequence.       */

/*
   Read/Write sequence.
   Elements can be dynamically inserted to or deleted from the sequence.
*/
#define CV_SEQUENCE_FIELDS()                                              \
    CV_TREE_NODE_FIELDS(CvSeq);                                           \
    int       total;          /* Total number of elements.            */  \
    int       elem_size;      /* Size of sequence element in bytes.   */  \
    schar*    block_max;      /* Maximal bound of the last block.     */  \
    schar*    ptr;            /* Current write pointer.               */  \
    int       delta_elems;    /* Grow seq this many at a time.        */  \
    CvMemStorage* storage;    /* Where the seq is stored.             */  \
    CvSeqBlock* free_blocks;  /* Free blocks list.                    */  \
    CvSeqBlock* first;        /* Pointer to the first sequence block. */

typedef struct CvSeq
{
    CV_SEQUENCE_FIELDS()
}
CvSeq;

#define CV_TYPE_NAME_SEQ             "opencv-sequence"
#define CV_TYPE_NAME_SEQ_TREE        "opencv-sequence-tree"

/*************************************** Set ********************************************/
/*
  Set.
  Order is not preserved. There can be gaps between sequence elements.
  After the element has been inserted it stays in the same place all the time.
  The MSB(most-significant or sign bit) of the first field (flags) is 0 iff the element exists.
*/
#define CV_SET_ELEM_FIELDS(elem_type)   \
    int  flags;                         \
    struct elem_type* next_free;

typedef struct CvSetElem
{
    CV_SET_ELEM_FIELDS(CvSetElem)
}
CvSetElem;

#define CV_SET_FIELDS()      \
    CV_SEQUENCE_FIELDS()     \
    CvSetElem* free_elems;   \
    int active_count;

typedef struct CvSet
{
    CV_SET_FIELDS()
}
CvSet;


#define CV_SET_ELEM_IDX_MASK   ((1 << 26) - 1)
#define CV_SET_ELEM_FREE_FLAG  (1 << (sizeof(int)*8-1))

/* Checks whether the element pointed by ptr belongs to a set or not */
#define CV_IS_SET_ELEM( ptr )  (((CvSetElem*)(ptr))->flags >= 0)

/************************************* Graph ********************************************/

/*
  We represent a graph as a set of vertices.
  Vertices contain their adjacency lists (more exactly, pointers to first incoming or
  outcoming edge (or 0 if isolated vertex)). Edges are stored in another set.
  There is a singly-linked list of incoming/outcoming edges for each vertex.

  Each edge consists of

     o   Two pointers to the starting and ending vertices
         (vtx[0] and vtx[1] respectively).

   A graph may be oriented or not. In the latter case, edges between
   vertex i to vertex j are not distinguished during search operations.

     o   Two pointers to next edges for the starting and ending vertices, where
         next[0] points to the next edge in the vtx[0] adjacency list and
         next[1] points to the next edge in the vtx[1] adjacency list.
*/
#define CV_GRAPH_EDGE_FIELDS()      \
    int flags;                      \
    float weight;                   \
    struct CvGraphEdge* next[2];    \
    struct CvGraphVtx* vtx[2];


#define CV_GRAPH_VERTEX_FIELDS()    \
    int flags;                      \
    struct CvGraphEdge* first;


typedef struct CvGraphEdge
{
    CV_GRAPH_EDGE_FIELDS()
}
CvGraphEdge;

typedef struct CvGraphVtx
{
    CV_GRAPH_VERTEX_FIELDS()
}
CvGraphVtx;

typedef struct CvGraphVtx2D
{
    CV_GRAPH_VERTEX_FIELDS()
    CvPoint2D32f* ptr;
}
CvGraphVtx2D;

/*
   Graph is "derived" from the set (this is set a of vertices)
   and includes another set (edges)
*/
#define  CV_GRAPH_FIELDS()   \
    CV_SET_FIELDS()          \
    CvSet* edges;

typedef struct CvGraph
{
    CV_GRAPH_FIELDS()
}
CvGraph;

#define CV_TYPE_NAME_GRAPH "opencv-graph"

/*********************************** Chain/Countour *************************************/

typedef struct CvChain
{
    CV_SEQUENCE_FIELDS()
    CvPoint  origin;
}
CvChain;

#define CV_CONTOUR_FIELDS()  \
    CV_SEQUENCE_FIELDS()     \
    CvRect rect;             \
    int color;               \
    int reserved[3];

typedef struct CvContour
{
    CV_CONTOUR_FIELDS()
}
CvContour;

typedef CvContour CvPoint2DSeq;

/****************************************************************************************\
*                                    Sequence types                                      *
\****************************************************************************************/

#define CV_SEQ_MAGIC_VAL             0x42990000

#define CV_IS_SEQ(seq) \
    ((seq) != NULL && (((CvSeq*)(seq))->flags & CV_MAGIC_MASK) == CV_SEQ_MAGIC_VAL)

#define CV_SET_MAGIC_VAL             0x42980000
#define CV_IS_SET(set) \
    ((set) != NULL && (((CvSeq*)(set))->flags & CV_MAGIC_MASK) == CV_SET_MAGIC_VAL)

#define CV_SEQ_ELTYPE_BITS           12
#define CV_SEQ_ELTYPE_MASK           ((1 << CV_SEQ_ELTYPE_BITS) - 1)

#define CV_SEQ_ELTYPE_POINT          CV_32SC2  /* (x,y) */
#define CV_SEQ_ELTYPE_CODE           CV_8UC1   /* freeman code: 0..7 */
#define CV_SEQ_ELTYPE_GENERIC        0
#define CV_SEQ_ELTYPE_PTR            CV_USRTYPE1
#define CV_SEQ_ELTYPE_PPOINT         CV_SEQ_ELTYPE_PTR  /* &(x,y) */
#define CV_SEQ_ELTYPE_INDEX          CV_32SC1  /* #(x,y) */
#define CV_SEQ_ELTYPE_GRAPH_EDGE     0  /* &next_o, &next_d, &vtx_o, &vtx_d */
#define CV_SEQ_ELTYPE_GRAPH_VERTEX   0  /* first_edge, &(x,y) */
#define CV_SEQ_ELTYPE_TRIAN_ATR      0  /* vertex of the binary tree   */
#define CV_SEQ_ELTYPE_CONNECTED_COMP 0  /* connected component  */
#define CV_SEQ_ELTYPE_POINT3D        CV_32FC3  /* (x,y,z)  */

#define CV_SEQ_KIND_BITS        2
#define CV_SEQ_KIND_MASK        (((1 << CV_SEQ_KIND_BITS) - 1)<<CV_SEQ_ELTYPE_BITS)

/* types of sequences */
#define CV_SEQ_KIND_GENERIC     (0 << CV_SEQ_ELTYPE_BITS)
#define CV_SEQ_KIND_CURVE       (1 << CV_SEQ_ELTYPE_BITS)
#define CV_SEQ_KIND_BIN_TREE    (2 << CV_SEQ_ELTYPE_BITS)

/* types of sparse sequences (sets) */
#define CV_SEQ_KIND_GRAPH       (1 << CV_SEQ_ELTYPE_BITS)
#define CV_SEQ_KIND_SUBDIV2D    (2 << CV_SEQ_ELTYPE_BITS)

#define CV_SEQ_FLAG_SHIFT       (CV_SEQ_KIND_BITS + CV_SEQ_ELTYPE_BITS)

/* flags for curves */
#define CV_SEQ_FLAG_CLOSED     (1 << CV_SEQ_FLAG_SHIFT)
#define CV_SEQ_FLAG_SIMPLE     (0 << CV_SEQ_FLAG_SHIFT)
#define CV_SEQ_FLAG_CONVEX     (0 << CV_SEQ_FLAG_SHIFT)
#define CV_SEQ_FLAG_HOLE       (2 << CV_SEQ_FLAG_SHIFT)

/* flags for graphs */
#define CV_GRAPH_FLAG_ORIENTED (1 << CV_SEQ_FLAG_SHIFT)

#define CV_GRAPH               CV_SEQ_KIND_GRAPH
#define CV_ORIENTED_GRAPH      (CV_SEQ_KIND_GRAPH|CV_GRAPH_FLAG_ORIENTED)

/* point sets */
#define CV_SEQ_POINT_SET       (CV_SEQ_KIND_GENERIC| CV_SEQ_ELTYPE_POINT)
#define CV_SEQ_POINT3D_SET     (CV_SEQ_KIND_GENERIC| CV_SEQ_ELTYPE_POINT3D)
#define CV_SEQ_POLYLINE        (CV_SEQ_KIND_CURVE  | CV_SEQ_ELTYPE_POINT)
#define CV_SEQ_POLYGON         (CV_SEQ_FLAG_CLOSED | CV_SEQ_POLYLINE )
#define CV_SEQ_CONTOUR         CV_SEQ_POLYGON
#define CV_SEQ_SIMPLE_POLYGON  (CV_SEQ_FLAG_SIMPLE | CV_SEQ_POLYGON  )

/* chain-coded curves */
#define CV_SEQ_CHAIN           (CV_SEQ_KIND_CURVE  | CV_SEQ_ELTYPE_CODE)
#define CV_SEQ_CHAIN_CONTOUR   (CV_SEQ_FLAG_CLOSED | CV_SEQ_CHAIN)

/* binary tree for the contour */
#define CV_SEQ_POLYGON_TREE    (CV_SEQ_KIND_BIN_TREE  | CV_SEQ_ELTYPE_TRIAN_ATR)

/* sequence of the connected components */
#define CV_SEQ_CONNECTED_COMP  (CV_SEQ_KIND_GENERIC  | CV_SEQ_ELTYPE_CONNECTED_COMP)

/* sequence of the integer numbers */
#define CV_SEQ_INDEX           (CV_SEQ_KIND_GENERIC  | CV_SEQ_ELTYPE_INDEX)

#define CV_SEQ_ELTYPE( seq )   ((seq)->flags & CV_SEQ_ELTYPE_MASK)
#define CV_SEQ_KIND( seq )     ((seq)->flags & CV_SEQ_KIND_MASK )

/* flag checking */
#define CV_IS_SEQ_INDEX( seq )      ((CV_SEQ_ELTYPE(seq) == CV_SEQ_ELTYPE_INDEX) && \
                                     (CV_SEQ_KIND(seq) == CV_SEQ_KIND_GENERIC))

#define CV_IS_SEQ_CURVE( seq )      (CV_SEQ_KIND(seq) == CV_SEQ_KIND_CURVE)
#define CV_IS_SEQ_CLOSED( seq )     (((seq)->flags & CV_SEQ_FLAG_CLOSED) != 0)
#define CV_IS_SEQ_CONVEX( seq )     0
#define CV_IS_SEQ_HOLE( seq )       (((seq)->flags & CV_SEQ_FLAG_HOLE) != 0)
#define CV_IS_SEQ_SIMPLE( seq )     1

/* type checking macros */
#define CV_IS_SEQ_POINT_SET( seq ) \
    ((CV_SEQ_ELTYPE(seq) == CV_32SC2 || CV_SEQ_ELTYPE(seq) == CV_32FC2))

#define CV_IS_SEQ_POINT_SUBSET( seq ) \
    (CV_IS_SEQ_INDEX( seq ) || CV_SEQ_ELTYPE(seq) == CV_SEQ_ELTYPE_PPOINT)

#define CV_IS_SEQ_POLYLINE( seq )   \
    (CV_SEQ_KIND(seq) == CV_SEQ_KIND_CURVE && CV_IS_SEQ_POINT_SET(seq))

#define CV_IS_SEQ_POLYGON( seq )   \
    (CV_IS_SEQ_POLYLINE(seq) && CV_IS_SEQ_CLOSED(seq))

#define CV_IS_SEQ_CHAIN( seq )   \
    (CV_SEQ_KIND(seq) == CV_SEQ_KIND_CURVE && (seq)->elem_size == 1)

#define CV_IS_SEQ_CONTOUR( seq )   \
    (CV_IS_SEQ_CLOSED(seq) && (CV_IS_SEQ_POLYLINE(seq) || CV_IS_SEQ_CHAIN(seq)))

#define CV_IS_SEQ_CHAIN_CONTOUR( seq ) \
    (CV_IS_SEQ_CHAIN( seq ) && CV_IS_SEQ_CLOSED( seq ))

#define CV_IS_SEQ_POLYGON_TREE( seq ) \
    (CV_SEQ_ELTYPE (seq) ==  CV_SEQ_ELTYPE_TRIAN_ATR &&    \
    CV_SEQ_KIND( seq ) ==  CV_SEQ_KIND_BIN_TREE )

#define CV_IS_GRAPH( seq )    \
    (CV_IS_SET(seq) && CV_SEQ_KIND((CvSet*)(seq)) == CV_SEQ_KIND_GRAPH)

#define CV_IS_GRAPH_ORIENTED( seq )   \
    (((seq)->flags & CV_GRAPH_FLAG_ORIENTED) != 0)

#define CV_IS_SUBDIV2D( seq )  \
    (CV_IS_SET(seq) && CV_SEQ_KIND((CvSet*)(seq)) == CV_SEQ_KIND_SUBDIV2D)

/****************************************************************************************/
/*                            Sequence writer & reader                                  */
/****************************************************************************************/

#define CV_SEQ_WRITER_FIELDS()                                     \
    int          header_size;                                      \
    CvSeq*       seq;        /* the sequence written */            \
    CvSeqBlock*  block;      /* current block */                   \
    schar*       ptr;        /* pointer to free space */           \
    schar*       block_min;  /* pointer to the beginning of block*/\
    schar*       block_max;  /* pointer to the end of block */

typedef struct CvSeqWriter
{
    CV_SEQ_WRITER_FIELDS()
}
CvSeqWriter;


#define CV_SEQ_READER_FIELDS()                                      \
    int          header_size;                                       \
    CvSeq*       seq;        /* sequence, beign read */             \
    CvSeqBlock*  block;      /* current block */                    \
    schar*       ptr;        /* pointer to element be read next */  \
    schar*       block_min;  /* pointer to the beginning of block */\
    schar*       block_max;  /* pointer to the end of block */      \
    int          delta_index;/* = seq->first->start_index   */      \
    schar*       prev_elem;  /* pointer to previous element */


typedef struct CvSeqReader
{
    CV_SEQ_READER_FIELDS()
}
CvSeqReader;

/****************************************************************************************/
/*                                Operations on sequences                               */
/****************************************************************************************/

#define  CV_SEQ_ELEM( seq, elem_type, index )                    \
/* assert gives some guarantee that <seq> parameter is valid */  \
(   assert(sizeof((seq)->first[0]) == sizeof(CvSeqBlock) &&      \
    (seq)->elem_size == sizeof(elem_type)),                      \
    (elem_type*)((seq)->first && (unsigned)index <               \
    (unsigned)((seq)->first->count) ?                            \
    (seq)->first->data + (index) * sizeof(elem_type) :           \
    cvGetSeqElem( (CvSeq*)(seq), (index) )))
#define CV_GET_SEQ_ELEM( elem_type, seq, index ) CV_SEQ_ELEM( (seq), elem_type, (index) )

/* Add element to sequence: */
#define CV_WRITE_SEQ_ELEM_VAR( elem_ptr, writer )     \
{                                                     \
    if( (writer).ptr >= (writer).block_max )          \
    {                                                 \
        cvCreateSeqBlock( &writer);                   \
    }                                                 \
    memcpy((writer).ptr, elem_ptr, (writer).seq->elem_size);\
    (writer).ptr += (writer).seq->elem_size;          \
}

#define CV_WRITE_SEQ_ELEM( elem, writer )             \
{                                                     \
    assert( (writer).seq->elem_size == sizeof(elem)); \
    if( (writer).ptr >= (writer).block_max )          \
    {                                                 \
        cvCreateSeqBlock( &writer);                   \
    }                                                 \
    assert( (writer).ptr <= (writer).block_max - sizeof(elem));\
    memcpy((writer).ptr, &(elem), sizeof(elem));      \
    (writer).ptr += sizeof(elem);                     \
}


/* Move reader position forward: */
#define CV_NEXT_SEQ_ELEM( elem_size, reader )                 \
{                                                             \
    if( ((reader).ptr += (elem_size)) >= (reader).block_max ) \
    {                                                         \
        cvChangeSeqBlock( &(reader), 1 );                     \
    }                                                         \
}


/* Move reader position backward: */
#define CV_PREV_SEQ_ELEM( elem_size, reader )                \
{                                                            \
    if( ((reader).ptr -= (elem_size)) < (reader).block_min ) \
    {                                                        \
        cvChangeSeqBlock( &(reader), -1 );                   \
    }                                                        \
}

/* Read element and move read position forward: */
#define CV_READ_SEQ_ELEM( elem, reader )                       \
{                                                              \
    assert( (reader).seq->elem_size == sizeof(elem));          \
    memcpy( &(elem), (reader).ptr, sizeof((elem)));            \
    CV_NEXT_SEQ_ELEM( sizeof(elem), reader )                   \
}

/* Read element and move read position backward: */
#define CV_REV_READ_SEQ_ELEM( elem, reader )                     \
{                                                                \
    assert( (reader).seq->elem_size == sizeof(elem));            \
    memcpy(&(elem), (reader).ptr, sizeof((elem)));               \
    CV_PREV_SEQ_ELEM( sizeof(elem), reader )                     \
}


#define CV_READ_CHAIN_POINT( _pt, reader )                              \
{                                                                       \
    (_pt) = (reader).pt;                                                \
    if( (reader).ptr )                                                  \
    {                                                                   \
        CV_READ_SEQ_ELEM( (reader).code, (reader));                     \
        assert( ((reader).code & ~7) == 0 );                            \
        (reader).pt.x += (reader).deltas[(int)(reader).code][0];        \
        (reader).pt.y += (reader).deltas[(int)(reader).code][1];        \
    }                                                                   \
}

#define CV_CURRENT_POINT( reader )  (*((CvPoint*)((reader).ptr)))
#define CV_PREV_POINT( reader )     (*((CvPoint*)((reader).prev_elem)))

#define CV_READ_EDGE( pt1, pt2, reader )               \
{                                                      \
    assert( sizeof(pt1) == sizeof(CvPoint) &&          \
            sizeof(pt2) == sizeof(CvPoint) &&          \
            reader.seq->elem_size == sizeof(CvPoint)); \
    (pt1) = CV_PREV_POINT( reader );                   \
    (pt2) = CV_CURRENT_POINT( reader );                \
    (reader).prev_elem = (reader).ptr;                 \
    CV_NEXT_SEQ_ELEM( sizeof(CvPoint), (reader));      \
}

/************ Graph macros ************/

/* Return next graph edge for given vertex: */
#define  CV_NEXT_GRAPH_EDGE( edge, vertex )                              \
     (assert((edge)->vtx[0] == (vertex) || (edge)->vtx[1] == (vertex)),  \
      (edge)->next[(edge)->vtx[1] == (vertex)])



/****************************************************************************************\
*             Data structures for persistence (a.k.a serialization) functionality        *
\****************************************************************************************/

/* "black box" file storage */
typedef struct CvFileStorage CvFileStorage;

/* Storage flags: */
#define CV_STORAGE_READ          0
#define CV_STORAGE_WRITE         1
#define CV_STORAGE_WRITE_TEXT    CV_STORAGE_WRITE
#define CV_STORAGE_WRITE_BINARY  CV_STORAGE_WRITE
#define CV_STORAGE_APPEND        2
#define CV_STORAGE_MEMORY        4
#define CV_STORAGE_FORMAT_MASK   (7<<3)
#define CV_STORAGE_FORMAT_AUTO   0
#define CV_STORAGE_FORMAT_XML    8
#define CV_STORAGE_FORMAT_YAML  16

/* List of attributes: */
typedef struct CvAttrList
{
    const char** attr;         /* NULL-terminated array of (attribute_name,attribute_value) pairs. */
    struct CvAttrList* next;   /* Pointer to next chunk of the attributes list.                    */
}
CvAttrList;

CV_INLINE CvAttrList cvAttrList( const char** attr CV_DEFAULT(NULL),
                                 CvAttrList* next CV_DEFAULT(NULL) )
{
    CvAttrList l;
    l.attr = attr;
    l.next = next;

    return l;
}

struct CvTypeInfo;

#define CV_NODE_NONE        0
#define CV_NODE_INT         1
#define CV_NODE_INTEGER     CV_NODE_INT
#define CV_NODE_REAL        2
#define CV_NODE_FLOAT       CV_NODE_REAL
#define CV_NODE_STR         3
#define CV_NODE_STRING      CV_NODE_STR
#define CV_NODE_REF         4 /* not used */
#define CV_NODE_SEQ         5
#define CV_NODE_MAP         6
#define CV_NODE_TYPE_MASK   7

#define CV_NODE_TYPE(flags)  ((flags) & CV_NODE_TYPE_MASK)

/* file node flags */
#define CV_NODE_FLOW        8 /* Used only for writing structures in YAML format. */
#define CV_NODE_USER        16
#define CV_NODE_EMPTY       32
#define CV_NODE_NAMED       64

#define CV_NODE_IS_INT(flags)        (CV_NODE_TYPE(flags) == CV_NODE_INT)
#define CV_NODE_IS_REAL(flags)       (CV_NODE_TYPE(flags) == CV_NODE_REAL)
#define CV_NODE_IS_STRING(flags)     (CV_NODE_TYPE(flags) == CV_NODE_STRING)
#define CV_NODE_IS_SEQ(flags)        (CV_NODE_TYPE(flags) == CV_NODE_SEQ)
#define CV_NODE_IS_MAP(flags)        (CV_NODE_TYPE(flags) == CV_NODE_MAP)
#define CV_NODE_IS_COLLECTION(flags) (CV_NODE_TYPE(flags) >= CV_NODE_SEQ)
#define CV_NODE_IS_FLOW(flags)       (((flags) & CV_NODE_FLOW) != 0)
#define CV_NODE_IS_EMPTY(flags)      (((flags) & CV_NODE_EMPTY) != 0)
#define CV_NODE_IS_USER(flags)       (((flags) & CV_NODE_USER) != 0)
#define CV_NODE_HAS_NAME(flags)      (((flags) & CV_NODE_NAMED) != 0)

#define CV_NODE_SEQ_SIMPLE 256
#define CV_NODE_SEQ_IS_SIMPLE(seq) (((seq)->flags & CV_NODE_SEQ_SIMPLE) != 0)

typedef struct CvString
{
    int len;
    char* ptr;
}
CvString;

/* All the keys (names) of elements in the readed file storage
   are stored in the hash to speed up the lookup operations: */
typedef struct CvStringHashNode
{
    unsigned hashval;
    CvString str;
    struct CvStringHashNode* next;
}
CvStringHashNode;

typedef struct CvGenericHash CvFileNodeHash;

/* Basic element of the file storage - scalar or collection: */
typedef struct CvFileNode
{
    int tag;
    struct CvTypeInfo* info; /* type information
            (only for user-defined object, for others it is 0) */
    union
    {
        double f; /* scalar floating-point number */
        int i;    /* scalar integer number */
        CvString str; /* text string */
        CvSeq* seq; /* sequence (ordered collection of file nodes) */
        CvFileNodeHash* map; /* map (collection of named file nodes) */
    } data;
}
CvFileNode;

#ifdef __cplusplus
extern "C" {
#endif
typedef int (CV_CDECL *CvIsInstanceFunc)( const void* struct_ptr );
typedef void (CV_CDECL *CvReleaseFunc)( void** struct_dblptr );
typedef void* (CV_CDECL *CvReadFunc)( CvFileStorage* storage, CvFileNode* node );
typedef void (CV_CDECL *CvWriteFunc)( CvFileStorage* storage, const char* name,
                                      const void* struct_ptr, CvAttrList attributes );
typedef void* (CV_CDECL *CvCloneFunc)( const void* struct_ptr );
#ifdef __cplusplus
}
#endif

typedef struct CvTypeInfo
{
    int flags;
    int header_size;
    struct CvTypeInfo* prev;
    struct CvTypeInfo* next;
    const char* type_name;
    CvIsInstanceFunc is_instance;
    CvReleaseFunc release;
    CvReadFunc read;
    CvWriteFunc write;
    CvCloneFunc clone;
}
CvTypeInfo;


/**** System data types ******/

typedef struct CvPluginFuncInfo
{
    void** func_addr;
    void* default_func_addr;
    const char* func_names;
    int search_modules;
    int loaded_from;
}
CvPluginFuncInfo;

typedef struct CvModuleInfo
{
    struct CvModuleInfo* next;
    const char* name;
    const char* version;
    CvPluginFuncInfo* func_tab;
}
CvModuleInfo;

/* Morphological operations */
enum
{
	CV_MOP_ERODE = 0,
	CV_MOP_DILATE = 1,
	CV_MOP_OPEN = 2,
	CV_MOP_CLOSE = 3,
	CV_MOP_GRADIENT = 4,
	CV_MOP_TOPHAT = 5,
	CV_MOP_BLACKHAT = 6
};

/* Sub-pixel interpolation methods */
enum
{
	CV_INTER_NN = 0,
	CV_INTER_LINEAR = 1,
	CV_INTER_CUBIC = 2,
	CV_INTER_AREA = 3,
	CV_INTER_LANCZOS4 = 4
};

/* ... and other image warping flags */
enum
{
	CV_WARP_FILL_OUTLIERS = 8,
	CV_WARP_INVERSE_MAP = 16
};

/* Threshold types */
enum
{
	CV_THRESH_BINARY = 0,  /* value = value > threshold ? max_value : 0       */
	CV_THRESH_BINARY_INV = 1,  /* value = value > threshold ? 0 : max_value       */
	CV_THRESH_TRUNC = 2,  /* value = value > threshold ? threshold : value   */
	CV_THRESH_TOZERO = 3,  /* value = value > threshold ? value : 0           */
	CV_THRESH_TOZERO_INV = 4,  /* value = value > threshold ? 0 : value           */
	CV_THRESH_MASK = 7,
	CV_THRESH_OTSU = 8  /* use Otsu algorithm to choose the optimal threshold value;
						combine the flag with one of the above CV_THRESH_* values */
};

/* Spatial and central moments */
typedef struct CvMoments
{
	double  m00, m10, m01, m20, m11, m02, m30, m21, m12, m03; /* spatial moments */
	double  mu20, mu11, mu02, mu30, mu21, mu12, mu03; /* central moments */
	double  inv_sqrt_m00; /* m00 != 0 ? 1/sqrt(m00) : 0 */
}
CvMoments;

/* Contour retrieval modes */
enum
{
	CV_RETR_EXTERNAL = 0,
	CV_RETR_LIST = 1,
	CV_RETR_CCOMP = 2,
	CV_RETR_TREE = 3,
	CV_RETR_FLOODFILL = 4
};

/* Contour approximation methods */
enum
{
	CV_CHAIN_CODE = 0,
	CV_CHAIN_APPROX_NONE = 1,
	CV_CHAIN_APPROX_SIMPLE = 2,
	CV_CHAIN_APPROX_TC89_L1 = 3,
	CV_CHAIN_APPROX_TC89_KCOS = 4,
	CV_LINK_RUNS = 5
};


/* Template matching methods */
enum
{
	CV_TM_SQDIFF = 0,
	CV_TM_SQDIFF_NORMED = 1,
	CV_TM_CCORR = 2,
	CV_TM_CCORR_NORMED = 3,
	CV_TM_CCOEFF = 4,
	CV_TM_CCOEFF_NORMED = 5
};

/* Filters used in pyramid decomposition */
enum
{
	CV_GAUSSIAN_5x5 = 7
};

/* Image smooth methods */
enum
{
	CV_BLUR_NO_SCALE = 0,
	CV_BLUR = 1,
	CV_GAUSSIAN = 2,
	CV_MEDIAN = 3,
	CV_BILATERAL = 4
};

/* Constants for color conversion */
enum
{
	CV_BGR2BGRA = 0,
	CV_RGB2RGBA = CV_BGR2BGRA,

	CV_BGRA2BGR = 1,
	CV_RGBA2RGB = CV_BGRA2BGR,

	CV_BGR2RGBA = 2,
	CV_RGB2BGRA = CV_BGR2RGBA,

	CV_RGBA2BGR = 3,
	CV_BGRA2RGB = CV_RGBA2BGR,

	CV_BGR2RGB = 4,
	CV_RGB2BGR = CV_BGR2RGB,

	CV_BGRA2RGBA = 5,
	CV_RGBA2BGRA = CV_BGRA2RGBA,

	CV_BGR2GRAY = 6,
	CV_RGB2GRAY = 7,
	CV_GRAY2BGR = 8,
	CV_GRAY2RGB = CV_GRAY2BGR,
	CV_GRAY2BGRA = 9,
	CV_GRAY2RGBA = CV_GRAY2BGRA,
	CV_BGRA2GRAY = 10,
	CV_RGBA2GRAY = 11,

	CV_BGR2BGR565 = 12,
	CV_RGB2BGR565 = 13,
	CV_BGR5652BGR = 14,
	CV_BGR5652RGB = 15,
	CV_BGRA2BGR565 = 16,
	CV_RGBA2BGR565 = 17,
	CV_BGR5652BGRA = 18,
	CV_BGR5652RGBA = 19,

	CV_GRAY2BGR565 = 20,
	CV_BGR5652GRAY = 21,

	CV_BGR2BGR555 = 22,
	CV_RGB2BGR555 = 23,
	CV_BGR5552BGR = 24,
	CV_BGR5552RGB = 25,
	CV_BGRA2BGR555 = 26,
	CV_RGBA2BGR555 = 27,
	CV_BGR5552BGRA = 28,
	CV_BGR5552RGBA = 29,

	CV_GRAY2BGR555 = 30,
	CV_BGR5552GRAY = 31,

	CV_BGR2XYZ = 32,
	CV_RGB2XYZ = 33,
	CV_XYZ2BGR = 34,
	CV_XYZ2RGB = 35,

	CV_BGR2YCrCb = 36,
	CV_RGB2YCrCb = 37,
	CV_YCrCb2BGR = 38,
	CV_YCrCb2RGB = 39,

	CV_BGR2HSV = 40,
	CV_RGB2HSV = 41,

	CV_BGR2Lab = 44,
	CV_RGB2Lab = 45,

	CV_BayerBG2BGR = 46,
	CV_BayerGB2BGR = 47,
	CV_BayerRG2BGR = 48,
	CV_BayerGR2BGR = 49,

	CV_BayerBG2RGB = CV_BayerRG2BGR,
	CV_BayerGB2RGB = CV_BayerGR2BGR,
	CV_BayerRG2RGB = CV_BayerBG2BGR,
	CV_BayerGR2RGB = CV_BayerGB2BGR,

	CV_BGR2Luv = 50,
	CV_RGB2Luv = 51,
	CV_BGR2HLS = 52,
	CV_RGB2HLS = 53,

	CV_HSV2BGR = 54,
	CV_HSV2RGB = 55,

	CV_Lab2BGR = 56,
	CV_Lab2RGB = 57,
	CV_Luv2BGR = 58,
	CV_Luv2RGB = 59,
	CV_HLS2BGR = 60,
	CV_HLS2RGB = 61,

	CV_BayerBG2BGR_VNG = 62,
	CV_BayerGB2BGR_VNG = 63,
	CV_BayerRG2BGR_VNG = 64,
	CV_BayerGR2BGR_VNG = 65,

	CV_BayerBG2RGB_VNG = CV_BayerRG2BGR_VNG,
	CV_BayerGB2RGB_VNG = CV_BayerGR2BGR_VNG,
	CV_BayerRG2RGB_VNG = CV_BayerBG2BGR_VNG,
	CV_BayerGR2RGB_VNG = CV_BayerGB2BGR_VNG,

	CV_BGR2HSV_FULL = 66,
	CV_RGB2HSV_FULL = 67,
	CV_BGR2HLS_FULL = 68,
	CV_RGB2HLS_FULL = 69,

	CV_HSV2BGR_FULL = 70,
	CV_HSV2RGB_FULL = 71,
	CV_HLS2BGR_FULL = 72,
	CV_HLS2RGB_FULL = 73,

	CV_LBGR2Lab = 74,
	CV_LRGB2Lab = 75,
	CV_LBGR2Luv = 76,
	CV_LRGB2Luv = 77,

	CV_Lab2LBGR = 78,
	CV_Lab2LRGB = 79,
	CV_Luv2LBGR = 80,
	CV_Luv2LRGB = 81,

	CV_BGR2YUV = 82,
	CV_RGB2YUV = 83,
	CV_YUV2BGR = 84,
	CV_YUV2RGB = 85,

	CV_BayerBG2GRAY = 86,
	CV_BayerGB2GRAY = 87,
	CV_BayerRG2GRAY = 88,
	CV_BayerGR2GRAY = 89,

	//YUV 4:2:0 formats family
	CV_YUV2RGB_NV12 = 90,
	CV_YUV2BGR_NV12 = 91,
	CV_YUV2RGB_NV21 = 92,
	CV_YUV2BGR_NV21 = 93,
	CV_YUV420sp2RGB = CV_YUV2RGB_NV21,
	CV_YUV420sp2BGR = CV_YUV2BGR_NV21,

	CV_YUV2RGBA_NV12 = 94,
	CV_YUV2BGRA_NV12 = 95,
	CV_YUV2RGBA_NV21 = 96,
	CV_YUV2BGRA_NV21 = 97,
	CV_YUV420sp2RGBA = CV_YUV2RGBA_NV21,
	CV_YUV420sp2BGRA = CV_YUV2BGRA_NV21,

	CV_YUV2RGB_YV12 = 98,
	CV_YUV2BGR_YV12 = 99,
	CV_YUV2RGB_IYUV = 100,
	CV_YUV2BGR_IYUV = 101,
	CV_YUV2RGB_I420 = CV_YUV2RGB_IYUV,
	CV_YUV2BGR_I420 = CV_YUV2BGR_IYUV,
	CV_YUV420p2RGB = CV_YUV2RGB_YV12,
	CV_YUV420p2BGR = CV_YUV2BGR_YV12,

	CV_YUV2RGBA_YV12 = 102,
	CV_YUV2BGRA_YV12 = 103,
	CV_YUV2RGBA_IYUV = 104,
	CV_YUV2BGRA_IYUV = 105,
	CV_YUV2RGBA_I420 = CV_YUV2RGBA_IYUV,
	CV_YUV2BGRA_I420 = CV_YUV2BGRA_IYUV,
	CV_YUV420p2RGBA = CV_YUV2RGBA_YV12,
	CV_YUV420p2BGRA = CV_YUV2BGRA_YV12,

	CV_YUV2GRAY_420 = 106,
	CV_YUV2GRAY_NV21 = CV_YUV2GRAY_420,
	CV_YUV2GRAY_NV12 = CV_YUV2GRAY_420,
	CV_YUV2GRAY_YV12 = CV_YUV2GRAY_420,
	CV_YUV2GRAY_IYUV = CV_YUV2GRAY_420,
	CV_YUV2GRAY_I420 = CV_YUV2GRAY_420,
	CV_YUV420sp2GRAY = CV_YUV2GRAY_420,
	CV_YUV420p2GRAY = CV_YUV2GRAY_420,

	//YUV 4:2:2 formats family
	CV_YUV2RGB_UYVY = 107,
	CV_YUV2BGR_UYVY = 108,
	//CV_YUV2RGB_VYUY = 109,
	//CV_YUV2BGR_VYUY = 110,
	CV_YUV2RGB_Y422 = CV_YUV2RGB_UYVY,
	CV_YUV2BGR_Y422 = CV_YUV2BGR_UYVY,
	CV_YUV2RGB_UYNV = CV_YUV2RGB_UYVY,
	CV_YUV2BGR_UYNV = CV_YUV2BGR_UYVY,

	CV_YUV2RGBA_UYVY = 111,
	CV_YUV2BGRA_UYVY = 112,
	//CV_YUV2RGBA_VYUY = 113,
	//CV_YUV2BGRA_VYUY = 114,
	CV_YUV2RGBA_Y422 = CV_YUV2RGBA_UYVY,
	CV_YUV2BGRA_Y422 = CV_YUV2BGRA_UYVY,
	CV_YUV2RGBA_UYNV = CV_YUV2RGBA_UYVY,
	CV_YUV2BGRA_UYNV = CV_YUV2BGRA_UYVY,

	CV_YUV2RGB_YUY2 = 115,
	CV_YUV2BGR_YUY2 = 116,
	CV_YUV2RGB_YVYU = 117,
	CV_YUV2BGR_YVYU = 118,
	CV_YUV2RGB_YUYV = CV_YUV2RGB_YUY2,
	CV_YUV2BGR_YUYV = CV_YUV2BGR_YUY2,
	CV_YUV2RGB_YUNV = CV_YUV2RGB_YUY2,
	CV_YUV2BGR_YUNV = CV_YUV2BGR_YUY2,

	CV_YUV2RGBA_YUY2 = 119,
	CV_YUV2BGRA_YUY2 = 120,
	CV_YUV2RGBA_YVYU = 121,
	CV_YUV2BGRA_YVYU = 122,
	CV_YUV2RGBA_YUYV = CV_YUV2RGBA_YUY2,
	CV_YUV2BGRA_YUYV = CV_YUV2BGRA_YUY2,
	CV_YUV2RGBA_YUNV = CV_YUV2RGBA_YUY2,
	CV_YUV2BGRA_YUNV = CV_YUV2BGRA_YUY2,

	CV_YUV2GRAY_UYVY = 123,
	CV_YUV2GRAY_YUY2 = 124,
	//CV_YUV2GRAY_VYUY = CV_YUV2GRAY_UYVY,
	CV_YUV2GRAY_Y422 = CV_YUV2GRAY_UYVY,
	CV_YUV2GRAY_UYNV = CV_YUV2GRAY_UYVY,
	CV_YUV2GRAY_YVYU = CV_YUV2GRAY_YUY2,
	CV_YUV2GRAY_YUYV = CV_YUV2GRAY_YUY2,
	CV_YUV2GRAY_YUNV = CV_YUV2GRAY_YUY2,

	// alpha premultiplication
	CV_RGBA2mRGBA = 125,
	CV_mRGBA2RGBA = 126,

	CV_RGB2YUV_I420 = 127,
	CV_BGR2YUV_I420 = 128,
	CV_RGB2YUV_IYUV = CV_RGB2YUV_I420,
	CV_BGR2YUV_IYUV = CV_BGR2YUV_I420,

	CV_RGBA2YUV_I420 = 129,
	CV_BGRA2YUV_I420 = 130,
	CV_RGBA2YUV_IYUV = CV_RGBA2YUV_I420,
	CV_BGRA2YUV_IYUV = CV_BGRA2YUV_I420,
	CV_RGB2YUV_YV12 = 131,
	CV_BGR2YUV_YV12 = 132,
	CV_RGBA2YUV_YV12 = 133,
	CV_BGRA2YUV_YV12 = 134,

	CV_COLORCVT_MAX = 135
};

/* Shape orientation */
enum
{
	CV_CLOCKWISE = 1,
	CV_COUNTER_CLOCKWISE = 2
};

/****************************************************************************************\
*                              Planar subdivisions                                       *
\****************************************************************************************/

typedef size_t CvSubdiv2DEdge;

#define CV_QUADEDGE2D_FIELDS()     \
	int flags;                     \
struct CvSubdiv2DPoint* pt[4]; \
	CvSubdiv2DEdge  next[4];

#define CV_SUBDIV2D_POINT_FIELDS()\
	int            flags;      \
	CvSubdiv2DEdge first;      \
	CvPoint2D32f   pt;         \
	int id;

#define CV_SUBDIV2D_VIRTUAL_POINT_FLAG (1 << 30)

typedef struct CvQuadEdge2D
{
	CV_QUADEDGE2D_FIELDS()
}
CvQuadEdge2D;

typedef struct CvSubdiv2DPoint
{
	CV_SUBDIV2D_POINT_FIELDS()
}
CvSubdiv2DPoint;

#define CV_SUBDIV2D_FIELDS()    \
	CV_GRAPH_FIELDS()           \
	int  quad_edges;            \
	int  is_geometry_valid;     \
	CvSubdiv2DEdge recent_edge; \
	CvPoint2D32f  topleft;      \
	CvPoint2D32f  bottomright;

typedef struct CvSubdiv2D
{
	CV_SUBDIV2D_FIELDS()
}
CvSubdiv2D;


typedef enum CvSubdiv2DPointLocation
{
	CV_PTLOC_ERROR = -2,
	CV_PTLOC_OUTSIDE_RECT = -1,
	CV_PTLOC_INSIDE = 0,
	CV_PTLOC_VERTEX = 1,
	CV_PTLOC_ON_EDGE = 2
}
CvSubdiv2DPointLocation;

typedef enum CvNextEdgeType
{
	CV_NEXT_AROUND_ORG = 0x00,
	CV_NEXT_AROUND_DST = 0x22,
	CV_PREV_AROUND_ORG = 0x11,
	CV_PREV_AROUND_DST = 0x33,
	CV_NEXT_AROUND_LEFT = 0x13,
	CV_NEXT_AROUND_RIGHT = 0x31,
	CV_PREV_AROUND_LEFT = 0x20,
	CV_PREV_AROUND_RIGHT = 0x02
}
CvNextEdgeType;

/* get the next edge with the same origin point (counterwise) */
#define  CV_SUBDIV2D_NEXT_EDGE( edge )  (((CvQuadEdge2D*)((edge) & ~3))->next[(edge)&3])


/* Contour approximation algorithms */
enum
{
	CV_POLY_APPROX_DP = 0
};

/* Shape matching methods */
enum
{
	CV_CONTOURS_MATCH_I1 = 1,
	CV_CONTOURS_MATCH_I2 = 2,
	CV_CONTOURS_MATCH_I3 = 3
};




/* Convexity defect */
typedef struct CvConvexityDefect
{
	CvPoint* start; /* point of the contour where the defect begins */
	CvPoint* end; /* point of the contour where the defect ends */
	CvPoint* depth_point; /* the farthest from the convex hull point within the defect */
	float depth; /* distance between the farthest point and the convex hull */
} CvConvexityDefect;

/*
Internal structure that is used for sequental retrieving contours from the image.
It supports both hierarchical and plane variants of Suzuki algorithm.
*/
typedef struct _CvContourScanner* CvContourScanner;

/* Freeman chain reader state */
typedef struct CvChainPtReader
{
	CV_SEQ_READER_FIELDS()
	char      code;
	CvPoint   pt;
	schar     deltas[8][2];
}
CvChainPtReader;



#endif /*__OPENCV_CORE_TYPES_H__*/

/* End of file. */
