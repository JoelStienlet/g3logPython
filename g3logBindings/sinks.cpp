
#include "intern_log.h"
#include "g3logBindings.h"

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
if( (_options & MULT_INSTANCES_ALLOWED) == 0) {
    // some sinks may not allow to be instantiated multiple times, for example syslog.
    if(_userNames.get_size() > 0) throw std::logic_error("new_Sink: this sink can only be instantiated once.");
    }
    
if(!_userNames.reserve(name)) { throw std::logic_error("new_Sink: name already reserved."); }
       

//   Problem: args are destroyed by python whenever it pleases it...
//   so we have to copy the args and store them until the program finishes.
//   auto sink = std::make_unique<g3logSinkCls>(std::forward<Args>(args)...); <-- we cannot simply forward the args
//   https://stackoverflow.com/questions/47848910/apply-function-on-each-element-in-parameter-pack
// NOTE: in practice, after PR --> store() no longer needed!
// 
auto sink = std::make_unique<g3logSinkCls>(std::forward<Args>(args)...);

std::unique_ptr<g3::SinkHandle<g3logSinkCls>> g3logHndl(singleton._instance.lock() -> worker.get() -> addSink( std::move(sink), g3logMsgMvr));
    
sinkkey_t key = _g3logPtrs.insert(std::move(g3logHndl));
_userNames.set_key( name, key);
    
return pySinkCls(singleton._instance.lock(), key);
}
      
template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
pySinkCls 
ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::
new_SinkHndl(const std::string& name)
{
sinkkey_t key =  _userNames.get_key(name);
return pySinkCls(singleton._instance.lock(), key);
}
      
// explicit instantiations: (see also worker.cpp)
template ClrTermSnkHndl ifaceLogWorker::ClrTermSinkIface_t::new_Sink<>(const std::string&);
template SysLogSnkHndl ifaceLogWorker::SysLogSinkIface_t::new_Sink<const char*>(const std::string&, const char*);
template LogRotateSnkHndl ifaceLogWorker::LogRotateSinkIface_t::new_Sink<const std::string&, const std::string&>(const std::string&, const std::string&, const std::string&);    

template ClrTermSnkHndl   ifaceLogWorker::ClrTermSinkIface_t::new_SinkHndl(const std::string& name);
template SysLogSnkHndl    ifaceLogWorker::SysLogSinkIface_t::new_SinkHndl(const std::string& name);
template LogRotateSnkHndl ifaceLogWorker::LogRotateSinkIface_t::new_SinkHndl(const std::string& name);

// ====================================================================
// =========================== Color Term  ============================
// ====================================================================

// ====================================================================
// =========================== SysLog  ================================
// ====================================================================

//
PyFuture<void> SysLogSnkHndl::setLogHeader(const char* change)
{
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setLogHeader bad key");
if(change == NULL) throw std::logic_error("SysLogSnkHndl::setLogHeader NULL header string");

auto p_HdrData = std::make_shared<Helper1StrStore> (change);

PyFuture<void> fut_for_py;
  { // raii mutex locking with access()
    g3::LockedObj<g3::SinkHandle<g3::SyslogSink> *> MtxPtr = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
    fut_for_py.take_fut(MtxPtr.p_hndl -> call(&g3::SyslogSink::setLogHeader, p_HdrData -> c_str()));
    p_HdrData -> set_future(fut_for_py.get_copy());
  }
_p_wrkrKeepalive -> pStore.get() -> store(p_HdrData); // store() locks a mutex: the _key mutex should be unlocked to avoid deadlocks
return fut_for_py;
}

//
PyFuture<void> SysLogSnkHndl::setIdentity(std::string& id)
{   
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

auto p_IdData = std::make_shared<Helper1StrStore> (id);

PyFuture<void> fut_for_py;
  { // raii mutex locking with access() 
     g3::LockedObj<g3::SinkHandle<g3::SyslogSink> *> MtxPtr = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
     fut_for_py.take_fut(MtxPtr.p_hndl -> call(&g3::SyslogSink::setIdentity, p_IdData -> c_str()));
     p_IdData -> set_future(fut_for_py.get_copy());
  }
_p_wrkrKeepalive -> pStore.get() -> store(p_IdData); // store() locks a mutex: the _key mutex should be unlocked to avoid deadlocks
return fut_for_py;
}

//
PyFuture<void> SysLogSnkHndl::echoToStderr()
{   
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

auto p_IdData = std::make_shared<StoredForThd<void>> ();

PyFuture<void> fut_for_py;
  { // raii mutex locking with access()
    g3::LockedObj<g3::SinkHandle<g3::SyslogSink> *> MtxPtr = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
    fut_for_py.take_fut(MtxPtr.p_hndl -> call(&g3::SyslogSink::echoToStderr));
    p_IdData -> set_future(fut_for_py.get_copy());
  }
_p_wrkrKeepalive -> pStore.get() -> store(p_IdData); // store() locks a mutex: the _key mutex should be unlocked to avoid deadlocks
return fut_for_py;
}


PyFuture<void> SysLogSnkHndl::muteStderr()
{   
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

auto p_IdData = std::make_shared<StoredForThd<void>> ();

PyFuture<void> fut_for_py;
  { // raii mutex locking with access()
    g3::LockedObj<g3::SinkHandle<g3::SyslogSink> *> MtxPtr = _p_wrkrKeepalive -> SysLogSinks._g3logPtrs.access(_key);
    fut_for_py.take_fut(MtxPtr.p_hndl -> call(&g3::SyslogSink::muteStderr)); 
    p_IdData -> set_future(fut_for_py.get_copy());
  }
_p_wrkrKeepalive -> pStore.get() -> store(p_IdData); // store() locks a mutex: the _key mutex should be unlocked to avoid deadlocks
return fut_for_py;
}

// ====================================================================
// ========================== LogRotate ===============================
// ====================================================================


PyFuture<void> LogRotateSnkHndl::setMaxArchiveLogCount(int max_size)
{
if(_key == InvalidSinkKey) throw std::logic_error("SysLogSnkHndl::setIdentity bad key");

auto p_IdData = std::make_shared<StoredForThd<void>> (); // nothing to store, as the data (max_size) is a simple int

PyFuture<void> fut_for_py;
  { // raii mutex locking with access()
    g3::LockedObj<g3::SinkHandle<LogRotate> *> MtxPtr = _p_wrkrKeepalive -> LogRotateSinks._g3logPtrs.access(_key);
    fut_for_py.take_fut(MtxPtr.p_hndl -> call(&LogRotate::setMaxArchiveLogCount, max_size));
    p_IdData -> set_future(fut_for_py.get_copy());
  }
_p_wrkrKeepalive -> pStore.get() -> store(p_IdData); // store() locks a mutex: the _key mutex should be unlocked to avoid deadlocks
return fut_for_py;
}



} // g3
