#ifndef __THREAD_HELPER_H__
#define __THREAD_HELPER_H__

#pragma once
#include "AutoLock.h"

template<class T>
class ThreadMgr {
public:
    ThreadMgr() {}

    virtual ~ThreadMgr() {
		Release();
	}

	T* Create() {
		CScopeLock lk(lock);

		T* p = nullptr;
		if (freeList.empty()) p = new T;
		else {
			p = *freeList.begin();
			freeList.pop_front();
		}

		if (p) usedList.push_back(p);

		return p;
	}

	void Delete(T* p) {
		CScopeLock lk(lock);
		if ( nullptr == p ) return ;

		auto itor = std::find(usedList.begin(), usedList.end(), p);
		if ( itor != usedList.end() )usedList.erase(itor);

		freeList.push_back(p);
	}

	void Stop() {
		CScopeLock lk(lock);

		auto itor = usedList.begin();
		for (; itor != usedList.end(); ++itor) {
			T* p = *itor;
			p->StopThread();
		}
	}

protected:
	void Release() {
		CScopeLock lk(lock);

		if (!freeList.empty()) {
			auto itor = freeList.begin();

			for (; itor != freeList.end(); ++itor) {
				T* p = *itor;
				delete p;
			}

			freeList.clear();
		}

		if (!usedList.empty()) {
			auto itor = usedList.begin();

			for (; itor != usedList.end(); ++itor) {
				T* p = *itor;
				p->StopThread();
				delete p;
			}

			usedList.clear();
		}
	}

private:
	CLock lock;
	list<T*> usedList;
	list<T*> freeList;
};

#endif // __THREAD_HELPER_H__