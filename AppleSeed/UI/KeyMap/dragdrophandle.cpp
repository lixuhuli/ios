#include "stdafx.h"
#include "dragdrophandle.h"

#include <windows.h>
#include <ole2.h>
#include "enumfetc.h"    

//---------------------------------------------------------------------
//                    CDropSource Constructor
//---------------------------------------------------------------------        

CDropSource::CDropSource()
{
	m_refs = 1;  
}   

//---------------------------------------------------------------------
//                    IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
CDropSource::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
	if(iid == IID_IUnknown || iid == IID_IDropSource)
	{
		*ppv = this;
		++m_refs;
		return NOERROR;
	}
	*ppv = NULL;
	return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
CDropSource::AddRef(void)
{
	return ++m_refs;
}


STDMETHODIMP_(ULONG)
CDropSource::Release(void)
{
	if(--m_refs == 0)
	{
		delete this;
		return 0;
	}
	return m_refs;
}  

//---------------------------------------------------------------------
//                    IDropSource Methods
//---------------------------------------------------------------------  

STDMETHODIMP
CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{  
	if (fEscapePressed)
		return ResultFromScode(DRAGDROP_S_CANCEL);
	else if (!(grfKeyState & MK_LBUTTON))
		return ResultFromScode(DRAGDROP_S_DROP);
	else
		return NOERROR;                  
}

STDMETHODIMP
CDropSource::GiveFeedback(DWORD dwEffect)
{
	return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);
}

//---------------------------------------------------------------------
//                    CDataObject Constructor
//---------------------------------------------------------------------        

CDataObject::CDataObject(LPSTR psz)
{
	m_refs = 1;    
	lstrcpyA(m_szBuffer, psz);
}   

//---------------------------------------------------------------------
//                    IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
CDataObject::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
	if(iid == IID_IUnknown || iid == IID_IDataObject)
	{
		*ppv = this;
		AddRef();
		return NOERROR;
	}
	*ppv = NULL;
	return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
CDataObject::AddRef(void)
{
	return ++m_refs;
}


STDMETHODIMP_(ULONG)
CDataObject::Release(void)
{
	if(--m_refs == 0)
	{
		delete this;
		return 0;
	}
	return m_refs;
}  

//---------------------------------------------------------------------
//                    IDataObject Methods    
//  
// The following methods are NOT supported for data transfer using the
// clipboard or drag-drop: 
//
//      IDataObject::SetData    -- return E_NOTIMPL
//      IDataObject::DAdvise    -- return OLE_E_ADVISENOTSUPPORTED
//                 ::DUnadvise
//                 ::EnumDAdvise
//      IDataObject::GetCanonicalFormatEtc -- return E_NOTIMPL
//                     (NOTE: must set pformatetcOut->ptd = NULL)
//---------------------------------------------------------------------  

STDMETHODIMP 
CDataObject::GetData(LPFORMATETC pformatetc, LPSTGMEDIUM pmedium) 
{   
	HGLOBAL hText; 
	LPSTR pszText;

	pmedium->tymed = NULL;
	pmedium->pUnkForRelease = NULL;
	pmedium->hGlobal = NULL;

	// This method is called by the drag-drop target to obtain the text
	// that is being dragged.
	if (pformatetc->cfFormat == CF_TEXT &&
		pformatetc->dwAspect == DVASPECT_CONTENT &&
		pformatetc->tymed == TYMED_HGLOBAL)
	{
		hText = GlobalAlloc(GMEM_SHARE | GMEM_ZEROINIT, sizeof(m_szBuffer)+1);    
		if (!hText)
			return ResultFromScode(E_OUTOFMEMORY);
		pszText = (LPSTR)GlobalLock(hText);
		lstrcpyA(pszText, m_szBuffer);
		GlobalUnlock(hText);

		pmedium->tymed = TYMED_HGLOBAL;
		pmedium->hGlobal = hText; 

		return ResultFromScode(S_OK);
	}
	return ResultFromScode(DATA_E_FORMATETC);
}

STDMETHODIMP 
CDataObject::GetDataHere(LPFORMATETC pformatetc, LPSTGMEDIUM pmedium)  
{
	return ResultFromScode(DATA_E_FORMATETC);    
}     

