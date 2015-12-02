#pragma once
class CFrame
{
public:
	struct PIXEL
	{
		unsigned char l;
	};
protected:
	PIXEL* m_pFrame;
public:
	int m_sx, m_sy;
	CFrame(int sx, int sy);
	CFrame(CFrame& Frame);
	PIXEL& GetPixel(int i, int j);
	CFrame();
	~CFrame();
};

