#pragma once
#include <cheat/internal/core.hpp>
#include <algorithm>
#include <sstream>
#include <Windows.h>
#include <Process.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <string>
#include <sstream>
#include <array>
#include <algorithm>
#include <optional>
#include <vector>
#include <TlHelp32.h>
#include <atomic>
#include <codecvt>
#include <winternl.h>
#include <list>

typedef int ( WINAPI *keyword )( DWORD );
static keyword NtGetAsyncKeyState;

typedef HRESULT( *present )( IDXGISwapChain *, UINT, UINT );
inline present c_original {  };


typedef void ( *fov_update_fn )( void *, float , float , bool );
inline fov_update_fn FOVUpdater {  };

typedef void ( *ui_update_fn )(void* );
inline ui_update_fn GUIText {  }; 


typedef unity::c_renderer*( *updates_chams )( void* );
inline updates_chams c_chams {  };

int DamageDone = 0;

bool ShouldShow = false;

std::vector<std::string> scores = {"test" };
int indexScore;
//static const char *items[] { "test" };

#pragma once
#define M_PI (float)3.14159265358979323846f
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI / 180.f) )
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI) )

namespace geometry
{
	namespace math
	{
		inline static float sqrtf( float number )
		{
			long i;
			float x2, y;
			const float threehalfs = 1.5F;

			x2 = number * 0.5F;
			y = number;
			i = *( long * ) &y;
			i = 0x5f3759df - ( i >> 1 );
			y = *( float * ) &i;
			y = y * ( threehalfs - ( x2 * y * y ) );
			y = y * ( threehalfs - ( x2 * y * y ) );

			return 1 / y;
		}
	}

	struct mat4x4_t
	{
		mat4x4_t()
			: m { { 0, 0, 0, 0 },
				 { 0, 0, 0, 0 },
				 { 0, 0, 0, 0 },
				 { 0, 0, 0, 0 } }
		{ }

		mat4x4_t( const mat4x4_t & ) = default;

		float *operator[]( size_t i ) { return m[i]; }
		const float *operator[]( size_t i ) const { return m[i]; }

