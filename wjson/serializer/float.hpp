//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2008-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/predef.hpp>
#include <wjson/error.hpp>
#include <wjson/parser.hpp>

#include <sstream>

namespace wjson{

namespace detail
{

template <class CharT, class Traits = std::char_traits<CharT>>
struct ostreambuf : public std::basic_streambuf<CharT, Traits>
{
    ostreambuf(CharT* buffer, std::streamsize bufferLength)
        : std::basic_streambuf<CharT, Traits>()
    {
        setp(buffer, buffer + bufferLength);
    }
};

template <class CharT, class Traits = std::char_traits<CharT>>
struct istreambuf : public std::basic_streambuf<CharT, Traits>
{
    istreambuf(CharT* buffer, std::streamsize bufferLength)
        : std::basic_streambuf<CharT, Traits>()
    {
        setg(buffer, buffer, buffer + bufferLength);
    }
};

}

template<typename T, int R>
class serializerF
{
public:
  template<typename P>
  P operator()( T v, P end)
  {
    const size_t bufsize = ( R == -1 ? 20 : 20 + R ) ;
    char buf[bufsize]={'\0'};
    
    detail::ostreambuf<char> streamBuf(buf, bufsize);
    std::ostream ss(&streamBuf);
    if ( R == -1 ) 
    {
      ss << std::scientific;
    }
    else
    {
      ss << std::fixed;
      ss.precision(R);
    }

    ss << v ;

    for (int i = 0; i < bufsize && buf[i]!='\0'; ++i)
    {
      *(end++) = buf[i];
    }
    return end;
  }
  
  template<typename P>
  P operator() ( T& v, P beg, P end, json_error* e )
  {
    if( beg==end)
      return create_error<error_code::UnexpectedEndFragment>( e, end );

    if ( parser::is_null(beg, end) )
    {
      v = T();
      return parser::parse_null(beg, end, e);
    }

    if ( !parser::is_number(beg, end) )
    {
      return create_error<error_code::InvalidNumber>( e, end, std::distance(beg, end) );
    }

    detail::istreambuf<char> streamBuf(const_cast<char*>(&(*beg)), std::distance(beg, end));
    std::istream ss(&streamBuf);
    ss >> v;
    return parser::parse_number(beg, end, e);
  }
};


template<int R>
class serializerT< value<float, R> >
  : public serializerF<float, R>
{
};

template<int R>
class serializerT< value<double, R> >
  : public serializerF<double, R>
{
};

template<int R>
class serializerT< value<long double, R> >
  : public serializerF<long double, R>
{
};

}
