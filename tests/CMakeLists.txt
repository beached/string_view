# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/string_view
#
cmake_policy(SET CMP0065 NEW)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#add_subdirectory(extern)
#include(cmake/test_compiler_options.cmake)

include(cmake/test_compiler_options.cmake)

add_library(daw_test INTERFACE)


if (DAW_USE_PACKAGE_MANAGEMENT)
    target_link_libraries(daw_test INTERFACE ${COMPILER_SPECIFIC_LIBS})
else ()
    target_link_libraries(daw_test INTERFACE daw::daw-header-libraries daw::daw-string-view ${COMPILER_SPECIFIC_LIBS})
endif ()


#Allows building all in some IDE's
add_custom_target(full)

add_executable( daw_string_view2_bin daw_string_view2_test.cpp )
target_link_libraries(daw_string_view2_bin PRIVATE daw_test)
add_test( daw_string_view2_test daw_string_view2_bin )
add_dependencies(full daw_string_view2_bin )

