#pragma once
#include <cheat/game/sdk.hpp>

#include <chrono>
#include <thread>
#include <iostream>

using namespace SDK;

uint64_t	 unity_player_module;

GameObjectManager *game_object_manager {};
World *world {};

std::vector<uintptr_t> players_list {};
std::vector<uintptr_t> item_list {};
std::vector<SDK::Shot*> bullet_list {};

player_t *player_hit;

uint64_t m_lItemProfile;
uint64_t m_lItemBasicInfo;
uint64_t m_lItemLocalization;
uint64_t m_lItemCoordinates;
uint64_t m_lItemLocationContainer;

struct player_info_t
{
	std::string name;
	D3DXVECTOR2 head_location;
	D3DXVECTOR2 feet_location;
	bone_struct_t player_bones;
};


auto cPlayerLoop() -> bool
{
	unity_player_module = ( DWORD64 ) Utility().GetModuleBase_Wrapper( "UnityPlayer.dll" );
	game_object_manager = Read<GameObjectManager *>( unity_player_module + 0x17F8D28 );


	if ( !game_object_manager )
		return false;

	camera.object = game_object_manager->get_fps_camera();

	if ( !camera.object )
		return false;

	world = game_object_manager->get_game_world();

	if ( !world )
		return false;

	if ( world  && isValidPointer((uintptr_t)world))
	{
		players_list = world->get_player_list();
		item_list = world->get_items_list();
		return true;

	}
}

std::vector<SDK::player_t *>Friendly { };

float distance;

SDK::player_t *findtarget()
{
	float ofov = cfg::fov;
	float nfov = 0;

	SDK::player_t *AimbotTarget {};

	AimbotTarget = 0;

	for ( auto Entity : players_list )
	{
		SDK::player_t *Player = reinterpret_cast< player_t * >( Entity );

		if ( Read<bool>( Entity + 0x07FF ) == true )
			continue;

		if ( !Entity && Player->is_dead() && Player->profile() == 0 && Player->is_local_player())
			continue;

		auto Position = Player->get_bone( bones::HumanHead );

		if ( Position != D3DXVECTOR3( 0, 0, 0 ) )
		{
			nfov = SDK::ScreenToEnemy( Position );
			if ( nfov == 0 )
				continue;

			if ( nfov < ofov )
			{
				ofov = nfov;
				AimbotTarget = Player;
			}
		}
	}

	return AimbotTarget;
}

SDK::player_t *FinalTarget { };

auto aimbot() -> bool
{
	////std::cout << "AIMBOT CALLED !!!!!!!!!" << std::endl;


	if ( !cfg::aimbot )
		return false;

	SDK::player_t *Enemy { };
	Enemy = findtarget();

	if ( Enemy->profile()->get_name() != "" )
	{
		auto HeadPosition = Enemy->get_bone( bones::HumanHead );
		auto MuzzleLocation = players::local->weapon()->get_fireport();
		D3DXVECTOR2 ScreenMuzzle;
		D3DXVECTOR2 ScreenHead;

		//if ( GetAsyncKeyState( VK_HOME ) & 0x8000 )
		//{

		//	Friendly.push_back( Enemy );

		//	
		//}
		//

		RGBA TraceColor = { 255, 223, 0, 255 };


		//for ( SDK::player_t *FriendlyCheck : Friendly )
		//{
		//	if ( FriendlyCheck->profile()->get_name() == Enemy->profile()->get_name() )
		//	{
		//		TraceColor = { 0, 255, 0, 255 };
		//		return true;
		//	}
		//	else
		//	{
		//		TraceColor = { 255, 223, 0, 255 };
		//	}
		//}

		if ( cfg::sel_Tracetypes == 0 )
		{
			if ( world_to_screen( MuzzleLocation, ScreenMuzzle ) )
			{
				if ( world_to_screen( HeadPosition, ScreenHead ) )
				{
					Line( ScreenMuzzle.x, ScreenMuzzle.y, ScreenHead.x, ScreenHead.y, TraceColor, 1 );
				}
			}
		}
		else if ( cfg::sel_Tracetypes == 1 )
		{
			if ( world_to_screen( HeadPosition, ScreenHead ) )
			{
				Line( Width - ( Width / 2 ), Height - Height + 540, ScreenHead.x, ScreenHead.y, TraceColor, 1 );
			}
		}
		else if ( cfg::sel_Tracetypes == 2 )
		{

		}


		if ( cfg::sel_aimtypes == 0 )
		{
			FinalTarget = Enemy;
			return true;
		}

		if (cfg::sel_aimtypes == 1 )
		{
			D3DXVECTOR2 CalculatedAngle = calculate_angle( MuzzleLocation, HeadPosition );

			if ( CalculatedAngle == D3DXVECTOR2( 0, 0 ) )
				return false;

			if ( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 )

			{
				players::local->movement()->set_viewangles( CalculatedAngle );
			}
		}
	}

	return true;
}