		auto transpose() -> mat4x4_t
		{
			mat4x4_t m;

			for ( int i = 0; i < 4; i++ )
				for ( int j = 0; j < 4; j++ )
					m.m[i][j] = this->m[j][i];

			return m;
		}

		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4];
		};
	};

	class vec2_t
	{
	public:
		float x;
		float y;

		vec2_t()
		{ }

		vec2_t( float x, float y )
			: x( x ), y( y )
		{ }
	};

	class vec3_t
	{
	public:
		float x;
		float y;
		float z;

		vec3_t()
		{ }

		vec3_t( float x, float y, float z )
			: x( x ), y( y ), z( z )
		{ }

		vec3_t operator+( const vec3_t &vector ) const
		{
			return vec3_t( x + vector.x, y + vector.y, z + vector.z );
		}

		vec3_t operator-( const vec3_t &vector ) const
		{
			return vec3_t( x - vector.x, y - vector.y, z - vector.z );
		}

		vec3_t operator * ( float number ) const
		{
			return vec3_t( x * number, y * number, z * number );
		}

		vec3_t operator/( float number ) const
		{
			return vec3_t( x / number, y / number, z / number );
		}

		vec3_t &operator/=( float number )
		{
			x /= number;
			y /= number;
			z /= number;

			return *this;
		}

		auto empty() -> bool { return x == 0 && y == 0 && z == 0; }

		auto length() -> float { return math::sqrtf( ( x * x ) + ( y * y ) + ( z * z ) ); }

		vec3_t normalized()
		{
			float len = length();
			return vec3_t( x / len, y / len, z / len );
		}

		auto distance_3d( const vec3_t &vector ) -> float { return ( *this - vector ).length(); }
		auto dot( const vec3_t &vector ) -> float { return x * vector.x + y * vector.y + z * vector.z; }
	};

	class vec4_t
	{
	public:
		float x;
		float y;
		float z;
		float w;

		vec4_t() { }
		vec4_t( float x, float y, float z, float w ) : x( x ), y( y ), z( z ), w( w ) { }

		auto dot( const vec4_t &vector ) -> float { return x * vector.x + y * vector.y + z * vector.z + w * vector.w; }
		auto distance( const vec4_t &vector ) -> float { return math::sqrtf( ( x - vector.x ) * ( x - vector.x ) + ( y - vector.y ) * ( y - vector.y ) + ( z - vector.z ) * ( z - vector.z ) + ( w - vector.w ) * ( w - vector.w ) ); }

		bool operator==( const vec4_t &vector ) const
		{
			return x == vector.x && y == vector.y && z == vector.z && w == vector.w;
		}

		bool operator!=( const vec4_t &vector ) const
		{
			return x != vector.x || y != vector.y || z != vector.z || w != vector.w;
		}

		vec4_t operator+( const vec4_t &vector ) const
		{
			return vec4_t( x + vector.x, y + vector.y, z + vector.z, w + vector.w );
		}

		vec4_t operator-( const vec4_t &vector ) const
		{
			return vec4_t( x - vector.x, y - vector.y, z - vector.z, w - vector.w );
		}

		vec4_t operator-() const
		{
			return vec4_t( -x, -y, -z, -w );
		}

		vec4_t operator*( float number ) const
		{
			return vec4_t( x * number, y * number, z * number, w * number );
		}

		vec4_t operator/( float number ) const
		{
			return vec4_t( x / number, y / number, z / number, w / number );
		}

		vec4_t &operator+=( const vec4_t &vector )
		{
			x += vector.x;
			y += vector.y;
			z += vector.z;
			w += vector.w;
			return *this;
		}

		vec4_t &operator-=( const vec4_t &vector )
		{
			x -= vector.x;
			y -= vector.y;
			z -= vector.z;
			w -= vector.w;
			return *this;
		}

		vec4_t &operator*=( float number )
		{
			x *= number;
			y *= number;
			z *= number;
			w *= number;
			return *this;
		}

		vec4_t &operator/=( float number )
		{
			x /= number;
			y /= number;
			z /= number;
			w /= number;
			return *this;
		}
	};
}

static float Height = 0;
static float Width = 0;

ImFont *m_pFont;
ImFont *esp_pFont;

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

inline float dist( const D3DXVECTOR3 p1, const D3DXVECTOR3 p2 )
{
	float x = p1.x - p2.x;
	float y = p1.y - p2.y;
	float z = p1.z - p2.z;
	return sqrt( x * x + y * y + z * z );
}

bool unload = false;

