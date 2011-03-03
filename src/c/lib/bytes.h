#ifndef __BYTES_H__
#define __BYTES_H__

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// TODO optimize all booleans into struct with bitfield :1 ^.^
#define bool uint8
#define true 1
#define false 0

#endif // __BYTES_H__