#define E

bool hit = false;

std::list<int> upper_part = { bones::HumanNeck, bones::HumanHead };
std::list<int> right_arm = { bones::HumanNeck, bones::HumanRUpperarm, bones::HumanRForearm1, bones::HumanRPalm };
std::list<int> left_arm = { bones::HumanNeck, bones::HumanLUpperarm, bones::HumanLForearm1, bones::HumanLPalm };
std::list<int> spine = { bones::HumanNeck, bones::HumanSpine1, bones::HumanSpine2, bones::HumanPelvis };

std::list<int> lower_right = { bones::HumanPelvis, bones::HumanRThigh1, bones::HumanRCalf, bones::HumanRFoot };
std::list<int> lower_left = { bones::HumanPelvis, bones::HumanLThigh1, bones::HumanLCalf, bones::HumanLFoot };
std::list<std::list<int>> skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };

std::vector<D3DXVECTOR3>hit_point;
std::vector<D3DXVECTOR3>origin_point;
int Animate = 1;


auto render() -> bool
{
	camera.position = Read<D3DXVECTOR3>( camera.object + 0x42C );

	for ( uintptr_t body : item_list )
	{
		if ( !body && cfg::corpse_esp)
			continue;

		uint64_t namePtr = Read<uint64_t>( body + 0x78 );

		if ( !( namePtr ) )
			continue;

		m_lItemProfile = Read<uint64_t>( body + 0x10 );
		if ( !( m_lItemProfile ) )
			continue;

		m_lItemBasicInfo = Read<uint64_t>( m_lItemProfile + 0x30 );
		if ( !( m_lItemBasicInfo ) )
			continue;

		m_lItemLocalization = Read<uint64_t>( m_lItemBasicInfo + 0x30 );
		if ( !( m_lItemLocalization ) )
			continue;

		m_lItemCoordinates = Read<uint64_t>( m_lItemLocalization + 0x8 );
		if ( !( m_lItemCoordinates ) )
			continue;

		m_lItemLocationContainer = Read<uint64_t>( m_lItemCoordinates + 0x38 );
		if ( !( m_lItemLocationContainer ) )
			continue;

		std::string Name = get_unicode_str( namePtr, 30 );

		if ( Name != "55d7217a4bdc2d86028b456d" )
			continue;


		D3DXVECTOR3 location = Read<D3DXVECTOR3>( m_lItemLocationContainer + 0x90 );
		D3DXVECTOR2 location_screen;

		if ( location_screen == D3DXVECTOR2( 0, 0 ) )
			continue;

		if ( world_to_screen( location, location_screen ) )
		{
			OutlinedText( m_pFont, "Corpse", ImVec2( location_screen.x, location_screen.y + 5 ), 14, { 255, 0, 0 , 255 }, true );
		}
	}


	for ( uintptr_t body : item_list )
	{
		if ( !(body) && !cfg::item_esp )
			continue;

		//if ( body == players::local )
		//	continue;

		uint64_t namePtr = Read<uint64_t>( body + 0x78 );

		if ( !( namePtr ) )
			continue;

		m_lItemProfile = Read<uint64_t>( body + 0x10 );
		if ( !( m_lItemProfile ) )
			continue;

		m_lItemBasicInfo = Read<uint64_t>( m_lItemProfile + 0x30 );
		if ( !( m_lItemBasicInfo ) )
			continue;

		m_lItemLocalization = Read<uint64_t>( m_lItemBasicInfo + 0x30 );
		if ( !( m_lItemLocalization ) )
			continue;

		m_lItemCoordinates = Read<uint64_t>( m_lItemLocalization + 0x8 );
		if ( !( m_lItemCoordinates ) )
			continue;

		m_lItemLocationContainer = Read<uint64_t>( m_lItemCoordinates + 0x38 );
		if ( !( m_lItemLocationContainer ) )
			continue;

		D3DXVECTOR3 location = Read<D3DXVECTOR3>( m_lItemLocationContainer + 0x90 );
		D3DXVECTOR2 location_screen;
		std::string Name = get_unicode_str( namePtr, 30 );

		for ( const std::pair<const std::string, std::string> &n : AllItems )
		{
			if ( Name == n.first )
			{

				Name = n.second;

				if ( Name == "Default Inventory" )
					continue;



				if ( world_to_screen( location, location_screen ) )
				{
					OutlinedText( m_pFont, Name, ImVec2( location_screen.x, location_screen.y + 5 ), 14, { 255, 255, 255 , 255 }, true );
				}
			}
		}
	}


	for ( uintptr_t entity : players_list )
	{
		if ( !isValidPointer( entity ) )
			continue;

		if ( !isValidPointer( camera.object ) )
			continue;

		if ( entity != 0 && camera.object )
		{
			player_t *player = reinterpret_cast< player_t * >( entity );

			if ( Read<bool>( entity + 0x07FF ))
			{
				players::local = player;

				if ( cfg::instant_aim )
					players::local->weapon()->instant_aim();

				if (cfg::no_recoil )
					players::local->weapon()->set_no_recoil();
				
				if (cfg::no_sway )
					players::local->weapon()->set_no_sway();
				
				if (cfg::inf_stamina )
					players::local->physical()->set_stamina( 100.0f );
				
				if (cfg::no_fall_dmg )
					players::local->physical()->set_no_fall_damage();
				
				if (cfg::fast_reload )
					players::local->profile()->magazine()->set_fast_reload();

				if ( cfg::air_walk )
					players::local->movement()->fly_hack();


				continue;
			}

			//if ( hit )
			//	printf( "hit" );

			uint64_t temp = camera.object;
			if ( !( temp = *( uint64_t * ) ( temp + 0x30 ) ) || !( temp = *( uint64_t * ) ( temp + 0x18 ) ) )
				return false;

			D3DXMATRIX temp_matrix;
			ReadMemory( ( const void * ) ( temp + 0xDC ), &temp_matrix, sizeof( temp_matrix ) );
			viewMatrix = temp_matrix;

			if ( player->profile()->get_name() == "" )
				continue;

			std::string Name = player->profile()->get_name();

			D3DXVECTOR3 HeadPosition = player->get_bone( bones::HumanHead );
			D3DXVECTOR3 BasePosition = ( player->get_bone( bones::HumanLFoot ) + player->get_bone( bones::HumanRFoot ) ) / 2;
			D3DXVECTOR3 LocalPosition = ( players::local->get_bone( bones::HumanLFoot ) + players::local->get_bone( bones::HumanRFoot ) ) / 2;

			distance = ( int ) ( dist( LocalPosition, BasePosition ) );


			if ( cfg::esp_skeleton )
			{

				D3DXVECTOR3 previous( 0, 0, 0 );
				D3DXVECTOR3 current;
				auto p1 = D3DXVECTOR2( 0, 0 );
				auto c1 = D3DXVECTOR2( 0, 0 );
				auto neckpos = player->get_bone( bones::HumanNeck );
				auto pelvispos = player->get_bone( bones::HumanPelvis );

				for ( auto a : skeleton )
				{
					previous = D3DXVECTOR3( 0, 0, 0 );
					for ( int bone : a )
					{
						current = bone == bones::HumanNeck ? neckpos : ( bone == bones::HumanPelvis ? pelvispos : player->get_bone( bone ) );
						if ( previous.x == 0.f )
						{
							previous = current;
							continue;
						}

						if ( world_to_screen( current, c1 ) )
						{
							if ( world_to_screen( previous, p1 ) )
							{
								Line( p1.x, p1.y, c1.x, c1.y, { 255,255,255,255 }, 1 );
								previous = current;
							}
						}
					}

				}
			}

			//MONO_METHOD( get_PlayerBody, "EFT::Player.get_PlayerBody()", -1, void * );

			//if ( get_PlayerBody )
			//{
			//	uint64_t SlotViews = Read<uint64_t>( reinterpret_cast< uint64_t >( get_PlayerBody ) + 0x50 );
			//	if ( SlotViews )
			//	{
			//		std::cout << "SlotViews" << SlotViews << std::endl;
			//	}
			//}

			D3DXVECTOR3 Origin = ( player->get_bone( bones::HumanLFoot ) + player->get_bone( bones::HumanRFoot ) ) / 2;

			D3DXVECTOR2 ScreenPosition, ScreenHead, HeadScreenPosition;
				
			if ( world_to_screen( BasePosition, ScreenPosition ) )
			{
				if ( world_to_screen( HeadPosition, HeadScreenPosition ) )
				{
					float BoxHeight = HeadScreenPosition.y - ScreenPosition.y;
					float BoxWidth = ( BoxHeight / 2 ) * 1.2f;
					D3DXVECTOR2 screened_left = ScreenPosition - D3DXVECTOR2( BoxWidth / 2, 0 );

					for ( D3DXVECTOR3 StartPosition : origin_point )
					{
						if ( StartPosition == D3DXVECTOR3( 0, 0, 0 ) )
							continue;

						for ( D3DXVECTOR3 EndPostion : hit_point )
						{
							if ( EndPostion == D3DXVECTOR3( 0, 0, 0 ) )
								continue;


							D3DXVECTOR2 BulletOriginPosition;
							D3DXVECTOR2 BulletHitPosition;

							world_to_screen( StartPosition, BulletOriginPosition );
							world_to_screen( EndPostion, BulletHitPosition );

								
							Line( BulletOriginPosition.x, BulletOriginPosition.y, BulletHitPosition.x, BulletHitPosition.y, { 255,0,255, 255 }, 1 );
								
								//OutlinedText( m_pFont, "Bullet", ImVec2( BulletScreenPosition.x, BulletScreenPosition.y ), 16.0f, { 255, 255, 255, 255 }, true );
							
						}
					}

					if ( cfg::esp_distance )
					{

						std::string Text = std::to_string( distance );

						std::string s = xor ( ".000000" );

						std::string::size_type i = Text.find( s );

						if ( i != std::string::npos )
							Text.erase( i, s.length() );

						ImVec2 DistanceTextSize = ImGui::CalcTextSize( Text.c_str() );

						OutlinedText( m_pFont, Text, ImVec2( ScreenPosition.x, ScreenPosition.y + 5 ), 10, { 54, 175, 173 , 255 }, true );
					}
					if ( ShouldShow )
					{
						//std::cout << "render" << std::endl;
						if ( player == player_hit )
						{
							auto Position = HeadScreenPosition;
							Position.y += 1.6;

							DrawNewTextImGui( Position.x, Position.y - ( Animate / 2 ), { 255, 255, 255, 255 }, ( "-" + std::to_string( DamageDone ) ).c_str() );
							Animate++;
						}
					}
					else
					{
						Animate = 1;
					}

					if ( cfg::boss_type )
					{
						if ( player->profile()->is_scav() )
						{
							Name = player->profile()->get_role();
						}
					}
					
					if ( cfg::esp_name )
					{
						ImVec2 PlayerTextSize = ImGui::CalcTextSize( Name.c_str() );
						OutlinedText( m_pFont, Name, ImVec2( HeadScreenPosition.x, HeadScreenPosition.y - 15 ), 10, { 255, 255, 255, 255 }, true );
					}
	
					if ( cfg::esp_corner )
					{
						//player->weapon()->
						
						CornerBox( ScreenPosition.x, ScreenPosition.y, BoxWidth, BoxHeight, 2, { 54, 175, 173, 255 } );
					}
					
				}
			}
		}
	}

	return true;
}


