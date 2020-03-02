
#include "intern_log.h"
#include "g3logPython.h"

namespace g3 {
    
// Example:  SysLogSinks interface:
//   pySinkCls    : SysLogSnkHndl
//   g3logSinkCls : g3::SyslogSink
template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
template<typename... Args>
pySinkCls 
ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::
new_Sink(const std::string& name, Args... args)
{   
if(!_userNames.reserve(name)) { throw std::logic_error("new_Sink: name already reserved."); }
    
auto sink = std::make_unique<g3logSinkCls>(std::forward<Args>(args)...);
std::unique_ptr<g3::SinkHandle<g3logSinkCls>> g3logHndl(singleton._instance.lock() -> worker.get() -> addSink( std::move(sink), g3logMsgMvr));
    
sinkkey_t key = _g3logPtrs.insert(std::move(g3logHndl));
_userNames.set_key( name, key);
    
return pySinkCls(singleton._instance.lock(), key);
}
      
// explicit instantiations: (see also worker.cpp)
template ClrTermSnkHndl ifaceLogWorker::ClrTermSinkIface_t::new_Sink<>(const std::string&);
template SysLogSnkHndl ifaceLogWorker::SysLogSinkIface_t::new_Sink<const char*>(const std::string&, const char*);
template LogRotateSnkHndl ifaceLogWorker::LogRotateSinkIface_t::new_Sink<const std::string&, const std::string&>(const std::string&, const std::string&, const std::string&);    

    
// ====================================================================
// =========================== Color Term  ============================
// ====================================================================

// ====================================================================
// =========================== SysLog  ================================
// ====================================================================

//
void SysLogSnkHndl::setLogHeader(const char* change)
{
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setLogHeader bad key");
if(change == NULL) throw std::logic_error("SysLogSnkHndl::setLogHeader NULL header string");

auto p_HdrData = std::make_shared<Helper1StrStore> (change);

  { // raii mutex locking with access()
    g3::LockedObj<g3::SinkHandle<g3::SyslogSink> *> MtxPtr = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
    p_HdrData -> set_future(MtxPtr.p_hndl -> call(&g3::SyslogSink::setLogHeader, p_HdrData -> c_str()));
  }
_p_wrkrKeepalive -> Store.store(p_HdrData); // store() locks a mutex: the _key mutex should be unlocked to avoid deadlocks
}

//
void SysLogSnkHndl::setIdentity(std::string& id)
{   
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

auto p_IdData = std::make_shared<Helper1StrStore> (id);

  { // raii mutex locking with access() 
     g3::LockedObj<g3::SinkHandle<g3::SyslogSink> *> MtxPtr = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
     p_IdData -> set_future(MtxPtr.p_hndl -> call(&g3::SyslogSink::setIdentity, p_IdData -> c_str()));
  }
_p_wrkrKeepalive -> Store.store(p_IdData); // store() locks a mutex: the _key mutex should be unlocked to avoid deadlocks
}

//
void SysLogSnkHndl::echoToStderr()
{   
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

  { // raii mutex locking with access()
    g3::LockedObj<g3::SinkHandle<g3::SyslogSink> *> MtxPtr = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
    MtxPtr.p_hndl -> call(&g3::SyslogSink::echoToStderr); 
  }
// TODO : add to store !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}




// ====================================================================
// ========================== LogRotate ===============================
// ====================================================================


void LogRotateSnkHndl::setMaxArchiveLogCount(int max_size)
{
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

// note: locks a mutex! note: reader lock. TODO : reader writer lock TODO: RAII
g3::SinkHandle<LogRotate> * p_g3_hndl = _p_wrkrKeepalive -> LogRotateSinks._g3logPtrs.accessTOREPLACE(_key);
try 
  {
    p_g3_hndl -> call(&LogRotate::setMaxArchiveLogCount, max_size); 
  } catch (...) {
    _p_wrkrKeepalive -> LogRotateSinks._g3logPtrs.done(_key); // unlocks the mutex
    throw;
  }
_p_wrkrKeepalive -> LogRotateSinks._g3logPtrs.done(_key); // unlocks the mutex
}



} // g3
