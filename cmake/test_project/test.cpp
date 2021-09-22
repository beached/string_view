// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#include <daw/daw_string_view2.h>

#include <cstdio>

int main( int argc, char ** argv ) {
	auto sv = daw::string_view2( argv[0] );
	if( !sv.empty( ) ) {
		std::puts( "All good\n" );
	} else {
		std::puts( "Empty argc[0]\n" );
	}
}

