#include <iostream>  
#include <cassert>


#include <future> 
#include <mutex>
#include <atomic>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "/usr/local/include/g3sinks/syslogsink.hpp"
#include "/usr/local/include/g3sinks/LogRotate.h"
#include "g3logPython.h"


namespace g3 {
    
// Example:  SysLogSinks interface:
//   pySinkCls    : SysLogSnkHndl
//   g3logSinkCls : g3::SyslogSink
template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
template<typename... Args>
pySinkCls ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::new_Sink(const std::string& name, Args... args)
{   
if(!_userNames.reserve(name)) { throw std::logic_error("new_Sink: name already reserved."); }
    
auto sink = std::make_unique<g3logSinkCls>(std::forward<Args>(args)...);
std::unique_ptr<g3::SinkHandle<g3logSinkCls>> g3logHndl(singleton._instance.lock() -> worker.get() -> addSink( std::move(sink), g3logMsgMvr));
    
sinkkey_t key = _g3logPtrs.insert(std::move(g3logHndl));
_userNames.set_key( name, key);
    
return pySinkCls(singleton._instance.lock(), key);
}
    

// explicit instantiations:
template SysLogSnkHndl ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>::new_Sink<const char*>(const std::string&, const char*);
template LogRotateSnkHndl ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>::new_Sink<const std::string&, const std::string&>(const std::string&, const std::string&, const std::string&);    

    
// ====================================================================
// =========================== SysLog  ================================
// ====================================================================

//
void SysLogSnkHndl::setLogHeader(const char* change)
{
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setLogHeader bad key");
if(change == NULL) throw std::logic_error("SysLogSnkHndl::setLogHeader NULL header string");

// note: locks a mutex! note: reader lock. TODO : reader writer lock TODO: RAII
g3::SinkHandle<g3::SyslogSink> * p_g3_hndl = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);

try 
  {
    p_g3_hndl -> call(&g3::SyslogSink::setLogHeaderNoJn, g3::DltrStr(change)); //         
  } catch (...) {
    _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.done(_key); // unlocks the mutex
    throw;
  }
 
_p_wrkrKeepalive -> SysLogSinks._g3logPtrs.done(_key); // unlocks the mutex
}

//
void SysLogSnkHndl::setIdentity(std::string& id)
{   
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

// note: locks a mutex! note: reader lock. TODO : reader writer lock TODO: RAII
g3::SinkHandle<g3::SyslogSink> * p_g3_hndl = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);

try 
  {
    (p_g3_hndl -> call)(&g3::SyslogSink::setIdentityNoJn, g3::DltrStr(id) ); 
  } catch (...) {
    _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.done(_key); // unlocks the mutex
    throw;
  }
_p_wrkrKeepalive -> SysLogSinks._g3logPtrs.done(_key); // unlocks the mutex
}

//
void SysLogSnkHndl::echoToStderr()
{   
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

// note: locks a mutex! note: reader lock. TODO : reader writer lock TODO: RAII
g3::SinkHandle<g3::SyslogSink> * p_g3_hndl = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
try 
  {
    p_g3_hndl -> call(&g3::SyslogSink::echoToStderr);
  } catch (...) {
    _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.done(_key); // unlocks the mutex
    throw;
  }
_p_wrkrKeepalive -> SysLogSinks._g3logPtrs.done(_key); // unlocks the mutex
}




// ====================================================================
// ========================== LogRotate ===============================
// ====================================================================


void LogRotateSnkHndl::setMaxArchiveLogCount(int max_size)
{
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

// note: locks a mutex! note: reader lock. TODO : reader writer lock TODO: RAII
g3::SinkHandle<LogRotate> * p_g3_hndl = _p_wrkrKeepalive -> LogRotateSinks._g3logPtrs.access(_key);
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
