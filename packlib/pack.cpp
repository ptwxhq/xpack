#include "common.h"
#include "pack.h"
#include "FileStruct.h"

#include <unordered_map>
#include <memory>


class AUTO_FREE_PACK : public XPackData
{
public:
	AUTO_FREE_PACK() {

	}
	operator const char* () {
		return (const char*)data;
	}

	operator unsigned char* () const {
		return data;
	}

	int GetLen() {
		return len;
	}
	~AUTO_FREE_PACK() {
		if (data)
		{
			delete[]data;
		}
	}

	void Free() {
		delete this;
	}

	void Move(unsigned char** to) {
		*to = data;
		data = 0;
		len = 0;
	}

	int len = 0;
	unsigned char* data = nullptr;
};


std::unordered_map<std::wstring, std::unique_ptr<CZFile>> g_memPackList;



BOOL PreLoadPackFile(const WCHAR* zipfile)
{
	auto pItem = std::make_unique<CZFile>();
	if (pItem->LoadPackFile(zipfile))
	{
		g_memPackList.insert(std::make_pair(zipfile, std::move(pItem)));
		return TRUE;
	}
	return FALSE;
}

int UnzipExistPackFile(const WCHAR* zipfile, const WCHAR* filepath, XPackData** out)
{
	int iRet = 0;
	do
	{
		auto itemzip = g_memPackList.find(zipfile);
		if (itemzip == g_memPackList.end())
		{
			iRet = -1;
			break;
		}

		CZFile& zf = *itemzip->second;

		zFileItem* zFItem = nullptr;
		if (!zf.GetItemInfo(filepath, &zFItem) || !zFItem)
		{
			iRet = -2;
			break;
		}

		auto pOut = new AUTO_FREE_PACK;
		pOut->len = zFItem->filelen;
		pOut->data = new unsigned char[pOut->len];


	
		DWORD dwChkLen = 0; //返回实际解压后的大小
		BOOL bSucc = zf.ExtraPackItem(*pOut, &dwChkLen, zFItem->filelen, zFItem->offset);

		if (!bSucc || dwChkLen != zFItem->filelen)
		{
			delete pOut;
			iRet = -3;
			break;
		}

		*out = pOut;


	} while (false);

	return iRet;
}


BOOL CleanLoadedPacks(const WCHAR* path)
{
	if (path && path[0])
		g_memPackList.erase(path);
	else
		g_memPackList.clear();

	return TRUE;
}

bool __stdcall zipFile2PackFile( const WCHAR* srcPath , const WCHAR* destZipfile)
{
	bool bRet = false;
	CZFile zfile;

	bRet = zfile.BuildFile( srcPath , destZipfile);
	
	return bRet;
}


 bool __stdcall exZipFile( const WCHAR* zipfile, const WCHAR* relafile, unsigned char ** buf, unsigned long * len)
 {
	 AUTO_FREE_PACK* test = nullptr;
	 if (UnzipExistPackFile(zipfile, relafile, (XPackData**)&test) == 0)
	 {
		 *len = test->GetLen();
		 test->Move(buf);
		 test->Free();
		 return true;
	 }

	 bool bRet = false;
	 CZFile zfile;

	 bRet = zfile.ExtractFile( zipfile , relafile , buf , len );

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

