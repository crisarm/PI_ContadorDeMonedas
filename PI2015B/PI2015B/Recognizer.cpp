#include "stdafx.h"
#include "Recognizer.h"


CRecognizer::CRecognizer(void)
{
}


CRecognizer::~CRecognizer(void)
{
}

#define MIN_VALUE 0x40
bool CRecognizer::detectoMonedaEnFila( CFrame* pFrame, int y )
{
	int count = 0;
	for( int x = 0; x < pFrame->m_sx; x++ )
	{
		unsigned char lum = pFrame->GetPixel(x,y).l;
		if( lum < MIN_VALUE )	
		{
			count++;
		}
	}
	return count > 10;
}
void CRecognizer::area( CFrame* pFrame )
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

	iDiamX = 0;
	bool filaSinMoneda;
	for( int y = firstY; y < pFrame->m_sy; ++y )
	{
		int iStart = 0, iEnd = 0;
		filaSinMoneda = true;
		for( int x=0; x < pFrame->m_sx; x++ )
		{
			if( pFrame->GetPixel(x,y).l < MIN_VALUE )	
			{
				iEnd = x;
				if( filaSinMoneda )
				{
					iStart = x;
					filaSinMoneda = false;
				}
			}
		}
		if( filaSinMoneda )
		{
			iDiamY = y - firstY;
			return;
		}
		int iFila = iEnd - iStart;
		if( iFila > iDiamX )
			iDiamX = iFila;
	}


	iDiamY = pFrame->m_sy - firstY;
}
