#include "common.h"
#include "FileStruct.h"
#include <zlib.h>
#include<sys/stat.h>


#pragma comment (lib , "zdll.lib")

const char zip_head[]={"xpack_2"};

CZFile::CZFile()
{
	m_rootPath[0] = '\0';
}

CZFile::~CZFile()
{

}

BOOL CZFile::BuildFile( const TCHAR *path , const TCHAR *zipfile)
{
	BOOL bRet = TRUE;
	_tcsncpy( m_rootPath , path , MAX_PATH );
	
	//获取属性
	struct _stat statbuf;
	if( _tstat( m_rootPath , &statbuf ) == 0 ){
		if ( statbuf.st_mode & _S_IFDIR )
		{
			//目录
			int lastch = _tcslen(m_rootPath)-1;
			if ( m_rootPath[lastch] == '\\' )
			{
				m_rootPath[lastch] = '\0';
			}
			Traversal(m_rootPath);
		}else{
			ZipItemFile fileitem( m_rootPath );
			m_files.push_back(fileitem);
		}
		WritePackFile( zipfile );
	}else{
		bRet = FALSE;
	}

	
	return bRet;
}

BOOL CZFile::Traversal( const TCHAR *path )
{
	TCHAR szFileMask[MAX_PATH]={0};
	WIN32_FIND_DATA fd;
	_tcsncpy( szFileMask , path , MAX_PATH );
	int last = _tcslen(szFileMask);
	if ( szFileMask[last] != '\\' )
	{
		_tcscat( szFileMask , _T("\\") );
	}
	_tcscat(szFileMask , _T("*.*"));
	
	HANDLE hFile = FindFirstFile( szFileMask, &fd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	do 
	{
		TCHAR szPath[MAX_PATH]={0};
		_stprintf( szPath , _T("%s\\%s") , path , fd.cFileName );
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){			
			if ( _tcscmp( fd.cFileName , _T(".") ) && _tcscmp( fd.cFileName , _T("..") ) )
			{								
				Traversal(szPath);
			}			
		}else{
			ZipItemFile fileitem( szPath );
			m_files.push_back(fileitem);						
		}
	} while (FindNextFile (hFile, &fd));
	FindClose (hFile);

	return TRUE;
}

DWORD CZFile::GetHeadLen(DWORD filesum)
{
	return strlen(zip_head) + filesum * sizeof(zFileItem)+ sizeof(DWORD) + 1;
}

DWORD CZFile::GetOffsetItemDesc( DWORD idx )
{
	return  strlen(zip_head) + sizeof(zFileItem)*idx + sizeof(DWORD)+1;
}


BOOL CZFile::WritePackFile(const TCHAR *zipfile)
{
	BOOL bRet = TRUE;
	int rela_pos = _tcslen( m_rootPath );
	FileList::iterator it = m_files.begin();
	FILE* fzip = _tfopen( zipfile , _T("wb") );
	if ( !fzip )
	{
		return FALSE;
	}
	//填充文件头
	DWORD filesum = m_files.size();
	DWORD  fhead_len = GetHeadLen(filesum);
	char * szfhead = new char[ fhead_len ];
	memset( szfhead , 0 , fhead_len );
	//文件头标识
	strcpy( szfhead , zip_head );
	//共打包了多少文件
	memcpy( szfhead + strlen(zip_head)+1 , &filesum  , sizeof(filesum) );
	fwrite( szfhead , 1 , fhead_len , fzip );
	delete []szfhead;

	int idx = 0;
	for ( ; it != m_files.end() ; ++it , ++idx )
	{
		FILE * file = _tfopen( it->szFilePath , _T("rb") );
		if ( file )
		{	
			//获取相对路径
			//WideCharToMultiByte( CP_ACP , 0 , it->szFilePath+rela_pos , -1 ,
			//	it->item.relapath , MAX_PATH , NULL , FALSE );
			wcsncpy_s( it->item.relapath, it->szFilePath+rela_pos, MAX_PATH );
			if ( it->item.relapath[0] == '\0' )
			{
				wcscpy_s( it->item.relapath , L"\\" );
			}

			//获取文件名
			TCHAR *ch =  _tcsrchr( it->szFilePath , '\\' );
			//WideCharToMultiByte( CP_ACP , 0 , ch+1 , -1 ,
			//	it->item.filename , MAX_PATH , NULL , FALSE );
			wcscpy_s( it->item.filename, ch+1 );

			//获取父节点名
			WCHAR szfindnode[MAX_PATH] = {0};
			wcscpy_s( szfindnode , it->item.relapath );
			WCHAR * prvch = wcsrchr(szfindnode , '\\');
			if ( prvch != szfindnode )
			{
				*prvch = '\0';
				WCHAR *prvch2 = wcsrchr( szfindnode , '\\' );
				if ( prvch2 )
				{
					wcscpy_s( it->item.parent , prvch2+1 );
				}				
			}else{
				wcscpy_s( it->item.parent , L"\\" );
			}

			struct stat fdesc;
			if( fstat( file->_file , &fdesc ) == 0 )
			{
				//文件大小
				it->item.filelen = fdesc.st_size;
			}
			fseek( fzip , 0L , SEEK_END );
			it->item.offset = ftell( fzip );
			DWORD ziplen = 0;
			if( !PackFile( file , fzip , &ziplen ) )
			{
				//没有成功写入打包文件
				it->item.complen = INVALID_NUM;
				bRet = FALSE;
			}else{
				it->item.complen = ziplen;
			}			
			fclose(file);

			//在打包文件头部写入单个压缩文件描述
			DWORD dwOffset = GetOffsetItemDesc( idx );
			fseek( fzip , dwOffset , SEEK_SET );
			fwrite( it->item.filename , 1 , sizeof(it->item.filename) , fzip );
			fwrite( it->item.relapath , 1 , sizeof(it->item.relapath) , fzip );
			fwrite( it->item.parent , 1 , sizeof(it->item.parent) , fzip );
			fwrite( &it->item.filelen , 1 , sizeof(it->item.filelen) , fzip );
			fwrite( &it->item.offset , 1 , sizeof(it->item.offset) , fzip );
			fwrite( &it->item.complen , 1 , sizeof(it->item.complen) , fzip );
			fwrite( it->item.desc , 1 , sizeof(it->item.desc) , fzip );
		}
	}

	fclose(fzip);
	return bRet;
}

