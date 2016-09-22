// xpack.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../packlib/pack.h"

#pragma comment (lib , "packlib.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	if ( argc > 2 )
	{
		zipFile2PackFile( argv[1] , argv[2]);
		return 0;
	}

	zipFile2PackFile( L"d:\\test士大121夫  (231)" , L"d:\\界面1.pack");

	unsigned char *buf = NULL;
	DWORD dwlen = 0;
	bool ext = exZipFile( L"D:\\士大夫 (2)\\界面1.pack" , L"\\图标1.png" , &buf , &dwlen );
	//exZipFile( "D:\\ui.pack" , "\\bookmark.htm" , &buf , &dwlen );

	if ( buf )
	{
		FILE *fp = fopen("d:\\test.png" , "wb");
		fwrite( buf , 1 , dwlen , fp );
		fclose(fp);
		freeBuf(buf);
	}
	return 0;
}

