// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#pragma once

#include "daw_string_view2_fwd.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_cpp_feature_check.h>
#include <daw/daw_exception.h>
#include <daw/daw_fnv1a_hash.h>
#include <daw/daw_generic_hash.h>
#include <daw/daw_math.h>
#include <daw/daw_move.h>
#include <daw/daw_swap.h>
#include <daw/daw_traits.h>
#include <daw/impl/daw_string_impl.h>
#include <daw/iterator/daw_back_inserter.h>
#include <daw/iterator/daw_iterator.h>

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <stdexcept>

#include <vector>

namespace daw {
	/*inline*/ namespace sv2 {
		struct nodiscard_t {};
		inline constexpr nodiscard_t nodiscard = nodiscard_t{ };

		template<typename CharT, CharT... needles>
		struct any_of_t {
			inline constexpr bool operator( )( CharT c ) const {
				return ( ( c == needles ) | ... );
			}
		};

		template<auto needle, auto... needles>
		inline static constexpr any_of_t<decltype( needle ), needle, needles...>
		  any_of{ };

		template<typename CharT, CharT... needles>
		struct none_of_t {
			inline constexpr bool operator( )( CharT c ) const {
				return ( ( c != needles ) & ... );
			}
		};

		template<auto needle, auto... needles>
		inline static constexpr none_of_t<decltype( needle ), needle, needles...>
		  none_of{ };

		namespace sv2_details {
			template<typename T>
			constexpr std::size_t find_predicate_result_size( T ) {
				return 1;
			}

			template<typename T>
			constexpr bool is_dynamic_sv_v = T::extent == dynamic_string_size;

			template<typename T>
			using has_datasize_test = typename std::remove_reference<
			  decltype( std::data( std::declval<T const &>( ) ) +
			            std::size( std::declval<T const &>( ) ) )>::type;

			template<typename T>
			using is_sv2_test = typename T::i_am_a_daw_string_view2;

			template<typename T>
			using is_sv2_t = daw::is_detected<is_sv2_test, T>;

			template<typename T, typename CharT>
			struct is_string_view_like
			  : std::conjunction<daw::is_detected<has_datasize_test, T>,
			                     daw::not_trait<is_sv2_t<T>>> {};

			static_assert( daw::is_detected_v<has_datasize_test, std::string> );
			static_assert( daw::not_trait<is_sv2_t<std::string>>::value );
			static_assert( is_string_view_like<std::string, char>::value );

			template<typename T, typename CharT>
			struct is_contigious_range_constructible
			  : std::is_constructible<T, CharT *, std::size_t> {};

			template<typename T, typename PointerType>
			inline constexpr bool is_char_pointer_v =
			  std::is_pointer_v<T> and std::is_convertible_v<T, PointerType>;

		} // namespace sv2_details

		template<typename CharT, string_view_bounds_type BoundsType,
		         std::ptrdiff_t Extent>
		struct basic_string_view {
			using value_type = CharT;
			using pointer = CharT *;
			using const_pointer = std::add_const_t<CharT> *;
			using reference = std::add_lvalue_reference_t<CharT>;
			using const_reference =
			  std::add_lvalue_reference_t<std::add_const_t<CharT>>;
			using const_iterator = const_pointer;
			using iterator = const_iterator;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using i_am_a_daw_string_view2 = void;
			static constexpr std::ptrdiff_t extent = Extent;

			template<typename, string_view_bounds_type, std::ptrdiff_t>
			friend struct ::daw::sv2::basic_string_view;

		private:
			static constexpr auto bp_eq = []( CharT l, CharT r ) { return l == r; };

			template<string_view_bounds_type B>
			static constexpr bool is_last_a_pointer_v =
			  B == string_view_bounds_type::pointer;

			using last_type = std::conditional_t<is_last_a_pointer_v<BoundsType>,
			                                     const_pointer, size_type>;
			static inline constexpr last_type default_last = [] {
				if constexpr( is_last_a_pointer_v<BoundsType> ) {
					return nullptr;
				} else {
					return 0;
				}
			}( );
			using last_difference_type =
			  std::conditional_t<is_last_a_pointer_v<BoundsType>, difference_type,
			                     size_type>;

			template<string_view_bounds_type Bounds>
			static constexpr last_type make_last( const_pointer f,
			                                      const_pointer l ) noexcept {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					(void)f;
					return l;
				} else {
					return static_cast<last_type>( l - f );
				}
			}

			template<string_view_bounds_type Bounds>
			static constexpr last_type make_last( const_pointer f,
			                                      size_type s ) noexcept {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					return f + static_cast<difference_type>( s );
				} else {
					(void)f;
					return s;
				}
			}

