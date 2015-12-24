#include "common.h"
#include "pack.h"
#include "FileStruct.h"



 bool __stdcall zipFile2PackFile( const char* srcPath , const char* destZipfile)
{
	bool bRet = false;
	CZFile zfile;
#ifdef _UNICODE
	WCHAR szPath[MAX_PATH] , szDestZip[MAX_PATH];
	MultiByteToWideChar( CP_ACP , 0 , srcPath , -1 , szPath , MAX_PATH );
	MultiByteToWideChar( CP_ACP , 0 , destZipfile , -1 , szDestZip , MAX_PATH );
	bRet = zfile.BuildFile( szPath , szDestZip);
#else
	bRet = zfile.BuildFile( srcPath , destZipfile);
#endif
	
	return bRet;
}


 bool __stdcall exZipFile( const char* zipfile, const char* relafile, unsigned char ** buf, unsigned long * len)
 {
	 bool bRet = false;
	 CZFile zfile;
#ifdef _UNICODE
	 WCHAR szZipFile[MAX_PATH] , szRelaFile[MAX_PATH];
	 MultiByteToWideChar( CP_ACP , 0 , zipfile , -1 , szZipFile , MAX_PATH );
	 MultiByteToWideChar( CP_ACP , 0 , relafile , -1 , szRelaFile , MAX_PATH );
	 bRet = zfile.ExtractFile( szZipFile , szRelaFile , buf , len );
#else
	 bRet = zfile.BuildFile( zipfile , relafile , buf , len);
#endif

	 return bRet;
 }

 void __stdcall freeExtfileBuf( const char* buf )
 {
	delete[] buf;
 }

 void __stdcall freeBuf(const unsigned char* buf)
 {
	 delete[] buf;
 }