static void HelpMarker( const char *desc )
{
	ImGui::TextDisabled( xor("(?)") );
	if ( ImGui::IsItemHovered() )
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
		ImGui::TextUnformatted( desc );
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

namespace cfg
{
	bool menu;

	bool air_walk = false;
	bool no_recoil = true;
	bool instant_aim = true;
	bool no_sway = true;
	bool inf_stamina = true;
	bool no_fall_dmg = true;
	bool fast_reload = true;

	bool esp_skeleton = true;
	bool esp_corner = true;
	bool esp_name = true;
	bool esp_distance = true;
	bool boss_type = true;
	bool esp_health = true;

	bool item_esp = true;
	bool corpse_esp = true;

	bool aimbot = true;

	static const char *aimtypes[] { "Silent", "Memory" };
	static int sel_aimtypes = 0;

	static const char *Tracetypes[] { "Barrel", "Middle", "Bottom", "Top", "Disable" };
	static int sel_Tracetypes = 0;

	bool fov_circle = true;
	float fov = 250;
	float CameraFOV = 0.0f;

	static const char *bones[] {"Head", "Neck", "Torso", "Penis" };
	static int sel_bones = 0;

	std::wstring ammo_count;
	
	
}


struct Color
{
	float r, g, b;
	Color operator+( Color addedColor )
	{
		return { r + addedColor.r, g + addedColor.g, b + addedColor.b };
	}

	Color operator-( Color addedColor )
	{
		return { r - addedColor.r, g - addedColor.g, b - addedColor.b };
	}

	Color operator*( float multiplier )
	{
		return { r * multiplier, g * multiplier, b * multiplier };
	}

	Color operator/( float dividend )
	{
		return { r / dividend, g / dividend, b / dividend };
	}
	Color floor()
	{
		return { floorf( r ), floorf( g ), floorf( b ) };
	}
};

BOOL valid_pointer( DWORD64 address )
{
	if ( !IsBadWritePtr( ( LPVOID ) address, ( UINT_PTR ) 8 ) ) return TRUE;
	else return FALSE;
}
template<typename T>
 T Read( DWORD_PTR address, const T &def = T() )
{
	if ( valid_pointer( address ) )
		return *( T * ) address;
	else
		return T();
}

template<typename T>
T Write( DWORD_PTR address, DWORD_PTR value, const T &def = T() )
{
	if ( valid_pointer( address ) )
		return *( T * ) address = value;
	else
		return T();
}
#define ToLower(Char) ((Char >= 'A' && Char <= 'Z') ? (Char + 32) : Char)
#include <immintrin.h>
#include <vector>
#include <TlHelp32.h>
#include <atomic>
#include <codecvt>
#include <winternl.h>
#include <list>

typedef struct LDR_DATA_TABLE_ENTRY_FIX
{
	PVOID Reserved1[2];
	LIST_ENTRY InMemoryOrderLinks;
	PVOID Reserved2[2];
	PVOID DllBase;
	PVOID Reserved3[1];
	ULONG64 SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	PVOID Reserved5[2];
#pragma warning(push)
#pragma warning(disable: 4201)
	union
	{
		ULONG CheckSum;
		PVOID Reserved6;
	} DUMMYUNIONNAME;
#pragma warning(pop)
	ULONG TimeDateStamp;
};

class Utility
{
public:

	template <typename StrType, typename StrType2>
	bool StrCmp( StrType Str, StrType2 InStr, bool Two )
	{
		if ( !Str || !InStr ) return false;
		wchar_t c1, c2; do
		{
			c1 = *Str++; c2 = *InStr++;
			c1 = ToLower( c1 ); c2 = ToLower( c2 );
			if ( !c1 && ( Two ? !c2 : 1 ) ) return true;
		} while ( c1 == c2 ); return false;
	}

	//GetModuleBase
	template <typename StrType>
	PBYTE GetModuleBase_Wrapper( StrType ModuleName )
	{
		PPEB_LDR_DATA Ldr = ( ( PTEB ) __readgsqword( FIELD_OFFSET( NT_TIB, Self ) ) )->ProcessEnvironmentBlock->Ldr; void *ModBase = nullptr;
		for ( PLIST_ENTRY CurEnt = Ldr->InMemoryOrderModuleList.Flink; CurEnt != &Ldr->InMemoryOrderModuleList; CurEnt = CurEnt->Flink )
		{
			LDR_DATA_TABLE_ENTRY_FIX *pEntry = CONTAINING_RECORD( CurEnt, LDR_DATA_TABLE_ENTRY_FIX, InMemoryOrderLinks );
			if ( !ModuleName || StrCmp( ModuleName, pEntry->BaseDllName.Buffer, false ) ) return ( PBYTE ) pEntry->DllBase;
		} return nullptr;
	}
};

uint64_t ReadChain( uint64_t base, const std::vector<uint64_t> &offsets )
{
	uint64_t result = *( uint64_t * ) ( base + offsets.at( 0 ) );
	for ( int i = 1; result && i < offsets.size(); i++ )
	{
		result = *( uint64_t * ) ( result + offsets.at( i ) );
	}
	return result;
}



void ReadMemory( const void *address, void *buffer, size_t size )
{
	DWORD back = NULL;

	DWORD instruction = 0x04;

	if ( VirtualProtect(( LPVOID ) address, size, instruction, &back ) )
	{
		memcpy(buffer, address, size );

		VirtualProtect(( LPVOID ) address, size, back, &back );
	}
}


std::string ReadASCII( uint64_t adr )
{
	char buf[64];
	ReadMemory( ( const void * ) adr, &buf, 64 );
	return buf;
}

void  WINAPIV DebugOut( const TCHAR *fmt, ... )
{
	TCHAR  s[1025];
	va_list  args;
	va_start( args, fmt );
	wvsprintf( s, fmt, args );
	va_end( args );
	OutputDebugString(( LPCSTR ) s );
}

#define eft_output(a) DebugOut( x("eft-sdk : %d : %s"), __LINE__, a);

typedef struct
{

	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;


std::string string_To_UTF8( const std::string &str )
{
	int nwLen = ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, NULL, 0 );

	wchar_t *pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory( pwBuf, nwLen * 2 + 2 );

	::MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen );

	int nLen = ::WideCharToMultiByte( CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL );

	char *pBuf = new char[nLen + 1];
	ZeroMemory( pBuf, nLen + 1 );

	::WideCharToMultiByte( CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL );

	std::string retStr( pBuf );

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

/* RGBA Structure (Red, Green, Blue, Alpha) */

static void FilledRectangle( int x, int y, int w, int h, RGBA color )
{
	ImGui::GetOverlayDrawList()->AddRectFilled( ImVec2( x, y ), ImVec2( x + w, y + h ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), 0, 0 );
}

void Box( int x, int y, int w, int h, RGBA color )
{
	ImGui::GetOverlayDrawList()->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), 0, 0 );
}

