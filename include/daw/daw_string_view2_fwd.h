// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#pragma once

#include <ciso646>
#include <cstddef>

namespace daw {
	namespace sv2 {
		enum class string_view_bounds_type { pointer, size };

#if not defined( DAW_SV_USE_PTRPTR ) and                                       \
  ( defined( DAW_SV_USE_PTRSIZE ) or                                           \
    defined( _MSC_VER ) and not defined( __clang__ ) )
		// MSVC has issues with the second item being a pointer
		inline constexpr string_view_bounds_type default_string_view_bounds_type =
		  string_view_bounds_type::size;
#else
		inline constexpr string_view_bounds_type default_string_view_bounds_type =
		  string_view_bounds_type::pointer;
#endif

		template<typename CharT, string_view_bounds_type BoundsType =
		                           default_string_view_bounds_type>
		struct basic_string_view;

		using string_view = basic_string_view<char>;
		using wstring_view = basic_string_view<wchar_t>;
#if defined( __cpp_char8_t )
		using u8string_view = basic_string_view<char8_t>;
#endif
		using u16string_view = basic_string_view<char16_t>;
		using u32string_view = basic_string_view<char32_t>;
	} // namespace sv2
} // namespace daw
