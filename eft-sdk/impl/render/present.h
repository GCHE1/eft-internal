#pragma once

#include <cheat/internal/core.hpp>
#include <cheat/internal/utility.hpp>
#include <cheat/internal/updater/updater.hpp>
#include <cheat/internal/actor/actors.hpp>

#include <impl/render/font.h>

ID3D11Device *device = nullptr;
ID3D11DeviceContext *immediateContext = nullptr;
ID3D11RenderTargetView *renderTargetView = nullptr;
static IDXGISwapChain *g_pSwapChain;



HRESULT( *presenth )( IDXGISwapChain *swapChain, UINT syncInterval, UINT flags ) = nullptr;
HRESULT( *resizeh )( IDXGISwapChain *swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags ) = nullptr;
WNDPROC oriWndProc = NULL;
HWND hwnd = NULL;

static int selecteditem = 0;

static auto vector_getter = [] ( void *vec, int idx, const char **out_text )
{
	auto &vector = *static_cast< std::vector<std::string>* >( vec );
	if ( idx < 0 || idx >= static_cast< int >( vector.size() ) ) { return false; }
	*out_text = vector.at( idx ).c_str();
	return true;
};

inline bool Combo( const char *label, int *currIndex, std::vector<std::string> &values )
{
	if ( values.empty() ) { return false; }
	return ImGui::Combo( label, currIndex, vector_getter,
		static_cast< void * >( &values ), values.size() );
}

inline bool ListBox( const char *label, int *currIndex, std::vector<std::string> &values, int height_in_items = -1 )
{
	if ( values.empty() ) { return false; }
	return ImGui::ListBox( label, currIndex, vector_getter,
		static_cast< void * >( &values ), values.size(), height_in_items );
}