void Rectangle( int x, int y, int w, int h, RGBA color, int thickness )
{
	ImGui::GetOverlayDrawList()->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), 0, 0, thickness );
}

void FilledCircle( int x, int y, int radius, RGBA color, int segments )
{
	ImGui::GetOverlayDrawList()->AddCircleFilled( ImVec2( x, y ), radius, ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), segments );
}

void Circle( int x, int y, int radius, RGBA color, int segments )
{
	ImGui::GetOverlayDrawList()->AddCircle( ImVec2( x, y ), radius, ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), segments );
}

void Triangle( int x1, int y1, int x2, int y2, int x3, int y3, RGBA color, float thickne )
{
	ImGui::GetOverlayDrawList()->AddTriangle( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), thickne );
}

void FilledTriangle( int x1, int y1, int x2, int y2, int x3, int y3, RGBA color )
{
	ImGui::GetOverlayDrawList()->AddTriangleFilled( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ) );
}

void Line( int x1, int y1, int x2, int y2, RGBA color, int thickness )
{
	if ( x2 != 0 && y2 != 0 )
	{
		ImGui::GetOverlayDrawList()->AddLine( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), thickness );
	}
}

void BottomHealthBar( int x, int y, int w, int h, int borderPx, RGBA color )
{
	FilledRectangle( x + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w - w + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
}

void CornerBox( int x, int y, int w, int h, int borderPx, RGBA color )
{
	int WidthThird = w / 4;
	int HeightThird = WidthThird;

	//topleft 
	FilledRectangle( x - w / 2 - ( 1 ), y - ( 1 ), WidthThird + 2, 3, { 0, 0,0, 255 } ); //h
	FilledRectangle( x - w / 2 - ( 1 ), y - ( 1 ), 3, HeightThird + 2, { 0, 0,0, 255 } ); //v

	//topright
	FilledRectangle( ( x + w / 2 ) - WidthThird - ( 1 ), y - ( 1 ), WidthThird + 2, 3, { 0, 0,0, 255 } ); //h
	FilledRectangle( x + w / 2 - ( 1 ), y - ( 1 ), 3, HeightThird + 2, { 0, 0,0, 255 } ); //v

	//bottom left
	FilledRectangle( x - w / 2 - ( 1 ), y + h - 4, WidthThird + 2, 3, { 0, 0,0, 255 } ); //h
	FilledRectangle( x - w / 2 - ( 1 ), ( y + h ) - HeightThird - 4, 3, HeightThird + 2, { 0, 0,0, 255 } );//v

	//bottom right
	FilledRectangle( ( x + w / 2 ) - WidthThird - ( 1 ), y + h - 4, WidthThird + 2, 3, { 0, 0,0, 255 } );//h
	FilledRectangle( x + w / 2 - ( 1 ), ( y + h ) - HeightThird - 4, 3, HeightThird + 3, { 0, 0,0, 255 } ); //v

	/* draw eight lines */
	//topleft 
	FilledRectangle( x - w / 2, y, WidthThird, 1, color ); //h
	FilledRectangle( x - w / 2, y, 1, HeightThird, color ); //v

	//topright
	FilledRectangle( ( x + w / 2 ) - WidthThird, y, WidthThird, 1, color ); //h
	FilledRectangle( x + w / 2, y, 1, HeightThird, color ); //v

	//bottom left
	FilledRectangle( x - w / 2, y + h - 3, WidthThird, 1, color ); //h
	FilledRectangle( x - w / 2, ( y + h ) - HeightThird - 3, 1, HeightThird, color );//v

	//bottom right
	FilledRectangle( ( x + w / 2 ) - WidthThird, y + h - 3, WidthThird, 1, color );//h
	FilledRectangle( x + w / 2, ( y + h ) - HeightThird - 3, 1, HeightThird + 1, color ); //v		
}

void BorderBox( int x, int y, int x2, int y2, int thickness, RGBA color )
{
	FilledRectangle( x, y, x2, thickness, color ); // top
	FilledRectangle( x, y + y2, x2, thickness, color ); // bottom
	FilledRectangle( x, y, thickness, y2, color ); // left
	FilledRectangle( x + x2, y, thickness, y2 + thickness, color ); // right
}

void NormalBox( int x, int y, int w, int h, int borderPx, RGBA color )
{
	FilledRectangle( x + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x + w - w + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x, y, borderPx, h, color ); //left 
	FilledRectangle( x, y + h - h + borderPx * 2, borderPx, h, color ); //left 
	FilledRectangle( x + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w - w + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w + borderPx, y, borderPx, h, color );//right 
	FilledRectangle( x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color );//right 
}

void NormalFilledBox( int x, int y, int w, int h, int borderPx, RGBA color )
{
	FilledRectangle( x + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x + w - w + borderPx, y, w, borderPx, color ); //top 
	FilledRectangle( x, y, borderPx, h, color ); //left 
	FilledRectangle( x, y + h - h + borderPx * 2, borderPx, h, color ); //left 
	FilledRectangle( x + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w - w + borderPx, y + h + borderPx, w, borderPx, color ); //bottom 
	FilledRectangle( x + w + borderPx, y, borderPx, h, color );//right 
	FilledRectangle( x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color );//right 
}
//void SideHealthBar( int x, int y, int height, int health, RGBA ColHealth)
//{
//	int box_h = ( height * health ) / 100;
//
//	RGBA ShiledBlackBar = { 0 , 0 , 0 , 255 };
//
//	ShiledBlackBar = { 0 , 0 , 0 , 255 };
//
//
//	if ( health > 0 )
//	{
//		FilledRectangle( x + 7 + 1, y, 4 + 1, height + 2, RGBA { 0, 0, 0, 255 } );
//		FilledRectangle( x + 7 + 1, y, 4 + 1, height + 2, RGBA { 0, 0, 0, 255 } );
//		FilledRectangle( x + 7, y, 4, box_h, ColHealth );
//	}
//
//
//}
//
void OutlinedText( ImFont *pFont, const std::string &text, const ImVec2 &pos, float size, RGBA color, bool center )
{
	if ( !pFont )
		return;

	std::stringstream stream( text );
	std::string line;


	float y = 0.0f;
	int i = 0;

	ImGui::PushFont( pFont );
	while ( std::getline( stream, line ) )
	{
		ImVec2 textSize = pFont->CalcTextSizeA( size, FLT_MAX, 0.0f, line.c_str() );

		if ( center )
		{

			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x - textSize.x / 2.0f ) + 1, ( pos.y + textSize.y * i ) + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x - textSize.x / 2.0f ) - 1, ( pos.y + textSize.y * i ) - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			//	ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			//	ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( pos.x - textSize.x / 2.0f, pos.y + textSize.y * i ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), line.c_str() );
		}
		else
		{//
			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x ) + 1, ( pos.y + textSize.y * i ) + 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( ( pos.x ) - 1, ( pos.y + textSize.y * i ) - 1 ), ImGui::GetColorU32( ImVec4( 0, 0, 0, 255 ) ), line.c_str() );
			//	ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				//ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText( pFont, size, ImVec2( pos.x, pos.y + textSize.y * i ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), line.c_str() );

		}

		y = pos.y + textSize.y * ( i + 1 );
		i++;
	}
	ImGui::PopFont();
	//return y;
}



