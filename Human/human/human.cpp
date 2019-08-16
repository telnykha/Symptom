// human.cpp : Defines the exported functions for the DLL application.
//
#include "awpipl.h"
#include "va_common.h"
#include "human.h"
#include "vautils.h"

#include "opencv2\dnn.hpp"
#include <opencv2\imgproc.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace cv;
using namespace dnn;
using namespace std;

extern "C"
{
#ifdef _DEBUG 
	#pragma comment(lib, "C:\\_alt\\_proj\\_thridparty\\opencv342\\opencv\\build\\x64\\vc14\\lib\\opencv_world342d.lib")
#else
	#pragma comment(lib, "C:\\_alt\\_proj\\_thridparty\\opencv342\\opencv\\build\\x64\\vc14\\lib\\opencv_world342.lib")
#endif 
}


typedef struct 
{
	int size;
	Net net;
	int inpWidth;
	int inpHeight;
	float confThreshold;
	float nmsThreshold;
}TheHuman;


HUMAN_API HANDLE   humanCreate(TVAInitParams* params, bool use_gpu)
{
	String modelConfiguration = "yolov3-tiny.cfg";
	String modelWeights = "yolov3-tiny.weights";

	TheHuman* p = new TheHuman();
	p->size = sizeof(TheHuman);
	p->inpWidth  = 416;
	p->inpHeight = 416;
	p->confThreshold = (float)params->EventSens;
	p->nmsThreshold = 0.4f;
	p->net = readNetFromDarknet(modelConfiguration, modelWeights);
	p->net.setPreferableBackend(DNN_BACKEND_OPENCV);
	if (!use_gpu)
		p->net.setPreferableTarget(DNN_TARGET_CPU);
	else
	{
		//todo: use darknet
		delete p;
		return NULL;
	}

	return (HANDLE)p;
}

vector<String> getOutputsNames(const Net& net)
{
	static vector<String> names;
	if (names.empty())
	{
		//Get the indices of the output layers, i.e. the layers with unconnected outputs
		vector<int> outLayers = net.getUnconnectedOutLayers();

		//get the names of all the layers in the network
		vector<String> layersNames = net.getLayerNames();

		// Get the names of the output layers in names
		names.resize(outLayers.size());
		for (size_t i = 0; i < outLayers.size(); ++i)
			names[i] = layersNames[outLayers[i] - 1];
	}
	return names;

}

// Remove the bounding boxes with low confidence using non-maxima suppression
void postprocess(Mat& frame, const vector<Mat>& outs, TheHuman* p, TVAResult* result)
{
	vector<int> classIds;
	vector<float> confidences;
	vector<Rect> boxes;

	for (size_t i = 0; i < outs.size(); ++i)
	{
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)outs[i].data;
		for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
		{
			Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
			Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > p->confThreshold)
			{
				int centerX = (int)(data[0] * frame.cols);
				int centerY = (int)(data[1] * frame.rows);
				int width = (int)(data[2] * frame.cols);
				int height = (int)(data[3] * frame.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				classIds.push_back(classIdPoint.x);
				confidences.push_back((float)confidence);
				boxes.push_back(Rect(left, top, width, height));
			}
		}
	}

	// Perform non maximum suppression to eliminate redundant overlapping boxes with
	// lower confidences
	vector<int> indices;
	int c = 0;
	NMSBoxes(boxes, confidences, p->confThreshold, p->nmsThreshold, indices);
	for (size_t i = 0; i < indices.size(); ++i)
	{
		int idx = indices[i];
		Rect box = boxes[idx];
		if (classIds[idx] == 0)
		{	
			UuidCreate(&result->blobs[c].id);
			result->blobs[c].XPos = box.x;
			result->blobs[c].YPos = box.y;
			result->blobs[c].Width = box.width;
			result->blobs[c].Height = box.height;
			c++;
			result->Num = c;
		}
	}
}

HUMAN_API HRESULT  humanProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAResult* result)
{
	TheHuman* p = (TheHuman*)hModule;
	if (p->size != sizeof(TheHuman))
		return E_FAIL;
	IplImage* img = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
	img->imageData = (char*)data;
	Mat frame = cv::cvarrToMat(img, true); 
	Mat blob; 
	blobFromImage(frame, blob, 1 / 255.0, cvSize(p->inpWidth, p->inpHeight), Scalar(0, 0, 0), true, false);
	p->net.setInput(blob);
	vector<Mat> outs;
	p->net.forward(outs, getOutputsNames(p->net));
	result->Num = 0;
	postprocess(frame, outs, p, result);
	return S_OK;
}
HUMAN_API HRESULT  humanRelease(HANDLE* hModule)
{
	TheHuman* p = (TheHuman*)hModule;
	if (p->size != sizeof(TheHuman))
		return E_FAIL;

	delete p;
	return S_OK; 
}