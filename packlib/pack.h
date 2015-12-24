#ifndef _PACK_H
#define _PACK_H

#ifdef __cplusplus
extern "C" {
#endif 

extern BOOL WINAPI zipFile2PackFile( const char* , const char* );

extern BOOL WINAPI exZipFile( const char* , const char* , unsigned char ** , unsigned long *);

extern void WINAPI freeExtfileBuf( const char* );

extern void WINAPI freeBuf(const unsigned char*);

#ifdef __cplusplus
}
#endif 


#endif