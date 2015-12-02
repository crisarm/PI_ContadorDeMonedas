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
	if (m_lstFrames.size() > 1)
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
				//YUY2* pRow  = (YUY2*)(pBuffer + j*pVIH->bmiHeader.biWidth * 2);
				YUY2* pRow  = (YUY2*)(pBuffer + j*pVIH->bmiHeader.biWidth / 2);
				for (int i = 0; i < pVIH->bmiHeader.biWidth / 2; i++)
				{
					pF->GetPixel( i*2, j ).l   = pRow->y0;
					pF->GetPixel( i*2+1, j ).l = pRow->y1;

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