void DrawNewTextImGui( int x, int y, RGBA color, const char *str )
{
	if ( x != 0 && y != 0 )
	{


		ImFont a;
		std::string utf_8_1 = std::string( str );
		std::string utf_8_2 = string_To_UTF8( utf_8_1 );
		ImGui::GetOverlayDrawList()->AddText( ImVec2( x, y ), ImGui::ColorConvertFloat4ToU32( ImVec4( color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0 ) ), utf_8_2.c_str() );
	}
}


ImGuiWindow &createscene()
{
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0 );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
	ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0, 0, 0, 0 ) );
	ImGui::Begin( xorstr( "##createscene" ), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar );
	auto &io = ImGui::GetIO();
	ImGui::SetWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
	ImGui::SetWindowSize( ImVec2( io.DisplaySize.x, io.DisplaySize.y ), ImGuiCond_Always );
	return *ImGui::GetCurrentWindow();
}

VOID destroyscene( ImGuiWindow &window )
{

	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar( 2 );
	ImGui::Render();
}

ImGuiStyle *dst;
ImFont *icons = nullptr;
ImFont *big = nullptr;
ImFont *bigger = nullptr;
ImFont *g_font = nullptr;

bool Tab( const char *icon, const char *label, const ImVec2 &size_arg, const bool selected )
{


	ImGuiWindow *window = ImGui::GetCurrentWindow();
	if ( window->SkipItems )
		return false;

	static float sizeplus = 0.f;

	ImGuiContext &g = *GImGui;
	const ImGuiStyle &style = g.Style;
	const ImGuiID id = window->GetID( label );
	const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );

	ImVec2 pos = window->DC.CursorPos;

	ImVec2 size = ImGui::CalcItemSize( size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f );

	const ImRect bb( pos, ImVec2( pos.x + size.x, pos.y + size.y ) );
	ImGui::ItemSize( size, style.FramePadding.y );
	if ( !ImGui::ItemAdd( bb, id ) )
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );

	auto animcolor3 = ImColor( 27, 27, 27, 255 );
	auto animcolor2 = ImColor( 255, 255, 255, 255 );
	auto animcolor = ImColor( 24, 24, 24, 255 );

	window->DrawList->AddRectFilled( { bb.Min.x,bb.Max.y }, { bb.Max.x,bb.Min.y }, animcolor );

	if ( selected )
		window->DrawList->AddRectFilled( { bb.Min.x,bb.Max.y }, { bb.Min.x + 2,bb.Min.y }, animcolor2 );

	if ( selected )
		window->DrawList->AddRectFilled( { bb.Min.x + 2,bb.Max.y }, { bb.Min.x + 148,bb.Min.y }, animcolor3 );

	ImGui::PushFont( big );

	if ( selected )
		window->DrawList->AddText( { bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize( label ).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize( label ).y / 2 }, ImColor( 225 / 255.f, 225 / 255.f, 225 / 255.f, 255.f / 255.f ), label );

	if ( !selected )
		window->DrawList->AddText( { bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize( label ).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize( label ).y / 2 }, ImColor( 75 / 255.f, 75 / 255.f, 75 / 255.f, 255.f / 255.f ), label );


	ImGui::PopFont();

	return pressed;
}
bool Radar = true;
bool teamRadar = false;
bool enemyRadar = true;
int xAxis_Radar = 150;
int yAxis_Radar = 150;
int radartype = 1;
int width_Radar = 250;
int height_Radar = 250;
float distance_Radar = 400.0f;

