/** 
* @file   endianness.h
* @brief  Convert Endianness of shorts, longs, long longs, regardless of architecture/OS
*
* Defines (without pulling in platform-specific network include headers):
* bswap16, bswap32, bswap64, ntoh16, hton16, ntoh32 hton32, ntoh64, hton64
*
* Should support linux / macos / solaris / windows.
* Supports GCC (on any platform, including embedded), MSVC2015, and clang,
* and should support intel, solaris, and ibm compilers as well.
*
* Released under MIT license
*
* Forked from https://gist.github.com/jtbr/7a43e6281e6cca353b33ee501421860c
*/

#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <stdlib.h>
#include <stdint.h>

/* Detect platform endianness at compile time */

// When available, these headers can improve platform endianness detection
#ifdef __has_include // C++17, supported as extension to C++11 in clang, GCC 5+, vs2015
    #if __has_include(<endian.h>)
        #include <endian.h> // gnu libc normally provides, linux
    #elif __has_include(<machine/endian.h>)
        #include <machine/endian.h> //open bsd, macos
    #elif __has_include(<sys/param.h>)
        #include <sys/param.h> // mingw, some bsd (not open/macos)
    #elif __has_include(<sys/isadefs.h>)
        #include <sys/isadefs.h> // solaris
    #endif
#endif

#if defined(__LITTLE_ENDIAN__)
    #define ENDIANNESS_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__)
    #define ENDIANNESS_BIG_ENDIAN
#endif

#if !defined(ENDIANNESS_LITTLE_ENDIAN) && !defined(ENDIANNESS_BIG_ENDIAN)
    #if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || \
     (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER == __BIG_ENDIAN) || \
     (defined(_BYTE_ORDER)  && defined(_BIG_ENDIAN)  && _BYTE_ORDER == _BIG_ENDIAN)   || \
     (defined(BYTE_ORDER)   && defined(BIG_ENDIAN)   && BYTE_ORDER == BIG_ENDIAN)     || \
     defined(_BIG_ENDIAN) || \
     defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
     defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__) || \
     defined(_M_PPC)
        #define ENDIANNESS_BIG_ENDIAN
    #elif (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
     (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN) || \
     (defined(_BYTE_ORDER)  && defined(_LITTLE_ENDIAN)  && _BYTE_ORDER == _LITTLE_ENDIAN)   || \
     (defined(BYTE_ORDER)   && defined(LITTLE_ENDIAN)   && BYTE_ORDER == LITTLE_ENDIAN)     || \
     defined(_LITTLE_ENDIAN) || /* solaris */ \
     defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
     defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) || \
     defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64) || /* msvc for intel processors */ \
     defined(_M_ARM) /* msvc code on arm executes in little endian mode */
        #define ENDIANNESS_LITTLE_ENDIAN
    #endif
#endif

/* Define byte-swap functions, using fast processor-native built-ins where possible */

#if defined(_MSC_VER) // needs to be first because msvc doesn't short-circuit after failing defined(__has_builtin)
    #define bswap16(x)     _byteswap_ushort((x))
    #define bswap32(x)     _byteswap_ulong((x))
    #define bswap64(x)     _byteswap_uint64((x))
#elif (defined(__GNUC__) || defined(__clang__))
    #define bswap16(x)     __builtin_bswap16((x))
    #define bswap32(x)     __builtin_bswap32((x))
    #define bswap64(x)     __builtin_bswap64((x))
#else
    /* even in this case, compilers often optimize by using native instructions */
    static inline uint16_t bswap16(uint16_t x) {
           return (( x  >> 8 ) & 0xffu ) | (( x  & 0xffu ) << 8 );
       }
    static inline uint32_t bswap32(uint32_t x) {
        return ((( x & 0xff000000u ) >> 24 ) |
                (( x & 0x00ff0000u ) >> 8  ) |
                (( x & 0x0000ff00u ) << 8  ) |
                (( x & 0x000000ffu ) << 24 ));
    }
    static inline uint64_t bswap64(uint64_t x) {
        return ((( x & 0xff00000000000000ull ) >> 56 ) |
                (( x & 0x00ff000000000000ull ) >> 40 ) |
                (( x & 0x0000ff0000000000ull ) >> 24 ) |
                (( x & 0x000000ff00000000ull ) >> 8  ) |
                (( x & 0x00000000ff000000ull ) << 8  ) |
                (( x & 0x0000000000ff0000ull ) << 24 ) |
                (( x & 0x000000000000ff00ull ) << 40 ) |
                (( x & 0x00000000000000ffull ) << 56 ));
    }
#endif


/* Defines network - host byte swaps as needed depending upon platform endianness */
// note that network order is big endian)

#if defined(ENDIANNESS_LITTLE_ENDIAN)
    #define ntoh16(x)     bswap16((x))
    #define hton16(x)     bswap16((x))
    #define ntoh32(x)     bswap32((x))
    #define hton32(x)     bswap32((x))
    #define ntoh64(x)     bswap64((x))
    #define hton64(x)     bswap64((x))

    #if !defined(be16toh)
        #define be16toh(x)    bswap16((x))
        #define be32toh(x)    bswap32((x))
        #define be64toh(x)    bswap64((x))
        #define le16toh(x)    (x)
        #define le32toh(x)    (x)
        #define le64toh(x)    (x)

        #define htobe16(x)    bswap16((x))
        #define htobe32(x)    bswap32((x))
        #define htobe64(x)    bswap64((x))
        #define htole16(x)    (x)
        #define htole32(x)    (x)
        #define htole64(x)    (x)
    #endif

#elif defined(ENDIANNESS_BIG_ENDIAN)
    #define ntoh16(x)     (x)
    #define hton16(x)     (x)
    #define ntoh32(x)     (x)
    #define hton32(x)     (x)
    #define ntoh64(x)     (x)
    #define hton64(x)     (x)

    #if !defined(be16toh)
        #define be16toh(x)    (x)
        #define be32toh(x)    (x)
        #define be64toh(x)    (x)
        #define le16toh(x)    bswap16((x))
        #define le32toh(x)    bswap32((x))
        #define le64toh(x)    bswap64((x))

        #define htobe16(x)    (x)
        #define htobe32(x)    (x)
        #define htobe64(x)    (x)
        #define htole16(x)    bswap16((x))
        #define htole32(x)    bswap32((x))
        #define htole64(x)    bswap64((x))
    #endif
#else
    #warning "UNKNOWN Platform / endianness; network / host byte swaps not defined."
#endif

#endif // ENDIANNESS_H
