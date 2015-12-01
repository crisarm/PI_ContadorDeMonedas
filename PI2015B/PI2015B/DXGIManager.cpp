#include "stdafx.h"
#include "DXGIManager.h"
#include <d3dcompiler.h>
#include <fstream>
using namespace std;

CDXGIManager::CDXGIManager(void)
{
	m_pDevice = 0;
	m_pContext = 0;
	m_pSwapChain = 0;
	m_pBackBuffer = 0;
}

CDXGIManager::~CDXGIManager(void)
{
}

void CDXGIManager::Uninitialize(void)
{
	if( m_pSwapChain )
		m_pSwapChain->SetFullscreenState( false, 0 );
	SAFE_RELEASE( m_pDevice );
	SAFE_RELEASE( m_pContext );
	SAFE_RELEASE( m_pSwapChain );
	SAFE_RELEASE( m_pBackBuffer );
}

IDXGIAdapter* CDXGIManager::EnumAdapters( HWND hWND)
{
	IDXGIFactory* pFactory;
	HRESULT hr = CreateDXGIFactory( IID_IDXGIFactory, (void**)&pFactory );
	if( FAILED(hr) )
	{
		MessageBox( hWND, L"Cambia de computadora", L"Error Fatal", MB_ICONERROR );
		return NULL;
	}
	int iAdapter = 0;
	IDXGIAdapter* pAdapter = 0;
	while(1)
	{
		DXGI_ADAPTER_DESC dad;
		if( FAILED( pFactory->EnumAdapters( iAdapter, &pAdapter) ) )
			break;
		TCHAR szMessage[ 1024 ];
		pAdapter->GetDesc( &dad );
		wsprintf( szMessage, L"Description: %s\r\nDedicated Memory: %d MB\r\nShared Memory: %d MB\r\nSystem Memory: %d",
			dad.Description,
			dad.DedicatedVideoMemory /(1024*1024),
			dad.SharedSystemMemory /(1024*1024),
			dad.DedicatedSystemMemory /(1024*1024) );
		switch( MessageBox( hWND, szMessage, L"Do you wish to use this device?",
			MB_YESNOCANCEL | MB_ICONQUESTION ) )
		{
		case IDYES:
			SAFE_RELEASE( pFactory );
			return pAdapter;
		case IDNO:
			break;
		case IDCANCEL:
			SAFE_RELEASE( pFactory );
			SAFE_RELEASE( pAdapter );
			return NULL;
		}
		iAdapter++;
	}
}

bool CDXGIManager::Initialize( HWND hWND, IDXGIAdapter* pAdapter, bool bFullScreen )
{
	RECT rc;
	GetClientRect( hWND, &rc );
	DXGI_SWAP_CHAIN_DESC dscd;
	memset( &dscd, 0, sizeof(DXGI_SWAP_CHAIN_DESC) );
	dscd.BufferCount = 2;
	dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dscd.BufferDesc.Height = rc.left;
	dscd.BufferDesc.Width = rc.bottom;
	dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;  // Full Screen
	dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	dscd.BufferDesc.RefreshRate.Denominator = 0;
	dscd.BufferDesc.RefreshRate.Numerator = 0;

	dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT 
					 | DXGI_USAGE_UNORDERED_ACCESS;
	dscd.OutputWindow = hWND;
	dscd.SampleDesc.Count = 1;
	dscd.SampleDesc.Quality = 0;
	dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	dscd.Windowed = ! bFullScreen;

	D3D_FEATURE_LEVEL Feature = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL DetectedFeature;
	if( pAdapter )
	{	// Try to use the adapter
		HRESULT hr = D3D11CreateDeviceAndSwapChain( pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0,
													&Feature, 1, D3D11_SDK_VERSION, &dscd,
													&m_pSwapChain, &m_pDevice, &DetectedFeature, &m_pContext );
		if( FAILED( hr ) )
			return false;
		Resize( rc.right, rc.bottom );
		return true;
	}
	else
	{	// Try to use the CPU
		HRESULT hr =  D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_WARP, 0, 0,
													&Feature, 1, D3D11_SDK_VERSION, &dscd,
													&m_pSwapChain, &m_pDevice, &DetectedFeature, &m_pContext );
		if( FAILED( hr ) )
			return false;
		Resize( rc.right, rc.bottom );
		return true;
	}
	return false;
}

