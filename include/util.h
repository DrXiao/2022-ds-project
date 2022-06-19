#ifndef __UTIL_H__
#define __UTIL_H__

#define GET_TIME(t) clock_gettime(CLOCK_MONOTONIC, t);
#define TIME_DIFF(t1, t2) (t2.tv_sec - t1.tv_sec) * (int) 1e9 - t1.tv_nsec + t2.tv_nsec

#endif