			template<string_view_bounds_type Bounds>
			constexpr const_pointer last_pointer( ) const {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					return m_last;
				} else {
					return m_first + static_cast<difference_type>( m_last );
				}
			}

			template<string_view_bounds_type Bounds>
			static constexpr size_type size( const_pointer f, last_type l ) {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					return static_cast<size_type>( l - f );
				} else {
					(void)f;
					return l;
				}
			}

			template<string_view_bounds_type Bounds>
			constexpr void dec_front( size_type n ) {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					m_first += static_cast<difference_type>( n );
				} else {
					m_first += static_cast<difference_type>( n );
					m_last -= n;
				}
			}

			template<string_view_bounds_type Bounds>
			constexpr void dec_back( size_type n ) {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					m_last -= static_cast<difference_type>( n );
				} else {
					m_last -= n;
				}
			}

			const_pointer m_first = nullptr;
			last_type m_last = default_last;

			template<typename ForwardIterator>
			static constexpr ForwardIterator find_not_of( ForwardIterator first,
			                                              ForwardIterator const last,
			                                              basic_string_view sv ) {
				traits::is_forward_access_iterator_test<ForwardIterator>( );
				traits::is_input_iterator_test<ForwardIterator>( );

				while( first != last ) {
					for( size_type n = 0; n < sv.size( ); ++n ) {
						if( *first == sv[0] ) {
							return first;
						}
					}
					++first;
				}
				return last;
			}

		public:
			static constexpr size_type const npos =
			  ( std::numeric_limits<size_type>::max )( );

			// constructors
			constexpr basic_string_view( ) noexcept = default;

			constexpr basic_string_view( std::nullptr_t ) noexcept
			  : m_first( nullptr )
			  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {}

			constexpr basic_string_view( std::nullptr_t, size_type n ) noexcept
			  : m_first( nullptr )
			  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {
				daw::exception::precondition_check(
				  n == 0, "Attempt to create a non-empty range with a nullptr" );
			}

			template<
			  typename CharPtr,
			  std::enable_if_t<sv2_details::is_char_pointer_v<CharPtr, const_pointer>,
			                   std::nullptr_t> = nullptr>
			constexpr basic_string_view( CharPtr s ) noexcept
			  : m_first( s )
			  , m_last(
			      make_last<BoundsType>( s, details::strlen<size_type>( s ) ) ) {}

			constexpr basic_string_view( const_pointer s, size_type count ) noexcept
			  : m_first( s )
			  , m_last( make_last<BoundsType>( s, count ) ) {}

			template<string_view_bounds_type B, difference_type E>
			constexpr basic_string_view( basic_string_view<CharT, B, E> sv,
			                             size_type count ) noexcept
			  : m_first( sv.begin( ) )
			  , m_last( make_last<BoundsType>( sv.begin( ),
			                                   ( std::min )( sv.size( ), count ) ) ) {
			}

			template<
			  typename StringView,
			  std::enable_if_t<sv2_details::is_string_view_like<
			                     daw::remove_cvref_t<StringView>, CharT>::value,
			                   std::nullptr_t> = nullptr>
			constexpr basic_string_view( StringView &&sv ) noexcept
			  : m_first( std::data( sv ) )
			  , m_last( make_last<BoundsType>( m_first, std::size( sv ) ) ) {}

			/***
			 * Construct a string_view from a string literal.  This constructor does
			 * not check for zero termination, but assumes that string_literal[N-1] is
			 * '\0'. It will string a string_view of length N-1
			 * @tparam N One plus length of string range
			 * @param string_literal A string literal to create a view over
			 */
			template<size_type N>
			constexpr basic_string_view( CharT const ( &string_literal )[N] ) noexcept
			  : m_first( string_literal )
			  , m_last( make_last<BoundsType>( string_literal, N - 1 ) ) {}

			template<typename T,
			         std::enable_if_t<sv2_details::is_contigious_range_constructible<
			                            T, CharT>::value,
			                          std::nullptr_t> = nullptr>
			explicit constexpr operator T( ) noexcept(
			  std::is_nothrow_constructible_v<T, CharT *, size_type> ) {
				return T{ data( ), size( ) };
			}

#if not defined( _MSC_VER ) or defined( __clang__ )
			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			constexpr auto
			operator=( basic_string_view<CharT, Bounds, Ex> rhs ) noexcept
			  -> std::enable_if_t<
			    ( sv2_details::is_dynamic_sv_v<basic_string_view> and Ex != extent ),
			    basic_string_view &> {

				m_first = rhs.data( );
				m_last = make_last<BoundsType>( rhs.data( ), rhs.size( ) );
				return *this;
			}