BOOL CZFile::PackFile( FILE *srcfile , FILE* zipfile , DWORD* dwZipDataLen )
{
	BOOL bRet = TRUE;
	fseek( zipfile , 0L , SEEK_END );

	*dwZipDataLen = 0;
	PackData packunit;
	unsigned char srcbuf[ONCE_PACK_SIZE]={0};

	//开始循环压缩文件
	int nread = fread_s( srcbuf , ONCE_PACK_SIZE, 1 , ONCE_PACK_SIZE , srcfile );
	while ( nread )
	{
		packunit.actualsize = sizeof(packunit.buf);
		if( compress( packunit.buf , &packunit.actualsize , srcbuf , nread ) )
		{
			bRet = FALSE;
			break;
		}
		nread = fread_s( srcbuf ,ONCE_PACK_SIZE, 1 , ONCE_PACK_SIZE , srcfile );
		if (nread)
		{
			packunit.eof='0';
		}else{
			packunit.eof = '1';
		}
		*dwZipDataLen += sizeof(char) + sizeof(DWORD) + packunit.actualsize;
		fwrite( &packunit.eof , 1 , sizeof(packunit.eof) , zipfile );
		fwrite( &packunit.actualsize , 1 , sizeof(packunit.actualsize) , zipfile );
		fwrite( packunit.buf , 1 , packunit.actualsize , zipfile );
	}

	return bRet;
}

BOOL CZFile::ExtractFile( const TCHAR *zipfile , const TCHAR *relafile , unsigned char **outbuf , DWORD* len )
{
	BOOL bRet = FALSE;
	*outbuf = NULL;
	*len = 0;
	if ( _taccess( zipfile , 0 ) == -1 )
	{
		return FALSE;
	}

	FILE *fp = _tfopen( zipfile , _T("rb") );
	if ( fp == NULL )
	{
		return FALSE;
	}

	char szBuf[PACK_BUF_SIZE];
	fread( szBuf , 1 , strlen(zip_head)+1 , fp );
	if ( strcmp( szBuf , zip_head ) )
	{
		//不是xpack文件
		return FALSE;
	}
	
	/*
	WCHAR szrelafile[MAX_PATH] = {0};
#ifdef _UNICODE
	//WideCharToMultiByte( CP_ACP , 0 , relafile , -1 , szrelafile , MAX_PATH , NULL , FALSE );
	wcsncpy_s( szrelafile, relafile, MAX_PATH );
#else
	strncpy( szrelafile , relafile );
#endif
	*/
	DWORD dwFileItem = 0;
	fread( &dwFileItem , 1 , sizeof(dwFileItem) , fp);

	DWORD dwidx = 0;
	for ( dwidx = 0 ; dwidx < dwFileItem ; ++dwidx )
	{
		zFileItem item;
		fread( item.filename , 1 , sizeof(item.filename) , fp );
		fread( item.relapath , 1 , sizeof(item.relapath) , fp );
		fread( item.parent , 1 , sizeof(item.parent) , fp );
		fread( &item.filelen , 1 , sizeof(item.filelen) , fp );
		fread( &item.offset , 1 , sizeof(item.offset) , fp );
		fread( &item.complen , 1 , sizeof(item.complen) , fp );
		fread( item.desc , 1 , sizeof(item.desc) , fp );

		if ( wcsicmp( relafile , item.relapath ) == 0 )
		{
			*len = item.filelen;
			*outbuf = new unsigned char[*len + 8];
			memset(  *outbuf , 0 , *len + 8 );
			DWORD dwChkLen = 0; //返回实际解压后的大小
			ExtraPackItem( fp , *outbuf , &dwChkLen , *len , item.offset);
			if ( dwChkLen != *len )
			{
				bRet = FALSE;
			}else{
				bRet = TRUE;
			}
			break;
		}
	}
	
	fclose(fp);
	return bRet;
}

BOOL CZFile::ExtraPackItem( FILE* zipfile , unsigned char *outbuf ,  DWORD* dwOriDataLen , DWORD totalsize , DWORD dwoffset )
{
	BOOL bRet = TRUE;
	int offset = fseek( zipfile , dwoffset , SEEK_SET);
	char szTempBuf[PACK_BUF_SIZE];
	*dwOriDataLen = 0;
	DWORD reminlen = totalsize - *dwOriDataLen;

	while(1){
		char eof;
		fread( &eof , 1 , sizeof(eof) , zipfile );
		DWORD dwPackLen = 0;
		fread(&dwPackLen , 1 , sizeof(dwPackLen) , zipfile ); //读取单个压缩包大小
		DWORD dwRead = fread( szTempBuf , 1 , dwPackLen , zipfile );
		if( uncompress( outbuf , &reminlen , (unsigned char*)szTempBuf , dwRead ) )
		{
			bRet = FALSE;
			break;
		}
		outbuf += reminlen;
		*dwOriDataLen += reminlen;
		reminlen = totalsize - *dwOriDataLen;

		if ( eof == '1' )
		{
			break;
		}
	}

	return bRet;
}