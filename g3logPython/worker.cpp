#include <iostream>  

#include <mutex>
#include <atomic>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "/usr/local/include/g3sinks/syslogsink.hpp"
#include "/usr/local/include/g3sinks/LogRotate.h"
#include "g3logPython.h"


namespace g3 {
    
// some globals for the singleton class:
std::once_flag ifaceLogWorker::sglt_t::initInstanceFlag;
std::once_flag ifaceLogWorker::sglt_t::killKeepaliveFlag;
std::weak_ptr<ifaceLogWorker> ifaceLogWorker::sglt_t::_instance;
std::shared_ptr<ifaceLogWorker> ifaceLogWorker::sglt_t::_keepalive;
bool ifaceLogWorker::sglt_t::_scoped;
    
// see description in .h    
std::shared_ptr<ifaceLogWorker> ifaceLogWorker::get_ifaceLogWorker(bool scope_lifetime )
{ 
  std::call_once(singleton.initInstanceFlag, &sglt_t::initSingleton, scope_lifetime);
  std::shared_ptr<ifaceLogWorker> pworker = singleton._instance.lock();
  if(singleton._scoped) {
      std::call_once(singleton.killKeepaliveFlag, &sglt_t::kill_keepalive); 
      }
  return pworker;
}
    
    
void ifaceLogWorker::sglt_t::initSingleton(bool scope)
{
// test if g3log is already started:
if(g3::internal::isLoggingInitialized()) {
    std::cerr << "ERROR: in ifaceLogWorker -> initSingleton() : g3log is already initialized." << std::endl << std::flush;
    throw std::logic_error("g3log already initialized");
    }
        
_keepalive = std::shared_ptr<ifaceLogWorker>(new ifaceLogWorker()); 
_instance = _keepalive; 
_scoped = scope;
    
// init g3log:
_instance.lock() -> worker = g3::LogWorker::createLogWorker();
g3::initializeLogging(_instance.lock() -> worker.get());
}
   
    
template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
g3::SinkHandle<g3logSinkCls> * ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::Ptr_Mnger::access(sinkkey_t key)
{
  _lock.lock(); // lock_shared()  TODO
  auto search = _key_to_uniquePtr.find(key);
  if (search == _key_to_uniquePtr.end()) throw std::logic_error("Ptr_Mnger::access element not in map");
  g3::SinkHandle<g3logSinkCls> *naked = search->second.get();
  if(naked == NULL)  throw std::logic_error("Ptr_Mnger::access null in map");
  return naked;
}
   

template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
void ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::Ptr_Mnger::done(sinkkey_t key)
{
  _lock.unlock(); // lock_shared() TODO
}

   
template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
sinkkey_t ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::Ptr_Mnger::insert(std::unique_ptr<g3::SinkHandle<g3logSinkCls>> g3logHandle)
{
sinkkey_t newkey;

  { // raii mutex scope
      std::lock_guard<std::mutex> raiiLock(_lock); // https://en.cppreference.com/w/cpp/thread/scoped_lock
      // https://en.cppreference.com/w/cpp/container/set :
      if(_free.empty()) {
          // generate a new key
          if(_in_use.empty()){
              // its our first key
              newkey = 1;
            } else {
              // get the value of the last key inserted
              auto lastkey = _in_use.end();
              newkey = 1 + (*lastkey);
            }
          _in_use.insert(newkey);
        } else {
          // reuse a previously-used key
          auto freekey = _free.end();
          newkey = *freekey;
          _free.erase(freekey);
        }
      _key_to_uniquePtr.insert({newkey,std::move(g3logHandle) });
    } // end of RAII mutex scope
        
return newkey;
}
   
template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
bool ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::Name_Mnger::reserve(const std::string& name)
{
  { // raii mutex scope
      std::lock_guard<std::mutex> raiiLock(_lock); // https://en.cppreference.com/w/cpp/thread/scoped_lock
      auto search = _name_to_key.find(name);
      if (search != _name_to_key.end()) {
          // exists already!
          return false;
          }
      _name_to_key.insert({name, InvalidSinkKey});
      return true;
  } // end of RAII mutex scope
 return false;   // <-- should not come here anyway.
}
   
template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
void ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>::Name_Mnger::set_key(const std::string& name, sinkkey_t key)
{
  { // raii mutex scope
      std::lock_guard<std::mutex> raiiLock(_lock); 
      _name_to_key[name] = key; // "name" must already exist.
  }
}
  
//
// explicit instantiations:
//
// explicit instantiation of Syslog:
// < class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls >
// Ex:
//     classg3logSinkCls    --> g3::SyslogSink
//     typename ClbkType    --> g3logMsgMvrcall_t
//     ClbkType g3logMsgMvr --> &g3::SyslogSink::syslog
//     class pySinkCls      --> SysLogSnkHndl 
//


template void ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, SysLogSnkHndl>::Ptr_Mnger::done(sinkkey_t key);

template g3::SinkHandle<g3::SyslogSink> * ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, SysLogSnkHndl>::Ptr_Mnger::access(sinkkey_t key);

template sinkkey_t ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>::Ptr_Mnger::insert(std::unique_ptr<g3::SinkHandle<g3::SyslogSink>>);

template bool ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>::Name_Mnger::reserve(const std::string& name);
template void ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>::Name_Mnger::set_key(const std::string& name, sinkkey_t key);

// explicit instantiation of LogRotate:

// < class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls >
// Ex:
//     classg3logSinkCls    --> LogRotate
//     typename ClbkType    --> g3logRotateMsgMvrcall_t
//     ClbkType g3logMsgMvr --> &LogRotate::save
//     class pySinkCls      --> LogRotateSnkHndl 
//

template void ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, LogRotateSnkHndl>::Ptr_Mnger::done(sinkkey_t key);

template g3::SinkHandle<LogRotate> * ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, LogRotateSnkHndl>::Ptr_Mnger::access(sinkkey_t key);

template sinkkey_t ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>::Ptr_Mnger::insert(std::unique_ptr<g3::SinkHandle<LogRotate>>);

template bool ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>::Name_Mnger::reserve(const std::string& name);
template void ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>::Name_Mnger::set_key(const std::string& name, sinkkey_t key);


} // g3