#endif

			constexpr basic_string_view( const_pointer first,
			                             const_pointer last ) noexcept
			  : m_first( first )
			  , m_last( make_last<BoundsType>( first, last ) ) {}

			//
			// END OF constructors
		public:
			template<
			  string_view_bounds_type Bounds, std::ptrdiff_t Ex,
			  std::enable_if_t<( Ex == daw::dynamic_string_size and Ex != Extent ),
			                   std::nullptr_t> = nullptr>
			constexpr operator basic_string_view<CharT, Bounds, Ex>( ) noexcept {
				return { m_first, m_last };
			}

			[[nodiscard]] constexpr const_iterator begin( ) const {
				return m_first;
			}

			[[nodiscard]] constexpr const_iterator cbegin( ) const {
				return m_first;
			}

			[[nodiscard]] constexpr const_iterator end( ) const {
				return last_pointer<BoundsType>( );
			}

			[[nodiscard]] constexpr const_iterator cend( ) const {
				return last_pointer<BoundsType>( );
			}

			[[nodiscard]] constexpr reverse_iterator rbegin( ) const {
				return const_reverse_iterator( end( ) );
			}

			[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const {
				return const_reverse_iterator( cend( ) );
			}

			[[nodiscard]] constexpr reverse_iterator rend( ) const {
				return const_reverse_iterator( begin( ) );
			}

			[[nodiscard]] constexpr const_reverse_iterator crend( ) const {
				return const_reverse_iterator( cbegin( ) );
			}

			[[nodiscard]] constexpr const_reference
			operator[]( size_type pos ) const {
				return m_first[pos];
			}

			[[nodiscard]] constexpr const_reference at( size_type pos ) const {
				daw::exception::precondition_check<std::out_of_range>(
				  pos < size( ), "Attempt to access basic_string_view past end" );
				return operator[]( pos );
			}

			[[nodiscard]] constexpr const_reference front( ) const {
				return *m_first;
			}

			[[nodiscard]] constexpr const_reference back( ) const {
				return *std::prev( end( ) );
			}

			[[nodiscard]] constexpr const_pointer data( ) const {
				return m_first;
			}

			[[nodiscard]] constexpr const_pointer data_end( ) const {
				return end( );
			}

			[[nodiscard]] constexpr size_type size( ) const {
				return size<BoundsType>( m_first, m_last );
			}

			[[nodiscard]] constexpr size_type length( ) const {
				return size( );
			}

			[[nodiscard]] constexpr bool empty( ) const {
				return size( ) == 0;
			}

			[[nodiscard]] constexpr explicit operator bool( ) const {
				return not empty( );
			}

			constexpr void remove_prefix( size_type n ) {
				dec_front<BoundsType>( ( std::min )( n, size( ) ) );
			}

			constexpr void remove_prefix( ) {
				remove_prefix( 1 );
			}

			constexpr void remove_suffix( size_type n ) {
				dec_back<BoundsType>( ( std::min )( n, size( ) ) );
			}

			constexpr void remove_suffix( ) {
				remove_suffix( 1 );
			}

			constexpr void clear( ) {
				m_first = nullptr;
				m_last = make_last<BoundsType>( nullptr, 0 );
			}

			[[nodiscard]] constexpr CharT pop_front( ) {
				auto result = front( );
				remove_prefix( );
				return result;
			}

			/// @brief create a substr of the first count characters and remove them
			/// from beginning
			/// @param count number of characters to remove and return
			/// @return a substr of size count starting at begin
			[[nodiscard]] constexpr basic_string_view pop_front( size_type count ) {
				basic_string_view result = substr( 0, count );
				remove_prefix( count );
				return result;
			}

			/// @brief searches for where, returns substring between front and
			/// where, then pops off the substring
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( basic_string_view where, nodiscard_t ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				return result;
			}

			/// @brief searches for where, returns substring between front and
			/// where, then pops off the substring
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view pop_front_until( CharT where,
			                                                           nodiscard_t ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				return result;
			}

			/// @brief searches for where, returns substring between front and
			/// where, then pops off the substring and the where string
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( basic_string_view where ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				remove_prefix( where.size( ) );
				return result;
			}

			/// @brief searches for where, returns substring between front and
			/// where, then pops off the substring and the where string
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view pop_front_until( CharT where ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				remove_prefix( );
				return result;
			}

			/// @brief creates a substr of the substr from begin to position
			/// reported true by predicate
			/// @tparam UnaryPredicate a unary predicate type that accepts a char
			/// and indicates with true when to stop
			/// @param pred predicate to determine where to split
			/// @return substring from beginning to position marked by predicate
			template<
			  typename UnaryPredicate,
			  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
			                   std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( UnaryPredicate pred, nodiscard_t ) {

				auto pos = find_first_of_if( DAW_MOVE( pred ) );
				return pop_front( pos );
			}

			/// @brief creates a substr of the substr from begin to position
			/// reported true by predicate
			/// @tparam UnaryPredicate a unary predicate type that accepts a char
			/// and indicates with true when to stop
			/// @param pred predicate to determine where to split
			/// @return substring from beginning to position marked by predicate
			template<
			  typename UnaryPredicate,
			  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
			                   std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( UnaryPredicate pred ) {
				auto result = pop_front_until( pred, nodiscard );
				remove_prefix( sv2_details::find_predicate_result_size( pred ) );
				return result;
			}

			[[nodiscard]] constexpr CharT pop_back( ) {
				auto result = back( );
				remove_suffix( );
				return result;
			}

			/// @brief create a substr of the last count characters and remove them
			/// from end
			/// @param count number of characters to remove and return
			/// @return a substr of size count ending at end of string_view
			[[nodiscard]] constexpr basic_string_view pop_back( size_type count ) {
				count = ( std::min )( count, size( ) );
				basic_string_view result = substr( size( ) - count, npos );
				remove_suffix( count );
				return result;
			}

			/// @brief searches for last where, returns substring between where and
			/// end, then pops off the substring
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			pop_back_until( basic_string_view where, nodiscard_t ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				return substr( pos + where.size( ) );
			}

			/// @brief searches for last where, returns substring between where and
			/// end, then pops off the substring
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view pop_back_until( CharT where,
			                                                          nodiscard_t ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				return substr( pos + where.size( ) );
			}

			/// @brief searches for last where, returns substring between where and
			/// end, then pops off the substring and the where string
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			pop_back_until( basic_string_view where ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				auto result = substr( pos + where.size( ) );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for the last where, returns substring between where
			/// and end, then pops off the substring and the where string
			/// @param where CharT to split string on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view pop_back_until( CharT where ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				auto result = substr( pos + 1 );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for last position UnaryPredicate would be true,
			/// returns substring between pred and end, then pops off the substring
			/// and the pred specified string
			/// @tparam UnaryPredicate a unary predicate type that accepts a char
			/// and indicates with true when to stop
			/// @param pred predicate to determine where to split
			/// @return substring from last position marked by predicate to end
			template<
			  typename UnaryPredicate,
			  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
			                   std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr basic_string_view
			pop_back_until( UnaryPredicate pred ) {

				auto pos = find_last_of_if( DAW_MOVE( pred ) );
				if( pos == npos ) {
					auto result = *this;
					remove_prefix( npos );
					return result;
				}
				auto result =
				  substr( pos + sv2_details::find_predicate_result_size( pred ) );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for where, returns substring between front and
			/// where, then pops off the substring and the where string. Do nothing
			/// if where is not found
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view
			try_pop_front_until( basic_string_view where ) {
				auto pos = find( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = pop_front( pos );
				remove_prefix( where.size( ) );
				return result;
			}

			/// @brief searches for last where, returns substring between where and
			/// end, then pops off the substring and the where string.  If where is
			/// not found, nothing is done
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			try_pop_back_until( basic_string_view where ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = substr( pos + where.size( ) );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for where, and disregards everything until the end
			/// of that
			/// @param where string to find and consume
			/// @return substring with everything up until the end of where removed
			[[nodiscard]] constexpr basic_string_view &
			remove_prefix_until( basic_string_view where ) {
				auto pos = find( where );
				remove_prefix( pos );
				remove_prefix( where.size( ) );
				return *this;
			}

			constexpr void resize( size_type n ) {
				daw::exception::precondition_check<std::out_of_range>( n < size( ) );
				m_last = std::next( m_first, static_cast<ptrdiff_t>( n ) );
			}

			constexpr size_type copy( pointer dest, size_type count,
			                          size_type pos ) const {
				daw::exception::precondition_check<std::out_of_range>(
				  pos <= size( ), "Attempt to access basic_string_view past end" );

				size_type const rlen = ( std::min )( count, size( ) - pos );
				if( rlen > 0 ) {
					auto const f =
					  std::next( begin( ), static_cast<difference_type>( pos ) );
					auto const l = std::next( f, static_cast<difference_type>( rlen ) );
					daw::algorithm::copy( f, l, dest );
				}
				return rlen;
			}

			constexpr size_type copy( pointer dest, size_type count ) const {
				return copy( dest, count, 0 );
			}

			[[nodiscard]] constexpr basic_string_view
			substr( size_type pos, size_type count ) const {
				daw::exception::precondition_check<std::out_of_range>(
				  pos <= size( ), "Attempt to access basic_string_view past end" );
				auto const rcount =
				  static_cast<size_type>( ( std::min )( count, size( ) - pos ) );
				return { m_first + pos, m_first + pos + rcount };
			}

			[[nodiscard]] constexpr basic_string_view substr( ) const {
				return substr( 0, npos );
			}

			[[nodiscard]] constexpr basic_string_view substr( size_type pos ) const {
				return substr( pos, npos );
			}

		public:
			template<string_view_bounds_type BL, std::ptrdiff_t ExL,
			         string_view_bounds_type BR, std::ptrdiff_t ExR>
			[[nodiscard]] static constexpr int
			compare( basic_string_view<CharT, BL, ExL> lhs,
			         basic_string_view<CharT, BR, ExR> rhs ) {
				auto const str_compare = []( CharT const *p0, CharT const *p1,
				                             size_type len ) {
					auto const last = p0 + len;
					while( p0 != last ) {
						if( *p0 != *p1 ) {
							if( *p0 < *p1 ) {
								return -1;
							}
							return 1;
						}
						++p0;
						++p1;
					}
					return 0;
				};

				auto cmp = str_compare( lhs.data( ), rhs.data( ),
				                        ( std::min )( lhs.size( ), rhs.size( ) ) );
				if( cmp == 0 ) {
					if( lhs.size( ) < rhs.size( ) ) {
						return -1;
					}
					if( rhs.size( ) < lhs.size( ) ) {
						return 1;
					}
				}
				return cmp;
			}

			template<typename StringView,
			         std::enable_if_t<
			           sv2_details::is_string_view_like<StringView, CharT>::value,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr int compare( StringView &&rhs ) const {
				return compare(
				  *this, basic_string_view( std::data( rhs ), std::size( rhs ) ) );
			}

			template<std::size_t N>
			[[nodiscard]] constexpr int compare( CharT const ( &rhs )[N] ) const {
				return compare( *this, basic_string_view( rhs, N - 1 ) );
			}

			constexpr int compare( size_type pos1, size_type count1,
			                       basic_string_view v ) const {
				return compare( substr( pos1, count1 ), v );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr int
			compare( size_type pos1, size_type count1,
			         basic_string_view<CharT, Bounds, Ex> v, size_type pos2,
			         size_type count2 ) const {
				return compare( substr( pos1, count1 ), v.substr( pos2, count2 ) );
			}

			[[nodiscard]] constexpr int compare( size_type pos1, size_type count1,
			                                     const_pointer s ) const {
				return compare( substr( pos1, count1 ),
				                basic_string_view<CharT, BoundsType>( s ) );
			}

			[[nodiscard]] constexpr int compare( size_type pos1, size_type count1,
			                                     const_pointer s,
			                                     size_type count2 ) const {
				return compare( substr( pos1, count1 ),
				                basic_string_view<CharT, BoundsType>( s, count2 ) );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find( basic_string_view<CharT, Bounds, Ex> v, size_type pos ) const {

				if( size( ) < v.size( ) ) {
					return npos;
				}
				if( v.empty( ) ) {
					return pos;
				}
				auto result =
				  details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
				if( end( ) == result ) {
					return npos;
				}
				return static_cast<size_type>( result - begin( ) );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find( basic_string_view<CharT, Bounds, Ex> v ) const {
				return find( v, 0 );
			}

			[[nodiscard]] constexpr size_type find( CharT c, size_type pos ) const {
				return find(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ), pos );
			}

			[[nodiscard]] constexpr size_type find( CharT c ) const {
				return find( c, 0 );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s, size_type pos,
			                                        size_type count ) const {
				return find( basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s,
			                                        size_type pos ) const {
				return find( basic_string_view<CharT, BoundsType>( s ), pos );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s ) const {
				return find( basic_string_view<CharT, BoundsType>( s ), 0 );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			rfind( basic_string_view<CharT, Bounds, Ex> v, size_type pos ) const {

				if( size( ) < v.size( ) ) {
					return npos;
				}
				pos = ( std::min )( pos, size( ) - v.size( ) );
				if( v.empty( ) ) {
					return pos;
				}
				const_iterator cur =
				  std::next( begin( ), static_cast<difference_type>( pos ) );
				while( true ) {
					if( details::compare( cur, v.begin( ), v.size( ) ) == 0 ) {
						return static_cast<size_type>( cur - begin( ) );
					}
					if( cur == begin( ) ) {
						return npos;
					}
					--cur;
				}
			}

			[[nodiscard]] constexpr size_type rfind( const_pointer s, size_type pos,
			                                         size_type count ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			rfind( basic_string_view<CharT, Bounds, Ex> v ) const {
				return rfind( v, npos );
			}

			[[nodiscard]] constexpr size_type rfind( CharT c, size_type pos ) const {
				return rfind(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ), pos );
			}

			[[nodiscard]] constexpr size_type rfind( CharT c ) const {
				return rfind(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ),
				  npos );
			}

			[[nodiscard]] constexpr size_type rfind( const_pointer s,
			                                         size_type pos ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s ), pos );
			}

			[[nodiscard]] constexpr size_type rfind( const_pointer s ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s ), npos );
			}

			/// Find the first item in v that is in string from pos
			/// \param v A range of characters to look for
			/// \param pos Starting position to start searching
			/// \return position of first item in v or npos
			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_first_of( basic_string_view<CharT, Bounds, Ex> v,
			               size_type pos ) const {
				if( pos >= size( ) or v.empty( ) ) {
					return npos;
				}
				auto const iter = details::find_first_of( begin( ) + pos, end( ),
				                                          v.begin( ), v.end( ), bp_eq );

				if( end( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( std::distance( begin( ), iter ) );
			}

			/// Find the first item in v that is in string from beginning
			/// \param v A range of characters to look for
			/// \return position of first item in v or npos
			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_first_of( basic_string_view<CharT, Bounds, Ex> v ) const {
				return find_first_of( v, 0 );
			}

			[[nodiscard]] constexpr size_type find_first_of( const_pointer str,
			                                                 size_type pos ) const {
				return find_first_of( basic_string_view<CharT, BoundsType>( str ),
				                      pos );
			}

			[[nodiscard]] constexpr size_type
			find_first_of( const_pointer str ) const {
				return find_first_of( basic_string_view<CharT, BoundsType>( str ), 0 );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			search( basic_string_view<CharT, Bounds, Ex> v, size_type pos ) const {
				if( pos + v.size( ) >= size( ) or v.empty( ) ) {
					return npos;
				}
				auto const iter =
				  details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
				if( cend( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( std::distance( begin( ), iter ) );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			search( basic_string_view<CharT, Bounds, Ex> v ) const {
				return search( v, 0 );
			}

			[[nodiscard]] constexpr size_t search( const_pointer str,
			                                       size_type pos ) const {
				return search( basic_string_view<CharT, BoundsType>( str ), pos );
			}

			[[nodiscard]] constexpr size_t search( const_pointer str ) const {
				return search( basic_string_view<CharT, BoundsType>( str ), 0 );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			search_last( basic_string_view<CharT, Bounds, Ex> v,
			             size_type pos ) const {
				if( pos + v.size( ) >= size( ) or v.empty( ) ) {
					return npos;
				}
				auto last_pos = pos;
				auto fpos = search( v, pos );
				while( fpos != npos ) {
					last_pos = fpos;
					fpos = search( v, fpos );
					if( fpos == last_pos ) {
						break;
					}
				}
				return last_pos;
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			search_last( basic_string_view<CharT, Bounds, Ex> v ) const {
				return search_last( v, 0 );
			}

			[[nodiscard]] constexpr size_t search_last( const_pointer str,
			                                            size_type pos ) const {
				return search_last( basic_string_view<CharT, BoundsType>( str ), pos );
			}

			[[nodiscard]] constexpr size_t search_last( const_pointer str ) const {
				return search_last( basic_string_view<CharT, BoundsType>( str ), 0 );
			}

			template<typename UnaryPredicate>
			[[nodiscard]] constexpr size_type
			find_first_of_if( UnaryPredicate pred, size_type pos ) const {

				(void)traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

				if( pos >= size( ) ) {
					return npos;
				}
				auto const iter =
				  details::find_first_of_if( cbegin( ) + pos, cend( ), pred );
				if( cend( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( iter - cbegin( ) );
			}

			template<typename UnaryPredicate>
			[[nodiscard]] constexpr size_type
			find_first_of_if( UnaryPredicate pred ) const {
				return find_first_of_if( pred, 0 );
			}

			template<typename UnaryPredicate>
			[[nodiscard]] constexpr size_type
			find_first_not_of_if( UnaryPredicate pred, size_type pos ) const {

				traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

				if( pos >= size( ) ) {
					return npos;
				}
				auto const iter =
				  details::find_first_not_of_if( begin( ) + pos, end( ), pred );
				if( end( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( std::distance( begin( ), iter ) );
			}

			template<typename UnaryPredicate>
			[[nodiscard]] constexpr size_type
			find_first_not_of_if( UnaryPredicate pred ) const {
				return find_first_not_of_if( pred, 0 );
			}

			[[nodiscard]] constexpr size_type find_first_of( CharT c,
			                                                 size_type pos ) const {
				return find_first_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ),
				  pos );
			}

			[[nodiscard]] constexpr size_type find_first_of( CharT c ) const {
				return find_first_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ), 0 );
			}

			[[nodiscard]] constexpr size_type
			find_first_of( const_pointer s, size_type pos, size_type count ) const {
				return find_first_of( basic_string_view<CharT, BoundsType>( s, count ),
				                      pos );
			}

		private:
			[[nodiscard]] constexpr size_type
			reverse_distance( const_reverse_iterator first,
			                  const_reverse_iterator last ) const {
				// Portability note here: std::distance is not NOEXCEPT, but calling
				// it with a string_view::reverse_iterator will not throw.
				return ( size( ) - 1u ) -
				       static_cast<size_t>( std::distance( first, last ) );
			}

		public:
			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_last_of( basic_string_view<CharT, Bounds, Ex> s,
			              size_type pos ) const {
				if( s.empty( ) ) {
					return npos;
				}
				if( pos >= size( ) ) {
					pos = 0;
				} else {
					pos = size( ) - ( pos + 1U );
				}
				auto haystack = substr( pos );
				auto iter = daw::algorithm::find_first_of(
				  haystack.rbegin( ), haystack.rend( ), s.rbegin( ), s.rend( ) );
				return iter == rend( ) ? npos : reverse_distance( rbegin( ), iter );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_last_of( basic_string_view<CharT, Bounds, Ex> s ) const {
				return find_last_of( s, npos );
			}

			[[nodiscard]] constexpr size_type find_last_of( CharT c,
			                                                size_type pos ) const {
				return find_last_of( basic_string_view( std::addressof( c ), 1 ), pos );
			}

			[[nodiscard]] constexpr size_type find_last_of( CharT c ) const {
				return find_last_of( basic_string_view( std::addressof( c ), 1 ),
				                     npos );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type find_last_of( CharT const ( &s )[N],
			                                                size_type pos ) {
				return find_last_of( basic_string_view<CharT, BoundsType>( s, N - 1 ),
				                     pos );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type find_last_of( CharT const ( &s )[N] ) {
				return find_last_of( basic_string_view<CharT, BoundsType>( s, N - 1 ),
				                     npos );
			}

			[[nodiscard]] constexpr size_type
			find_last_of( const_pointer s, size_type pos, size_type count ) const {
				return find_last_of( basic_string_view<CharT, BoundsType>( s, count ),
				                     pos );
			}

			[[nodiscard]] constexpr size_type find_last_of( const_pointer s,
			                                                size_type pos ) const {
				return find_last_of( basic_string_view<CharT, BoundsType>( s ), pos );
			}

			[[nodiscard]] constexpr size_type find_last_of( const_pointer s ) const {
				return find_last_of( basic_string_view<CharT, BoundsType>( s ), npos );
			}

			template<typename UnaryPredicate>
			[[nodiscard]] constexpr size_type find_last_of_if( UnaryPredicate pred,
			                                                   size_type pos ) const {

				(void)traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

				auto haystack = substr( 0, pos );
				auto iter = daw::algorithm::find_if( haystack.crbegin( ),
				                                     haystack.crend( ), pred );
				return iter == crend( ) ? npos : reverse_distance( crbegin( ), iter );
			}

			template<typename UnaryPredicate>
			[[nodiscard]] constexpr size_type
			find_last_of_if( UnaryPredicate pred ) const {
				return find_last_of_if( pred, npos );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_first_not_of( basic_string_view<CharT, Bounds, Ex> v,
			                   size_type pos ) const {
				if( pos >= size( ) ) {
					return npos;
				}
				if( v.empty( ) ) {
					return pos;
				}

				auto haystack = substr( pos );
				const_iterator iter = details::find_first_not_of(
				  haystack.begin( ), haystack.end( ), v.begin( ),
				  std::next( v.begin( ), static_cast<ptrdiff_t>( v.size( ) ) ), bp_eq );
				if( end( ) == iter ) {
					return npos;
				}

				return static_cast<size_type>( std::distance( begin( ), iter ) );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_first_not_of( basic_string_view<CharT, Bounds, Ex> v ) const {
				return find_first_not_of( v, 0 );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( CharT c, size_type pos ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ),
				  pos );
			}

			[[nodiscard]] constexpr size_type find_first_not_of( CharT c ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ), 0 );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( const_pointer s, size_type pos,
			                   size_type count ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( const_pointer s, size_type pos ) const {
				return find_first_not_of( basic_string_view<CharT, BoundsType>( s ),
				                          pos );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type
			find_first_not_of( CharT const( &&s )[N], size_type pos ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( s, N - 1 ), pos );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( const_pointer s ) const {
				return find_first_not_of( basic_string_view<CharT, BoundsType>( s ),
				                          0 );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type
			find_first_not_of( CharT const( &&s )[N] ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( s, N - 1 ), 0 );
			}

			template<
			  typename UnaryPredicate,
			  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
			                   std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr size_type
			find_last_not_of_if( UnaryPredicate pred, size_type pos ) const {

				if( empty( ) ) {
					return npos;
				}
				if( pos > size( ) ) {
					pos = size( ) - 1;
				}
				for( auto n = static_cast<difference_type>( pos ); n >= 0; --n ) {
					if( not pred( m_first[n] ) ) {
						return static_cast<size_type>( n );
					}
				}
				return npos;
			}

			template<
			  typename UnaryPredicate,
			  std::enable_if_t<traits::is_unary_predicate_v<UnaryPredicate, CharT>,
			                   std::nullptr_t> = nullptr>
			[[nodiscard]] constexpr size_type
			find_last_not_of_if( UnaryPredicate pred ) const {
				return find_last_not_of_if( pred, npos );
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_last_not_of( basic_string_view<CharT, Bounds, Ex> v,
			                  size_type pos ) const {

				if( empty( ) ) {
					return npos;
				}
				if( pos > size( ) ) {
					pos = size( ) - 1;
				}
				if( v.empty( ) ) {
					return pos;
				}
				for( auto n = static_cast<difference_type>( pos ); n >= 0; --n ) {
					if( v.find( m_first[n] ) == npos ) {
						return static_cast<size_type>( n );
					}
				}
				return npos;
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr size_type
			find_last_not_of( basic_string_view<CharT, Bounds, Ex> v ) const {
				return find_last_not_of( v, npos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( CharT c, size_type pos ) const {
				return find_last_not_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ),
				  pos );
			}

			[[nodiscard]] constexpr size_type find_last_not_of( CharT c ) const {
				return find_last_not_of( c, npos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( const_pointer s, size_type pos,
			                  size_type count ) const {
				return find_last_not_of(
				  basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( const_pointer s, size_type pos ) const {
				return find_last_not_of( basic_string_view<CharT, BoundsType>( s ),
				                         pos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( const_pointer s ) const {
				return find_last_not_of( basic_string_view<CharT, BoundsType>( s ),
				                         npos );
			}

			[[nodiscard]] constexpr bool starts_with( CharT c ) const {
				if( empty( ) ) {
					return false;
				}
				return front( ) == c;
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr bool
			starts_with( basic_string_view<CharT, Bounds, Ex> s ) const {
				if( s.size( ) > size( ) ) {
					return false;
				}
				auto lhs = begin( );
				while( not s.empty( ) ) {
					if( *lhs++ != s.front( ) ) {
						return false;
					}
					s.remove_prefix( );
				}
				return true;
			}

			[[nodiscard]] constexpr bool starts_with( const_pointer s ) const {
				return starts_with( basic_string_view<CharT, BoundsType>( s ) );
			}

			[[nodiscard]] constexpr bool ends_with( CharT c ) const {
				if( empty( ) ) {
					return false;
				}
				return back( ) == c;
			}

			template<string_view_bounds_type Bounds, std::ptrdiff_t Ex>
			[[nodiscard]] constexpr bool
			ends_with( basic_string_view<CharT, Bounds, Ex> s ) const {
				if( s.size( ) > size( ) ) {
					return false;
				}
				auto lhs = rbegin( );
				while( not s.empty( ) ) {
					if( *lhs++ != s.back( ) ) {
						return false;
					}
					s.remove_suffix( );
				}
				return true;
			}

			[[nodiscard]] constexpr bool ends_with( const_pointer s ) const {
				return ends_with( basic_string_view<CharT, BoundsType>( s ) );
			}

			[[nodiscard]] constexpr bool
			operator==( basic_string_view rhs ) noexcept {
				return compare( rhs ) == 0;
			}

			template<typename StringView,
			         std::enable_if_t<
			           sv2_details::is_string_view_like<StringView, CharT>::value,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] friend constexpr bool
			operator==( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) == 0;
			}

			[[nodiscard]] friend constexpr bool
			operator==( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) == 0;
			}

			[[nodiscard]] constexpr bool
			operator!=( basic_string_view rhs ) noexcept {
				return compare( rhs ) != 0;
			}

			[[nodiscard]] friend constexpr bool
			operator!=( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) != 0;
			}

			template<typename StringView,
			         std::enable_if_t<
			           sv2_details::is_string_view_like<StringView, CharT>::value,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] friend constexpr bool
			operator!=( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) != 0;
			}

			[[nodiscard]] constexpr bool operator<( basic_string_view rhs ) noexcept {
				return compare( rhs ) < 0;
			}

			[[nodiscard]] friend constexpr bool
			operator<( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) < 0;
			}

			template<typename StringView,
			         std::enable_if_t<
			           sv2_details::is_string_view_like<StringView, CharT>::value,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] friend constexpr bool
			operator<( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) < 0;
			}

			[[nodiscard]] constexpr bool
			operator<=( basic_string_view rhs ) noexcept {
				return compare( rhs ) <= 0;
			}

			[[nodiscard]] friend constexpr bool
			operator<=( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) <= 0;
			}

			template<typename StringView,
			         std::enable_if_t<
			           sv2_details::is_string_view_like<StringView, CharT>::value,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] friend constexpr bool
			operator<=( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) <= 0;
			}

			[[nodiscard]] constexpr bool operator>( basic_string_view rhs ) noexcept {
				return compare( rhs ) > 0;
			}

			[[nodiscard]] friend constexpr bool
			operator>( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) > 0;
			}

			template<typename StringView,
			         std::enable_if_t<
			           sv2_details::is_string_view_like<StringView, CharT>::value,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] friend constexpr bool
			operator>( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) > 0;
			}

			[[nodiscard]] constexpr bool
			operator>=( basic_string_view rhs ) noexcept {
				return compare( rhs ) >= 0;
			}

			[[nodiscard]] friend constexpr bool
			operator>=( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) >= 0;
			}

			template<typename StringView,
			         std::enable_if_t<
			           sv2_details::is_string_view_like<StringView, CharT>::value,
			           std::nullptr_t> = nullptr>
			[[nodiscard]] friend constexpr bool
			operator>=( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) >= 0;
			}
		}; // basic_string_view

		// CTAD
		template<typename CharT>
		basic_string_view( CharT const *s, std::size_t count )
		  -> basic_string_view<CharT>;

		template<typename CharT, std::size_t N>
		basic_string_view( CharT const ( &/*string_literal*/ )[N] )
		  -> basic_string_view<CharT, default_string_view_bounds_type, N - 1>;
		//
		//
		namespace string_view_literals {
			[[nodiscard]] constexpr string_view
			operator"" _sv( char const *str, std::size_t len ) noexcept {
				return string_view{ str, len };
			}

#if defined( __cpp_char8_t )
			[[nodiscard]] constexpr u8string_view
			operator"" _sv( char8_t const *str, std::size_t len ) noexcept {
				return u8string_view{ str, len };
			}
#endif

			[[nodiscard]] constexpr u16string_view
			operator"" _sv( char16_t const *str, std::size_t len ) noexcept {
				return u16string_view{ str, len };
			}

			[[nodiscard]] constexpr u32string_view
			operator"" _sv( char32_t const *str, std::size_t len ) noexcept {
				return u32string_view{ str, len };
			}

			[[nodiscard]] constexpr wstring_view
			operator"" _sv( wchar_t const *str, std::size_t len ) noexcept {
				return wstring_view{ str, len };
			}
		} // namespace string_view_literals

		template<typename CharT, string_view_bounds_type Bounds,
		         std::ptrdiff_t Extent>
		[[nodiscard]] constexpr auto
		fnv1a_hash( daw::sv2::basic_string_view<CharT, Bounds, Extent> sv ) {
			return daw::fnv1a_hash( sv.data( ), sv.size( ) );
		}

		template<std::size_t HashSize = sizeof( daw::genhash_uint_t ),
		         typename CharT, string_view_bounds_type Bounds,
		         std::ptrdiff_t Extent>
		[[nodiscard]] constexpr size_t
		generic_hash( daw::sv2::basic_string_view<CharT, Bounds, Extent> sv ) {
			return daw::generic_hash<HashSize>( sv.data( ), sv.size( ) );
		}
	} // namespace sv2
} // namespace daw

namespace std {
	template<typename CharT, daw::sv2::string_view_bounds_type Bounds,
	         std::ptrdiff_t Ex>
	struct hash<daw::sv2::basic_string_view<CharT, Bounds, Ex>> {
		[[nodiscard]] constexpr size_t
		operator( )( daw::sv2::basic_string_view<CharT, Bounds, Ex> s ) {
			return daw::sv2::fnv1a_hash( s );
		}
	};
} // namespace std
