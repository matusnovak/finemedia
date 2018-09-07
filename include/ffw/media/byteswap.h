#ifndef FFW_MEDIA_BYTESWAP
#define FFW_MEDIA_BYTESWAP

#include <algorithm>

namespace ffw {
    #ifdef FFW_WINDOWS_MSVC
    /**
    * @ingroup media
    */
    inline unsigned short byteSwap16(unsigned short Value) {
        return _byteswap_ushort(Value);
    }
    /**
    * @ingroup media
    */
    inline unsigned long byteSwap32(unsigned long Value) {
        return _byteswap_ulong(Value);
    }
    /**
    * @ingroup media
    */
    inline unsigned long long byteSwap64(unsigned long long Value) {
        return _byteswap_uint64(Value);
    }
#else
    /**
    * @ingroup media
    */
    inline unsigned short byteSwap16(unsigned short Value) {
        return __builtin_bswap16(Value);
    }
    /**
    * @ingroup media
    */
    inline unsigned long byteSwap32(unsigned long Value) {
        return __builtin_bswap32(Value);
    }
    /**
    * @ingroup media
    */
    inline unsigned long long byteSwap64(unsigned long long Value) {
        return __builtin_bswap64(Value);
    }
#endif
}

#endif
