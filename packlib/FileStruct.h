#ifndef FILESTRUCT_H
#define FILESTRUCT_H
#include <vector>

#define ONCE_PACK_SIZE 4096
#define PACK_BUF_SIZE ONCE_PACK_SIZE+2048

struct zFileItem
{
	/*文件名*/
	WCHAR filename[MAX_PATH];

	/*相对路径*/
	WCHAR relapath[MAX_PATH];

	/*父节点*/
	WCHAR parent[MAX_PATH];

	/*文件原始大小*/
	DWORD filelen;

	/*压缩文件中的偏移位置*/
	DWORD offset;

	/*压缩后的大小*/
	DWORD complen;

	/**/
	char desc[16];

	zFileItem(){
		memset( filename , 0 , sizeof(filename) );
		memset( relapath , 0 , sizeof(relapath) );
		memset( parent , 0 , sizeof(parent) );
		memset( desc , 0 , sizeof(desc) );
		strcpy_s( desc , "desc_fin" );
		filelen = INVALID_NUM;
		offset = INVALID_NUM;
		complen = INVALID_NUM;
	}
};

struct ZipItemFile{

	TCHAR szFilePath[MAX_PATH];
	zFileItem item;
	ZipItemFile( const TCHAR* szFile ){
		_tcsncpy_s( szFilePath , szFile , MAX_PATH );
	}
};

//压缩单元数据
struct PackData{
	char eof;
	DWORD actualsize;	
	unsigned char buf[PACK_BUF_SIZE];
	PackData(){
		eof = '1';
		actualsize = sizeof(buf);		
		memset( buf , 0 , PACK_BUF_SIZE );		
	}
};

class CZFile
{
	typedef std::vector<ZipItemFile> FileList;
public:
	CZFile();
	virtual ~CZFile();
	BOOL BuildFile( const TCHAR *path , const TCHAR *zipfile);
	BOOL ExtractFile( const TCHAR *zipfile , const TCHAR *relafile , unsigned char **outbuf , DWORD* len );
	
protected:
	/*遍历文件夹*/
	BOOL Traversal(const TCHAR *path);

	/*写入打包文件*/
	BOOL WritePackFile(const TCHAR *zipfile);

	/*打包文件*/
	BOOL PackFile( FILE *srcfile , FILE* zipfile , DWORD* dwZipDataLen );

	/*计算文件头部的长度*/
	DWORD GetHeadLen(DWORD filesum);

	/*计算压缩文件描述偏移地址*/
	DWORD GetOffsetItemDesc( DWORD idx );

	BOOL ExtraPackItem( FILE* zipfile , unsigned char *outbuf ,  DWORD* dwOriDataLen  , DWORD totalsize , DWORD dwoffset);


	FileList m_files;

private:
	TCHAR m_rootPath[MAX_PATH];
};


#endif