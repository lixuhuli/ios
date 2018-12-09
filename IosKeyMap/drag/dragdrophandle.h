#ifndef NC_DragAndDropHandle_H
#define NC_DragAndDropHandle_H

#pragma once

#include <windows.h>
#include <ole2.h>
#include "objidl.h"

#define BUF_LEN 50

class FAR CDropSource : public IDropSource
{
public:    
	CDropSource();

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppvObj);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	/* IDropSource methods */
	STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
	STDMETHOD(GiveFeedback)(DWORD dwEffect);

private:
	ULONG m_refs;     
};  

class FAR CDataObject : public IDataObject
{
public:
	CDataObject(LPSTR psz);     

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppvObj);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	/* IDataObject methods */    
	STDMETHOD(GetData)(LPFORMATETC pformatetcIn,  LPSTGMEDIUM pmedium );
	STDMETHOD(GetDataHere)(LPFORMATETC pformatetc, LPSTGMEDIUM pmedium );
	STDMETHOD(QueryGetData)(LPFORMATETC pformatetc );
	STDMETHOD(GetCanonicalFormatEtc)(LPFORMATETC pformatetc, LPFORMATETC pformatetcOut);
	STDMETHOD(SetData)(LPFORMATETC pformatetc, STGMEDIUM FAR * pmedium,
		BOOL fRelease);
	STDMETHOD(EnumFormatEtc)(DWORD dwDirection, LPENUMFORMATETC FAR* ppenumFormatEtc);
	STDMETHOD(DAdvise)(FORMATETC FAR* pFormatetc, DWORD advf, 
		LPADVISESINK pAdvSink, DWORD FAR* pdwConnection);
	STDMETHOD(DUnadvise)(DWORD dwConnection);
	STDMETHOD(EnumDAdvise)(LPENUMSTATDATA FAR* ppenumAdvise);

private:
	ULONG m_refs;   
	char m_szBuffer[BUF_LEN];            // Text being transferred
};

class FAR CDropTarget : public IDropTarget
{
public:    
	CDropTarget();

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppvObj);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	/* IDropTarget methods */
	STDMETHOD(DragEnter)(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHOD(DragLeave)();
	STDMETHOD(Drop)(LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect); 

	/* Utility function to read type of drag from key state*/
	STDMETHOD_(BOOL, QueryDrop)(DWORD grfKeyState, LPDWORD pdwEffect);

private:
	ULONG m_refs;  
	BOOL m_bAcceptFmt;
};


#endif