#include "stdio.h"
#include "_LF.h"
int main(int argc, char* argv[])
{
	awpImage* image = NULL;
	AWPRESULT res = AWP_OK;
	res = awpCreateImage(&image, 216, 72, 3, AWP_BYTE);
	if (res != AWP_OK)
	{
		printf("ERROR: Cannot create image.\n");
		_AWP_SAFE_RELEASE_(image)
		return -1;
	}
	TLFSemanticImageDescriptor average;
	average.SetImage(image);


	for (int i = 0; i < 8; i++)
	{
		awpRect rect;
		rect.left = 0;
		rect.right = 12;
		rect.top = 0;
		rect.bottom = 24;
		UUID id;
		LF_UUID_CREATE(id);
		TLFDetectedItem* item = new TLFDetectedItem(&rect, 0, "0", 0, 0, 12,24, "adigit.exe", id);
		average.AddDetectedItem(item);
	}
	TLFDBLabeledImages db("");
	db.InitDB("");
	printf("database images count = %i\n", db.GetImagesCount());
	printf("database items count = %i\n", db.GetItemsCount());
	if (db.GetImagesCount() == 0 || db.GetItemsCount() == 0)
	{
		printf("ERROR: empty database.\n");
		_AWP_SAFE_RELEASE_(image)
		return -1;
	}
	int count = 0;
	for (int i = 0; i < db.GetImagesCount(); i++)
	{
		TLFSemanticImageDescriptor* d = db.GetDescriptor(i);
		printf("db_entry %i num items %i\n", i, d->GetCount());
		if (d->GetCount() == 8)
		{
			for (int j = 0; j < 8; j++)
			{
				TLFDetectedItem* item = d->GetDetectedItem(j);
				TLFDetectedItem* si = average.GetDetectedItem(j);
				TLFRect* sb = si->GetBounds();
				TLFRect* bounds = item->GetBounds();
				awpRect  r = bounds->GetRect();
				awpRect  sr = sb->GetRect();
				sr.left += r.left;
				sr.top += r.top;
				sr.right += r.right;
				sr.bottom += r.bottom;
				if (i > 0)
				{
					sr.left /= 2;
					sr.top /= 2;
					sr.right /= 2;
					sr.bottom /= 2;
				}
				si->SetBounds(sr);
			}
			count++;
		}
	}
	printf("count = %i\n", count);
	if (count == 0)
	{
		printf("ERROR: the number of suitable elements is zero \n ");
		_AWP_SAFE_RELEASE_(image)
		return -1;
	}

	//for (int i = 0; i < 8; i++)
	//{
	//	TLFDetectedItem* si = average.GetDetectedItem(i);
	//	TLFRect* sb = si->GetBounds();
	//	awpRect  sr = sb->GetRect();
	//	sr.left /= count;
	//	sr.right /= count;
	//	sr.top /= count;
	//	sr.bottom /= count;
	//	si->SetBounds(sr);
	//}
	average.SaveXML("average.xml");
	awpSaveImage("average.jpg", image);
	_AWP_SAFE_RELEASE_(image)
	return 0;
}	