#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <mutex>
#include "Process/ProcessInfo.h"
#include "Process/ProcessModule.h"
#include "Process/ProcessWindow.h"

class Process
{
public:
    enum State {
        Running = 0,
        Stop = 1
    };
    inline static State ProcState = State::Running;


    // Parameter
    ProcessInfo InfoMgr;
    ProcessModule ModuleMgr;
    ProcessWindow WindowMgr;

    Process() {
        InfoMgr = ProcessInfo();
        ModuleMgr = ProcessModule();
        WindowMgr = ProcessWindow();
    }
    ~Process() {}
private:
};

inline Process ProcMgr = Process();