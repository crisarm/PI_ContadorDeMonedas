#include "stdafx.h"
#include "Recognizer.h"


CRecognizer::CRecognizer(void)
{
}


CRecognizer::~CRecognizer(void)
{
}


bool lineaNegra( CFrame* pFrame, int& y )
{
	for( int x = 0; x < pFrame->m_sx; x++ )
		if( pFrame->GetPixel(x,y).r > 50 )	
			return false;
	return true;
}
int area( CFrame* pFrame )
{
	int firstY = 0;
	for( int y = 0; y < pFrame->m_sy; y++ )
	{
		if( ! lineaNegra( pFrame, y ) )
		{
			firstY = y;
			break;
		}
	}

	int area = 0;
	bool lineaActualNegra;
	for( int y = firstY; y < pFrame->m_sy; y++ )
	{
		lineaActualNegra = true;
		for( int x=0; x < pFrame->m_sx; x++ )
		{
			if( pFrame->GetPixel(x,y).r > 50 )	
			{
				area++;
				lineaActualNegra = false;
			}
		}
		if( lineaActualNegra )
			return area;
	}

	return area;
}
