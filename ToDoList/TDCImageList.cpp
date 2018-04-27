// TDCImageList.cpp: implementation of the CTDCImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "TDCImageList.h"

#include "..\shared\sysimagelist.h"
#include "..\shared\filemisc.h"
#include "..\shared\enbitmap.h"
#include "..\shared\misc.h"
#include "..\shared\icon.h"
#include "..\shared\graphicsmisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

enum
{
	TDCIL_LOADEDSET		= 0x01,
	TDCIL_LOADEDIMAGES	= 0x02,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDCImageList::CTDCImageList()
{

}

CTDCImageList::~CTDCImageList()
{

}

BOOL CTDCImageList::LoadDefaultImages(BOOL bWantToolbars)
{
	return LoadImages(_T(""), TDCIL_MAGENTA, TRUE, bWantToolbars);
}

BOOL CTDCImageList::LoadImages(const CString& sTaskList, COLORREF crTransparent,
							   BOOL bWantDefaultIcons, BOOL bWantToolbars)
{
	DeleteImageList(); // always

	if (Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 200))
	{
		// Add a dummy placeholder for the 'folder' icon which we
		// will replace once we have rescaled the imagelist
		Add(CIcon(IDI_NULL, 16));
		MapImage(0, this);

		// because the icon set must come first for backwards compatibility
		// we must first see if any other images exist before we add them.
		// We do this by passing NULL for the image list
		CString sTasklistPath(FileMisc::GetFolderFromFilePath(sTaskList));
		CString sAppResPath(FileMisc::GetAppResourceFolder());

		if (bWantDefaultIcons)
		{
			// load images from tasklist location
			DWORD dwResult = 0;
			
			if (FileMisc::FileExists(sTaskList))
				dwResult = LoadImagesFromFolder(sTasklistPath, crTransparent, NULL);

			// else try application location
			if (!dwResult)
				dwResult = LoadImagesFromFolder(sAppResPath, crTransparent, NULL);

			// Load default icon set if no icon set loaded
			if ((dwResult & TDCIL_LOADEDSET) == 0)
			{
				CBitmap bm;

				if (bm.LoadBitmap(IDB_TASKICONS_STD))
					AddImage(_T(""), bm, crTransparent, this);
			}
		}

		if (bWantToolbars)
		{
			CBitmap bm;
			
			if (bm.LoadBitmap(IDB_APP_TOOLBAR_STD))
				AddImage(_T(""), bm, crTransparent, this);

			bm.DeleteObject();

			if (bm.LoadBitmap(IDB_APP_EXTRA_STD))
				AddImage(_T(""), bm, crTransparent, this);
		}

		// then add the user's images
		DWORD dwResult = 0;

		if (FileMisc::FileExists(sTaskList))
			dwResult = LoadImagesFromFolder(sTasklistPath, crTransparent, this);

		// else try application location
		if (!dwResult)
			dwResult = LoadImagesFromFolder(sAppResPath, crTransparent, this);

		ScaleByDPIFactor();

		// Replace the first image with the actual folder icon
		VERIFY(Replace(0, CIcon(CSysImageList().ExtractFolderIcon())) == 0);
	}

	return (GetSafeHandle() != NULL);
}

int CTDCImageList::GetImageIndex(const CString& sImageName) const
{
	int nIndex = -1;
	
	if (!sImageName.IsEmpty())
		m_mapNameToIndex.Lookup(sImageName, nIndex);
	
	return nIndex;
}

CString CTDCImageList::GetImageName(int nIndex) const
{
	CString sName;

	m_mapIndexToName.Lookup(nIndex, sName);

	return sName;
}

BOOL CTDCImageList::Draw(CDC* pDC, const CString& sImageName, POINT pt, UINT nStyle) const
{
	return Draw(pDC, GetImageIndex(sImageName), pt, nStyle);
}

BOOL CTDCImageList::Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle) const
{
	CTDCImageList* pThis = const_cast<CTDCImageList*>(this);

	return pThis->CImageList::Draw(pDC, nImage, pt, nStyle);
}

BOOL CTDCImageList::AddImage(const CString& sImageFile, CBitmap& bmImage, COLORREF crTransparent, CTDCImageList* pImages)
{
	ASSERT (pImages);

	if (!pImages)
		return FALSE;

	int nStartIndex = pImages->GetImageCount();
	
	if (pImages->Add(&bmImage, crTransparent))
	{
		// map the images
		int nEndIndex = pImages->GetImageCount() - 1;
		
		// single image -> map by filename
		if (nEndIndex == nStartIndex)
		{
			CString sName = FileMisc::GetFileNameFromPath(sImageFile);
			
			MapImage(nStartIndex, sName, pImages);
		}
		else // map by image index
		{
			for (int nIndex = nStartIndex; nIndex <= nEndIndex; nIndex++)
			{
				MapImage(nIndex, pImages);
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CTDCImageList::MapImage(int nIndex, CTDCImageList* pImages)
{
	MapImage(nIndex, Misc::Format(nIndex), pImages);
}

void CTDCImageList::MapImage(int nIndex, const CString& sName, CTDCImageList* pImages)
{
	pImages->m_mapNameToIndex[sName] = nIndex;
	pImages->m_mapIndexToName[nIndex] = sName;
}

DWORD CTDCImageList::LoadImagesFromFolder(const CString& sFolder, COLORREF crTransparent, CTDCImageList* pImages)
{
	DWORD dwResult = 0;

	// load user's icon set	
	if (LoadImage(sFolder + _T("\\Icons\\TaskIcons.bmp"), crTransparent, pImages))
		dwResult |= TDCIL_LOADEDSET;
	
	// finally add any individual image files
	CFileFind ff;
	CString sSearchPath = sFolder + _T("\\Icons\\*.*");
	BOOL bContinue = ff.FindFile(sSearchPath);
	
	while (bContinue)
	{
		bContinue = ff.FindNextFile();
		
		if (!ff.IsDots() && !ff.IsDirectory())
		{
			if (LoadImage(ff.GetFilePath(), crTransparent, pImages))
			{
				dwResult |= TDCIL_LOADEDIMAGES;

				if (pImages == NULL)
					return dwResult;
			}
		}
	}

	return dwResult;
}

BOOL CTDCImageList::LoadImage(const CString& sImageFile, COLORREF crTransparent, CTDCImageList* pImages)
{
	CEnBitmap image;
	
	if (image.LoadImage(sImageFile, crTransparent, 16, 16))
	{
		if (pImages == NULL)
			return TRUE;

		// else
		return AddImage(sImageFile, image, crTransparent, pImages);
	}

	return FALSE;
}
