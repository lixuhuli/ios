#pragma once
#ifndef _UILIB_UTIL_INCLUDE_H_
#define _UILIB_UTIL_INCLUDE_H_

#include <functional>

namespace DuiLib {

template<typename T = void>
class CDlegateParam : public CDelegateBase {
public:
    typedef std::function<bool(T* param)> InvokeFuncStorage;

	CDlegateParam(InvokeFuncStorage invoke, void* address) 
		: CDelegateBase(address, nullptr) {
		invoke_func = invoke;
	}

	CDlegateParam(const CDlegateParam& rhs) 
		: CDelegateBase(rhs) { 
		invoke_func = rhs.invoke_func;
	}

	virtual ~CDlegateParam() {}

	virtual CDelegateBase* Copy() const { 
		return new CDlegateParam(*this); 
	}
	
protected:
	virtual bool Invoke(void* param) {
        T* t = reinterpret_cast<T*>(param);
		return invoke_func(t);
	}

protected:
	InvokeFuncStorage invoke_func;
};

class CDlegateWithOutParam : public CDelegateBase {
public:
    typedef std::function<bool()> InvokeFuncStorage;

    CDlegateWithOutParam(InvokeFuncStorage invoke, void* address)
        : CDelegateBase(address, nullptr) {
        invoke_func = invoke;
    }

    CDlegateWithOutParam(const CDlegateWithOutParam& rhs)
        : CDelegateBase(rhs) {
        invoke_func = rhs.invoke_func;
    }

    virtual ~CDlegateWithOutParam() {}

    virtual CDelegateBase* Copy() const {
        return new CDlegateWithOutParam(*this);
    }

protected:
    virtual bool Invoke(void* param) {
        return invoke_func();
    }

protected:
    InvokeFuncStorage invoke_func;
};

template<typename T>
inline CDlegateParam<T> MakeDelegate(const std::function<bool(T*)>& invoke, void* address = nullptr) {
	return CDlegateParam<T>(invoke, address);
}

inline CDlegateWithOutParam MakeDelegate(const std::function<bool()>& invoke, void* address = nullptr) {
    return CDlegateWithOutParam(invoke, address);
}

template<class T, class O, class C>
inline CDlegateParam<T> MakeDelegate(O* object, bool (C::* func)(T*)) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1), object);
}

template<class O, class C>
inline CDlegateWithOutParam MakeDelegate(O* object, bool (C::* func)()) {
    return CDlegateWithOutParam(std::bind(func, object), object);
}

// This must compile with vs2013.
// template<typename O, typename C, typename T, typename ... ARGS>
// inline CDlegateParam<T> MakeDelegate(O* object, bool(C::* func)(T*, ARGS&&... args)) {
//     return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, std::forward<ARGS>(args)...));
// }

template<class T, class O, class C, class P1>
inline CDlegateParam<T> MakeDelegate(
    O* object, 
    bool (C::* func)(T*, P1 p1), 
    P1 p1) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, p1), object);
}

template<class O, class C, class P1>
inline CDlegateWithOutParam MakeDelegate(
    O* object,
    bool (C::* func)(P1 p1),
    P1 p1) {
    return CDlegateWithOutParam(std::bind(func, object, p1), object);
}

template<class T, class O, class C, class P1, class P2>
inline CDlegateParam<T> MakeDelegate(
    O* object, 
    bool (C::* func)(T*, P1 p1, P2 p2), 
    P1 p1, P2 p2) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, p1, p2), object);
}

template<class O, class C, class P1, class P2>
inline CDlegateWithOutParam MakeDelegate(
    O* object,
    bool (C::* func)(P1 p1, P2 p2),
    P1 p1, P2 p2) {
    return CDlegateWithOutParam(std::bind(func, object, p1, p2), object);
}

template<class T, class O, class C, class P1, class P2, class P3>
inline CDlegateParam<T> MakeDelegate(
    O* object, 
    bool (C::* func)(T*, P1 p1, P2 p2, P3 p3), 
    P1 p1, P2 p2, P3 p3) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, p1, p2, p3), object);
}

template<class O, class C, class P1, class P2, class P3>
inline CDlegateWithOutParam MakeDelegate(
    O* object,
    bool (C::* func)(P1 p1, P2 p2, P3 p3),
    P1 p1, P2 p2, P3 p3) {
    return CDlegateWithOutParam(std::bind(func, object, p1, p2, p3), object);
}

template<class T, class O, class C, class P1, class P2, class P3, class P4>
inline CDlegateParam<T> MakeDelegate(
    O* object, 
    bool (C::* func)(T*, P1 p1, P2 p2, P3 p3, P4 p4), 
    P1 p1, P2 p2, P3 p3, P4 p4) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, p1, p2, p3, p4), object);
}

template<class O, class C, class P1, class P2, class P3, class P4>
inline CDlegateWithOutParam MakeDelegate(
    O* object,
    bool (C::* func)(P1 p1, P2 p2, P3 p3, P4 p4),
    P1 p1, P2 p2, P3 p3, P4 p4) {
    return CDlegateWithOutParam(std::bind(func, object, p1, p2, p3, p4), object);
}

template<class T, class O, class C, class P1, class P2, class P3, class P4, class P5>
inline CDlegateParam<T> MakeDelegate(
    O* object, 
    bool (C::* func)(T*, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5), 
    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, p1, p2, p3, p4, p5), object);
}

template<class O, class C, class P1, class P2, class P3, class P4, class P5>
inline CDlegateWithOutParam MakeDelegate(
    O* object,
    bool (C::* func)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5),
    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
    return CDlegateWithOutParam(std::bind(func, object, p1, p2, p3, p4, p5), object);
}

template<class T, class O, class C, class P1, class P2, class P3, class P4, class P5, class P6>
inline CDlegateParam<T> MakeDelegate(
    O* object, 
    bool (C::* func)(T*, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6), 
    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, p1, p2, p3, p4, p5, p6), object);
}

template<class O, class C, class P1, class P2, class P3, class P4, class P5, class P6>
inline CDlegateWithOutParam MakeDelegate(
    O* object,
    bool (C::* func)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6),
    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) {
    return CDlegateWithOutParam(std::bind(func, object, p1, p2, p3, p4, p5, p6), object);
}

template<class T, class O, class C, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline CDlegateParam<T> MakeDelegate(
    O* object,
    bool (C::* func)(T*, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7),
    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
    return CDlegateParam<T>(std::bind(func, object, std::placeholders::_1, p1, p2, p3, p4, p5, p6, p7), object);
}

template<class O, class C, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
inline CDlegateWithOutParam MakeDelegate(
    O* object,
    bool (C::* func)(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7),
    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) {
    return CDlegateWithOutParam(std::bind(func, object, p1, p2, p3, p4, p5, p6, p7), object);
}

}

#endif	// !defined(_UILIB_UTIL_INCLUDE_H_)