//todo: make this utility common for all modules 
TLFZones* ConvertVAParamsToZones(TVAInitParams& params)
{
	if (params.NumZones == 0)
		return NULL;
	TLFZones* zones = new TLFZones();
	for (int i = 0; i < params.NumZones; i++)
	{
		TVAZone vz = params.Zones[i];
		if (vz.IsRect)
		{
			awp2DPoint lt, rb;
			lt.X = vz.Rect.LeftTop.X;
			lt.Y = vz.Rect.LeftTop.Y;
			rb.X = vz.Rect.RightBottom.X;
			rb.Y = vz.Rect.RightBottom.Y;

			TLF2DRect rect(lt, rb);
			TLFZone* z = new TLFZone(rect);
			zones->AddZone(z);
		}
		else
		{
			TLF2DContour* c = new TLF2DContour();
			for (int i = 0; i < vz.NumPoints; i++)
			{
				awp2DPoint p;
				p.X = vz.Points[i].X;
				p.Y = vz.Points[i].Y;
				c->AddPoint(p);
			}
			TLFZone* z = new TLFZone(*c);
			zones->AddZone(z);
			delete c;
		}
	}
	return zones;
}