STDMETHODIMP 
CDataObject::QueryGetData(LPFORMATETC pformatetc) 
{   
	// This method is called by the drop target to check whether the source
	// provides data is a format that the target accepts.
	if (pformatetc->cfFormat == CF_TEXT 
		&& pformatetc->dwAspect == DVASPECT_CONTENT
		&& pformatetc->tymed & TYMED_HGLOBAL)
		return ResultFromScode(S_OK); 
	else return ResultFromScode(S_FALSE);
}

STDMETHODIMP 
CDataObject::GetCanonicalFormatEtc(LPFORMATETC pformatetc, LPFORMATETC pformatetcOut)
{ 
	pformatetcOut->ptd = NULL; 
	return ResultFromScode(E_NOTIMPL);
}        

STDMETHODIMP 
CDataObject::SetData(LPFORMATETC pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{   
	// A data transfer object that is used to transfer data
	//    (either via the clipboard or drag/drop does NOT
	//    accept SetData on ANY format.
	return ResultFromScode(E_NOTIMPL);
}


STDMETHODIMP 
CDataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC FAR* ppenumFormatEtc)
{ 
	// A standard implementation is provided by OleStdEnumFmtEtc_Create
	// which can be found in \ole2\samp\ole2ui\enumfetc.c in the OLE 2 SDK.
	// This code from ole2ui is copied to the enumfetc.c file in this sample.

	SCODE sc = S_OK;
	FORMATETC fmtetc;

	fmtetc.cfFormat = CF_TEXT;
	fmtetc.dwAspect = DVASPECT_CONTENT;
	fmtetc.tymed = TYMED_HGLOBAL;
	fmtetc.ptd = NULL;
	fmtetc.lindex = -1;

	if (dwDirection == DATADIR_GET){
		*ppenumFormatEtc = OleStdEnumFmtEtc_Create(1, &fmtetc);
		if (*ppenumFormatEtc == NULL)
			sc = E_OUTOFMEMORY;

	} else if (dwDirection == DATADIR_SET){
		// A data transfer object that is used to transfer data
		//    (either via the clipboard or drag/drop does NOT
		//    accept SetData on ANY format.
		sc = E_NOTIMPL;
		goto error;
	} else {
		sc = E_INVALIDARG;
		goto error;
	}

error:
	return ResultFromScode(sc);
}

STDMETHODIMP 
CDataObject::DAdvise(FORMATETC FAR* pFormatetc, DWORD advf, 
					 LPADVISESINK pAdvSink, DWORD FAR* pdwConnection)
{ 
	return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);
}


STDMETHODIMP 
CDataObject::DUnadvise(DWORD dwConnection)
{ 
	return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);
}

STDMETHODIMP 
CDataObject::EnumDAdvise(LPENUMSTATDATA FAR* ppenumAdvise)
{ 
	return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);
}


//////////////////////////////////////////////////////////////////////////
CDropTarget::CDropTarget()
{
	m_refs = 1; 
	m_bAcceptFmt = FALSE;
}   

//---------------------------------------------------------------------
//                    IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
CDropTarget::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
	if(iid == IID_IUnknown || iid == IID_IDropTarget)
	{
		*ppv = this;
		AddRef();
		return NOERROR;
	}
	*ppv = NULL;
	return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
CDropTarget::AddRef(void)
{
	return ++m_refs;
}


STDMETHODIMP_(ULONG)
CDropTarget::Release(void)
{
	if(--m_refs == 0)
	{
		delete this;
		return 0;
	}
	return m_refs;
}  

//---------------------------------------------------------------------
//                    IDropTarget Methods
//---------------------------------------------------------------------  

STDMETHODIMP
CDropTarget::DragEnter(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{  
	FORMATETC fmtetc;

	fmtetc.cfFormat = CF_TEXT;
	fmtetc.ptd      = NULL;
	fmtetc.dwAspect = DVASPECT_CONTENT;  
	fmtetc.lindex   = -1;
	fmtetc.tymed    = TYMED_HGLOBAL; 

	// Does the drag source provide CF_TEXT, which is the only format we accept.    
	m_bAcceptFmt = (NOERROR == pDataObj->QueryGetData(&fmtetc)) ? TRUE : FALSE;    

	QueryDrop(grfKeyState, pdwEffect);
	return NOERROR;
}

STDMETHODIMP
CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
	QueryDrop(grfKeyState, pdwEffect);
	return NOERROR;
}

