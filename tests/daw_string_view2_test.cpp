// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#include <daw/daw_benchmark.h>
#include <daw/daw_string_view2.h>
#include <daw/daw_utility.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace daw::sv2::string_view_literals;
#define FALSE( b ) ( !( b ) )
namespace daw {

	enum class tmp_e { a, b, c };

	constexpr bool is_equal_nc( daw::sv2::string_view lhs,
	                            daw::sv2::string_view rhs ) noexcept {
		if( lhs.size( ) != rhs.size( ) ) {
			return false;
		}
		bool result = true;
		for( size_t n = 0; n < lhs.size( ); ++n ) {
			result &= ( lhs[n] | ' ' ) == ( rhs[n] | ' ' );
		}
		return result;
	}

	constexpr tmp_e tmp_e_from_str( daw::sv2::string_view str ) {
		if( is_equal_nc( str, "a" ) ) {
			return tmp_e::a;
		}
		if( is_equal_nc( str, "b" ) ) {
			return tmp_e::b;
		}
		if( is_equal_nc( str, "c" ) ) {
			return tmp_e::c;
		}
#if defined( DAW_USE_EXCEPTIONS )
		throw std::runtime_error( "unknown http request method" );
#else
		std::cerr << "Unknown http request method\n";
		std::abort( );
#endif
	}

	constexpr daw::sv2::string_view do_something( daw::sv2::string_view str,
	                                              tmp_e &result ) {
		str = str.substr( 0, str.find_first_of( ' ' ) );
		result = tmp_e_from_str( str );

		return str;
	}

	constexpr void daw_string_view_constexpr_001( ) {
		daw::sv2::string_view a = "A test";
		tmp_e result = tmp_e::b;
		auto str = do_something( a, result );
		::Unused( str );
		daw::expecting( result == tmp_e::a );
	}

	void daw_string_view_find_last_of_001( ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		std::string_view const b = "abcdefghijklm";
		auto const pos = a.find_last_of( "ij" );
		auto const pos2 = b.find_last_of( "ij" );
		daw::expecting( pos, pos2 );

		auto const es = a.find_last_of( "lm" );
		auto const es2 = b.find_last_of( "lm" );
		daw::expecting( es, es2 );
	}

