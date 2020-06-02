/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITY_DEFINE_H
#define TRINITY_DEFINE_H

#include "CompilerDefs.h"

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#  if !defined(__STDC_FORMAT_MACROS)
#    define __STDC_FORMAT_MACROS
#  endif
#  if !defined(__STDC_CONSTANT_MACROS)
#    define __STDC_CONSTANT_MACROS
#  endif
#  if !defined(_GLIBCXX_USE_NANOSLEEP)
#    define _GLIBCXX_USE_NANOSLEEP
#  endif
#  if defined(HELGRIND)
#    include <valgrind/helgrind.h>
#    undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE
#    undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER
#    define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A) ANNOTATE_HAPPENS_BEFORE(A)
#    define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A)  ANNOTATE_HAPPENS_AFTER(A)
#  endif
#endif

#include <cstddef>
#include <cinttypes>
#include <climits>

#define TRINITY_LITTLEENDIAN 0
#define TRINITY_BIGENDIAN    1

#if !defined(TRINITY_ENDIAN)
#  if defined (BOOST_BIG_ENDIAN)
#    define TRINITY_ENDIAN TRINITY_BIGENDIAN
#  else
#    define TRINITY_ENDIAN TRINITY_LITTLEENDIAN
#  endif
#endif

#if TRINITY_PLATFORM == TRINITY_PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX 260
#  define _USE_MATH_DEFINES
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#  ifndef DECLSPEC_DEPRECATED
#    define DECLSPEC_DEPRECATED __declspec(deprecated)
#  endif //DECLSPEC_DEPRECATED
#else // TRINITY_PLATFORM != TRINITY_PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX PATH_MAX
#  define DECLSPEC_NORETURN
#  define DECLSPEC_DEPRECATED
#endif // TRINITY_PLATFORM

#if !defined(COREDEBUG)
#  define TRINITY_INLINE inline
#else //COREDEBUG
#  if !defined(TRINITY_DEBUG)
#    define TRINITY_DEBUG
#  endif //TRINITY_DEBUG
#  define TRINITY_INLINE
#endif //!COREDEBUG

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#  define ATTR_NORETURN __attribute__((__noreturn__))
#  define ATTR_PRINTF(F, V) __attribute__ ((__format__ (__printf__, F, V)))
#  define ATTR_DEPRECATED __attribute__((__deprecated__))
#else //TRINITY_COMPILER != TRINITY_COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F, V)
#  define ATTR_DEPRECATED
#endif //TRINITY_COMPILER == TRINITY_COMPILER_GNU

#ifdef TRINITY_API_USE_DYNAMIC_LINKING
#  if TRINITY_COMPILER == TRINITY_COMPILER_MICROSOFT
#    define TC_API_EXPORT __declspec(dllexport)
#    define TC_API_IMPORT __declspec(dllimport)
#  elif TRINITY_COMPILER == TRINITY_COMPILER_GNU
#    define TC_API_EXPORT __attribute__((visibility("default")))
#    define TC_API_IMPORT
#  else
#    error compiler not supported!
#  endif
#else
#  define TC_API_EXPORT
#  define TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_COMMON
#  define TC_COMMON_API TC_API_EXPORT
#else
#  define TC_COMMON_API TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_DATABASE
#  define TC_DATABASE_API TC_API_EXPORT
#else
#  define TC_DATABASE_API TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_SHARED
#  define TC_SHARED_API TC_API_EXPORT
#else
#  define TC_SHARED_API TC_API_IMPORT
#endif

#ifdef TRINITY_API_EXPORT_GAME
#  define TC_GAME_API TC_API_EXPORT
#else
#  define TC_GAME_API TC_API_IMPORT
#endif

#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_C(N)

#define SZFMTD "%" PRIuPTR

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

#ifdef COVERITY_CPP14_COMPATIBLE
/* C++17 missing bits required for Coverity */
#include <string>
#include <type_traits>
#include <utility>

namespace std
{
    template< class M, class N>
    constexpr std::common_type_t<M, N> lcm(M m, N /*n*/)
    {
        // not implemented in C++14
        return m;
    }

    template <class... Types>
    class variant
    {
    public:
        template <class _Ty>
        variant& operator =(_Ty&& /*other*/)
        {
            // not implemented in C++14
            return *this;
        }
    };

    template <class Visitor, class... Variants>
    std::string visit(Visitor&& /*vis*/, Variants&&... /*vars*/)
    {
        // not implemented in C++14
        return "";
    }

    template<class T>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi)
    {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

    template <class F, class Tuple>
    constexpr decltype(auto) apply(F&& /*f*/, Tuple&& /*t*/)
    {
        // not implemented in C++14
        return false;
    }

#ifndef WIN32
    template <class C>
    constexpr auto size(const C& c) -> decltype(c.size())
    {
        return c.size();
    }

    template <class T, std::size_t N>
    constexpr std::size_t size(const T(&array)[N]) noexcept
    {
        (void)array;
        return N;
    }

    template <class C>
    constexpr auto data(const C& c) -> decltype(c.data())
    {
        return c.data();
    }

    template <class C>
    constexpr auto data(C & c) -> decltype(c.data())
    {
        return c.data();
    }

    template<class...> struct disjunction : std::false_type { };
    template<class B1> struct disjunction<B1> : B1 { };
    template<class B1, class... Bn>
    struct disjunction<B1, Bn...>
        : std::conditional_t<bool(B1::value), B1, disjunction<Bn...>> { };

    template< class Base, class Derived >
    constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

    template< class T >
    constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

    template< class T >
    constexpr bool is_unsigned_v = is_unsigned<T>::value;

    template< class T >
    constexpr bool is_signed_v = is_signed<T>::value;

    template< class T, class U >
    constexpr bool is_assignable_v = is_assignable<T, U>::value;

    template< class T >
    constexpr std::size_t tuple_size_v = tuple_size<T>::value;

    template< class T >
    constexpr bool is_integral_v = is_integral<T>::value;

    template< class T, class U >
    constexpr bool is_same_v = is_same<T, U>::value;

    template< class T >
    constexpr bool is_floating_point_v = is_floating_point<T>::value;
#endif
}
#endif

#endif //TRINITY_DEFINE_H
