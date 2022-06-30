#pragma once
#include "managed_system.hpp"
#pragma once
/*
 * Copyright 2017 - 2018 Justas Masiulis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define JM_XORSTR_DISABLE_AVX_INTRINSICS 1

#include <immintrin.h>
#include <cstdint>
#include <cstddef>
#include <utility>

#include <string>
#include <vector>



#define SAPPHIRE_FUNC( mod, type ) reinterpret_cast< decltype( &type ) >( chloe::get_exported_function( chloe::get_sapphire_module( ##mod ), #type ) )
#define SAPPHIRE_TYPE( mod, type ) reinterpret_cast< type >( chloe::get_exported_function( chloe::get_sapphire_module( ##mod ), ( #type ) ) );

namespace chloe
{
#define contains_record( address, type, field ) ( ( type* )( ( char* )( address ) - ( std::uintptr_t )( & ( (type* ) 0 ) -> field ) ) )
#define loword(l) ((std::uint16_t)(((std::uintptr_t)(l)) & 0xffff))

	struct saph_module_t
	{
		void *base;
		const wchar_t *name;
		size_t size;
		saph_module_t( void *b, const wchar_t *n, size_t s ) : base( b ), name( n ), size( s ) { }
		saph_module_t() : base( 0 ), name( L"" ), size( 0 ) { }
	};

	namespace nt
	{
		struct list_entry
		{
			struct list_entry *Flink;
			struct list_entry *Blink;
		};

		struct unicode_string
		{
			unsigned short Length;
			unsigned short MaximumLength;
			wchar_t *Buffer;
		};

		struct peb_ldr_data
		{
			unsigned long Length;
			unsigned long Initialized;
			const char *SsHandle;
			list_entry InLoadOrderModuleList;
			list_entry InMemoryOrderModuleList;
			list_entry InInitializationOrderModuleList;
		};

		struct peb
		{
			unsigned char   Reserved1[2];
			unsigned char   BeingDebugged;
			unsigned char   Reserved2[1];
			const char *Reserved3[2];
			peb_ldr_data *Ldr;
		};

		struct ldr_data_table_entry
		{
			list_entry InLoadOrderModuleList;
			list_entry InMemoryOrderLinks;
			list_entry InInitializationOrderModuleList;
			void *DllBase;
			void *EntryPoint;
			union
			{
				unsigned long SizeOfImage;
				const char *_dummy;
			};
			unicode_string FullDllName;
			unicode_string BaseDllName;
		};

		struct image_dos_header
		{
			unsigned short e_magic;
			unsigned short e_cblp;
			unsigned short e_cp;
			unsigned short e_crlc;
			unsigned short e_cparhdr;
			unsigned short e_minalloc;
			unsigned short e_maxalloc;
			unsigned short e_ss;
			unsigned short e_sp;
			unsigned short e_csum;
			unsigned short e_ip;
			unsigned short e_cs;
			unsigned short e_lfarlc;
			unsigned short e_ovno;
			unsigned short e_res[4];
			unsigned short e_oemid;
			unsigned short e_oeminfo;
			unsigned short e_res2[10];
			long e_lfanew;
		};

		struct image_file_header
		{
			unsigned short Machine;
			unsigned short NumberOfSections;
			unsigned long TimeDateStamp;
			unsigned long PointerToSymbolTable;
			unsigned long NumberOfSymbols;
			unsigned short SizeOfOptionalHeader;
			unsigned short Characteristics;
		};

		struct image_export_directory
		{
			unsigned long Characteristics;
			unsigned long TimeDateStamp;
			unsigned short MajorVersion;
			unsigned short MinorVersion;
			unsigned long Name;
			unsigned long Base;
			unsigned long NumberOfFunctions;
			unsigned long NumberOfNames;
			unsigned long AddressOfFunctions;
			unsigned long AddressOfNames;
			unsigned long AddressOfNameOrdinals;
		};

		struct image_data_directory
		{
			unsigned long VirtualAddress;
			unsigned long Size;
		};

		struct image_optional_header
		{
			unsigned short Magic;
			unsigned char MajorLinkerVersion;
			unsigned char MinorLinkerVersion;
			unsigned long SizeOfCode;
			unsigned long SizeOfInitializedData;
			unsigned long SizeOfUninitializedData;
			unsigned long AddressOfEntryPoint;
			unsigned long BaseOfCode;
			unsigned long long ImageBase;
			unsigned long SectionAlignment;
			unsigned long FileAlignment;
			unsigned short MajorOperatingSystemVersion;
			unsigned short MinorOperatingSystemVersion;
			unsigned short MajorImageVersion;
			unsigned short MinorImageVersion;
			unsigned short MajorSubsystemVersion;
			unsigned short MinorSubsystemVersion;
			unsigned long Win32VersionValue;
			unsigned long SizeOfImage;
			unsigned long SizeOfHeaders;
			unsigned long CheckSum;
			unsigned short Subsystem;
			unsigned short DllCharacteristics;
			unsigned long long SizeOfStackReserve;
			unsigned long long SizeOfStackCommit;
			unsigned long long SizeOfHeapReserve;
			unsigned long long SizeOfHeapCommit;
			unsigned long LoaderFlags;
			unsigned long NumberOfRvaAndSizes;
			image_data_directory DataDirectory[16];
		};

		struct image_nt_headers
		{
			unsigned long Signature;
			image_file_header FileHeader;
			image_optional_header OptionalHeader;
		};
	}

	inline auto get_sapphire_module( const wchar_t *name ) -> const chloe::saph_module_t
	{

		const chloe::nt::peb *peb = reinterpret_cast< chloe::nt::peb * >( __readgsqword( 0x60 ) );
		if ( !peb )
			return {}; // how the fuck did we even get here?

		const chloe::nt::list_entry head = peb->Ldr->InMemoryOrderModuleList;

		for ( auto curr = head; curr.Flink != &peb->Ldr->InMemoryOrderModuleList; curr = *curr.Flink )
		{
			chloe::nt::ldr_data_table_entry *mod = reinterpret_cast< chloe::nt::ldr_data_table_entry * >( contains_record( curr.Flink, chloe::nt::ldr_data_table_entry, InMemoryOrderLinks ) );

			if ( mod->BaseDllName.Buffer )
			{
				if ( chloe::crt::string::wcsicmp_insensitive( mod->BaseDllName.Buffer, name ) )
				{
					return chloe::saph_module_t { mod->DllBase, mod->BaseDllName.Buffer, mod->SizeOfImage };
				}
			}
		}

		return {};
	}

	inline auto get_exported_function( const chloe::saph_module_t module, const char *function ) -> const void *
	{
		if ( !module.base )
			return {};

		const auto base = reinterpret_cast< std::uintptr_t >( module.base );
		const chloe::nt::image_dos_header *dos_header = reinterpret_cast< chloe::nt::image_dos_header * >( base );
		const chloe::nt::image_nt_headers *nt_header = reinterpret_cast< chloe::nt::image_nt_headers * >( base + dos_header->e_lfanew );

		const chloe::nt::image_data_directory *data_directory = reinterpret_cast< const chloe::nt::image_data_directory * >( &nt_header->OptionalHeader.DataDirectory[0] );
		const chloe::nt::image_export_directory *image_export_directory = reinterpret_cast< chloe::nt::image_export_directory * >( base + data_directory->VirtualAddress );

		const auto *const rva_table = reinterpret_cast< const unsigned long * >( base + image_export_directory->AddressOfFunctions );
		const auto *const ord_table = reinterpret_cast< const unsigned short * >( base + image_export_directory->AddressOfNameOrdinals );

		if ( image_export_directory )
		{
			for ( unsigned int idx { 0 }; idx < image_export_directory->NumberOfNames; idx++ )
			{
				const auto fn_name = reinterpret_cast< const char * >( base + reinterpret_cast< const unsigned long * >( base + image_export_directory->AddressOfNames )[idx] );

				if ( chloe::crt::string::strcmp( fn_name, function ) )
					return reinterpret_cast< void * >( base + ( ( std::uint32_t * ) ( base + image_export_directory->AddressOfFunctions ) )[ord_table[idx]] );
			}
		}

		return { };
	}
}

#define CREATE_TYPE( name, args ) using mono_##name = args; inline mono_##name name;
#define ASSIGN_TYPE( name ) name = SAPPHIRE_TYPE( ( L"mono-2.0-bdwgc.dll" ), mono_##name );

#define MONO_OBFUSCATED_METHOD( fn_name, assembly, klass_name, method, args ) \
	static auto fn_name = reinterpret_cast< args >( mono_lib::get_obfuscated_method( mono_lib::get_obfuscated_class( assembly, klass_name ), method ) )

#define MONO_METHOD( fn_name, name, arg_count, args ) \
	static auto fn_name = reinterpret_cast< args >( mono_lib::method( name, arg_count ) )
//autisismsmsms

#define MONO_METHOD_DIRECT( fn_name, namespace_, class_name, name, arg_count, args ) \
	static auto fn_name = reinterpret_cast< args >( mono_lib::method_without_stupid( namespace_, class_name, name, arg_count ) )


#define MONO_METHOD_ARGS( fn_name, name, arg_count, arg_name, args ) \
	static auto fn_name = reinterpret_cast< args >( mono_lib::arg_method( name, arg_count, arg_name ) )

namespace mono_lib
{
	struct sanitized_method_t { std::string ns, kl, mt; };

	CREATE_TYPE( get_root_domain, std::uintptr_t( * )( ) );
	CREATE_TYPE( class_get_methods, std::uintptr_t( * )( std::uintptr_t, std::uintptr_t * ) );
	CREATE_TYPE( method_get_param_count, int ( * )( std::uintptr_t ) );
	CREATE_TYPE( assembly_get_image, std::uintptr_t( * )( std::uintptr_t ) );
	CREATE_TYPE( object_new, std::uintptr_t( * )( std::uintptr_t, std::uintptr_t ) );
	CREATE_TYPE( class_from_name, std::uintptr_t( * )( std::uintptr_t, const char *, const char * ) );
	CREATE_TYPE( field_static_get_value, std::uintptr_t( * )( std::uintptr_t, std::uintptr_t * ) );
	CREATE_TYPE( class_get_fields, std::uintptr_t( * )( std::uintptr_t, std::uintptr_t * ) );
	CREATE_TYPE( field_get_offset, std::uintptr_t( * )( std::uintptr_t ) );
	CREATE_TYPE( class_get_type, std::uintptr_t( * )( std::uintptr_t ) );
	CREATE_TYPE( type_get_object, std::uintptr_t( * )( std::uintptr_t, std::uintptr_t ) );
	CREATE_TYPE( compile_method, std::uintptr_t( * )( std::uintptr_t ) );
	CREATE_TYPE( assembly_foreach, void( * )( void *, void * ) );
	CREATE_TYPE( class_get_method_from_name, std::uintptr_t( * )( std::uintptr_t, const char *name, int ) );
	CREATE_TYPE( method_signature, std::uintptr_t( * )( std::uintptr_t ) );
	CREATE_TYPE( signature_get_param_count, int( * )( std::uintptr_t ) );
	CREATE_TYPE( method_get_param_names, void( * )( std::uintptr_t, const char ** ) );
	CREATE_TYPE( array_new, std::uintptr_t( * )( std::uintptr_t, std::uintptr_t, int ) );
	CREATE_TYPE( array_addr_with_size, char *( * )( std::uintptr_t, int, std::uintptr_t ) );
	CREATE_TYPE( get_byte_class, std::uintptr_t( * )( ) );
	CREATE_TYPE( image_get_table_info, std::uintptr_t( * )( std::uintptr_t, int ) );
	CREATE_TYPE( table_info_get_rows, int( * )( std::uintptr_t ) );
	CREATE_TYPE( image_get_table_rows, int( * )( std::uintptr_t, int ) );
	CREATE_TYPE( class_get, std::uintptr_t( * )( std::uintptr_t, std::uint32_t ) );
	CREATE_TYPE( class_get_name, const char *( * )( std::uintptr_t ) );
	CREATE_TYPE( class_get_namespace, const char *( * )( std::uintptr_t ) );
	CREATE_TYPE( domain_assembly_open, std::uintptr_t( * )( std::uintptr_t, const char * ) );
	CREATE_TYPE( method_get_name, const char *( * )( std::uintptr_t ) );
	CREATE_TYPE( thread_attach, void( * )( std::uintptr_t ) );
	CREATE_TYPE( jit_info_table_find, std::uintptr_t( * )( std::uintptr_t, std::uintptr_t ) );

	inline void init()
	{
		ASSIGN_TYPE( get_root_domain );
		ASSIGN_TYPE( class_get_methods );
		ASSIGN_TYPE( method_get_param_count );
		ASSIGN_TYPE( assembly_get_image );
		ASSIGN_TYPE( object_new );
		ASSIGN_TYPE( class_from_name );
		ASSIGN_TYPE( field_static_get_value );
		ASSIGN_TYPE( class_get_fields );
		ASSIGN_TYPE( field_get_offset );
		ASSIGN_TYPE( class_get_type );
		ASSIGN_TYPE( type_get_object );
		ASSIGN_TYPE( compile_method );
		ASSIGN_TYPE( assembly_foreach );
		ASSIGN_TYPE( class_get_method_from_name );
		ASSIGN_TYPE( method_signature );
		ASSIGN_TYPE( signature_get_param_count );
		ASSIGN_TYPE( method_get_param_names );
		ASSIGN_TYPE( array_new );
		ASSIGN_TYPE( array_addr_with_size );
		ASSIGN_TYPE( get_byte_class );
		ASSIGN_TYPE( image_get_table_info );
		ASSIGN_TYPE( table_info_get_rows );
		ASSIGN_TYPE( image_get_table_rows );
		ASSIGN_TYPE( class_get );
		ASSIGN_TYPE( class_get_name );
		ASSIGN_TYPE( class_get_namespace );
		ASSIGN_TYPE( domain_assembly_open );
		ASSIGN_TYPE( method_get_name );
		ASSIGN_TYPE( thread_attach );
		ASSIGN_TYPE( jit_info_table_find );

		thread_attach( get_root_domain() );
	}

	inline void _cdecl enumerator( void *callback, std::vector<void *> *vec ) // pushes back every assembly loaded, even ones outside of root domain
	{
		vec->push_back( callback );
	}

	inline auto get_obfuscated_class( const char *assembly_name, const char *klass_name ) -> const std::uintptr_t
	{
		const auto assembly = domain_assembly_open( get_root_domain(), assembly_name );
		if ( !assembly )
			return { };

		const auto img = assembly_get_image( std::uintptr_t( assembly ) );
		if ( !img )
			return { };

		auto type_def = image_get_table_info( img, 2 );
		if ( !type_def )
			return { };

		auto type_def_count = table_info_get_rows( type_def );
		if ( type_def_count <= 0 )
			return { };

		for ( int i = 0; i < type_def_count; i++ )
		{
			auto this_class = class_get( img, 0x02000000 | i + 1 );
			if ( !this_class )
				continue;

			auto name = class_get_name( this_class );
			auto klass_namespace = class_get_namespace( this_class );

			if ( ( std::uint8_t ) name[0] == 0xEE )
			{
				char name_buff[32];
				sprintf_s( name_buff, 32, ( "\\u%04X" ), chloe::crt::string::utf8_to_utf16( ( char * ) name ) );
				name = name_buff;
			}

			if ( chloe::crt::string::strcmp( ( char * ) name, ( char * ) klass_name ) )
			{
				return this_class;
			}
		}

		return { };
	}

	inline auto find_class( const char *klass_name, const char *name_space = "" ) -> const std::uintptr_t
	{
		const auto domain = get_root_domain();

		std::vector<void *> v;
		assembly_foreach( mono_lib::enumerator, &v );

		for ( auto assembly : v )
		{
			if ( !assembly )
				continue;

			const auto img = assembly_get_image( std::uintptr_t( assembly ) );
			if ( !img )
				continue;

			const auto klass = class_from_name( img, name_space, klass_name );
			if ( !klass )
				continue;

			return klass;
		}

		return {};
	}




































































	typedef enum
	{
		MONO_TOKEN_MODULE = 0x00000000,
		MONO_TOKEN_TYPE_REF = 0x01000000,
		MONO_TOKEN_TYPE_DEF = 0x02000000,
		MONO_TOKEN_FIELD_DEF = 0x04000000,
		MONO_TOKEN_METHOD_DEF = 0x06000000,
		MONO_TOKEN_PARAM_DEF = 0x08000000,
		MONO_TOKEN_INTERFACE_IMPL = 0x09000000,
		MONO_TOKEN_MEMBER_REF = 0x0a000000,
		MONO_TOKEN_CUSTOM_ATTRIBUTE = 0x0c000000,
		MONO_TOKEN_PERMISSION = 0x0e000000,
		MONO_TOKEN_SIGNATURE = 0x11000000,
		MONO_TOKEN_EVENT = 0x14000000,
		MONO_TOKEN_PROPERTY = 0x17000000,
		MONO_TOKEN_MODULE_REF = 0x1a000000,
		MONO_TOKEN_TYPE_SPEC = 0x1b000000,
		MONO_TOKEN_ASSEMBLY = 0x20000000,
		MONO_TOKEN_ASSEMBLY_REF = 0x23000000,
		MONO_TOKEN_FILE = 0x26000000,
		MONO_TOKEN_EXPORTED_TYPE = 0x27000000,
		MONO_TOKEN_MANIFEST_RESOURCE = 0x28000000,
		MONO_TOKEN_GENERIC_PARAM = 0x2a000000,
		MONO_TOKEN_METHOD_SPEC = 0x2b000000,

		/*
		 * These do not match metadata tables directly
		 */
		 MONO_TOKEN_STRING = 0x70000000,
		 MONO_TOKEN_NAME = 0x71000000,
		 MONO_TOKEN_BASE_TYPE = 0x72000000
	} MonoTokenType;


	typedef enum
	{
		MONO_TABLE_MODULE,
		MONO_TABLE_TYPEREF,
		MONO_TABLE_TYPEDEF,
		MONO_TABLE_FIELD_POINTER,
		MONO_TABLE_FIELD,
		MONO_TABLE_METHOD_POINTER,
		MONO_TABLE_METHOD,
		MONO_TABLE_PARAM_POINTER,
		MONO_TABLE_PARAM,
		MONO_TABLE_INTERFACEIMPL,
		MONO_TABLE_MEMBERREF, /* 0xa */
		MONO_TABLE_CONSTANT,
		MONO_TABLE_CUSTOMATTRIBUTE,
		MONO_TABLE_FIELDMARSHAL,
		MONO_TABLE_DECLSECURITY,
		MONO_TABLE_CLASSLAYOUT,
		MONO_TABLE_FIELDLAYOUT, /* 0x10 */
		MONO_TABLE_STANDALONESIG,
		MONO_TABLE_EVENTMAP,
		MONO_TABLE_EVENT_POINTER,
		MONO_TABLE_EVENT,
		MONO_TABLE_PROPERTYMAP,
		MONO_TABLE_PROPERTY_POINTER,
		MONO_TABLE_PROPERTY,
		MONO_TABLE_METHODSEMANTICS,
		MONO_TABLE_METHODIMPL,
		MONO_TABLE_MODULEREF, /* 0x1a */
		MONO_TABLE_TYPESPEC,
		MONO_TABLE_IMPLMAP,
		MONO_TABLE_FIELDRVA,
		MONO_TABLE_ENCLOG,
		MONO_TABLE_ENCMAP,
		MONO_TABLE_ASSEMBLY, /* 0x20 */
		MONO_TABLE_ASSEMBLYPROCESSOR,
		MONO_TABLE_ASSEMBLYOS,
		MONO_TABLE_ASSEMBLYREF,
		MONO_TABLE_ASSEMBLYREFPROCESSOR,
		MONO_TABLE_ASSEMBLYREFOS,
		MONO_TABLE_FILE,
		MONO_TABLE_EXPORTEDTYPE,
		MONO_TABLE_MANIFESTRESOURCE,
		MONO_TABLE_NESTEDCLASS,
		MONO_TABLE_GENERICPARAM, /* 0x2a */
		MONO_TABLE_METHODSPEC,
		MONO_TABLE_GENERICPARAMCONSTRAINT,
		MONO_TABLE_UNUSED8,
		MONO_TABLE_UNUSED9,
		MONO_TABLE_UNUSED10,
		/* Portable PDB tables */
		MONO_TABLE_DOCUMENT, /* 0x30 */
		MONO_TABLE_METHODBODY,
		MONO_TABLE_LOCALSCOPE,
		MONO_TABLE_LOCALVARIABLE,
		MONO_TABLE_LOCALCONSTANT,
		MONO_TABLE_IMPORTSCOPE,
		MONO_TABLE_STATEMACHINEMETHOD,
		MONO_TABLE_CUSTOMDEBUGINFORMATION

	#define MONO_TABLE_LAST MONO_TABLE_CUSTOMDEBUGINFORMATION
	#define MONO_TABLE_NUM (MONO_TABLE_LAST + 1)

	} MonoMetaTableEnum;
































































	inline const uintptr_t get_class_from_ptr( uint32_t ptr )
	{
		const auto domain = get_root_domain( );

		std::vector<void *> v;
		assembly_foreach( mono_lib::enumerator, &v );

		for ( auto assembly : v )
		{
			if ( !assembly )
				continue;

			const auto img = assembly_get_image( std::uintptr_t( assembly ) );
			if ( !img )
				continue;

			int total = image_get_table_rows( img, MONO_TABLE_TYPEDEF );
			if ( ptr > ( MONO_TOKEN_TYPE_DEF | total ) )
				continue;

			const auto klass = class_get( img, ptr );
			if ( !klass )
				continue;

			return klass;
		}

		return {};
	}












	static uintptr_t get_class_ptr_by_name( const char *klass_name )
	{
		const auto domain = get_root_domain( );

		std::vector<void *> v;
		assembly_foreach( mono_lib::enumerator, &v );

		for ( auto assembly : v )
		{
			if ( !assembly )
				continue;

			const auto img = assembly_get_image( std::uintptr_t( assembly ) );
			if ( !img )
				continue;

			int i, total;

			total = image_get_table_rows( img, MONO_TABLE_TYPEDEF );
			for ( i = 1; i <= total; ++i )
			{
				uintptr_t klass = class_get( img, MONO_TOKEN_TYPE_DEF | i );
				const char *naem = class_get_name(  klass);
				if ( strcmp( naem, klass_name ) == 0 )
					return klass;
			}
		}

		return 0;
	}


































	inline auto type_object( const char *name_space, const char *name ) -> const std::uintptr_t
	{
		auto klass = find_class( name, name_space );
		return type_get_object( get_root_domain(), class_get_type( klass ) );
	}

	// im sorry this is messy but its a lot cleaner in the end.
	inline auto sanitize_method_name( const std::string name ) -> const sanitized_method_t
	{
		std::string ns, kl, method_name;

		if ( auto ns_break = name.find( ( "::" ) ) )
			ns = name.substr( 0, ns_break );

		if ( name.find( ( "::" ) ) == std::string::npos )
			ns = ( "" );

		if ( name.find( ( "::" ) ) )
		{
			kl = name.substr( name.find( ( "::" ), 2 ) + 2 );
			kl = kl.substr( 0, kl.find( ( "." ), 0 ) );
		}

		if ( name.find( ( "::" ) ) == std::string::npos )
			kl = name.substr( 0, name.find( ( "." ), 0 ) );

		method_name = name.substr( name.find( ( "." ), 0 ) + 1 );

		if ( method_name.find( ( "()" ) ) )
			method_name.erase( method_name.find( ( "()" ) ), 2 );

		return { ns, kl, method_name };
	}

	inline auto method( const std::string name, int argument_number = -1 ) -> const std::uintptr_t
	{
		std::uintptr_t iter {};

		const auto st = sanitize_method_name( name );
		const auto klass = find_class( st.kl.c_str( ), st.ns.c_str( ) );
		if ( !klass )
			return{ };

		const auto method = class_get_method_from_name( klass, st.mt.c_str( ), argument_number );
		if ( !method )
			return { };

		return compile_method( method );
	}

	inline auto method_without_stupid( const std::string namespace_name, const std::string klass_name, const std::string method_name, int argument_number = -1 ) -> const std::uintptr_t
	{
		std::uintptr_t iter {};

		const auto klass = find_class( klass_name.c_str(), namespace_name.c_str() );
		if ( !klass )
			return{ };

		const auto method = class_get_method_from_name( klass, method_name.c_str(), argument_number );
		if ( !method )
			return { };

		return compile_method( method );
	}

	inline auto get_obfuscated_method( const std::uintptr_t klass, const char *method_name ) -> const std::uintptr_t
	{
		std::uintptr_t iter {};

		while ( auto method = class_get_methods( klass, &iter ) )
		{
			auto name = method_get_name( method );

			if ( ( std::uint8_t ) name[0] == 0xEE )
			{
				char name_buff[32];
				sprintf_s( name_buff, 32, ( "\\u%04X" ), chloe::crt::string::utf8_to_utf16( ( char * ) name ) );
				name = name_buff;
			}

			if ( chloe::crt::string::strcmp( name, method_name ) )
			{
				return compile_method( method );
			}
		}

		return {};
	}


	inline auto method( const char *ns, const char *klass_name, const char *name, int argument_number = -1 ) -> const std::uintptr_t
	{
		std::uintptr_t iter {};

		const auto klass = find_class( klass_name, ns );
		if ( !klass )
			return{ };

		const auto method = class_get_method_from_name( klass, name, argument_number );
		if ( !method )
			return { };

		return compile_method( method );
	}

	inline auto arg_method( const std::string name, int argument_number = -1, const char *arg_name = "" ) -> const std::uintptr_t
	{
		std::uintptr_t iter {};

		const auto st = sanitize_method_name( name );
		const auto klass = find_class( st.kl.c_str(), st.ns.c_str() );
		if ( !klass )
			return{ };

		while ( const auto method = class_get_methods( klass, &iter ) )
		{
			const auto signature = method_signature( method );
			if ( !signature )
				continue;

			auto param_count = signature_get_param_count( signature );
			if ( !param_count || param_count < argument_number || param_count > argument_number )
				continue;

			const char *_names[128] = { 0 };

			method_get_param_names( method, _names );

			for ( int i = 0; i < param_count; i++ )
			{
				if ( _names[i] )
				{
					if ( chloe::crt::string::strcmp( _names[i], arg_name ) )
						return compile_method( method );
				}
			}

		}

		return { };
	}
}