ID3D11ComputeShader* CDXGIManager::CompileCS( wchar_t* pszFileName, char* pszEntryPoint )
{
	ID3D10Blob* pIL = NULL;			// Intermediate Language, Bytecode del shader en DXIL
	ID3D10Blob* pErrors = NULL;		// Error log buffer.
	ID3D11ComputeShader* pCS = NULL; // The compute shader

#ifdef _DEBUG
	DWORD dwOption = D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS |
					 D3DCOMPILE_DEBUG;
#else
	DWORD dwOption = D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_ENABLE_STRICTNESS |
					 D3DCOMPILE_DEBUG;
#endif
	
	HRESULT hr = D3DCompileFromFile( pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
									 pszEntryPoint, "cs_5_0", dwOption, 0, &pIL, &pErrors );
	if( FAILED(hr) )
	{
		if( pErrors )
			MessageBoxA( NULL, (char*)pErrors->GetBufferPointer(), "ERRORES", MB_ICONERROR );
			
		SAFE_RELEASE( pErrors );
		SAFE_RELEASE( pIL );
		return NULL;
	}
	// Crear el compute shader en código nativo
	hr = m_pDevice->CreateComputeShader( pIL->GetBufferPointer(), pIL->GetBufferSize(), NULL, &pCS );
	if( FAILED(hr) )
	{
		SAFE_RELEASE( pErrors );
		SAFE_RELEASE( pIL );
		return NULL;
	}
	SAFE_RELEASE( pErrors );
	SAFE_RELEASE( pIL );
	return pCS;
}


				struct PIXEL 
				{
					unsigned char r, g, b, a;
				};
