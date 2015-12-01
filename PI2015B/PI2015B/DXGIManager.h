#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include "Frame.h"

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(X) if((X)){ (X)->Release(); (X)=0; }
#endif

class CDXGIManager
{
protected:
	ID3D11Device* m_pDevice; // Recurso de hardware/software de computación (Factory)
	ID3D11DeviceContext* m_pContext; // Cola de comandos del dispositivo
	IDXGISwapChain*		 m_pSwapChain; // Cadena de Intercambio
	ID3D11Texture2D*	 m_pBackBuffer; // BackBuffer en SwapChain

public:
	bool Initialize( HWND hWND, IDXGIAdapter* pAdapter, bool bFullScreen );
	ID3D11ComputeShader* CompileCS( wchar_t* pszFileName, char* pszEntryPoint );
	void Uninitialize(void);
	void Resize( int sx, int sy );

	ID3D11Device* GetDevice(){  return m_pDevice;  }
	ID3D11DeviceContext* GetContext(){  return m_pContext; }
	IDXGISwapChain*	 GetSwapChain(){	return m_pSwapChain;  }
	ID3D11Texture2D* GetBackBuffer(){	return m_pBackBuffer;  }

	IDXGIAdapter* EnumAdapters( HWND hWND );
	ID3D11Texture2D* LoadTexture( char* pszFileName, int nBitmMapLevels = -1,
								  unsigned long(*pPixel)(unsigned long) = NULL );
	ID3D11Texture2D* LoadTexture( CFrame* pFrame );

	CDXGIManager(void);
	~CDXGIManager(void);
};

