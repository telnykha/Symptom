// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>



// TODO: reference additional headers your program requires here
#ifdef HAVE_CVCONFIG_H
#include "cvconfig.h"
#endif

#include "core.hpp"
#include "core_c.h"
#include "internal.hpp"
#include "operations.hpp"


#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef HAVE_TEGRA_OPTIMIZATION
#include "opencv2/core/core_tegra.hpp"
#else
#define GET_OPTIMIZED(func) (func)
#endif

namespace cv
{

	// -128.f ... 255.f
	extern const float g_8x32fTab[];
#define CV_8TO32F(x)  cv::g_8x32fTab[(x)+128]

	extern const ushort g_8x16uSqrTab[];
#define CV_SQR_8U(x)  cv::g_8x16uSqrTab[(x)+255]

	extern const char* g_HersheyGlyphs[];

	extern const uchar g_Saturate8u[];
#define CV_FAST_CAST_8U(t)   (assert(-256 <= (t) && (t) <= 512), cv::g_Saturate8u[(t)+256])
#define CV_MIN_8U(a,b)       ((a) - CV_FAST_CAST_8U((a) - (b)))
#define CV_MAX_8U(a,b)       ((a) + CV_FAST_CAST_8U((b) - (a)))


#if defined WIN32 || defined _WIN32
	void deleteThreadAllocData();
	void deleteThreadRNGData();
#endif

	template<typename T1, typename T2 = T1, typename T3 = T1> struct OpAdd
	{
		typedef T1 type1;
		typedef T2 type2;
		typedef T3 rtype;
		T3 operator ()(const T1 a, const T2 b) const { return saturate_cast<T3>(a + b); }
	};

	template<typename T1, typename T2 = T1, typename T3 = T1> struct OpSub
	{
		typedef T1 type1;
		typedef T2 type2;
		typedef T3 rtype;
		T3 operator ()(const T1 a, const T2 b) const { return saturate_cast<T3>(a - b); }
	};

	template<typename T1, typename T2 = T1, typename T3 = T1> struct OpRSub
	{
		typedef T1 type1;
		typedef T2 type2;
		typedef T3 rtype;
		T3 operator ()(const T1 a, const T2 b) const { return saturate_cast<T3>(b - a); }
	};

	template<typename T> struct OpMin
	{
		typedef T type1;
		typedef T type2;
		typedef T rtype;
		T operator ()(const T a, const T b) const { return std::min(a, b); }
	};

	template<typename T> struct OpMax
	{
		typedef T type1;
		typedef T type2;
		typedef T rtype;
		T operator ()(const T a, const T b) const { return std::max(a, b); }
	};

	inline Size getContinuousSize(const Mat& m1, int widthScale = 1)
	{
		return m1.isContinuous() ? Size(m1.cols*m1.rows*widthScale, 1) :
			Size(m1.cols*widthScale, m1.rows);
	}

	inline Size getContinuousSize(const Mat& m1, const Mat& m2, int widthScale = 1)
	{
		return (m1.flags & m2.flags & Mat::CONTINUOUS_FLAG) != 0 ?
			Size(m1.cols*m1.rows*widthScale, 1) : Size(m1.cols*widthScale, m1.rows);
	}

	inline Size getContinuousSize(const Mat& m1, const Mat& m2,
		const Mat& m3, int widthScale = 1)
	{
		return (m1.flags & m2.flags & m3.flags & Mat::CONTINUOUS_FLAG) != 0 ?
			Size(m1.cols*m1.rows*widthScale, 1) : Size(m1.cols*widthScale, m1.rows);
	}

	inline Size getContinuousSize(const Mat& m1, const Mat& m2,
		const Mat& m3, const Mat& m4,
		int widthScale = 1)
	{
		return (m1.flags & m2.flags & m3.flags & m4.flags & Mat::CONTINUOUS_FLAG) != 0 ?
			Size(m1.cols*m1.rows*widthScale, 1) : Size(m1.cols*widthScale, m1.rows);
	}

	inline Size getContinuousSize(const Mat& m1, const Mat& m2,
		const Mat& m3, const Mat& m4,
		const Mat& m5, int widthScale = 1)
	{
		return (m1.flags & m2.flags & m3.flags & m4.flags & m5.flags & Mat::CONTINUOUS_FLAG) != 0 ?
			Size(m1.cols*m1.rows*widthScale, 1) : Size(m1.cols*widthScale, m1.rows);
	}

	struct NoVec
	{
		size_t operator()(const void*, const void*, void*, size_t) const { return 0; }
	};

	extern volatile bool USE_SSE2;
	extern volatile bool USE_SSE4_2;
	extern volatile bool USE_AVX;

	enum { BLOCK_SIZE = 1024 };

#ifdef HAVE_IPP
	static inline IppiSize ippiSize(int width, int height) { IppiSize sz = { width, height }; return sz; }
	static inline IppiSize ippiSize(Size _sz)              { IppiSize sz = { _sz.width, _sz.height }; return sz; }
#endif

#if defined HAVE_IPP && (IPP_VERSION_MAJOR >= 7)
#define ARITHM_USE_IPP 1
#define IF_IPP(then_call, else_call) then_call
#else
#define ARITHM_USE_IPP 0
#define IF_IPP(then_call, else_call) else_call
#endif

	inline bool checkScalar(const Mat& sc, int atype, int sckind, int akind)
	{
		if (sc.dims > 2 || (sc.cols != 1 && sc.rows != 1) || !sc.isContinuous())
			return false;
		int cn = CV_MAT_CN(atype);
		if (akind == _InputArray::MATX && sckind != _InputArray::MATX)
			return false;
		return sc.size() == Size(1, 1) || sc.size() == Size(1, cn) || sc.size() == Size(cn, 1) ||
			(sc.size() == Size(1, 4) && sc.type() == CV_64F && cn <= 4);
	}

	void convertAndUnrollScalar(const Mat& sc, int buftype, uchar* scbuf, size_t blocksize);

}


enum {
	BORDER_REPLICATE = IPL_BORDER_REPLICATE, BORDER_CONSTANT = IPL_BORDER_CONSTANT,
	BORDER_REFLECT = IPL_BORDER_REFLECT, BORDER_WRAP = IPL_BORDER_WRAP,
	BORDER_REFLECT_101 = IPL_BORDER_REFLECT_101, BORDER_REFLECT101 = BORDER_REFLECT_101,
	BORDER_TRANSPARENT = IPL_BORDER_TRANSPARENT,
	BORDER_DEFAULT = BORDER_REFLECT_101, BORDER_ISOLATED = 16
};