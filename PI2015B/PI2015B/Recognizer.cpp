#include "stdafx.h"
#include "Recognizer.h"


CRecognizer::CRecognizer(void)
{
}


CRecognizer::~CRecognizer(void)
{
}


bool CRecognizer::detectoMonedaEnFila( CFrame* pFrame, int y )
{
	int count = 0;
	for( int x = 0; x < pFrame->m_sx; x++ )
	{
		if( pFrame->GetPixel(x,y).l < 0x30 )	
		{
			count++;
		}
	}
	return count > 10;
}
int CRecognizer::area( CFrame* pFrame )
{
	int firstY = 0;
	for( int y = firstY; y < pFrame->m_sy; ++y )
	{
		if( detectoMonedaEnFila( pFrame, y ) )
		{
			firstY = y;
			break;
		}
	}

	int area = 0;
	bool filaSinMoneda;
	for( int y = firstY; y < pFrame->m_sy; ++y )
	{
		filaSinMoneda = true;
		for( int x=0; x < pFrame->m_sx; x++ )
		{
			if( pFrame->GetPixel(x,y).l < 0x30 )	
			{
				area++;
				filaSinMoneda = false;
			}
		}
		if( filaSinMoneda )
			return area;
	}

	return area;
}
