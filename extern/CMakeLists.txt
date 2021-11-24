# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/utf_range
#

option( DAW_ENABLE_TESTING "Build unit tests" OFF )
if( DAW_ENABLE_TESTING )
	set( DAW_ENABLE_TESTING OFF )
endif( )

include( FetchContent )

FetchContent_Declare(
	daw_header_libraries
	GIT_REPOSITORY https://github.com/beached/header_libraries.git
	GIT_TAG fbc3887c25842e3360b87bae40a4f8e1b1316f0a
)

FetchContent_MakeAvailable(daw_header_libraries)

