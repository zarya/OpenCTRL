#ifndef __GENERIC_STDOUT_H__
#define __GENERIC_STDOUT_H__

#define hwDbgInitialize() // not needed

#define hwDbgPrintLine(str) printf("%s\n", str)

#define hwDbgPrint(str) printf("%s", str)

#endif //__GENERIC_STDOUT_H__
