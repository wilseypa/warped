#ifndef _RTM_H
#define _RTM_H 1

/*
 * Copyright (c) 2012,2013 Intel Corporation
 * Author: Andi Kleen
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* Official RTM intrinsics interface matching gcc/icc, but works
   on older gcc compatible compilers and binutils. */

#define L1DSZ 64

#define _XACQUIRE_PREFIX   ".byte 0xF2; "
#define _XRELEASE_PREFIX   ".byte 0xF3; "

#define _XBEGIN_STARTED		(~0u)
#define _XABORT_EXPLICIT	(1 << 0)
#define _XABORT_RETRY		(1 << 1)
#define _XABORT_CONFLICT	(1 << 2)
#define _XABORT_CAPACITY	(1 << 3)
#define _XABORT_DEBUG		(1 << 4)
#define _XABORT_NESTED		(1 << 5)
#define _XABORT_CODE(x)		(((x) >> 24) & 0xff)

#define _XA_EXPLICIT    0
#define _XA_RETRY       1
#define _XA_CONFLICT    2
#define _XA_CAPACITY    3   

#define _ABORT_LOCK_BUSY    0xff

#define ABORT_COUNT(type, status)   \
    do {                            \
        if (status & (1 << type))   \
            tsxAbrtType[type]++;   \
    } while (0)

#define TSXRTM_RETRIES 10

#define __rtm_force_inline __attribute__((__always_inline__)) inline
#define __hle_force_inline __attribute__((__always_inline__)) inline

static __hle_force_inline int _xacquire(int *lockOwner, const unsigned int *threadNumber)
{
    unsigned char ret;
    asm volatile("mov $-0x1, %%eax\n"
                 _XACQUIRE_PREFIX "lock cmpxchg %2, %1\n" 
                 "sete %0"
                 : "=q"(ret), "=m"(*lockOwner)
                 : "r"(*threadNumber) 
                 : "memory", "%eax");
    return (int) ret;
}

static __hle_force_inline int _xrelease(int *lockOwner, const unsigned int *threadNumber)
{
    unsigned char ret;
    asm volatile("mov %2, %%eax\n"
                 _XRELEASE_PREFIX "lock cmpxchg %3, %1\n"
                 "sete %0"
                 : "=q"(ret), "=m"(*lockOwner)
                 : "r"(*threadNumber), "r"(-0x1) 
                 : "memory", "%eax");
    return (int) ret;
}

static __rtm_force_inline int _xbegin(void)
{
	int ret = _XBEGIN_STARTED;
	asm volatile(".byte 0xc7,0xf8 ; .long 0" : "+a" (ret) :: "memory");
	return ret;
}

static __rtm_force_inline void _xend(void)
{
	 asm volatile(".byte 0x0f,0x01,0xd5" ::: "memory");
}

static __rtm_force_inline void _xabort(const unsigned int status)
{
	asm volatile(".byte 0xc6,0xf8,%P0" :: "i" (status) : "memory");
}

static __rtm_force_inline int _xtest(void)
{
	unsigned char out;
	asm volatile(".byte 0x0f,0x01,0xd6 ; setnz %0" : "=r" (out) :: "memory");
	return out;
}

static inline void delay(void)
{
    asm volatile("pause" ::: "memory");
}

#endif
