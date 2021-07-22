#ifndef FILESTRUCT_H
#define FILESTRUCT_H

#include <unordered_map>
#include <memory>

#define ONCE_PACK_SIZE 4096
#define PACK_BUF_SIZE ONCE_PACK_SIZE+2048

struct zFileItem
{
	/*�ļ���*/
	WCHAR filename[MAX_PATH];

	/*���·��*/
	WCHAR relapath[MAX_PATH];

	/*���ڵ�*/
	WCHAR parent[MAX_PATH];

	/*�ļ�ԭʼ��С*/
	DWORD filelen;

	/*ѹ���ļ��е�ƫ��λ��*/
	DWORD offset;

	/*ѹ����Ĵ�С*/
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

struct zFileItemV1
{
	/*�ļ���*/
	char filename[MAX_PATH];

	/*���·��*/
	char relapath[MAX_PATH];

	/*���ڵ�*/
	char parent[MAX_PATH];

	/*�ļ�ԭʼ��С*/
	DWORD filelen;

	/*ѹ���ļ��е�ƫ��λ��*/
	DWORD offset;

	/*ѹ����Ĵ�С*/
	DWORD complen;

	/**/
	char desc[16];

	zFileItemV1() {
		memset(filename, 0, sizeof(filename));
		memset(relapath, 0, sizeof(relapath));
		memset(parent, 0, sizeof(parent));
		memset(desc, 0, sizeof(desc));
		strcpy_s(desc, "desc_fin");
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

//ѹ����Ԫ����
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
	
	BOOL LoadPackFile(const WCHAR* zipfile);

	BOOL GetItemInfo(const WCHAR* relafile, zFileItem** pZip);

	BOOL ExtraPackItem(unsigned char* outbuf, DWORD* dwOriDataLen, DWORD totalsize, DWORD dwoffset);
protected:
	/*�����ļ���*/
	BOOL Traversal(const TCHAR *path);

	/*д�����ļ�*/
	BOOL WritePackFile(const TCHAR *zipfile, TCHAR* rootPath);

	/*����ļ�*/
	BOOL PackFile( FILE *srcfile , FILE* zipfile , DWORD* dwZipDataLen );

	/*�����ļ�ͷ���ĳ���*/
	DWORD GetHeadLen(DWORD filesum);

	/*����ѹ���ļ�����ƫ�Ƶ�ַ*/
	DWORD GetOffsetItemDesc( DWORD idx );


	BOOL ExtraPackItem(FILE* zipfile, unsigned char* outbuf, DWORD* dwOriDataLen, DWORD totalsize, DWORD dwoffset);

	FileList m_buildfiles;

private:
	int m_xpackVer = 0;

	std::unique_ptr<unsigned char[]> m_xpackData;
	
	std::unordered_map<std::wstring, std::unique_ptr< zFileItem>> m_ExistData;
};


#endif