void pkRender()
{
	ImGuiStyle *style = &ImGui::GetStyle();
	ImVec4 *colors = style->Colors;
	style->WindowRounding = 0;
	style->WindowTitleAlign = ImVec2( 0.01, 0.5 );

	style->GrabRounding = 1;
	style->GrabMinSize = 20;
	style->FrameRounding = 0;
	style->FramePadding = ImVec2( 5, 5 );
	style->TouchExtraPadding = ImVec2( 5, 5 );
	style->WindowPadding = ImVec2( 5, 5 );
	style->DisplaySafeAreaPadding = ImVec2( 5, 5 );
	style->DisplayWindowPadding = ImVec2( 5, 5 );
	
	colors[ImGuiCol_Text] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_TextDisabled] = ImVec4( 0.00f, 0.40f, 0.41f, 1.00f );
	colors[ImGuiCol_WindowBg] = ImVec4( 0.021f, 0.021f, 0.021f, 1.00f );
	colors[ImGuiCol_ChildWindowBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ImGuiCol_PopupBg] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_Border] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ImGuiCol_FrameBg] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.44f, 0.80f, 0.80f, 0.27f );
	colors[ImGuiCol_FrameBgActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_TitleBg] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.00f, 0.00f, 0.00f, 0.54f );
	colors[ImGuiCol_TitleBgActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_MenuBarBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.20f );
	colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.125f, 0.125f, 0.125f, 0.36f );
	colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.125f, 0.125f, 0.125f, 0.36f );
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_CheckMark] = ImVec4( 1.00f, 1.00f, 1.00f, 0.68f );
	colors[ImGuiCol_SliderGrab] = ImVec4( 1.00f, 1.00f, 1.00f, 0.36f );
	colors[ImGuiCol_SliderGrabActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.76f );
	colors[ImGuiCol_Button] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_ButtonHovered] = ImVec4( 0.125f, 0.125f, 0.125f, 0.36f );
	colors[ImGuiCol_ButtonActive] = ImVec4( 0.125f, 0.125f, 0.125f, 1.00f );
	colors[ImGuiCol_Header] = ImVec4( 1.00f, 1.00f, 1.00f, 0.33f );
	colors[ImGuiCol_HeaderHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.42f );
	colors[ImGuiCol_HeaderActive] = ImVec4( 1.00f, 1.00f, 1.00f, 0.54f );
	colors[ImGuiCol_Column] = ImVec4( 0.00f, 0.50f, 0.50f, 0.33f );
	colors[ImGuiCol_ColumnHovered] = ImVec4( 0.00f, 0.50f, 0.50f, 0.47f );
	colors[ImGuiCol_ColumnActive] = ImVec4( 0.00f, 0.70f, 0.70f, 1.00f );
	colors[ImGuiCol_ResizeGrip] = ImVec4( 1.00f, 1.00f, 1.00f, 0.54f );
	colors[ImGuiCol_ResizeGripHovered] = ImVec4( 1.00f, 1.00f, 1.00f, 0.74f );
	colors[ImGuiCol_ResizeGripActive] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotLines] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotLinesHovered] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotHistogram] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.00f, 1.00f, 1.00f, 1.00f );
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4( 0.04f, 0.10f, 0.09f, 0.51f );

	ImGui::StyleColorsClassic();

	ImGui::GetOverlayDrawList()->AddRectFilled( ImVec2( 15, 15 ), ImVec2( 265, 52 ), ImColor( 0, 0, 0, 150 ) );
	ImGui::GetOverlayDrawList()->AddRect( ImVec2( 15, 15 ), ImVec2( 265, 52 ), ImColor( 173, 195, 230, 255 ) );
	ImGui::GetOverlayDrawList()->AddText( ImVec2( 22, 24 ), ImColor( 255, 255, 255 ), ( "  Relapticz harrased my cat" ) );
	
	if ( cfg::fov_circle )
		Circle( Width / 2, Height / 2, cfg::fov, { 255, 255, 255, 255 }, 150 );

	ImGui::SetWindowSize( ImVec2( 398.000, 477.000 ) );



	if (cfg::menu )
	{
		//if ( ImGui::Begin( xor ( "Visual" ), NULL, ImGuiWindowFlags_NoResize ) )
		//{
		//	ImGui::SetWindowSize( ImVec2( 260, 502) );

		//}

		if ( ImGui::Begin( xor ( "Veil" ), NULL, ImGuiWindowFlags_NoResize ) )
		{
			ImGui::SetWindowSize( ImVec2( 398.000, 477.000 ) );

			static int switchTabs;

			if ( ImGui::Button( xor ( "Aimbot" ), ImVec2( 131.0f, 0.0f ) ) )
				switchTabs = 0;
			ImGui::SameLine( 0.0, 2.0f );
			if ( ImGui::Button( xor ( "Visuals" ), ImVec2( 127.0f, 0.0f ) ) )
				switchTabs = 1;
			ImGui::SameLine( 0.0, 2.0f );
			if ( ImGui::Button( xor ( "Miscellaneous" ), ImVec2( 125.0f, 0.0f ) ) )
				switchTabs = 2;

			switch ( switchTabs )
			{
				case 0:
					ImGui::Checkbox( "Enabled", &cfg::aimbot );

					//ImGui::ListBox( "test", &selecteditem, Test, IM_ARRAYSIZE( Test ), 2 );

					if ( cfg::aimbot )
					{
						ImGui::Checkbox( xor ( "Draw FOV" ), &cfg::fov_circle );
						ImGui::Text( xor ( "Field of View" ) );
						ImGui::SliderFloat( xor ( "    " ), &cfg::fov, 1, 500, ( xor ( "%.3g" ) ) );
						ImGui::Combo( xor ( "	" ), &cfg::sel_aimtypes, cfg::aimtypes, IM_ARRAYSIZE( cfg::aimtypes ) );
						ImGui::Combo( xor ( "	 " ), &cfg::sel_bones, cfg::bones, IM_ARRAYSIZE( cfg::bones ) );
						ImGui::Combo( xor ( "	  " ), &cfg::sel_Tracetypes, cfg::Tracetypes, IM_ARRAYSIZE( cfg::Tracetypes ) );
					}
					break;
				case 1:
					//ListBox( "##test", &indexScore, scores );


					if ( ImGui::TreeNode( xor ( "Players" ) ) )
					{
						ImGui::Checkbox( xor ( "Skeleton" ), &cfg::esp_skeleton );
						ImGui::Checkbox( xor ( "Corner Box" ), &cfg::esp_corner );
						ImGui::Checkbox( xor ( "Name" ), &cfg::esp_name );
						ImGui::Checkbox( xor ( "Distance" ), &cfg::esp_distance );
						ImGui::Checkbox( xor ( "Boss Type" ), &cfg::boss_type );

						ImGui::TreePop();
					}

					if ( ImGui::TreeNode( xor ( "Items" ) ) )
					{

		/*				if (ImGui::Button( "Test" ))
						{
							std::string e( "shitballs" );
							scores.push_back( e );
						}*/

						ImGui::Checkbox( xor ( "Corpse" ), &cfg::corpse_esp );
						ImGui::Checkbox( xor ( "All" ), &cfg::item_esp );
						ImGui::TreePop();
					}
					break;

				case 2:
					if ( ImGui::TreeNode( xor ( "Exploits" ) ) )
					{
						ImGui::Checkbox( xor ( "Air Walk" ), &cfg::air_walk );
						ImGui::Checkbox( xor ( "No Recoil" ), &cfg::no_recoil );
						ImGui::Checkbox( xor ( "Instant Aim" ), &cfg::instant_aim );
						ImGui::Checkbox( xor ( "No Sway" ), &cfg::no_sway );
						ImGui::Checkbox( xor ( "Infinite Stamina" ), &cfg::inf_stamina );
						ImGui::Checkbox( xor ( "No Fall Damage" ), &cfg::no_fall_dmg );
						ImGui::Checkbox( xor ( "Fast Reload" ), &cfg::fast_reload );
						ImGui::TreePop();
					}

					if ( ImGui::TreeNode( xor ( "Camera" ) ) )
					{
						ImGui::Text( xor ( "FOV" ) );
						ImGui::SliderFloat( xor ( "		" ), &cfg::CameraFOV, 1, 50, xor( "%.2g" ) );
						ImGui::TreePop();
					}
					break;
			}
		}
		ImGui::End();

	}
}
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) && menu )
	{
		return true;
	}

	return CallWindowProcW(oriWndProc, hWnd, msg, wParam, lParam );
}

