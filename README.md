# string_view

An enhanced string_view implementation with parsing helpers.

## BNF like code

One can build simple parsers quickly via methods like `parse_front_until`

```c++
struct uri_parts {
   string_view scheme;
   string_view authority;
   string_view path;
   string_view query;
   string_view fragment;
};
// scheme://authority:port/path/?query#fragment
uri_parts parse_uri( string_view uri_string ) {
    return {
       /* scheme    */ uri_string.pop_fron_until( "://" ),
       /* authority */ uri_string.pop_front_until( any_of<'/', '?', '#'>, nodiscard ),
       /* path      */ uri_string.pop_front_until( any_of<'?', '#'> ),
       /* query     */ uri_string.pop_front_until( '#' ),
       /* fragment  */ uri_string
    };
}

```