STDMETHODIMP
CDropTarget::DragLeave()
{   
	m_bAcceptFmt = FALSE;   
	return NOERROR;
}

STDMETHODIMP
CDropTarget::Drop(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)  
{   
	FORMATETC fmtetc;   
	STGMEDIUM medium;   
	HGLOBAL hText;
	LPSTR pszText;
	HRESULT hr;

	if (QueryDrop(grfKeyState, pdwEffect))
	{      
		fmtetc.cfFormat = CF_TEXT;
		fmtetc.ptd = NULL;
		fmtetc.dwAspect = DVASPECT_CONTENT;  
		fmtetc.lindex = -1;
		fmtetc.tymed = TYMED_HGLOBAL;       

		// User has dropped on us. Get the CF_TEXT data from drag source
		hr = pDataObj->GetData(&fmtetc, &medium);
		if (FAILED(hr))
			goto error; 

		// Display the data and release it.
		hText = medium.hGlobal;
		pszText = (LPSTR)GlobalLock(hText);
		//SetDlgItemText(ghwnd, ID_EDIT, pszText);
		GlobalUnlock(hText);
		ReleaseStgMedium(&medium);
	}
	return NOERROR;      

error:
	*pdwEffect = DROPEFFECT_NONE;
	return hr; 
}   

/* OleStdGetDropEffect
** -------------------
**
** Convert a keyboard state into a DROPEFFECT.
**
** returns the DROPEFFECT value derived from the key state.
**    the following is the standard interpretation:
**          no modifier -- Default Drop     (0 is returned)
**          CTRL        -- DROPEFFECT_COPY
**          SHIFT       -- DROPEFFECT_MOVE
**          CTRL-SHIFT  -- DROPEFFECT_LINK
**
**    Default Drop: this depends on the type of the target application.
**    this is re-interpretable by each target application. a typical
**    interpretation is if the drag is local to the same document
**    (which is source of the drag) then a MOVE operation is
**    performed. if the drag is not local, then a COPY operation is
**    performed.
*/
#define OleStdGetDropEffect(grfKeyState)    \
	( (grfKeyState & MK_CONTROL) ?          \
	( (grfKeyState & MK_SHIFT) ? DROPEFFECT_LINK : DROPEFFECT_COPY ) :  \
	( (grfKeyState & MK_SHIFT) ? DROPEFFECT_MOVE : 0 ) )

//---------------------------------------------------------------------
// CDropTarget::QueryDrop: Given key state, determines the type of 
// acceptable drag and returns the a dwEffect. 
//---------------------------------------------------------------------   
STDMETHODIMP_(BOOL)
CDropTarget::QueryDrop(DWORD grfKeyState, LPDWORD pdwEffect)
{  
	DWORD dwOKEffects = *pdwEffect; 

	if (!m_bAcceptFmt)
		goto dropeffect_none; 

	*pdwEffect = OleStdGetDropEffect(grfKeyState);
	if (*pdwEffect == 0) {
		// No modifier keys used by user while dragging. Try in order: MOVE, COPY.
		if (DROPEFFECT_MOVE & dwOKEffects)
			*pdwEffect = DROPEFFECT_MOVE;
		else if (DROPEFFECT_COPY & dwOKEffects)
			*pdwEffect = DROPEFFECT_COPY; 
		else goto dropeffect_none;   
	} 
	else {
		// Check if the drag source application allows the drop effect desired by user.
		// The drag source specifies this in DoDragDrop
		if (!(*pdwEffect & dwOKEffects))
			goto dropeffect_none; 
		// We don't accept links
		if (*pdwEffect == DROPEFFECT_LINK)
			goto dropeffect_none; 
	}  
	return TRUE;

dropeffect_none:
	*pdwEffect = DROPEFFECT_NONE;
	return FALSE;
} 