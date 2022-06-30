/*
          ___           ___           ___
         /\  \         /\  \         /\  \
        /::\  \       /::\  \        \:\  \
       /:/\:\  \     /:/\:\  \        \:\  \
      /::\~\:\  \   /::\~\:\  \       /::\  \
     /:/\:\ \:\__\ /:/\:\ \:\__\     /:/\:\__\
     \:\~\:\ \/__/ \/__\:\ \/__/    /:/  \/__/
      \:\ \:\__\        \:\__\     /:/  /
       \:\ \/__/         \/__/     \/__/
        \:\__\
         \/__/
         
*/

#include <cheat/internal/core.hpp>

#define E

auto DllMain( void *, std::uint32_t call_reason, void * ) -> bool
{
    if ( call_reason != 1 )
        return false;
    //    AllocConsole();
    //freopen( "conout$", "w", stdout );
    //freopen( "conin$", "r", stdin );
    //SetConsoleTitleA( "Tarkov Internal Debug Window" );
    //::SetWindowPos( GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
    //::ShowWindow( GetConsoleWindow(), SW_NORMAL );
    mono_lib::init();
    {
        if ( !c_swapchain() )
            return false;
    }


    //auto klass = mono_lib::find_class( "Skin", "Diz.Skinning" );
    //if ( !klass )
    //    printf( "nah it don't.\n" );
    //
    //mono_lib::class_get_method_from_name( klass, "get_SkinnedMeshRenderer", -1 );

    //if ( mono_lib::did_find_be_fat( ) )
    //    MessageBox( NULL, "in beater", "no way", MB_OK );
    //else
    //    MessageBox( NULL, "yelling at brb", "not a way", MB_OK );

    return true;
}
