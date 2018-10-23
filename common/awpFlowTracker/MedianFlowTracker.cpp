/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*
 * MedianFlowTracker.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */
#include "stdafx.h"
#include "MedianFlowTracker.h"

#include <cmath>

#include "FBTrack.h"

using namespace cv;

namespace tld
{

MedianFlowTracker::MedianFlowTracker()
{
    trackerBB = NULL;
}

MedianFlowTracker::~MedianFlowTracker()
{
    cleanPreviousData();
}

void MedianFlowTracker::cleanPreviousData()
{
    delete trackerBB;
    trackerBB = NULL;
}

void MedianFlowTracker::track(const Mat &prevMat, const Mat &currMat, Rect *prevBB)
{
    if(prevBB != NULL)
    {
        if(prevBB->width <= 0 || prevBB->height <= 0)
        {
            return;
        }

        float bb_tracker[] = {prevBB->x, prevBB->y, prevBB->width + prevBB->x - 1, prevBB->height + prevBB->y - 1};
        float scale;

        IplImage prevImg = prevMat;
        IplImage currImg = currMat;

        int success = fbtrack(&prevImg, &currImg, bb_tracker, bb_tracker, &scale);

        //Extract subimage
        float x, y, w, h;
        x = floor(bb_tracker[0] + 0.5);
        y = floor(bb_tracker[1] + 0.5);
        w = floor(bb_tracker[2] - bb_tracker[0] + 1 + 0.5);
        h = floor(bb_tracker[3] - bb_tracker[1] + 1 + 0.5);

        //TODO: Introduce a check for a minimum size
        if(!success || x < 0 || y < 0 || w <= 0 || h <= 0 || x + w > currMat.cols || y + h > currMat.rows || x != x || y != y || w != w || h != h) //x!=x is check for nan
        {
            //Leave it empty
        }
        else
        {
            trackerBB = new Rect(x, y, w, h);
        }
    }
}


MedianFlowMultiTracker::MedianFlowMultiTracker()
{
	trackerBB = NULL;
	m_numRects = 0;
	m_status  = NULL;
	m_id = NULL;
}

MedianFlowMultiTracker::~MedianFlowMultiTracker()
{
	cleanPreviousData();
}
void MedianFlowMultiTracker::cleanPreviousData()
{
	delete[]  trackerBB;
	delete[]  m_id;
	delete[]  m_status;
	m_numRects = 0;
}

void MedianFlowMultiTracker::track(const Mat &prevMat, const Mat &currMat, cv::Rect *prevBB, UUID* id, int* status, int numRects)
{
	cleanPreviousData();
	
	if (numRects > 0)
	{
		
		m_id		= new UUID[numRects];
		m_status    = new int[numRects];
		trackerBB	= new cv::Rect[numRects];	

		float* bb_tracker = new float[4*numRects];
		float* scale      = new float[numRects];
		
		for (int j = 0; j < numRects; j++)
		{
			bb_tracker[4*j] = prevBB[j].x;
			bb_tracker[4*j+1] = prevBB[j].y;
			bb_tracker[4*j+2] = prevBB[j].width + prevBB[j].x - 1;
			bb_tracker[4*j+3] = prevBB[j].height + prevBB[j].y - 1;

			m_id[j] = id[j];
			m_status[j] = 1;
		}

        IplImage prevImg = prevMat;
        IplImage currImg = currMat;

		int success = fbmultitrack(&prevImg, &currImg, bb_tracker, bb_tracker, scale, numRects);

		for (int j = 0; j < numRects; j++)
		{
			float x, y, w, h;
			x = floor(bb_tracker[4*j] + 0.5);
			y = floor(bb_tracker[4*j+ 1] + 0.5);
			w = floor(bb_tracker[4*j + 2] - bb_tracker[4*j] + 1 + 0.5);
			h = floor(bb_tracker[4*j + 3] - bb_tracker[4*j + 1] + 1 + 0.5);	
	
			if(x < 0 || y < 0 || w <= 0 || h <= 0 || x + w > currMat.cols || y + h > currMat.rows || x != x || y != y || w != w || h != h) 
			{
				m_status[j] = 0;
			}
			else
			{
				trackerBB[j].x = x;
				trackerBB[j].y = y;
				trackerBB[j].width = w;
				trackerBB[j].height = h;
			}
		}

		delete[] bb_tracker;
		delete[] scale;
	}
}
} /* namespace tld */


