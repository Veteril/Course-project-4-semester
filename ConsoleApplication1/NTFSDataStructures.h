#pragma once

#include <windows.h>
#include <atlstr.h>

namespace wsp
{

	struct MFTEntryHeader
	{
		CHAR   m_caSignature[4]; //  "FILE" or "BAAD"
		WORD   m_wFixupArrayOffset;
		WORD   m_wFixupArraySize;
		UINT64 m_uLSNlogFileNumber;
		WORD   m_wSequenceNumber;
		WORD   m_wHardLinkCount;
		WORD   m_wAttributeOffset;
		WORD   m_wFlags;
		DWORD  m_dwUsedEntrySize;
		DWORD  m_dwAllocatedEntrySize;
		UINT64 m_uBaseRecordAddress;
		WORD   m_wNextAttributeID;
	};

	enum MFT_ENTRY_HEADER_FLAGS
	{
		DELETED_FILE,
		FILE,
		DELETED_CATALOG,
		CATALOG
	};

	enum ATTRIBUTE_TYPES
	{
		STANDART_INFORMATION = 0x10,
		ATTRIBUTE_LIST = 0x20,
		FILE_NAME = 0x30,
		OBJECT_ID = 0x40,
		SECURITY_DESCRIPTOR = 0x50,
		VOLUME_NAME = 0x60,
		VOLUME_INFORMATION = 0x70,
		DATA = 0x80,
		INDEX_ROOT = 0x90,
		INDEX_ALLOCATION = 0xa0,
		BITMAP = 0xb0,
		REPARSE_POINT = 0xc0,
		EA_INFORMATION = 0xd0,
		EA = 0xe0,
		PROPERTY_SET = 0xf0,
		LOGGED_UTILITY_STREAM = 0x100
	};

	struct AttributeHeader
	{
		DWORD m_dwAttributeTypeID;
		DWORD m_dwAttributeSize;
		CHAR  m_cNonresidentFlag; // 1 - non-resident attribute
		CHAR  m_cNameLenght;
		WORD  m_wNameOffset;
		WORD  m_wFlags;
		WORD  m_wAttributeID;

		union
		{
			struct Resident
			{
				DWORD m_dwContentSize;
				WORD  m_wContentOffset;
			} m_Resident;

			struct Nonresident
			{
				UINT64 m_uRunlistStartingVCN;
				UINT64 m_uRunlistFinalVCN;
				WORD   m_wRunlistOffset;
				WORD   m_uCompressionUnitSize;
				CHAR   m_caUnused[4];
				UINT64 m_uAllocatedContentSize;
				UINT64 m_uActualContentSize;
				UINT64 m_uInitializedContentSize;
			} m_Nonresident;
		} m_Attr;
	};

	struct STANDART_INFORMATION_ATTR
	{
		UINT64 m_uCreationTime;
		UINT64 m_uModificationTime;
		UINT64 m_MFTModificationTime;
		UINT64 m_FileAccessTime;
		DWORD  m_dwFags;
		DWORD  m_dwMaxNumberOfVersions;
		DWORD  m_dwVersionNumber;
		DWORD  m_dwClassID;
		DWORD  m_dwOwnerID;
		DWORD  m_dwSecurityID;
		UINT64 m_uQuotaChanged;
		UINT64 m_USN;
	};

	enum STANDART_INFORMATION_FLAGS // the same falgs are used in $FILE_NAME attribute
	{
		READ_ONLY = 1,
		HIDDEN_FILE = 2,
		SYSTEM_FILE = 4,
		ARCHIVE_FILE = 0x20,
		DEVICE = 0x40,
		USUAL_FILE = 0x80,
		TEMP_FILE = 0x100,
		SPARSE_FILE = 0x200,
		CONNECTION_POINT = 0x400,
		COMPRESSED_FILE = 0x800,
		OFFLINE_FILE = 0x1000,
		NONINDEXABLE_CONTENT = 0x2000,
		ENCRYPTED_FILE = 24000
	};

#define FILE_NAME_ATTR_SIZE 66 // sizeof(FILE_NAME_ATTR) != real size of that attribute on disk

	struct FILE_NAME_ATTR
	{
		UINT64 m_uParentCatalogBaseAddress;
		UINT64 m_uFileCreationTime;
		UINT64 m_uModificationTime;
		UINT64 m_uMFTModificationTime;
		UINT64 m_uFileAccessTime;
		UINT64 m_uAllocatedFileSize;
		UINT64 m_uActualFileSize;
		DWORD  m_dwFalgs;
		DWORD  m_dwConnectionPoint;
		CHAR   m_cNameLength; // in unicode characters
		CHAR   m_cNamespace;

		// file name in unicode is located right after last field of that structure
	};

	struct VOLUME_INFORMATION_ATTR
	{
		CHAR m_caUnused[8];
		CHAR m_cMainVersion;
		CHAR m_cAdditionalVersion;
		WORD m_wFlags;
	};

	enum VOLUME_INFORMATION_FLAGS
	{
		UPDATING = 1,
		CHANGING_LOGFILE_SIZE = 2,
		UPDATING_VOLUME = 4,
		MOUNTING_IN_NT = 8,
		CHANGE_LOG_DELETING = 0x10,
		OBJECT_IDENTIFIERS_RECOVERY = 0x20,
		CHANGING_CHKDSK = 0x8000
	};

	struct VolumeInfo
	{
		VOLUME_INFORMATION_ATTR m_VolInfoAttr;
		WCHAR* m_pszVolumeLabel = NULL;

		~VolumeInfo() { delete[] m_pszVolumeLabel; }

		VolumeInfo() = default;

		VolumeInfo(VolumeInfo&& rhs) noexcept
			: m_VolInfoAttr(rhs.m_VolInfoAttr),
			m_pszVolumeLabel(rhs.m_pszVolumeLabel)
		{
			rhs.m_pszVolumeLabel = NULL;
		}
	};

	struct INDEX_ROOT_ATTR
	{
		DWORD m_dwIndexAttributeType; // enum ntfs::ATTRIBUTE_TYPES
		DWORD m_dwSortingRule;
		DWORD m_dwIndexRecordSizeInBytes; 
		CHAR  m_cIndexRecordSizeInClusters;
		CHAR  m_caUnused[3];
	};

	struct INDEX_ALLOCATION_ATTR
	{
		CHAR   m_caSignature[4]; // "INDX"
		WORD   m_wFixupArrayOffset;
		WORD   m_wFixupArraySize; // number of elements
		UINT64 m_uLogFileLsnumber;
		UINT64 m_uRecordVCN;
	};

	struct NodeHeader
	{
		DWORD m_dwIndexEntryListOffset;  // relative to the node index header beginning
		DWORD m_dwEndOfListUsedPartOffset; // the offset of the used portion of the index elements list relative to the node index header beginning
		// the offset of the end of the allcoated portion of the index elements list buffer relative to the node index header beginning
		DWORD m_dwEndOfListAllocatedPartOffset; 
		DWORD m_dwFlags;
	};

	struct IndexEntry
	{
		UINT64 m_uMFTReference;
		WORD   m_wLength;
		WORD   m_wFileNameAttrLength;
		DWORD  m_dwFlags;
	};

} // namespace
