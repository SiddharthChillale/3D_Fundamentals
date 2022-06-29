
#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "GDIPlusManager.h"
#include "MainException.h"
#include "Surface.h"
#include "Colors.h"
#include "Vec2.h"
#include "TexVertex.h"

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

class Graphics
{
public:
	class Exception : public MainException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	void DrawLine( const Vec2& p1,const Vec2& p2,Color c )
	{
		DrawLine( p1.x,p1.y,p2.x,p2.y,c );
	}
	void DrawLine( float x1,float y1,float x2,float y2,Color c );
	void DrawTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, Color c);
	void DrawTriangleTex(const TexVertex& v0, const TexVertex& v1, const TexVertex& v2, const Surface& tex);
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c )
	{
		sysBuffer.PutPixel( x,y,c );
	}
	~Graphics();
private:
	void DrawFlatTopTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, Color c);
	void DrawFlatBottomTriangle(const Vec2& v0, const Vec2& v1, const Vec2& v2, Color c);
	void DrawFlatTopTriangleTex(const TexVertex& v0, const TexVertex& v1, const TexVertex& v2, const Surface& tex);
	void DrawFlatBottomTriangleTex(const TexVertex& v0, const TexVertex& v1, const TexVertex& v2, const Surface& tex);
private:
	GDIPlusManager										gdipMan;
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Surface												sysBuffer;
public:
	static constexpr unsigned int ScreenWidth = 800u;
	static constexpr unsigned int ScreenHeight = 800u;
};