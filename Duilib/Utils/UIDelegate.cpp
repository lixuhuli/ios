#include "StdAfx.h"

namespace DuiLib {

CDelegateBase::CDelegateBase(void* pObject, void* pFn) 
{
    m_pObject = pObject;
    m_pFn = pFn; 
}

CDelegateBase::CDelegateBase(const CDelegateBase& rhs) 
{
    m_pObject = rhs.m_pObject;
    m_pFn = rhs.m_pFn; 
}

CDelegateBase::~CDelegateBase()
{

}

bool CDelegateBase::Equals(const CDelegateBase& rhs) const 
{
    return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; 
}

bool CDelegateBase::operator() (void* param) 
{
    return Invoke(param); 
}

void* CDelegateBase::GetFn() 
{
    return m_pFn; 
}

void* CDelegateBase::GetObject() 
{
    return m_pObject; 
}

CEventSource::CEventSource(CEventSource& otherSource)
{
    for (int i = 0; i < otherSource.m_aDelegates.GetSize(); i++) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(otherSource.m_aDelegates[i]);
        if (pObject)
            m_aDelegates.Add(pObject->Copy());
    }
}

CEventSource::CEventSource() : m_bTestDeleted(false)
{

}

CEventSource::~CEventSource()
{
	m_bTestDeleted = true;
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject) delete pObject;
    }
}

CEventSource::operator bool()
{
    return m_aDelegates.GetSize() > 0;
}

void CEventSource::operator+= (const CDelegateBase& d)
{ 
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) return;
    }

    m_aDelegates.Add(d.Copy());
}

void CEventSource::operator+= (FnType pFn)
{ 
    (*this) += MakeDelegate(pFn);
}

void CEventSource::Clear()
{
	for (int i = 0; i < m_aDelegates.GetSize(); i++) 
	{
		CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);

		if (pObject != NULL)
		{
			delete pObject;
		}
	}
	m_aDelegates.Empty();
}
void CEventSource::operator-= (const CDelegateBase& d) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && pObject->Equals(d) ) {
            delete pObject;
            m_aDelegates.Remove(i);
            return;
        }
    }
}
void CEventSource::operator-= (FnType pFn)
{ 
    (*this) -= MakeDelegate(pFn);
}

bool CEventSource::operator() (void* param) 
{
    for( int i = 0; i < m_aDelegates.GetSize(); i++ ) {
        CDelegateBase* pObject = static_cast<CDelegateBase*>(m_aDelegates[i]);
        if( pObject && !(*pObject)(param) ) return false;
		if(TestDeleted()) return false;
    }
    return true;
}

bool CEventSource::TestDeleted()
{
	bool bRet = true;
	try
	{
		bRet = m_bTestDeleted;
	}
	catch(...)
	{
	}

	return bRet;
}

CEventSets::CEventSets()
{

}

CEventSets::~CEventSets()
{
    EventMap::iterator it = m_events.begin();
    while (it != m_events.end())
    {
        delete it->second;
        it++;

    }
    m_events.clear();
}

void CEventSets::Subscribe(QUI_STRING name, CDelegateBase& subscriber)
{
    CEventSource * pSource = GetEventObject(name, true);
    *pSource += subscriber;
}

void CEventSets::ResetEventSets(CEventSets& otherSet)
{
    EventMap& otherEventMap = otherSet.m_events;
    for (EventMap::iterator iMap = otherEventMap.begin();
        iMap != otherEventMap.end(); ++iMap)
    {
        m_events[iMap->first] = new CEventSource(*(iMap->second));
    }
}

CEventSource* CEventSets::GetEventObject(QUI_STRING& name, bool autoGen /*= false*/)
{
    EventMap::iterator pos = m_events.find(name);
    if (pos == m_events.end())
    {
        if (autoGen)
        {
            AddEvent(name);
            return m_events.find(name)->second;
        }
        else
            return NULL;
    }
    return pos->second;
}

void CEventSets::AddEvent(QUI_STRING& name)
{
    CEventSource* eventObj = new CEventSource();
    m_events[name] = eventObj;
}

bool CEventSets::FireEvent(QUI_STRING eventName, void* pEvent)
{
    CEventSource* obj = GetEventObject(eventName);
    if (obj != NULL)
        return (*obj)(pEvent);
    return true;
}

void CEventSets::UnSubscribe(QUI_STRING name, CDelegateBase& subscriber)
{
    CEventSource* obj = GetEventObject(name);
    if (obj != NULL)
    {
        *obj -= subscriber;
    }
}

} // namespace DuiLib