auto hook_initialize_shot( void *instance, void *ammo, int fragmentIndex, int seed, D3DXVECTOR3 origin, D3DXVECTOR3 direction,
						   float initialSpeed, float speed, float bulletMassKg, float bulletDiameterM, float damage, float penetrationPower, 
						   float penetrationChance, float ricochetChance, float fragmentationChance, float deviationChance, int minFragmentsCount, int maxFragmentsCount,
						   void *defaultBallisticCollider, void *randoms, float ballisticCoefficient, SDK::player_t *player, void *weapon, int fireIndex, void *parent ) -> void
{

//	//std::cout << "called INITSHOT hook" << std::endl;

	//if (!cfg::aimbot && cfg::sel_aimtypes != 0)
	//	return WeaponShot( instance, ammo, fragmentIndex, seed, origin, direction, initialSpeed, speed, bulletMassKg, bulletDiameterM, damage, penetrationPower, penetrationChance, ricochetChance, fragmentationChance, deviationChance, minFragmentsCount, maxFragmentsCount, defaultBallisticCollider, randoms, ballisticCoefficient, player, weapon, fireIndex, parent );

	if ( world && FinalTarget != players::local && camera.object != 0 )
	{

		if (FinalTarget )
		{

			auto HeadPosition = FinalTarget->get_bone( bones::HumanHead);

			//NormalizeAngles( HeadPosition );

			if ( HeadPosition != D3DXVECTOR3( 0, 0, 0 ) )
			{
				direction = ( HeadPosition - origin );
			}

		}
	}

//	ricochetChance = -0.00f;

	
	return WeaponShot( instance, ammo, fragmentIndex, seed, origin, direction, initialSpeed, speed, bulletMassKg, bulletDiameterM, damage, penetrationPower, penetrationChance, ricochetChance, fragmentationChance, deviationChance, minFragmentsCount, maxFragmentsCount, defaultBallisticCollider, randoms, ballisticCoefficient, player, weapon, fireIndex, parent );
}

