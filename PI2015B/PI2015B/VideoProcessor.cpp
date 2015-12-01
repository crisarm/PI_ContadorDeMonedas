#include "stdafx.h"
#include "VideoProcessor.h"


CVideoProcessor::CVideoProcessor()
{
	InitializeCriticalSection(&m_csLock);
}
CVideoProcessor::~CVideoProcessor()
{
	DeleteCriticalSection(&m_csLock);
}

void CVideoProcessor::Push(CFrame* pFrame)
{
	EnterCriticalSection(&m_csLock);
	if (m_lstFrames.size() > 60)
	{
		CFrame* pFrameToDestroy = m_lstFrames.front();
		m_lstFrames.pop_front();
		delete pFrameToDestroy;
	}
	m_lstFrames.push_back(pFrame);
	LeaveCriticalSection(&m_csLock);
}
CFrame*  CVideoProcessor::Pull()
{
	CFrame* pFrame = NULL;
	EnterCriticalSection(&m_csLock);
	if (m_lstFrames.size())
	{
		pFrame = m_lstFrames.front();
		m_lstFrames.pop_front();
	}
	LeaveCriticalSection(&m_csLock);
	return pFrame;
}

HRESULT CVideoProcessor::SampleCB(double SampleTime, IMediaSample *pSample)
{
	return S_OK;
}
HRESULT CVideoProcessor::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
	if (MEDIATYPE_Video == m_mt.majortype)
	{
		if (MEDIASUBTYPE_YUY2 == m_mt.subtype)
		{
			//[ Y0 U0 Y1 V0] [ Y2 U1 Y3 V1] 
			VIDEOINFOHEADER* pVIH=(VIDEOINFOHEADER*)m_mt.pbFormat;
			CFrame* pF = new CFrame(
				pVIH->bmiHeader.biWidth,
				pVIH->bmiHeader.biHeight);
			struct YUY2
			{
				unsigned char y0, u0, y1, v0;
			}; // /*
			for (int j = 0; j < pVIH->bmiHeader.biHeight; j++)
			{
				YUY2* pRow  = (YUY2*)(pBuffer + j*pVIH->bmiHeader.biWidth * 2);
				for (int i = 0; i < pVIH->bmiHeader.biWidth / 2; i++)
				{
					int l0 = 298 * (pRow->y0 - 16);
					int l1 = 298 * (pRow->y0 - 16);

					int D = pRow->u0 - 128;
					int E = pRow->v0 - 128;

					int sR, sG, sB;
					sR = sG = sB = 128;
					sR +=  409*E;
					sG += -100*D-208*E;
					sB +=  516*D;
					
					CFrame::PIXEL Color;
					int R = (l0 + sR) / 256;
					int G = (l0 + sG) / 256;
					int B = (l0 + sB) / 256;
					
					Color.r = (unsigned char)( R > 255 ? 255 : R );
					Color.g = (unsigned char)( G > 255 ? 255 : G );
					Color.b = (unsigned char)( B > 255 ? 255 : B );
					pF->GetPixel( i*2 , j ) = Color;
					
					R = (l1 + sR) / 256;
					G = (l1 + sG) / 256;
					B = (l1 + sB) / 256;
					Color.r = (unsigned char)( R > 255 ? 255 : R );
					Color.g = (unsigned char)( G > 255 ? 255 : G );
					Color.b = (unsigned char)( B > 255 ? 255 : B );
					pF->GetPixel( i*2+1 , j ) = Color;

					pRow++;
				}
			}  // */
			 /*
			for (int j = 0; j < pVIH->bmiHeader.biHeight; j++)
			{
				unsigned char* pRow = (unsigned char*)(pBuffer + j*pVIH->bmiHeader.biWidth*2);
				for (int i = 0; i < pVIH->bmiHeader.biWidth; i++)
				{
					CFrame::PIXEL LummaGray;
					LummaGray.r = LummaGray.g = LummaGray.b = *pRow;
					pF->GetPixel(i , j) = LummaGray;
					pRow+=2;
				}
			} // */
			Push(pF);
		}
	}
	return S_OK;
}