ID3D11Texture2D* CDXGIManager::LoadTexture( char* pszFileName, int nBitmMapLevels,
											unsigned long(*pPixel)(unsigned long) )
{
	fstream in;
	in.open( pszFileName, ios::in | ios::binary );
	if( !in.is_open() )
		return NULL;
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	memset( &bfh, 0, sizeof(bfh) );
	memset( &bih, 0, sizeof(bih) );
	in.read( (char*) &bfh.bfType, 2 );
	
	if( 'MB' != bfh.bfType )
		return NULL;
	in.read( (char*) &bfh.bfSize, sizeof(bfh) - 2 );
	in.read( (char*) &bih.biSize, 4 );

	if( sizeof( BITMAPINFOHEADER ) != bih.biSize )
		return NULL;

	in.read( (char*) &bih.biWidth, sizeof( bih ) - 4 );

	//3. Carga de información de imagen
	unsigned long RowLength = 4 * ( (bih.biBitCount * bih.biWidth + 31) / 32);
	
	//4. Crear recurso de andamiaje para CPU->GPU
	//Los recursos de andamiaje están en espacio del CPU
	D3D11_TEXTURE2D_DESC dtd;
	ID3D11Texture2D *pStaging = 0;
	memset( &dtd, 0, sizeof(dtd) );

	dtd.ArraySize = 1;
	dtd.BindFlags = 0;
	dtd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	dtd.Usage  = D3D11_USAGE_STAGING;
	dtd.Height = bih.biHeight;
	dtd.Width  = bih.biWidth;
	dtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dtd.MipLevels = 1;
	dtd.SampleDesc.Count = 1;
	dtd.MiscFlags = 0;
	HRESULT hr = m_pDevice->CreateTexture2D( &dtd, NULL, &pStaging );
	if( FAILED( hr ) )
	{
		*(int*)0 = 0; // Solo para DEBUG
		return NULL;
	}
	switch( bih.biBitCount )
	{
		case 1:
		{
			RGBQUAD Palette[2];
			int nColors = 0 == bih.biClrUsed ? 2 : bih.biClrUsed;
			in.read( (char*)Palette, nColors * sizeof(RGBQUAD) );
			unsigned char *pRow = new unsigned char[RowLength];
			
			D3D11_MAPPED_SUBRESOURCE ms;
			m_pContext->Map( pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms );

			unsigned char masks[8];
			masks[7] = 1;
			for(int n=6; n>=0; n--)
				masks[ n ] = masks[ n+1 ] * 2;

			for( int j=bih.biHeight-1; j>=0; j-- )
			{
				PIXEL *pLine = (PIXEL*) ( (char*)ms.pData + j * ms.RowPitch );
				in.read( (char*)pRow, RowLength );
				for( int i=0; i < bih.biWidth; i++ )
				{
					RGBQUAD C = Palette[  pRow[i/8] & masks[i%8] ? 1 : 0  ]; //Reading value of the respective bit
					PIXEL Color;
					Color.r = C.rgbRed;
					Color.g = C.rgbGreen;
					Color.b = C.rgbBlue;
					Color.a = 0xff;
					pLine[i] = Color;
				}
			}
			m_pContext->Unmap( pStaging, 0 );
			delete[] pRow;
		}
		break;
		case 4:
		{
			RGBQUAD Palette[16];
			int nColors = 0 == bih.biClrUsed ? 16 : bih.biClrUsed;
			in.read( (char*)Palette, nColors * sizeof(RGBQUAD) );
			unsigned char *pRow = new unsigned char[RowLength];
			
			D3D11_MAPPED_SUBRESOURCE ms;
			m_pContext->Map( pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms );

			for( int j=bih.biHeight-1; j>=0; j-- )
			{
				PIXEL *pLine = (PIXEL*) ( (char*)ms.pData + j * ms.RowPitch );
				in.read( (char*)pRow, RowLength );
				for( int i=0; i < bih.biWidth; i+=2 )
				{
					unsigned char ucByte = pRow[i/2];
					RGBQUAD C;
					PIXEL Color;
						
					C = Palette[ ucByte/16 ]; // Index from first 4 bits
					Color.r = C.rgbRed;
					Color.g = C.rgbGreen;
					Color.b = C.rgbBlue;
					Color.a = 0xff;
					pLine[i] = Color; // First Pixel
						
					C = Palette[ ucByte & 0x0f ]; // Index from last 4 bits
					Color.r = C.rgbRed;
					Color.g = C.rgbGreen;
					Color.b = C.rgbBlue;
					Color.a = 0xff;
					pLine[i+1] = Color; // Second Pixel
				}
			}
			m_pContext->Unmap( pStaging, 0 );
			delete[] pRow;
		}
		break;
		case 8:
		{
			RGBQUAD Palette[256];
			int nColors = 0 == bih.biClrUsed ? 256 : bih.biClrUsed;
			in.read( (char*)Palette, nColors * sizeof(RGBQUAD) );
			unsigned char *pRow = new unsigned char[RowLength];
			
			D3D11_MAPPED_SUBRESOURCE ms;
			m_pContext->Map( pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms );

			for( int j=bih.biHeight-1; j>=0; j-- )
			{
				PIXEL *pLine = (PIXEL*) ( (char*)ms.pData + j * ms.RowPitch );
				in.read( (char*)pRow, RowLength );
				
				for( int i=0; i < bih.biWidth; i++ )
				{
					RGBQUAD C = Palette[ pRow[i] ];
					PIXEL Color;
					Color.r = C.rgbRed;
					Color.g = C.rgbGreen;
					Color.b = C.rgbBlue;
					Color.a = 0xff;
					
					pLine[i] = Color;
				}
			}
			m_pContext->Unmap( pStaging, 0 );
			delete[] pRow;
		}
		break;
		case 24:
		{
			unsigned char *pRow = new unsigned char[RowLength];

			D3D11_MAPPED_SUBRESOURCE ms;
			m_pContext->Map( pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms );

			for( int j=bih.biHeight-1; j>=0; j-- )
			{
				PIXEL *pLine = (PIXEL*) ( (char*)ms.pData + j * ms.RowPitch );
				in.read( (char*)pRow, RowLength );
				for( int i=0; i < bih.biWidth; i++ )
				{
					PIXEL Color;
					int index = 3*i;
					Color.b = pRow[ index++ ];
					Color.g = pRow[ index++ ];
					Color.r = pRow[ index ];
					Color.a = 0xff;
					pLine[i] = Color;
				}
			}
			m_pContext->Unmap( pStaging, 0 );
			delete[] pRow;
		}
		break;
		case 32:
		{
			unsigned char *pRow = new unsigned char[RowLength];

			D3D11_MAPPED_SUBRESOURCE ms;
			m_pContext->Map( pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms );

			for( int j=bih.biHeight-1; j>=0; j-- )
			{
				PIXEL *pLine = (PIXEL*) ( (char*)ms.pData + j * ms.RowPitch );
				in.read( (char*)pRow, RowLength );
				for( int i=0; i < bih.biWidth; i++ )
				{
					PIXEL Color;
					int index = 4*i;
					Color.b = pRow[ index++ ];
					Color.g = pRow[ index++ ];
					Color.r = pRow[ index++ ];
					Color.a = pRow[ index ];
					pLine[i] = Color;
				}
				//in.write( (char*)pRow, RowLength );
			}
			m_pContext->Unmap( pStaging, 0 );
			delete[] pRow;
		}
		break;
  	}

	//5. Transferir de CPU a GPU
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.CPUAccessFlags = 0;
	dtd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D* pTexture = 0;
	hr = m_pDevice->CreateTexture2D( &dtd, NULL, &pTexture );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pStaging );
		return NULL;
	}
	m_pContext->CopyResource( pTexture, pStaging );
	SAFE_RELEASE( pStaging );
	return pTexture;
}

