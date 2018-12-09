#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once
#include <map>

#ifdef _UNICODE
#define QUI_STRING std::wstring
#else
#define QUI_STRING std::string
#endif

namespace DuiLib {

class UILIB_API CDelegateBase	 
{
public:
    CDelegateBase(void* pObject, void* pFn);
    CDelegateBase(const CDelegateBase& rhs);
    virtual ~CDelegateBase();
    bool Equals(const CDelegateBase& rhs) const;
    bool operator() (void* param);
    virtual CDelegateBase* Copy() const = 0; // add const for gcc

protected:
    void* GetFn();
    void* GetObject();
    virtual bool Invoke(void* param) = 0;

private:
    void* m_pObject;
    void* m_pFn;
};

class CDelegateStatic: public CDelegateBase
{
    typedef bool (*Fn)(void*);
public:
    CDelegateStatic(Fn pFn) : CDelegateBase(NULL, pFn) { } 
    CDelegateStatic(const CDelegateStatic& rhs) : CDelegateBase(rhs) { } 
    virtual CDelegateBase* Copy() const { return new CDelegateStatic(*this); }

protected:
    virtual bool Invoke(void* param)
    {
        Fn pFn = (Fn)GetFn();
        return (*pFn)(param); 
    }
};

template <class O, class T>
class CDelegate : public CDelegateBase
{
    typedef bool (T::* Fn)(void*);
public:
    CDelegate(O* pObj, Fn pFn) : CDelegateBase(pObj, &pFn), m_pFn(pFn) { }
    CDelegate(const CDelegate& rhs) : CDelegateBase(rhs) { m_pFn = rhs.m_pFn; } 
    virtual CDelegateBase* Copy() const { return new CDelegate(*this); }

protected:
    virtual bool Invoke(void* param)
    {
        O* pObject = (O*) GetObject();
        return (pObject->*m_pFn)(param); 
    }  

private:
    Fn m_pFn;
};

template <class O, class T>
CDelegate<O, T> MakeDelegate(O* pObject, bool (T::* pFn)(void*))
{
    return CDelegate<O, T>(pObject, pFn);
}

inline CDelegateStatic MakeDelegate(bool (*pFn)(void*))
{
    return CDelegateStatic(pFn); 
}

class UILIB_API CEventSource
{
    typedef bool (*FnType)(void*);
public:
    ~CEventSource();
    CEventSource();
    CEventSource(CEventSource& otherSource);
    operator bool();
    void operator+= (const CDelegateBase& d); // add const for gcc
    void operator+= (FnType pFn);
    void operator-= (const CDelegateBase& d);
    void operator-= (FnType pFn);
    bool operator() (void* param);
public:
	void Clear();
protected:
    CStdPtrArray m_aDelegates;

	bool m_bTestDeleted;
	bool TestDeleted();
};

class UILIB_API CEventSets
{
    typedef std::map<QUI_STRING, CEventSource*> EventMap;
public:
    CEventSets();
    virtual ~CEventSets();
    void ResetEventSets(CEventSets& pEventSets);
    void Subscribe(QUI_STRING name, CDelegateBase& subscriber);
    void UnSubscribe(QUI_STRING name, CDelegateBase& subscriber);
    CEventSource* GetEventObject(QUI_STRING& name, bool autoGen = false);
    void AddEvent(QUI_STRING& name);
    bool FireEvent(QUI_STRING eventName, void* pEvent);//TEventUI

protected:
    EventMap m_events;
};

} // namespace DuiLib

#endif // __UIDELEGATE_H__