	void daw_string_view_find_last_of_002( ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( "", 0 );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_of_003( ) {
		auto a = daw::sv2::string_view( );
		auto const pos = a.find_last_of( "a" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_of_004( ) {
		auto a = daw::sv2::string_view( "this is a test" );
		auto const pos = a.find_last_of( "a", 5 );
		daw::expecting( 8U, pos );
	}

	constexpr void
	daw_string_view_find_last_of_005( daw::sv2::string_view needle ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( needle );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	constexpr void
	daw_string_view_find_last_of_006( daw::sv2::string_view needle ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( needle, 1000U );
		daw::expecting( 0U, pos );
	}

	constexpr void
	daw_string_view_find_last_of_007( daw::sv2::string_view needle ) {
		auto a = daw::sv2::string_view( "abcdefghijklm" );
		auto const pos = a.find_last_of( needle, a.size( ) - 1 );
		daw::expecting( 0U, pos );
	}

	void daw_string_view_find_first_of_if_001( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of_if( []( auto c ) { return c == 'c'; } );
		daw::expecting( 2U, pos );
	}

	void daw_string_view_find_first_of_if_002( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of_if( []( auto c ) { return c == 'c'; }, 1000 );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_if_003( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of_if( []( auto c ) { return c == 'x'; } );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_not_of_if_001( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_not_of_if( []( auto c ) { return c < 'c'; } );
		daw::expecting( 2U, pos );
	}

	void daw_string_view_find_first_not_of_if_002( ) {
		daw::sv2::string_view const a = "";
		auto pos = a.find_first_not_of_if( []( auto c ) { return c < 'c'; } );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_not_of_if_003( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_not_of_if( []( auto c ) { return c < 'x'; } );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_001( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "def" );
		daw::expecting( 3U, pos );
	}

	void daw_string_view_find_first_of_002( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "def", 100 );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_003( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_of_004( ) {
		daw::sv2::string_view const a = "abcdefghijklm";
		auto pos = a.find_first_of( "x" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_first_not_of_001( ) {
		daw::sv2::string_view const a = "abcabfghijklm";
		auto pos = a.find_first_not_of( "abc" );
		daw::expecting( 5U, pos );
	}

	void daw_string_view_find_first_not_of_002( ) {
		daw::sv2::string_view const a = "abcabfghijklm";
		auto pos = a.find_first_not_of( "abc", 1000 );
		daw::expecting( daw::sv2::string_view::npos, pos );
		pos = a.find_first_not_of( "" );
		daw::expecting( 0U, pos );
		pos = a.find_first_not_of( "abc", 4U );
		daw::expecting( 5U, pos );
		pos = a.find_first_not_of( a );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_not_of_001( ) {
		std::string_view const str = "abcabfghijklm";
		daw::sv2::string_view const sv = "abcabfghijklm";
		auto pos = str.find_last_not_of( "abc" );
		auto pos_sv = sv.find_last_not_of( "abc" );
		daw::expecting( pos, pos_sv );
	}

	void daw_string_view_find_last_not_of_002( ) {
		std::string_view const str = "abcabfghijklmabc";
		daw::sv2::string_view const sv = str;

		for( std::size_t n = 0; n < sv.size( ); ++n ) {
			auto pos = str.find_last_not_of( "abc", n );
			auto pos_sv = sv.find_last_not_of( "abc", n );
			daw::expecting( pos, pos_sv );
		}
	}

	void daw_string_view_find_last_not_of_003( ) {
		std::string_view const str = "abcabfghijklmabc";
		daw::sv2::string_view const sv = str;
		daw::sv2::string_view const needle = "abc";

		for( std::size_t n = 0; n < sv.size( ); ++n ) {
			auto pos = str.find_last_not_of( "abc", n );
			auto pos_sv = sv.find_last_not_of( needle, n );
			daw::expecting( pos, pos_sv );
		}
	}

	void daw_string_view_find_last_not_of_004( ) {
		daw::sv2::string_view const sv{ };
		auto pos = sv.find_last_not_of( "a" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_find_last_not_of_005( ) {
		daw::sv2::string_view const sv{ "dfsdfsdfsd" };
		auto pos = sv.find_last_not_of( "" );
		daw::expecting( 9U, pos );
	}

	void daw_string_view_find_last_not_of_if_001( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv =
		  sv.find_last_not_of_if( []( char c ) { return std::isspace( c ) != 0; } );
		daw::expecting( 13U, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_002( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv = sv.find_last_not_of_if(
		  []( char c ) { return std::isspace( c ) != 0; }, 6 );
		daw::expecting( 5U, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_003( ) {
		daw::sv2::string_view const sv = "";
		auto pos_sv =
		  sv.find_last_not_of_if( []( char c ) { return std::isspace( c ) != 0; } );
		daw::expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_004( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv = sv.find_last_not_of_if( []( char ) { return false; } );
		daw::expecting( sv.size( ) - 1, pos_sv );
	}

	void daw_string_view_find_last_not_of_if_005( ) {
		daw::sv2::string_view const sv = "abcabf ghijklm     \n";
		auto pos_sv = sv.find_last_not_of_if( []( char ) { return true; } );
		daw::expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_string_view_search_001( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search( "aaa" );
		daw::expecting( 5U, pos );
	}

	void daw_string_view_search_002( ) {
		daw::sv2::string_view const a = "";
		auto pos = a.search( "aaa" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_003( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search( "xdf" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_last_001( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa" );
		daw::expecting( 5U, pos );
	}

	void daw_string_view_search_last_002( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa", 3 );
		daw::expecting( 5U, pos );
	}

	void daw_string_view_search_last_003( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_last_004( ) {
		daw::sv2::string_view const a = "";
		auto pos = a.search_last( "a" );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void daw_string_view_search_last_005( ) {
		daw::sv2::string_view const a = "abcdeaaaijklm";
		auto pos = a.search_last( "aaa", 100 );
		daw::expecting( daw::sv2::string_view::npos, pos );
	}

	void tc001( ) {
		daw::sv2::string_view view;
		puts( "Constructs an empty string" );

		{ daw::expecting( view.empty( ) ); }
		puts( "Is 0 bytes in size" );

		{ daw::expecting( view.size( ) == 0 ); }

		puts( "Points to null" );

		{ daw::expecting( view.data( ) == nullptr ); }
	}

	//----------------------------------------------------------------------------

	void tc002( ) {
		std::string str = "Hello world";
		daw::sv2::string_view view = str;

		puts( "Constructs a non-empty string" );
		{ daw::expecting( FALSE( view.empty( ) ) ); }

		puts( "Has non-zero size" );
		{ daw::expecting( view.size( ) != 0 ); }

		puts( "Points to original string" );
		{ daw::expecting( view.data( ) == str.data( ) ); }
	}
	//----------------------------------------------------------------------------

	void tc003( ) {
		auto empty_str = "";
		auto non_empty_str = "Hello World";

		puts( "Is empty with empty string" );
		{
			daw::sv2::string_view view = empty_str;

			daw::expecting( view.empty( ) );
		}

		puts( "Is non-empty with non-empty string" );
		{
			daw::sv2::string_view view = non_empty_str;

			daw::expecting( FALSE( view.empty( ) ) );
		}

		puts( "Is size 0 with empty string" );
		{
			daw::sv2::string_view view = empty_str;

			daw::expecting( view.size( ) == 0 );
		}

		puts( "Is not size 0 with non-empty string" );
		{
			daw::sv2::string_view view = non_empty_str;

			daw::expecting( view.size( ) != 0 );
		}

		puts( "Points to original string" );
		{
			daw::sv2::string_view view = non_empty_str;

			daw::expecting( view.data( ) == non_empty_str );
		}
	}

	//----------------------------------------------------------------------------

	void tc004( ) {
		daw::sv2::string_view empty = "";
		daw::sv2::string_view view = "Hello world";

		puts( "Is empty with empty string" );
		{ daw::expecting( empty.empty( ) ); }

		puts( "Is non-empty with non-empty string" );
		{ daw::expecting( FALSE( view.empty( ) ) ); }

		puts( "Is size 0 with empty string" );
		{ daw::expecting( empty.size( ) == 0 ); }

		puts( "Is not size 0 with non-empty string" );
		{ daw::expecting( view.size( ) != 0 ); }
	}

	//----------------------------------------------------------------------------
	// Capacity
	//----------------------------------------------------------------------------

	void tc004capacity( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns non-zero for non-empty string" );
		{ daw::expecting( view.size( ) == std::char_traits<char>::length( str ) ); }

		puts( "Returns 0 for empty string" );
		{
			view = "";

			daw::expecting( view.size( ) == 0 );
		}
	}

	//----------------------------------------------------------------------------

	void tc005capacity( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns non-zero for non-empty string" );
		{
			daw::expecting( view.length( ) == std::char_traits<char>::length( str ) );
		}

		puts( "Returns 0 for empty string" );
		{
			view = "";

			daw::expecting( view.length( ) == 0 );
		}
	}

	//----------------------------------------------------------------------------

	void tc006capacity( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns false on non-empty string" );
		{ daw::expecting( FALSE( view.empty( ) ) ); }

		puts( "Returns true on empty string" );
		{
			view = "";

			daw::expecting( view.empty( ) );
		}
	}

	//----------------------------------------------------------------------------
	// Element Access
	//----------------------------------------------------------------------------

	void tc007accessor( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Points to original data source" );
		{ daw::expecting( view.data( ) == str ); }
	}

	//----------------------------------------------------------------------------

	void tc008accessor( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Points to original data source" );
		{ daw::expecting( view.data( ) == str ); }
	}

	//----------------------------------------------------------------------------

	void tc009accessor( ) {
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns reference to entry at position" );
		{ daw::expecting( &view[0] == str ); }
	}

	//----------------------------------------------------------------------------

	void tc010accessor( ) {
#if defined( DAW_USE_EXCEPTIONS )
		const char *str = "Hello World";
		daw::sv2::string_view view = str;

		puts( "Returns reference to entry at position" );
		{ daw::expecting( &view.at( 0 ) == str ); }

		puts( "Throws when out of range" );
		bool good = false;
		try {
			(void)view.at( 11 );
		} catch( std::out_of_range const & ) { good = true; } catch( ... ) {
		}
		if( not good ) {
			puts( "Expected out of range exception" );
			std::abort( );
		}
#endif
	}

	//----------------------------------------------------------------------------

	void tc011accessor( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Returns reference to first character" );
		{ daw::expecting( view.front( ) == 'H' ); }
	}

	//----------------------------------------------------------------------------

	void tc012accessor( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Returns reference to last character" );
		{ daw::expecting( view.back( ) == 'd' ); }
	}

	//----------------------------------------------------------------------------
	// Modifiers
	//----------------------------------------------------------------------------

	void tc013modifier( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Removes first n characters" );
		{
			view.remove_prefix( 6 );
			auto result = view == "World";
			daw::expecting( result );
		}
	}

	//----------------------------------------------------------------------------

	void tc014modifier( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Removes last n characters" );
		{
			view.remove_suffix( 6 );

			daw::expecting( view, "Hello" );
			daw::expecting( view == "Hello" );
		}
	}

	//----------------------------------------------------------------------------

	void tc015modifier( ) {
		const char *str1 = "Hello World";
		const char *str2 = "Goodbye World";
		daw::sv2::string_view view1 = str1;
		daw::sv2::string_view view2 = str2;

		puts( "Swaps entries" );
		{
			std::swap( view1, view2 );

			daw::expecting(
			  ( ( view1.data( ) == str2 ) && ( view2.data( ) == str1 ) ) );
		}
	}

	//----------------------------------------------------------------------------
	// String Operations
	//----------------------------------------------------------------------------
	void tc016conversion( ) {
		daw::sv2::string_view view = "Hello World";

		std::string string = static_cast<std::string>( view );

		puts( "Copies view to new location in std::string" );
		{ daw::expecting( view.data( ) != string.data( ) ); }

		puts( "Copied string contains same contents as view" );
		{ daw::expecting( string == "Hello World" ); }
	}
	//----------------------------------------------------------------------------

	void tc017conversion( ) {
		daw::sv2::string_view view = "Hello World";

		std::string string = static_cast<std::string>( view );

		puts( "Copies view to new location in std::string" );
		{ daw::expecting( view.data( ) != string.data( ) ); }

		puts( "Copied string contains same contents as view" );
		{ daw::expecting( string == "Hello World" ); }
	}

	//----------------------------------------------------------------------------
	// Operations
	//----------------------------------------------------------------------------

	void tc018operation( ) {
#if defined( DAW_USE_EXCEPTIONS )
		daw::sv2::string_view const view = "Hello World";

		puts( "Throws std::out_of_range if pos >= view.size()" );
		{
			char result[12];

			bool good = false;
			try {
				view.copy( result, 12, 12 );
			} catch( std::out_of_range const & ) { good = true; } catch( ... ) {
			}
			if( not good ) {
				puts( "Expected out_of_range_exception" );
				std::abort( );
			}
		}

		puts( "Copies entire string" );
		{
			char result[11];
			view.copy( result, 11 );

			daw::expecting( std::strncmp( result, "Hello World", 11 ) == 0 );
		}

		puts( "Copies remaining characters if count > size" );
		{
			char result[11];
			view.copy( result, 20 );

			daw::expecting( std::strncmp( result, "Hello World", 11 ) == 0 );
		}

		puts( "Copies part of the string" );
		{
			char result[11];
			view.copy( result, 5 );

			daw::expecting( std::strncmp( result, "Hello", 5 ) == 0 );
		}

		puts( "Copies part of the string, offset from the beginning" );
		{
			char result[11];
			view.copy( result, 10, 6 );

			daw::expecting( std::strncmp( result, "World", 5 ) == 0 );
		}

		puts( "Returns number of characters copied" );
		{
			char result[11];

			daw::expecting( view.copy( result, 20 ) == 11 );
		}
#endif
	}

	//----------------------------------------------------------------------------

	void tc019operation( ) {
#if defined( DAW_USE_EXCEPTIONS )
		daw::sv2::string_view view = "Hello World";

		puts( "Returns the full string when given no args" );
		{
			auto substr = view.substr( );
			daw::expecting( substr == "Hello World" );
		}

		puts( "Returns last part of string" );
		{
			auto substr = view.substr( 6 );
			daw::expecting( substr == "World" );
		}

		puts( "Substring returns at most count characters" );
		{
			auto substr = view.substr( 6, 1 );
			daw::expecting( substr, "W" );
			daw::expecting( substr == "W" );
		}

		puts( "Returns up to end of string if length > size" );
		{
			auto substr = view.substr( 6, 10 );
			daw::expecting( substr == "World" );
		}

		puts( "Throws std::out_of_range if pos > size" );

		bool good = false;
		try {
			(void)view.substr( 15 );
		} catch( std::out_of_range const & ) { good = true; } catch( ... ) {
		}
		if( not good ) {
			puts( "Expected out_of_range_exception" );
			std::abort( );
		}
#endif
	}

	//----------------------------------------------------------------------------

	void tc020comparison( ) {
		puts( "Returns 0 for identical views" );
		{
			daw::sv2::string_view view = "Hello World";

			daw::expecting( view.compare( "Hello World" ) == 0 );
		}

		puts( "Returns nonzero for different views" );
		{
			daw::sv2::string_view view = "Hello World";

			daw::expecting( view.compare( "Goodbye World" ) != 0 );
		}

		puts( "Returns > 0 for substring beginning with same string" );
		{
			daw::sv2::string_view view = "Hello World";

			daw::expecting( view.compare( "Hello" ) > 0 );
		}

		puts( "Returns < 0 for superstring beginning with self" );
		{
			daw::sv2::string_view view = "Hello";

			daw::expecting( view.compare( "Hello World" ) < 0 );
		}

		puts(
		  "Returns < 0 for same-sized string compared to character greater than "
		  "char" );
		{
			daw::sv2::string_view view = "1234567";

			daw::expecting( view.compare( "1234667" ) < 0 );
		}

		puts(
		  "Returns > 0 for same-sized string compared to character less than "
		  "char" );
		{
			daw::sv2::string_view view = "1234567";

			daw::expecting( view.compare( "1234467" ) > 0 );
		}
	}

	//----------------------------------------------------------------------------

	void tc021comparison( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Is equal" );
		{
			puts( "Returns true for equal string views" );
			{
				daw::sv2::string_view view2 = "Hello World";

				daw::expecting( view == view2 );
			}

			puts( "Returns true for equal strings with left char array" );
			{ daw::expecting( "Hello World" == view ); }

			puts( "Returns true for equal strings with right char array" );
			{ daw::expecting( view == "Hello World" ); }

			puts( "Returns true for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( str == view );
			}

			puts( "Returns true for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( view == str );
			}

			puts( "Returns true for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( str == view );
			}

			puts( "Returns true for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( view == str );
			}
		}

		puts( "Is not equal" );
		{
			puts( "Returns false for non-equal string views" );
			{
				daw::sv2::string_view view2 = "Goodbye World";

				daw::expecting( FALSE( view == view2 ) );
			}

			puts( "Returns false for non-equal strings with left char array" );
			{ daw::expecting( FALSE( ( "Goodbye World" == view ) ) ); }

			puts( "Returns false for non-equal strings with right char array" );
			{ daw::expecting( FALSE( ( view == "Goodbye World" ) ) ); }

			puts( "Returns false for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( FALSE( str == view ) );
			}

			puts( "Returns false for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( FALSE( view == str ) );
			}

			puts( "Returns false for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( FALSE( str == view ) );
			}

			puts( "Returns false for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( FALSE( view == str ) );
			}

			puts( "Two unequal string_views of size 1" );
			{
				std::string str1 = "1";
				std::string str2 = "2";
				daw::sv2::string_view sv1{ str1 };
				daw::sv2::string_view sv2{ str2 };
				daw::expecting( FALSE( sv1 == sv2 ) );
			}
			puts( "Two equal string_views of size 1" );
			{
				std::string str1 = "1";
				std::string str2 = "1";
				daw::sv2::string_view sv1{ str1 };
				daw::sv2::string_view sv2{ str2 };
				daw::expecting( sv1, sv2 );
			}
		}
	}

	//----------------------------------------------------------------------------

	void tc022comparison( ) {
		daw::sv2::string_view view = "Hello World";

		puts( "Is equal" );
		{
			puts( "Returns false for equal string views" );
			{
				daw::sv2::string_view view2 = "Hello World";

				daw::expecting( FALSE( view != view2 ) );
			}

			puts( "Returns false for equal strings with left char array" );
			{ daw::expecting( FALSE( "Hello World" != view ) ); }

			puts( "Returns false for equal strings with right char array" );
			{ daw::expecting( FALSE( view != "Hello World" ) ); }

			puts( "Returns false for equal strings with left char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( FALSE( str != view ) );
			}

			puts( "Returns false for equal strings with right char ptr" );
			{
				const char *str = "Hello World";
				daw::expecting( FALSE( view != str ) );
			}

			puts( "Returns false for equal strings with left std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( FALSE( str != view ) );
			}

			puts( "Returns false for equal strings with right std::string" );
			{
				std::string str = "Hello World";
				daw::expecting( FALSE( view != str ) );
			}
		}

		puts( "Is not equal" );
		{
			puts( "Returns true for non-equal string views" );
			{
				daw::sv2::string_view view2 = "Goodbye World";

				daw::expecting( view != view2 );
			}

			puts( "Returns true for non-equal strings with left char array" );
			{ daw::expecting( "Goodbye World" != view ); }

			puts( "Returns true for non-equal strings with right char array" );
			{ daw::expecting( view != "Goodbye World" ); }

			puts( "Returns true for non-equal strings with left char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( str != view );
			}

			puts( "Returns true for non-equal strings with right char ptr" );
			{
				const char *str = "Goodbye World";
				daw::expecting( view != str );
			}

			puts( "Returns true for non-equal strings with left std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( str != view );
			}

			puts( "Returns true for non-equal strings with right std::string" );
			{
				std::string str = "Goodbye World";
				daw::expecting( view != str );
			}
		}
	}

	void daw_can_be_string_view_starts_with_001( ) {
		daw::expecting(
		  daw::sv2::string_view{ "This is a test" }.starts_with( "This" ) );
	}

	void daw_can_be_string_view_starts_with_002( ) {
		daw::expecting( daw::sv2::string_view{ "This is a test" }.starts_with(
		  daw::sv2::string_view{ "This" } ) );
	}

	void daw_can_be_string_view_starts_with_003( ) {
		daw::expecting(
		  daw::sv2::string_view{ "This is a test" }.starts_with( 'T' ) );
	}

	void daw_can_be_string_view_starts_with_004( ) {
		daw::expecting(
		  not daw::sv2::string_view{ "This is a test" }.starts_with( "ahis" ) );
	}

	void daw_can_be_string_view_starts_with_005( ) {
		daw::expecting( not daw::sv2::string_view{ "This is a test" }.starts_with(
		  daw::sv2::string_view{ "ahis" } ) );
	}

	void daw_can_be_string_view_starts_with_006( ) {
		daw::expecting(
		  not daw::sv2::string_view{ "This is a test" }.starts_with( 'a' ) );
	}

	void daw_can_be_string_view_starts_with_007( ) {
		daw::expecting( not daw::sv2::string_view{ }.starts_with( 'a' ) );
	}

	void daw_can_be_string_view_starts_with_008( ) {
		daw::expecting( not daw::sv2::string_view{ }.starts_with( " " ) );
	}

	void daw_can_be_string_view_starts_with_009( ) {
		daw::expecting( not daw::sv2::string_view{ " " }.starts_with( "    " ) );
	}

	void daw_can_be_string_view_ends_with_001( ) {
		daw::expecting(
		  daw::sv2::string_view{ "This is a test" }.ends_with( "test" ) );
	}

	void daw_can_be_string_view_ends_with_002( ) {
		daw::expecting( daw::sv2::string_view{ "This is a test" }.ends_with(
		  daw::sv2::string_view{ "test" } ) );
	}

	void daw_can_be_string_view_ends_with_003( ) {
		daw::expecting(
		  daw::sv2::string_view{ "This is a test" }.ends_with( 't' ) );
	}

	void daw_can_be_string_view_ends_with_004( ) {
		daw::expecting(
		  not daw::sv2::string_view{ "This is a test" }.ends_with( "aest" ) );
	}

	void daw_can_be_string_view_ends_with_005( ) {
		daw::expecting( not daw::sv2::string_view{ "This is a test" }.ends_with(
		  daw::sv2::string_view{ "aest" } ) );
	}

	void daw_can_be_string_view_ends_with_006( ) {
		daw::expecting(
		  not daw::sv2::string_view{ "This is a test" }.ends_with( 'a' ) );
	}

	void daw_can_be_string_view_ends_with_007( ) {
		daw::expecting( not daw::sv2::string_view{ "a" }.ends_with( "hello" ) );
	}

	void daw_can_be_string_view_ends_with_008( ) {
		daw::expecting( not daw::sv2::string_view{ }.ends_with( ' ' ) );
	}

	void daw_can_be_string_view_ends_with_009( ) {
		daw::expecting( not daw::sv2::string_view{ }.ends_with( " " ) );
	}

	void daw_pop_front_until_sv_test_001( ) {
		std::string_view str = "This is a test";
		auto sv = daw::sv2::string_view( str.data( ), str.size( ) );
		daw::expecting( "This"_sv, sv.pop_front_until( " " ) );
		daw::expecting( "is"_sv, sv.pop_front_until( " " ) );
		daw::expecting( "a"_sv, sv.pop_front_until( " " ) );
		daw::expecting( "test"_sv, sv.pop_front_until( " " ) );
		daw::expecting( sv.empty( ) );
	}

	void daw_pop_front_test_001( ) {
		daw::sv2::string_view sv = "This is a test";
		auto result = sv.pop_front( );
		daw::expecting( result, 'T' );
	}

	void daw_pop_front_count_test_001( ) {
		daw::sv2::string_view sv = "This is a test";
		auto result = sv.pop_front( 4 );
		daw::expecting( result, "This" );
	}

	void daw_pop_back_count_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto result = sv.pop_back( 4 );
		daw::expecting( result, "test" );
		daw::expecting( sv, "This is a " );
	}

	void daw_pop_back_until_sv_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		daw::expecting( "test"_sv, sv.pop_back_until( " " ) );
		daw::expecting( "a"_sv, sv.pop_back_until( " " ) );
		daw::expecting( "is"_sv, sv.pop_back_until( " " ) );
		daw::expecting( "This"_sv, sv.pop_back_until( " " ) );
		daw::expecting( sv.empty( ) );
	}

	void daw_pop_back_until_sv_test_002( ) {
		daw::sv2::string_view sv = "This is a test";
		auto result = sv.pop_back_until( []( char c ) { return c == 'x'; } );
		daw::expecting_message( sv.empty( ), "Expected empty result" );
		daw::expecting_message( result.end( ) == sv.end( ),
		                        "Expected same end( )" );
	}

	void daw_pop_front_pred_test_001( ) {
		std::string_view str = "This is1a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto lhs = sv.pop_front_until( []( auto c ) { return std::isdigit( c ); } );
		daw::expecting( lhs, "This is" );
		daw::expecting( sv, "a test" );
	}

	void daw_pop_back_pred_test_001( ) {
		daw::sv2::string_view sv = "This is1a test";
		auto result =
		  sv.pop_back_until( []( auto c ) { return std::isdigit( c ); } );
		daw::expecting( "This is", sv );
		daw::expecting( "a test", result );
	}

	void daw_try_pop_back_until_sv_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		daw::expecting( sv.try_pop_back_until( " " ), "test" );
		daw::expecting( sv.try_pop_back_until( " " ), "a" );
		daw::expecting( sv.try_pop_back_until( " " ), "is" );
		daw::expecting( sv.try_pop_back_until( " " ), "" );
		daw::expecting( sv, "This" );
	}

	void daw_try_pop_back_until_sv_test_002( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto result = sv.try_pop_back_until( "blah" );
		daw::expecting( result.empty( ) );
		daw::expecting( sv == str );
	}

	void daw_try_pop_front_until_sv_test_001( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		daw::expecting( sv.try_pop_front_until( " " ), "This" );
		daw::expecting( sv.try_pop_front_until( " " ), "is" );
		daw::expecting( sv.try_pop_front_until( " " ), "a" );
		daw::expecting( sv.try_pop_front_until( " " ) != "test" );
		daw::expecting( sv, "test" );
	}

	void daw_try_pop_front_until_sv_test_002( ) {
		std::string_view str = "This is a test";
		daw::sv2::string_view sv{ str.data( ), str.size( ) };
		auto result = sv.try_pop_front_until( "blah" );
		daw::expecting( result.empty( ) );
		daw::expecting( sv == str );
		daw::sv2::string_view s{ };
		s.remove_prefix( );
	}

// Workaround
#if not defined( _MSC_VER ) or defined( __clang__ )
	static_assert(
	  decltype( sv2::basic_string_view( "This is a test" ) )::extent == 14 );

	template<size_t N>
	constexpr bool extent_test_001( char const ( &str )[N] ) noexcept {
		sv2::basic_string_view sv( str );
		return decltype( sv )::extent >= 0;
	}
	void daw_extent_test_001( ) {
		daw::expecting( extent_test_001( "this is a test" ) );
	}
	static_assert( extent_test_001( "this is a test" ) );

	template<size_t N>
	constexpr bool extent_to_dynamic_001( char const ( &str )[N] ) noexcept {
		sv2::basic_string_view sv( str );
		sv2::string_view svb{ };
		svb = sv;
		(void)svb;
		return decltype( sv )::extent >= 0 and
		       decltype( svb )::extent == daw::dynamic_string_size;
	}
	static_assert( extent_to_dynamic_001( "Testing testing 1 2 3" ) );
	void daw_extent_to_dynamic_test_001( ) {
		daw::expecting( extent_to_dynamic_001( "Testing testing 1 2 3" ) );
	}
#endif

	constexpr bool ensure_same_at_ct( ) {
		constexpr daw::sv2::string_view sv_cx = "a";
		static_assert( sv_cx.size( ) == 1 );
		static_assert( not sv_cx.empty( ) );
		static_assert( *( sv_cx.data( ) +
		                  static_cast<std::ptrdiff_t>( sv_cx.size( ) ) ) == '\0' );
		static_assert( sv_cx.end( ) - sv_cx.begin( ) == 1 );
		daw::sv2::string_view sv_run = "a";
		if( sv_run != sv_cx ) {
			std::cerr << "Runtime/Constexpr not the same\n";
			std::cerr << "Should have size 1 and not be empty\n";
			std::cerr << "size( ) = " << sv_cx.size( );
			std::cerr << " empty( ) = " << sv_cx.empty( ) << '\n';
			return false;
		}
		return true;
	}

	void daw_diff_assignment_001( ) {
#if not defined( _MSC_VER ) or defined( __clang__ )
		daw::sv2::basic_string_view a = "This is a test";
		daw::sv2::string_view b = "Hello";
		daw::expecting_message( a != b, "Expected equal" );
		static_assert( not std::is_same_v<decltype( a ), decltype( b )> );
		// Should have different types
		b = a;
		daw::expecting_message( a == b, "Expected equal" );
#endif
	}

	void daw_literal_test_001( ) {
		auto sv_char = "Hello"_sv;
		static_assert(
		  std::is_same_v<char, typename decltype( sv_char )::value_type> );
		auto sv_wchar_t = L"Hello"_sv;
		static_assert(
		  std::is_same_v<wchar_t, typename decltype( sv_wchar_t )::value_type> );
#if defined( __cpp_char8_t )
		auto sv_char8_t = u8"Hello"_sv;
		static_assert(
		  std::is_same_v<char8_t, typename decltype( sv_char8_t )::value_type> );
#endif
		static_assert(
		  std::is_same_v<wchar_t, typename decltype( sv_wchar_t )::value_type> );
		auto sv_char16_t = u"Hello"_sv;
		static_assert(
		  std::is_same_v<wchar_t, typename decltype( sv_wchar_t )::value_type> );
		static_assert(
		  std::is_same_v<char16_t, typename decltype( sv_char16_t )::value_type> );
		auto sv_char32_t = U"Hello"_sv;
		static_assert(
		  std::is_same_v<char32_t, typename decltype( sv_char32_t )::value_type> );
	}

	void daw_stdhash_test_001( ) {
		std::hash<daw::sv2::string_view> h{ };
		daw::sv2::string_view message = "Hello World!";
		auto hash = h( message );
		daw::expecting( std::uint64_t{ 0x8C0E'C8D1'FB9E'6E32ULL }, hash );
	}

	void daw_generichash_test_001( ) {
		daw::sv2::string_view message = "Hello World!";
		auto hash = daw::sv2::generic_hash( message );
		daw::expecting( std::uint64_t{ 0x8C0E'C8D1'FB9E'6E32ULL }, hash );
	}

	void daw_rfind_test_001( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.rfind( "is" );
		daw::expecting( 5U, pos_sv );
	}

	void daw_rfind_test_002( ) {
		daw::sv2::string_view const sv = "";
		auto pos_sv = sv.rfind( "is" );
		daw::expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_rfind_test_003( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.rfind( "" );
		daw::expecting( 16U, pos_sv );
	}

	void daw_find_test_001( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.find( "is" );
		daw::expecting( 2U, pos_sv );
	}

	void daw_find_test_002( ) {
		daw::sv2::string_view const sv = "";
		auto pos_sv = sv.find( "is" );
		daw::expecting( daw::sv2::string_view::npos, pos_sv );
	}

	void daw_find_test_003( ) {
		daw::sv2::string_view const sv = "This is a string";
		auto pos_sv = sv.find( "" );
		daw::expecting( 0U, pos_sv );
	}
} // namespace daw

int main( ) {
	daw::expecting( daw::ensure_same_at_ct( ) );
	daw::daw_string_view_constexpr_001( );
	daw::daw_string_view_find_last_of_001( );
	daw::daw_string_view_find_last_of_002( );
	daw::daw_string_view_find_last_of_003( );
	daw::daw_string_view_find_last_of_004( );
	daw::sv2::string_view arg = "";
	daw::do_not_optimize( arg );
	daw::daw_string_view_find_last_of_005( arg );
	daw::sv2::string_view arg2 = "a";
	daw::do_not_optimize( arg2 );
	daw::daw_string_view_find_last_of_006( arg2 );
	daw::daw_string_view_find_last_of_007( arg2 );
	daw::daw_string_view_find_first_of_if_001( );
	daw::daw_string_view_find_first_of_if_002( );
	daw::daw_string_view_find_first_of_if_003( );
	daw::daw_string_view_find_first_not_of_if_001( );
	daw::daw_string_view_find_first_not_of_if_002( );
	daw::daw_string_view_find_first_not_of_if_003( );
	daw::daw_string_view_find_first_of_001( );
	daw::daw_string_view_find_first_of_002( );
	daw::daw_string_view_find_first_of_003( );
	daw::daw_string_view_find_first_of_004( );
	daw::daw_string_view_find_first_not_of_001( );
	daw::daw_string_view_find_first_not_of_002( );
	daw::daw_string_view_find_last_not_of_001( );
	daw::daw_string_view_find_last_not_of_002( );
	daw::daw_string_view_find_last_not_of_003( );
	daw::daw_string_view_find_last_not_of_004( );
	daw::daw_string_view_find_last_not_of_005( );
	daw::daw_string_view_find_last_not_of_if_001( );
	daw::daw_string_view_find_last_not_of_if_002( );
	daw::daw_string_view_find_last_not_of_if_003( );
	daw::daw_string_view_find_last_not_of_if_004( );
	daw::daw_string_view_find_last_not_of_if_005( );
	daw::daw_string_view_search_001( );
	daw::daw_string_view_search_002( );
	daw::daw_string_view_search_003( );
	daw::daw_string_view_search_last_001( );
	daw::daw_string_view_search_last_002( );
	daw::daw_string_view_search_last_003( );
	daw::daw_string_view_search_last_004( );
	daw::daw_string_view_search_last_005( );
	daw::tc001( );
	daw::tc002( );
	daw::tc003( );
	daw::tc004( );
	daw::tc004capacity( );
	daw::tc005capacity( );
	daw::tc006capacity( );
	daw::tc007accessor( );
	daw::tc008accessor( );
	daw::tc009accessor( );
	daw::tc010accessor( );
	daw::tc011accessor( );
	daw::tc012accessor( );
	daw::tc013modifier( );
	daw::tc014modifier( );
	daw::tc015modifier( );
	daw::tc016conversion( );
	daw::tc017conversion( );
	daw::tc018operation( );
	daw::tc019operation( );
	daw::tc020comparison( );
	daw::tc021comparison( );
	daw::tc022comparison( );
	daw::daw_can_be_string_view_starts_with_001( );
	daw::daw_can_be_string_view_starts_with_002( );
	daw::daw_can_be_string_view_starts_with_003( );
	daw::daw_can_be_string_view_starts_with_004( );
	daw::daw_can_be_string_view_starts_with_005( );
	daw::daw_can_be_string_view_starts_with_006( );
	daw::daw_can_be_string_view_starts_with_007( );
	daw::daw_can_be_string_view_starts_with_008( );
	daw::daw_can_be_string_view_starts_with_009( );
	daw::daw_can_be_string_view_ends_with_001( );
	daw::daw_can_be_string_view_ends_with_002( );
	daw::daw_can_be_string_view_ends_with_003( );
	daw::daw_can_be_string_view_ends_with_004( );
	daw::daw_can_be_string_view_ends_with_005( );
	daw::daw_can_be_string_view_ends_with_006( );
	daw::daw_can_be_string_view_ends_with_007( );
	daw::daw_can_be_string_view_ends_with_008( );
	daw::daw_can_be_string_view_ends_with_009( );
	daw::daw_pop_front_test_001( );
	daw::daw_pop_front_count_test_001( );
	daw::daw_pop_front_until_sv_test_001( );
	daw::daw_pop_back_count_test_001( );
	daw::daw_pop_back_until_sv_test_001( );
	daw::daw_pop_back_until_sv_test_002( );
	daw::daw_pop_front_pred_test_001( );
	daw::daw_pop_back_pred_test_001( );
	daw::daw_try_pop_back_until_sv_test_001( );
	daw::daw_try_pop_back_until_sv_test_002( );
	daw::daw_try_pop_front_until_sv_test_001( );
	daw::daw_try_pop_front_until_sv_test_002( );
	daw::daw_diff_assignment_001( );
	daw::daw_literal_test_001( );
	daw::daw_stdhash_test_001( );
	daw::daw_generichash_test_001( );
	daw::daw_rfind_test_001( );
	daw::daw_rfind_test_002( );
	daw::daw_rfind_test_003( );
	daw::daw_find_test_001( );
	daw::daw_find_test_002( );
	daw::daw_find_test_003( );
#if not defined( _MSC_VER ) or defined( __clang__ )
	daw::daw_extent_to_dynamic_test_001( );
	daw::daw_extent_test_001( );
#endif
}