static D3DXVECTOR3 RotatePoint( D3DXVECTOR3 EntityPos, D3DXVECTOR3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool *viewCheck )
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -( EntityPos.y - LocalPlayerPos.y );
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * ( float ) ( M_PI / 180.0F );
	x_1 = ( float ) ( r_2 * ( float ) cos( ( double ) ( yawToRadian ) ) - r_1 * sin( ( double ) ( yawToRadian ) ) ) / 20;
	y_1 = ( float ) ( r_2 * ( float ) sin( ( double ) ( yawToRadian ) ) + r_1 * cos( ( double ) ( yawToRadian ) ) ) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if ( x_1 < 5 )
		x_1 = 5;

	if ( x_1 > sizeX - 5 )
		x_1 = sizeX - 5;

	if ( y_1 < 5 )
		y_1 = 5;

	if ( y_1 > sizeY - 5 )
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return D3DXVECTOR3( x_1, y_1, 0 );
}


void DrawRadarPoint( D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist, int xAxis, int yAxis, int width, int height, D3DXCOLOR color )
{
	bool out = false;
	D3DXVECTOR3 siz;
	siz.x = width;
	siz.y = height;
	D3DXVECTOR3 pos;
	pos.x = xAxis;
	pos.y = yAxis;
	bool ck = false;

	//FilledRectangle( pos.x, pos.y, siz.x, siz.y, { 0,0,0, 43 } );

	D3DXVECTOR3 single = RotatePoint( EneamyPos, LocalPos, pos.x, pos.y, siz.x, siz.y, LocalPlayerY, 2.f, &ck );
	if ( eneamyDist >= 0.f && eneamyDist < distance_Radar )
	{
		//if (radartype == 0)
		//	Drawing::DrawOutlinedText(font, std::to_string((int)eneamyDist), ImVec2(single.x, single.y), 11, { 255, 255, 255, 255 }, true);
		//else
		FilledRectangle( single.x, single.y, 7, 7, { 255, 255, 255, 255 } );

	}
}