ID3D11Texture2D* CDXGIManager::LoadTexture( CFrame* pFrame )
{
	//1. Crear recurso de andamiaje para CPU->GPU
	//Los recursos de andamiaje están en espacio del CPU
	D3D11_TEXTURE2D_DESC dtd;
	ID3D11Texture2D *pStaging = 0;
	memset( &dtd, 0, sizeof(dtd) );

	dtd.ArraySize = 1;
	dtd.BindFlags = 0;
	dtd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	dtd.Usage  = D3D11_USAGE_STAGING;
	dtd.Height = pFrame->m_sy;
	dtd.Width  = pFrame->m_sx;
	dtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dtd.MipLevels = 1;
	dtd.SampleDesc.Count = 1;
	dtd.MiscFlags = 0;
	HRESULT hr = m_pDevice->CreateTexture2D( &dtd, NULL, &pStaging );
	if( FAILED( hr ) )
	{
		//*(int*)0 = 0; // Solo para DEBUG
		return NULL;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_pContext->Map( pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms );

	for( int j=pFrame->m_sy - 1; j>=0; j-- )
	{
		CFrame::PIXEL *pLine = (CFrame::PIXEL*) ( (char*)ms.pData + j * ms.RowPitch );
		for( int i=0; i < pFrame->m_sx; i++ )
			pLine[i] = pFrame->GetPixel( i, j );
	}
	m_pContext->Unmap( pStaging, 0 );

	//2. Transferir de CPU a GPU
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.CPUAccessFlags = 0;
	dtd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D* pTexture = 0;
	hr = m_pDevice->CreateTexture2D( &dtd, NULL, &pTexture );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pStaging );
		return NULL;
	}
	m_pContext->CopyResource( pTexture, pStaging );
	SAFE_RELEASE( pStaging );
	return pTexture;
}

void CDXGIManager::Resize( int sx, int sy )
{
	m_pContext->ClearState();
	SAFE_RELEASE( m_pBackBuffer );
	m_pSwapChain->ResizeBuffers( 2, sx, sy, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
	m_pSwapChain->GetBuffer( 0, IID_ID3D11Texture2D, (void**) &m_pBackBuffer );
}


