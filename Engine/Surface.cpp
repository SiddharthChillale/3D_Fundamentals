
#define FULL_WINTARD
#include "MainException.h"
#include "WinDefines.h"
#include "Surface.h"

namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#include <sstream>

#pragma comment( lib,"gdiplus.lib" )

void Surface::PutPixelAlpha( unsigned int x,unsigned int y,Color c )
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < width );
	assert( y < height );
	// load source pixel
	const Color d = GetPixel( x,y );

	// blend channels
	const unsigned char rsltRed = (c.GetR() * c.GetA() + d.GetR() * (255u - c.GetA())) / 256u;
	const unsigned char rsltGreen = (c.GetG() * c.GetA() + d.GetG() * (255u - c.GetA())) / 256u;
	const unsigned char rsltBlue = (c.GetB() * c.GetA() + d.GetB() * (255u - c.GetA())) / 256u;

	// pack channels back into pixel and fire pixel onto surface
	PutPixel( x,y,{ rsltRed,rsltGreen,rsltBlue } );
}

Surface Surface::FromFile( const std::wstring & name )
{
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int pitch = 0;
	std::unique_ptr<Color[]> pBuffer = nullptr;

	{
		Gdiplus::Bitmap bitmap( name.c_str() );
		if( bitmap.GetLastStatus() != Gdiplus::Status::Ok )
		{
			std::wstringstream ss;
			ss << L"Loading image [" << name << L"]: failed to load.";
			throw Exception( _CRT_WIDE( __FILE__ ),__LINE__,ss.str() );
		}

		pitch = width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>( width * height );

		for( unsigned int y = 0; y < height; y++ )
		{
			for( unsigned int x = 0; x < width; x++ )
			{
				Gdiplus::Color c;
				bitmap.GetPixel( x,y,&c );
				pBuffer[y * pitch + x] = c.GetValue();
			}
		}
	}

	return Surface( width,height,pitch,std::move( pBuffer ) );
}

void Surface::Save( const std::wstring & filename ) const
{
	auto GetEncoderClsid = [&filename]( const WCHAR* format,CLSID* pClsid ) -> void
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize( &num,&size );
		if( size == 0 )
		{
			std::wstringstream ss;
			ss << L"Saving surface to [" << filename << L"]: failed to get encoder; size == 0.";
			throw Exception( _CRT_WIDE( __FILE__ ),__LINE__,ss.str() );
		}

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc( size ));
		if( pImageCodecInfo == nullptr )
		{
			std::wstringstream ss;
			ss << L"Saving surface to [" << filename << L"]: failed to get encoder; failed to allocate memory.";
			throw Exception( _CRT_WIDE( __FILE__ ),__LINE__,ss.str() );
		}

		GetImageEncoders( num,size,pImageCodecInfo );

		for( UINT j = 0; j < num; ++j )
		{
			if( wcscmp( pImageCodecInfo[j].MimeType,format ) == 0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free( pImageCodecInfo );
				return;
			}
		}

		free( pImageCodecInfo );
		std::wstringstream ss;
		ss << L"Saving surface to [" << filename << 
			L"]: failed to get encoder; failed to find matching encoder.";
		throw Exception( _CRT_WIDE( __FILE__ ),__LINE__,ss.str() );
	};

	CLSID bmpID;
	GetEncoderClsid( L"image/bmp",&bmpID );
	Gdiplus::Bitmap bitmap( width,height,pitch * sizeof( Color ),PixelFormat32bppARGB,(BYTE*)pBuffer.get() );
	if( bitmap.Save( filename.c_str(),&bmpID,nullptr ) != Gdiplus::Status::Ok )
	{
		std::wstringstream ss;
		ss << L"Saving surface to [" << filename << L"]: failed to save.";
		throw Exception( _CRT_WIDE( __FILE__ ),__LINE__,ss.str() );
	}
}

void Surface::Copy( const Surface & src )
{
	assert( width == src.width );
	assert( height == src.height );
	if( pitch == src.pitch )
	{
		memcpy( pBuffer.get(),src.pBuffer.get(),pitch * height * sizeof( Color ) );
	}
	else
	{
		for( unsigned int y = 0; y < height; y++ )
		{
			memcpy( &pBuffer[pitch * y],&src.pBuffer[pitch * y],sizeof( Color )* width );
		}
	}
}
