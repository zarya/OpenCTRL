#ifndef __GENERIC_STDOUT_H__

#ifdef __cplusplus
extern "C"
{
#endif
     extern void debugInitialize();
     extern void debugPrintLine(char *_str);
     extern void debugPrint(char *_str);
#ifdef __cplusplus
}
#endif

#endif //__GENERIC_STDOUT_H__
