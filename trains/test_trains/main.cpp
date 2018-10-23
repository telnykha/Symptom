#include "stdio.h"
#include "_LF.h"
#include "trains.h"
#include "..\trains\LFTrainsModule.h"

TRAINSHANDLE h = NULL;
TLFTrains*   hc = NULL;
awpColor dg0 = {0,0,0};
awpColor dg1 = { 255, 255, 255 };
awpColor dg2 = { 255, 0, 0 };
awpColor dg3 = { 0, 255, 0 };
awpColor dg4 = { 0, 0, 255 };
awpColor dg5 = { 255, 255, 0 };
awpColor dg6 = { 0, 255, 255 };
awpColor dg7 = { 255, 0, 255 };
awpColor dg8 = { 0, 0, 128 };
awpColor dg9 = { 128, 0, 0 };

static awpColor GetDigitColor(const char* lpDigit)
{
	if (strcmp(lpDigit, "0") == 0)
		return dg0;
	if (strcmp(lpDigit, "1") == 0)
		return dg1;
	if (strcmp(lpDigit, "2") == 0)
		return dg2;
	if (strcmp(lpDigit, "3") == 0)
		return dg3;
	if (strcmp(lpDigit, "4") == 0)
		return dg4;
	if (strcmp(lpDigit, "5") == 0)
		return dg5;
	if (strcmp(lpDigit, "6") == 0)
		return dg6;
	if (strcmp(lpDigit, "7") == 0)
		return dg7;
	if (strcmp(lpDigit, "8") == 0)
		return dg8;
	if (strcmp(lpDigit, "9") == 0)
		return dg9;

	return dg0;
}