bool menu = true;

bool firstS = false;

void pkRadar( D3DXVECTOR3 EneamyPos, D3DXVECTOR3 LocalPos, float LocalPlayerY, float eneamyDist )
{
	ImGuiStyle *style = &ImGui::GetStyle();
	style->WindowRounding = 0.2f;
	ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.13529413f, 0.14705884f, 0.15490198f, 0.82f ) );
	ImGuiWindowFlags TargetFlags;
	if ( menu )
		TargetFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	else
		TargetFlags = ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;

	if ( !firstS )
	{
		ImGui::SetNextWindowPos( ImVec2 { 1200, 60 }, ImGuiCond_Once );
		firstS = true;
	}

	ImGui::SetNextWindowSize( { 250, 250 } );
	ImGui::Begin( xorstr( "Radar" ), 0, TargetFlags );
	//if (ImGui::Begin(xorstr("Radar", 0, ImVec2(200, 200), -1.f, TargetFlags))) {
	{
		ImDrawList *Draw = ImGui::GetOverlayDrawList();
		ImVec2 DrawPos = ImGui::GetCursorScreenPos();
		ImVec2 DrawSize = ImGui::GetContentRegionAvail();
		ImVec2 midRadar = ImVec2( DrawPos.x + ( DrawSize.x / 2 ), DrawPos.y + ( DrawSize.y / 2 ) );
		ImGui::GetWindowDrawList()->AddLine( ImVec2( midRadar.x - DrawSize.x / 2.f, midRadar.y ), ImVec2( midRadar.x + DrawSize.x / 2.f, midRadar.y ), IM_COL32( 255, 255, 255, 255 ) );
		ImGui::GetWindowDrawList()->AddLine( ImVec2( midRadar.x, midRadar.y - DrawSize.y / 2.f ), ImVec2( midRadar.x, midRadar.y + DrawSize.y / 2.f ), IM_COL32( 255, 255, 255, 255 ) );

		DrawRadarPoint( EneamyPos, LocalPos, LocalPlayerY, eneamyDist, DrawPos.x, DrawPos.y, DrawSize.x, DrawSize.y, { 255, 255, 255, 255 } );

		ImGui::End();
	}
	ImGui::PopStyleColor();
}