bool oncee = false;

memory_hook::memhook_context *initshot_hook;

HRESULT pkHook( IDXGISwapChain *swapchain, UINT sync, UINT flags )
{

	g_pSwapChain = swapchain;

	if ( !device )
	{
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };

		if ( FAILED( g_pSwapChain->GetDevice( IID_PPV_ARGS( &device ) ) ) )
		{
			return 0;
		}

		ID3D11Texture2D *renderTarget = 0;
		ID3D11Texture2D *backBuffer = 0;
		swapchain->GetDevice( __uuidof( device ), ( PVOID * ) &device );
		device->GetImmediateContext( &immediateContext );

		swapchain->GetBuffer( 0, __uuidof( renderTarget ), ( PVOID * ) &renderTarget );
		device->CreateRenderTargetView( renderTarget, nullptr, &renderTargetView );
		renderTarget->Release();
		swapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( PVOID * ) &backBuffer );
		backBuffer->GetDesc( &backBufferDesc );

		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;
		immediateContext->RSGetViewports( &num_viewports, &viewport );
		hwnd = FindWindow( "UnityWndClass", NULL );


		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;


		ImFontConfig font_config;
		font_config.OversampleH = 1; 
		font_config.OversampleV = 1;
		font_config.FontDataOwnedByAtlas = false;
		font_config.PixelSnapH = 1;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x044F, // Cyrillic
			0,
		};	

		m_pFont = io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Arial.ttf", 13.0f );
		//io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Arial.ttf", 13.0f );


		Width = ( float ) backBufferDesc.Width;
		Height = ( float ) backBufferDesc.Height;
		ImGui_ImplDX11_Init( hwnd, device, immediateContext );
		ImGui::CreateContext();
	}

	if ( GetAsyncKeyState( VK_INSERT ) & 1 )
		cfg::menu = !cfg::menu;

	auto &window = createscene();

	if ( cPlayerLoop() )
	{
		render();

		aimbot();
	}
	
	unloadCheat();
	pkRender();

	destroyscene( window ); //Destroy

	immediateContext->OMSetRenderTargets( 1, &renderTargetView, nullptr );
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
	return c_original(swapchain, sync, flags );
}