//////////////////////////////////////////////////////////////////////////////// 
// 
// Copyright (c) 2018 The Sun.AC Authors . All Rights Reserved. 
// Use of this source code is governed by a BSD-style license that can be 
// found in the LICENSE file. 
// 
//////////////////////////////////////////////////////////////////////////////// 
  
#ifndef _REF_OBSERVER_LIST_INCLUDE_H_ 
#define _REF_OBSERVER_LIST_INCLUDE_H_ 
  
#include <limits>
#include <vector>
#include "cef_callback.h"
#include "cef_bind.h"
#include "callback_base.h"

void Carryed(MainMessageLoop* loop, const base::Closure& closure);

template <class ObserverType>
class RefObserverList {
 public:

  RefObserverList() { Clear(); }
  virtual ~RefObserverList() { Clear(); }

  bool might_have_observers() const {
      return size() != 0;

  }
  typedef scoped_refptr<CallbackBase<ObserverType>> REF_OBSERVER_TYPE;

  // add an observer to the list.  an observer should not be added to
  // the same list more than once.
  void AddObserver(REF_OBSERVER_TYPE obs) {
      if (!obs) { return; } // just check
      if (HasObserver(obs)) { NOTREACHED() << "Observers can only be added once!"; return; }
      observers_.push_back(obs);
  }

  // Remove an observer from the list if it is in the list.
  void RemoveObserver(REF_OBSERVER_TYPE obs) {
      if (observers_.empty()) return;
      auto iter = std::find_if(observers_.begin(), observers_.end(),
          [&](const typename LIST_TYPE::value_type& node) {
          return node && node == obs;
      });
      if (iter == observers_.end()) return;
      observers_.erase(iter);
  }

  bool HasObserver(REF_OBSERVER_TYPE obs) const {
      if (!might_have_observers()) return false;
      auto it = std::find_if(observers_.begin(), observers_.end(),
          [&](const typename LIST_TYPE::value_type& node) {
          return node && node == obs;
      });
      return it != observers_.end();
  }

  void Clear() { observers_.clear(); }

  typedef std::vector<REF_OBSERVER_TYPE> LIST_TYPE;
  LIST_TYPE _Observers() { return observers_; }

private:
    size_t size() const { return observers_.size(); }
    LIST_TYPE observers_;

public:
  // "Go func" for template loop will be production more code
  // so just use for_each
  // func param keep const P&
  inline void Go(void(ObserverType::*func)()) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr());
              Carryed(from, closure);
          }
      }
  }

  template <typename P>
  inline void Go(void(ObserverType::*func)(const P&), const P& p) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p);
              Carryed(from, closure);
          }
      }
  }

  inline void Go(void(ObserverType::*func)(bool p), bool p) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p);
              Carryed(from, closure);
          }
      }
  }

  inline void Go(void(ObserverType::*func)(int p), int p) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p);
              Carryed(from, closure);
          }
      }
  }

  inline void Go(void(ObserverType::*func)(int64 p), int64 p) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p);
              Carryed(from, closure);
          }
      }
  }

  template <typename P, typename P1>
  inline void Go(void(ObserverType::*func)(const P&, const P1&), const P& p, const P1& p1) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);
              Carryed(from, closure);
          }
      }
  }

  inline void Go(void(ObserverType::*func)(bool, bool), bool p, bool p1) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);
              Carryed(from, closure);
          }
      }
  }

  inline void Go(void(ObserverType::*func)(int, bool), int p, bool p1) {      if (might_have_observers()) {          auto observers = _Observers();          for each (auto obs in observers) {              if (!obs) continue;              auto from = obs->from_message_loop();              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);              Carryed(from, closure);          }      }  }

  inline void Go(void(ObserverType::*func)(int64, bool), int64 p, bool p1) {      if (might_have_observers()) {          auto observers = _Observers();          for each (auto obs in observers) {              if (!obs) continue;              auto from = obs->from_message_loop();              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);              Carryed(from, closure);          }      }  }

  inline void Go(void(ObserverType::*func)(int, int), int p, int p1) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);
              Carryed(from, closure);
          }
      }
  }

  inline void Go(void(ObserverType::*func)(int64, int64), int64 p, int64 p1) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);
              Carryed(from, closure);
          }
      }
  }

  inline void Go(void(ObserverType::*func)(bool, int), bool p, int p1) {      if (might_have_observers()) {          auto observers = _Observers();          for each (auto obs in observers) {              if (!obs) continue;              auto from = obs->from_message_loop();              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);              Carryed(from, closure);          }      }  }

  inline void Go(void(ObserverType::*func)(bool, int64), bool p, int64 p1) {      if (might_have_observers()) {          auto observers = _Observers();          for each (auto obs in observers) {              if (!obs) continue;              auto from = obs->from_message_loop();              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1);              Carryed(from, closure);          }      }  }

  template <typename P, typename P1, typename P2>
  inline void Go(void(ObserverType::*func)(const P&, const P1&, const P2&), const P& p, const P1& p1, const P2& p2) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1, p2);
              Carryed(from, closure);
          }
      }
  }

  template <typename P, typename P1, typename P2, typename P3>
  inline void Go(void(ObserverType::*func)(
      const P& p, const P1& p1, const P2& p2, const P3& p3),
      const P& p, const P1& p1, const P2& p2, const P3& p3) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1, p2, p3);
              Carryed(from, closure);
          }
      }
  }

  template <typename P, typename P1, typename P2, typename P3, typename P4>
  inline void Go(void(ObserverType::*func)(
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4),
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1, p2, p3, p4);
              Carryed(from, closure);
          }
      }
  }

  template <typename P, typename P1, typename P2, typename P3, typename P4, typename P5>
  inline void Go(void(ObserverType::*func)(
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5),
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1, p2, p3, p4, p5);
              Carryed(from, closure);
          }
      }
  }

  template <typename P, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  inline void Go(void(ObserverType::*func)(
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6),
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1, p2, p3, p4, p5, p6);
              Carryed(from, closure);
          }
      }
  }

  template <typename P, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  inline void Go(void(ObserverType::*func)(
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7),
      const P& p, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7) {
      if (might_have_observers()) {
          auto observers = _Observers();
          for each (auto obs in observers) {
              if (!obs) continue;
              auto from = obs->from_message_loop();
              auto closure = base::Bind(func, obs->GetWeakPtr(), p, p1, p2, p3, p4, p5, p6, p7);
              Carryed(from, closure);
          }
      }
  }
};

#endif  // !#define (_REF_OBSERVER_LIST_INCLUDE_H_)