const char* c_digits[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

awpImage* DrawClusters(awpImage* img)
{
	awpImage* result = NULL;
	awpCreateImage(&result, img->sSizeX, img->sSizeY, 3, AWP_BYTE);
	awpFill(result, 128);
	TLFSemanticImageDescriptor* d = hc->GetRawSescriptor();
	for (int i = 0; i < d->GetCount(); i++)
	{
		TLFDetectedItem* di = d->GetDetectedItem(i);
		TLFRect* r = di->GetBounds();
		awpPoint p = r->Center();
		awpRect rect = r->GetRect();
		awpColor c = GetDigitColor(di->GetType().c_str());
		//awpDrawCRect(result, &rect, c.bBlue, c.bGreen, c.bRed, 1);
		awpDrawCPoint(result, p, c.bBlue, c.bGreen, c.bRed, 4);
	}
	for (int i = 0; i < 10; i++)
	{
		awpRect rect;
		rect.left = i * 24;
		rect.right = rect.left + 24;
		rect.top = 70;
		rect.bottom = 78;
		string str = c_digits[i];
		awpColor c = GetDigitColor(str.c_str());
		awpFillCRect(result, &rect, c.bBlue, c.bGreen, c.bRed);
	}
	awpImage* tmp = NULL;
	awpImage* tmp1 = NULL;
	awpCreateImage(&tmp, img->sSizeX, img->sSizeY, 3, AWP_BYTE);
	awpPutChannel(tmp, img, 0);
	awpPutChannel(tmp, img, 1);
	awpPutChannel(tmp, img, 2);
	awpCreateImage(&tmp1, img->sSizeX, 2*img->sSizeY, 3, AWP_BYTE);
	awpPoint pos;
	pos.X = 0;
	pos.Y = 0;
	awpPasteRect(tmp, tmp1, pos);
	pos.X = 0;
	pos.Y = 78;
	awpPasteRect(result, tmp1, pos);
	awpReleaseImage(&tmp);
	awpReleaseImage(&result);

	return tmp1;
}

static void Print_Debug_Log(const char* lpFileName)
{

	if (!LFDirExist("cluster"))
		LFCreateDir("cluster");
	string strFileNameCluster = "cluster\\";

	strFileNameCluster += LFGetFileName(lpFileName);
	strFileNameCluster += ".jpg";

	TLFImage* img = hc->GetOCRImage();
	if (img != NULL)
	{
		awpImage* result = DrawClusters(img->GetImage());
		if (result != NULL)
		{
			awpSaveImage(strFileNameCluster.c_str(), result);
			awpReleaseImage(&result);
		}
	}
}

int Usage()
{
	printf("Usage: test_trains key filename \n");
	printf("key = i - test image file with jpg, awp extention. \n");
	printf("key = t - test file list in the txt file with format: image_name result. \n");
	printf("key = d - test image files in the directory. \n");

	return -1;
}

int testFileList(const char* lpFileName)
{
	
	FILE* f = fopen(lpFileName, "r+t");
	if (f == NULL)
	{
		printf("Cannot open file:%s\n", lpFileName);
		return -1;
	}
	char name[1024];
	char result[10];
	char num[10];
	int success = 0;
	int count = 0;
	int notfound = 0;
	std::string ts = "58243957";
	while (fscanf(f, "%s\t%s\n", name, num) != EOF)
	{
		TLFImage image;
		if (!image.LoadImage0(name))
		{
			printf("ERROR: cannot load image.\n");
			continue;
		}

		std::string path = LFGetFilePath(name);
		path += "\\train_test_err\\";
		if (!LFDirExist(path.c_str()))
			LFCreateDir(path.c_str());

		count++;
		TVARect obj_box;
		memset(result, 0, sizeof(result));
		unsigned long t = LFGetTickCount();
		if (trainsProcess(h, (unsigned char*)image.GetImage()->pPixels, image.GetImage()->sSizeX,
			image.GetImage()->sSizeY, image.GetImage()->bChannels, result, obj_box) > 0)
		{
			if (strcmp(num, result) == 0)
				success++;
			else
			{
				std::string fname = path + LFGetFileName(name);
				fname += ".awp";
				image.SaveImage(fname.c_str());
			//	printf("%s\n", fname.c_str());
			}
			printf("%i\t%i\t%i\t%i\t%s\t%s\n", count, notfound, success, LFGetTickCount() - t, result, ts.c_str());
		}
		else
		{
			std::string fname = path + LFGetFileName(name);
			fname += ".awp";
			image.SaveImage(fname.c_str());
			printf("there is not number on this image.\n");
			notfound++;
		}
	}
	if (count != 0)
	{
		printf("success = %f %\n", 100*(float)success/(float)count);
	}
	else
	{	
		printf("There are not images to test.\n");
		return -1;
	}
	fclose(f);

	return 0;
}

int testImage(const char* lpFileName)
{
	TLFImage image;
	if (!image.LoadImage0(lpFileName))
	{
		printf("ERROR: cannot load image.\n");
		return -1;
	}

	TVARect obj_box;
	char result[10];
	memset(result, 0, sizeof(result));
	unsigned long t = LFGetTickCount();
	if (trainsProcess(h, (unsigned char*)image.GetImage()->pPixels, image.GetImage()->sSizeX,
		image.GetImage()->sSizeY, image.GetImage()->bChannels, result, obj_box) > 0)
	{
		
		printf("%s\t%i\n", result, LFGetTickCount() - t);
	}
	else
	{
		printf("there is not number on this image.\n");
	}


	return 0;
}

int testDirectory(const char* lpDir)
{
	TLFDBLabeledImages db;
	if (!db.InitDB(lpDir))
		return -1;
	int count = 0;
	int total = 0;
	for (int i = 0; i < db.GetImagesCount(); i++)
	{
		TLFDBSemanticDescriptor* d = db.GetDescriptor(i);
		if (d == NULL)
			continue;
		TLFDetectedItem* di = d->GetDetectedItem(0);
		if (di == NULL)
			continue;

		const char* lpImageFileName = d->GetImageFile();
		TLFImage image;
		if (!image.LoadImage0(lpImageFileName))
			continue;

		TVARect obj_box;
		char result[10];
		memset(result, 0, sizeof(result));
		unsigned long t = LFGetTickCount();
		int res = trainsProcess(h, (unsigned char*)image.GetImage()->pPixels, image.GetImage()->sSizeX,
			image.GetImage()->sSizeY, image.GetImage()->bChannels, result, obj_box);
		if (res == 1)
			continue;
		string comment = di->GetComment();
		string sresult = result;
		int flag = 0;
		if (comment == "")
			continue;
		if (strcmp(result, comment.c_str()) == 0)
		{
			flag = 1;
			count++;
		}
		else
		{
			Print_Debug_Log(lpImageFileName);
			printf("%s\t%i\t%s\t%s\t%i\t%i\n", lpImageFileName, res, result, comment.c_str(), flag, LFGetTickCount() - t);
		}

		total++;
	}
	printf("SUCCESS = %f\n", 100.*(float)count/(float)total);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
		return Usage();


	TVARect roi;
	roi.LeftTop.X = 5;
	roi.LeftTop.Y =5;
	roi.RightBottom.X = 95;
	roi.RightBottom.Y = 95;
	h = trainsInit(roi, 1, 1);
	if (h == NULL)
	{
		printf("ERROR: cannot init OCR engine.\n");
		return -1;
	}
	hc = (TLFTrains*)h;
	std::string key = argv[1];
	int res = 0;
	if (key == "i")
		res = testImage(argv[2]);
	else if (key == "t")
		res = testFileList(argv[2]);
	else if (key == "d")
		res = testDirectory(argv[2]);
	else
	{
		Usage();
		res = -1;
	}
	trainsClose(h);
	return res;
}