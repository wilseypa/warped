
#include "WarpedConfig.h"               // for HAVE_STDINT_H, etc

#ifdef HAVE_STDINT_H
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>                     // for INT32_MAX, INT32_MIN, etc
#endif
#define _WARPED64MAX INT64_MAX;
#define _WARPED64MIN INT64_MIN;
#define _WARPED32MAX INT32_MAX;
#define _WARPED32MIN INT32_MIN;
#elif defined(SIZEOF_LONG_LONG_) // defined in warped-config.h
#include <limits.h>

#define _WARPED64MAX LONG_LONG_MAX;
#define _WARPED64MIN LONG_LONG_MIN;
#define _WARPED32MAX INT_MAX;
#define _WARPED32MIN INT_MIN;
#else
#include <limits.h>

#define _WARPED64MAX LONG_MAX;
#define _WARPED64MIN LONG_MIN;
#define _WARPED32MAX INT_MAX;
#define _WARPED32MIN INT_MIN;
#endif

#include "warped.h"

warped64_t
getWarped64Max() {
    return _WARPED64MAX;
}

warped64_t
getWarped64Min() {
    return _WARPED64MIN;
}

warped32_t
getWarped32Max() {
    return _WARPED32MAX;
}

warped32_t
getWarped32Min() {
    return _WARPED32MIN;
}
