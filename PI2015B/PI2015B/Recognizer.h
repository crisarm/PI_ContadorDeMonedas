#pragma once
#include "Frame.h"

class CRecognizer
{
public:
	CRecognizer(void);
	~CRecognizer(void);

	bool detectoMonedaEnFila( CFrame* pFrame, int y );
	int area( CFrame* pFrame );
};

