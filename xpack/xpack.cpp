// xpack.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../packlib/pack.h"

#pragma comment (lib , "packlib.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	//zipFile2PackFile( "e:\\webui" , "e:\\ui.pack");

	unsigned char *buf = NULL;
	DWORD dwlen = 0;
	bool ext = exZipFile( L"D:\\士大夫 (2)\\ui.pack" , L"\\Index.html" , &buf , &dwlen );
	//exZipFile( "D:\\ui.pack" , "\\bookmark.htm" , &buf , &dwlen );

	if ( buf )
	{
		FILE *fp = fopen("d:\\test.html" , "wb");
		fwrite( buf , 1 , dwlen , fp );
		fclose(fp);
		freeBuf(buf);
	}
	return 0;
}

