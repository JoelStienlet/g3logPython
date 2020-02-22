/*

  Python wrapper for g3log

*/

#pragma once

#include <memory>
#include <map>
#include <set>
#include <climits>

namespace g3 {
    
// interface classes:
class ifaceLogWorker;
// handles for the sinks:
class SysLogSnkHndl;
class LogRotateSnkHndl;
// singleton interface to g3log:
std::shared_ptr<ifaceLogWorker> getifaceLogWorker();

// for the sink std::maps :
typedef unsigned int sinkkey_t;
#define InvalidSinkKey (0)

// typedefs of message mover functions:
typedef void (g3::SyslogSink::* g3logMsgMvrcall_t)(g3::LogMessageMover) ;
typedef void (LogRotate::* g3logRotateMsgMvrcall_t)(std::string) ;



// This class is providing a common (py & c++) interface for the unique g3log logger instance.
// note that this is a singleton (as there's only one g3log instance) 
class ifaceLogWorker
{
public:
  //
  // each sink gets its own interface in ifaceLogWorker
  // 

  //
  // description of the "class SinkHndlAccess" template:
  // this class is providing the interface for each sink type in ifaceLogWorker.
  //
  //   g3logSinkCls : the class defined by g3log
  //   pySinkCls : a specialized handle class for the sink ( SysLogSnkHndl, LogRotateSnkHndl, ... )
  //   g3logMsgMvr : the mover function to pass the log string to the sink
  //   ClbkType : type of the mover function
  //
  // note: this class (thus the ifaceLogWorker singleton) will also store the sink handles returned by g3log
  // (note: cannot be declared outside of the container: https://stackoverflow.com/questions/1021793/how-do-i-forward-declare-an-inner-class )
  template< class g3logSinkCls, typename ClbkType, ClbkType g3logMsgMvr, class pySinkCls>
  class SinkHndlAccess
    {
    public:
    
      // Sink creation: call new_Sink() for the chosen sink type.
      //  the purpose of the "name" passed to new_Sink() is to facilitate the access to a specific sink without the burden to keep its handle around.
      //  note: currently g3log has no method to remove a sink once inserted, but that may change. 
      template<typename... Args>
      pySinkCls new_Sink(const std::string& name, Args... args); // if name is empty (""), this call is equivalent to new_Sink()
    
      // returns a new sink handle to an existing sink, defined by his name (previously created by new_Sink(std::string& name) )
      pySinkCls new_SinkHndl(const std::string& name);
    
    public:
      SinkHndlAccess(const SinkHndlAccess &) = delete;
      SinkHndlAccess &operator=(const SinkHndlAccess &) = delete;
    
    private:
    
      class Ptr_Mnger
        {
        public:
          Ptr_Mnger(const Ptr_Mnger &) = delete;
          sinkkey_t insert(std::unique_ptr<g3::SinkHandle<g3logSinkCls>>); // lock + unlock of mutex
          g3::SinkHandle<g3logSinkCls> *access(sinkkey_t key); // locks the mutex. call done() once finished to release it. TODO: RAII
          void done(sinkkey_t key); // unlocks the mutex locked by access().
          void remove(sinkkey_t); // lock + unlock of mutex
          
        private:
          friend class ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>;
          Ptr_Mnger(){};
          std::mutex _lock; // protects all the datastructures herein
          std::set<sinkkey_t> _in_use; // keys in use
          std::set<sinkkey_t> _free; // deleted keys for reuse
          std::map<sinkkey_t, std::unique_ptr<g3::SinkHandle<g3logSinkCls>>> _key_to_uniquePtr;
        };
      
      class Name_Mnger
        {
        public:
          Name_Mnger(const Name_Mnger &) = delete;
          bool reserve(const std::string& name); // lock + unlock of mutex. returns "true" when successfully reserved (if name already exists: returns false)
          void set_key(const std::string& name, sinkkey_t key); // lock + unlock of mutex
          sinkkey_t get_key(const std::string& name); // lock + unlock of mutex
          void remove(const std::string& name); // lock + unlock of mutex
        private:
          friend class ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>;
          Name_Mnger(){};
          std::mutex _lock; // protects all the datastructures herein
          std::map<std::string, sinkkey_t> _name_to_key;
          // note: no key to name here: key management should be done elsewhere. ( Ptr_Mnger )
        };
    
    private:
          
      friend class ifaceLogWorker; // can only be constructed in ifaceLogWorker
      SinkHndlAccess() {};
      
      friend class SysLogSnkHndl;
      friend class LogRotateSnkHndl; 
      Ptr_Mnger _g3logPtrs;
      Name_Mnger _userNames;
      
    }; // class SinkHndlAccess
    
    
public:
   
  // Interfaces to the sinks.
  // each sink type has its own interface class here.
  // Access is thread safe, but contains locks.
  ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>    SysLogSinks;
  ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>    LogRotateSinks;

