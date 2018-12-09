#include "stdafx.h"
#include "ref_observer_list.h"

void Carryed(MainMessageLoop* loop, const base::Closure& closure) {
    if (!loop) { closure.Run(); return; }
    if (loop->RunsTasksOnCurrentThread()) closure.Run();
    else loop->PostClosure(closure);
}