std::uintptr_t game_object, audio_source;


auto resolve_bodypart_type( int type ) -> std::string
{

	switch ( type )
	{
		case 0:
			return " in the Head" ;
		case 1:
			return " in the Chest";
		case 2:
			return " in the Stomach";
		case 3:
			return  " in the Left Arm";
		case 4:
			return  " in the Right Arm";
		case 5:
			return  " in the Left Leg";
		case 6:
			return  " in the Right Leg";
		case 7:
			return  " in Common";
	}
}


auto hk_apply_shot( player_t *instance, hit_info_t *damageInfo, int bodyPartType, void *shotId ) -> std::uintptr_t
{

	auto victim = instance;
	auto victim_name = victim->profile()->get_name();

	auto player = damageInfo->player;
	auto damage_type = damageInfo->type;
	auto damage = damageInfo->damage;

	if ( instance->profile()->is_scav() )
	{
		victim_name = instance->profile()->get_role();
	}

	if ( damage_type == 512 && player == players::local)
	{
		hit = true;
		hit_point.clear();
		origin_point.clear();

		auto part = resolve_bodypart_type( bodyPartType );

		//std::string e( "Hit " );
		//e += victim_name;
		//e += part;
		//e += std::string( " for " );
		//e += damage;
		//e += "damage";
		PlaySoundA( "C:\\quake.wav", NULL, SND_FILENAME | SND_ASYNC );

		hit_point.push_back( damageInfo->hit_point );
		origin_point.push_back( damageInfo->hit_origin );
		
		//scores.push_back( e );
		//std::cout << "Hit " << victim_name << part << " for " << static_cast< int >( damage ) << " damage." << std::endl;
		DamageDone = static_cast< int >( damage );//damage;
		player_hit = instance;
	}

	hit = false;


	return hookedApplyShot( instance, damageInfo, bodyPartType, shotId );

}


auto hitmarkerthread() -> void
{
	for ( ;; )
	{
		if ( world )
		{
			 if (players::local && hit) 
			 {
				 //printf( "ok 1\n" );

				 ShouldShow = true;
				 Sleep( 1500 );
				 ShouldShow = false;
				// printf( "ok 2\n" );

			 }
		}
		//printf( "test" );
	}
	return;
}