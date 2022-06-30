#pragma once
#include <impl/render/present.h>
#include <impl/hooking/memory_hook.h>
#include <cheat/internal/updater/updater.hpp>


memory_hook::memhook_context *present_hook;
memory_hook::memhook_context *gui_hook;
memory_hook::memhook_context *chams_hook;
memory_hook::memhook_context *fov_hook;
memory_hook::memhook_context *apply_shot;

auto ricochet() -> bool
{
	//std::cout << "isRicochet->called = true" << std::endl;;
	return true;
}
inline unity::c_asset_bundle *bundle;
unity::c_shader *shader;


auto hk_get_skinned_mesh_renderer( void* instance ) -> unity::c_renderer*
{
	//const auto render = c_chams( )
	unity::c_renderer *render = c_chams( instance );


	do
	{
		if ( !render )
			break;

		if ( !bundle )
		{
			std::uintptr_t data = mono_lib::array_new( mono_lib::get_root_domain( ), mono_lib::get_byte_class( ), sizeof( outline ) );

			for ( auto i = 0; i < sizeof( outline ); i++ )
			{
				auto __p = reinterpret_cast< std::uint8_t * >( mono_lib::array_addr_with_size( data, sizeof( std::uint8_t ), i ) );
				*__p = outline [ i ];
			}

			bundle = unity::c_asset_bundle::load_from_memory( data );
		}

		unity::c_material *material = render->get_material( );
		if ( !material )
			break;

		shader = bundle->load_asset<unity::c_shader *>( E( L"outline" ), mono_lib::type_object( E( "UnityEngine" ), E( "Shader" ) ) );

		//std::cout << shader << std::endl;

		if ( shader )
		{
			material->set_shader( shader );
			material->set_color<color_t>( ( L"_FirstOutlineColor" ), { 255,255,255, 255 } );
			material->set_float( L"_FirstOutlineWidth", 0.03f );
			material->set_color<color_t>( ( L"_SecondOutlineColor" ), { 65,105,225, 255 } );
			material->set_float( L"_SecondOutlineWidth", 0.0030f );

		}

	} while ( 0 );
	
	return render;
}

auto hk_set_fov( void *instance, float x, float time, bool applyFovOnCamera ) -> void
{
	x += cfg::CameraFOV;
	return FOVUpdater( instance, x, time, applyFovOnCamera );
}
auto hook_gui_label1( void *instance ) -> void
{
	const static auto set_session_id = reinterpret_cast< void( * )( void *, managed_system::c_string ) >( mono_lib::method( E( "EFT.UI" ), E( "PreloaderUI" ), E( "SetSessionId" ), 1 ) );
	if (!set_session_id )
		return GUIText( instance );

	set_session_id( instance, E( L"Veil EFT Internal" ) );


	return GUIText( instance );
}


auto c_swapchain() -> bool
{
	auto level = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC Description;
	IDXGISwapChain *swap_chain = nullptr;
	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *context = nullptr;

	HWND Window = FindWindow( "UnityWndClass", NULL );

	if ( !Window )
	{
		return false;
	}

	NtGetAsyncKeyState = ( keyword ) lazy_import( GetProcAddress )( lazy_import( GetModuleHandleA )( xorstr( "win32u.dll" ) ), xorstr( "NtUserGetAsyncKeyState" ) );  //getasynckeystate hijacked on nt
	ZeroMemory( &Description, sizeof Description );

	Description.BufferCount = 1;
	Description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Description.OutputWindow = Window;
	Description.SampleDesc.Count = 1;
	Description.Windowed = TRUE;
	Description.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Description.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	lazy_import( D3D11CreateDeviceAndSwapChain )( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &level, 1, D3D11_SDK_VERSION, &Description, &swap_chain, &device, nullptr, &context );

	uint64_t *swap_chain_vtable = *( uint64_t ** ) swap_chain;
	MONO_METHOD_DIRECT( skin, "Diz.Skinning", "Skin", "get_SkinnedMeshRenderer", -1, void * );
//	printf( "THING: %p\n", skin );

//	printf( "THING: %p\n", skin );
	chams_hook = memory_hook::create_context( std::uintptr_t( skin ) );
	c_chams = chams_hook->hook( hk_get_skinned_mesh_renderer );
	chams_hook->enable_hook( );

	//if ( skin )
	//{
	//	printf( "THING: %p\n", skin );
	//	chams_hook = memory_hook::create_context( std::uintptr_t( skin ) );
	//	c_chams = chams_hook->hook( hk_get_skinned_mesh_renderer );
	//	chams_hook->enable_hook( );
	//}

	// EFT.Player.ApplyShot
	//MONO_METHOD( app_shot, "EFT::Player.ApplyShot()", -1, std::uintptr_t * );

	
	//mono_lib::find_class( "Diz.Skinning.Skin" );

	//std::cout << skin << std::endl;

	MONO_OBFUSCATED_METHOD( set_fov, E( "Assembly-CSharp" ), E( "\\uE771" ), E( "SetFov" ), void * );

	if ( set_fov )
	{
		fov_hook = memory_hook::create_context( std::uintptr_t( set_fov ) );
		FOVUpdater = fov_hook->hook( hk_set_fov );
		fov_hook->enable_hook();
	}

	//MONO_OBFUSCATED_METHOD( initialize_shot, ( "Assembly-CSharp" ), ( "\\uEA5B" ), ( "\\uE004" ), void * );

	//if ( initialize_shot )
	//{
	//	////printf( "INT SHOT IS VALID\n" );

	//	initshot_hook = memory_hook::create_context( std::uintptr_t( initialize_shot ) );
	//	WeaponShot = initshot_hook->hook( hook_initialize_shot );
	//	initshot_hook->enable_hook();
	//}


	gui_hook = memory_hook::create_context( mono_lib::method( E( "EFT.UI" ), E( "PreloaderUI" ), E( "Update" ), -1 ) );
	GUIText = gui_hook->hook( hook_gui_label1 );
	gui_hook->enable_hook();

	std::thread penis( hitmarkerthread );
	penis.detach();

	MONO_METHOD( app_shot, "EFT::Player.ApplyShot()", -1, std::uintptr_t * );

	if ( app_shot )
	{
		apply_shot = memory_hook::create_context( app_shot );
		hookedApplyShot = apply_shot->hook( hk_apply_shot );
		apply_shot->enable_hook();
	}

	present_hook = memory_hook::create_context( swap_chain_vtable[8] );
	c_original = present_hook->hook( pkHook );
	present_hook->enable_hook();

	oriWndProc = ( WNDPROC ) SetWindowLongPtr(Window, GWLP_WNDPROC, ( LONG_PTR ) WndProc );
	return true;
}

auto unloadCheat() -> bool
{


	if ( NtGetAsyncKeyState( VK_DELETE ) & 0x8000 )
	{
		HWND Window = FindWindow( "UnityWndClass", NULL );
		SetWindowLongPtr( Window, GWLP_WNDPROC, ( LONG_PTR ) oriWndProc );
		present_hook->destroy_hook();
		fov_hook->destroy_hook();
		//gui_hook->destroy_hook();
		initshot_hook->destroy_hook();
		apply_shot->destroy_hook();
		return true;
	}
	
	return false;
}