  // scope_lifetime on first call:
  //  - when set to false (default), the interface remains alive until the program exits. 
  //  - when set to true, the interface will be destoyed when the user releases his last shared_ptr. 
  // That parameter is only used on the first call, any subsequent call will ignore this argument.
  static std::shared_ptr<ifaceLogWorker> get_ifaceLogWorker(bool scope_lifetime = false);

  // may be useful for debug purposes:
  void print_addr(){ {std::cout << singleton._instance.lock().get() << std::endl;} }  
    
public: 
  ifaceLogWorker(const ifaceLogWorker &) = delete;
  ifaceLogWorker &operator=(const ifaceLogWorker &) = delete;
  //~ifaceLogWorker() {std::cerr << "ifaceLogWorker deleted" << std::endl;};
        
private:
  ifaceLogWorker() = default;
  static struct  sglt_t{
      static std::once_flag initInstanceFlag;
      static std::once_flag killKeepaliveFlag;
      static std::weak_ptr<ifaceLogWorker> _instance;
      static std::shared_ptr<ifaceLogWorker> _keepalive;
      static bool _scoped;
      static void initSingleton(bool scope);
      // the singleton is deleted in any case. But if the keepalive pointer is nullified, the singleton is deleted as soon as the last shared_ptr held by
      // the user is released. If the keepalive pointer is not nullified, the singleton is only deleted at the end of the process.
      static void kill_keepalive(){ _keepalive = nullptr; }
    } singleton;
    
  std::unique_ptr<LogWorker> worker;
 
  // TODO : check if they're all required here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  friend class SysLogSnkHndl;   // allows that class to call addSink() and call()
  friend class LogRotateSnkHndl; 
  friend class cmmnSinkHndl;
};
  
    
// generic common (cmmn) sink handle, for the python handle classes (SysLogSnkHndl...)   
// abstract base class
class cmmnSinkHndl
{
public:
  cmmnSinkHndl() = delete;
  //cmmnSinkHndl(const cmmnSinkHndl &) = delete;
  cmmnSinkHndl &operator=(const cmmnSinkHndl &) = delete;
  
private:
  friend class SysLogSnkHndl;    // gives access to the private constructor
  friend class LogRotateSnkHndl; 
  
  cmmnSinkHndl(std::shared_ptr<ifaceLogWorker> pworker, sinkkey_t key) : _p_wrkrKeepalive(pworker), _key(key) {};
  
  std::shared_ptr<ifaceLogWorker> _p_wrkrKeepalive; // as long as all handles aren't destroyed we can't destroy the logworker
  sinkkey_t _key; // for the map: _key -> unique_ptr
};
    
// ------------------------------------------------------------------------------
// These classes provide the interface for sink interaction from python.
// here we define a class for each sink, providing a simple interface for 
// calling the specific sink functions, with the "call()" method from the handle
// returned by g3log.
// Basically they're just a copy of each sink's interface.
// See g3log's documentation: every sink method call has to 
// be made through the call() method provided by g3log's handle.
// ------------------------------------------------------------------------------

// Don't template here, as these are essentially sink-specific interfaces
class SysLogSnkHndl: private cmmnSinkHndl
{
public:
    
  void setLogHeader(const char* change);
  void echoToStderr(); // enables the Linux extension LOG_PERROR

  void setIdentity(std::string& id);
  void setFacility(int facility);
  void setOption(int option);
  void setLevelMap(std::map<int, int> const& m);

  void setLevel(SyslogSink::LogLevel level, int syslevel);

public:
  SysLogSnkHndl() = delete;
  //SysLogSnkHndl(const SysLogSnkHndl &) = delete;
  SysLogSnkHndl &operator=(const SysLogSnkHndl &) = delete;
  
private:
  friend class ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>;
  SysLogSnkHndl(std::shared_ptr<ifaceLogWorker> pworker, sinkkey_t key) : cmmnSinkHndl(pworker, key) {};
  
}; // SysLogSnkHndl
    
    
class LogRotateSnkHndl: private cmmnSinkHndl
{
public:  
  
  void save(std::string& logEnty);
  std::string changeLogFile(const std::string& log_directory, const std::string& new_name="");
  std::string logFileName();
  void setMaxArchiveLogCount(int max_size);
  int getMaxArchiveLogCount();
  void setFlushPolicy(size_t flush_policy); // 0: never (system auto flush), 1 ... N: every n times
  void flush();
  void setMaxLogSize(int max_file_size_in_bytes);
  int getMaxLogSize();
  
public:
  LogRotateSnkHndl() = delete;
  //LogRotateSnkHndl(const LogRotateSnkHndl &) = delete;
  LogRotateSnkHndl &operator=(const LogRotateSnkHndl &) = delete;
  
private:
  friend class ifaceLogWorker::SinkHndlAccess<LogRotate, g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>;
  LogRotateSnkHndl(std::shared_ptr<ifaceLogWorker> pworker, sinkkey_t key) : cmmnSinkHndl(pworker, key)  {};
  
}; // LogRotateSnkHndl  

    
} // g3
