/*

  Python wrapper for g3log

On fedora 32:
   python3-pybind11
   python3-devel

*/

#pragma once

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "g3log/loglevels.hpp"

// #include <src/g3log/syslogsink.hpp>
#include <g3sinks/syslogsink.hpp>
#include <g3sinks/LogRotate.h>
#include "ColorTermSink.h"

#include <climits>
#include <cstring>

#include <iostream>
#include <map>
#include <memory>
#include <set>

namespace g3 {
    
void receivelog(const char *file, int line, const char* functionname, int level_val, const char *message);

enum class pyLEVEL : int
{
    pyDEBUG,
    pyINFO,
    pyWARNING,
    pyFATAL
};

    
// interface classes:
class ifaceLogWorker;
// handles for the sinks:
class SysLogSnkHndl;
class LogRotateSnkHndl;
class ClrTermSnkHndl;

class FD_Mnger; // for the file descriptor interface

// singleton interface to g3log:
std::shared_ptr<ifaceLogWorker> getifaceLogWorker();

// for the sink std::maps :
typedef unsigned int sinkkey_t;
#define InvalidSinkKey (0)

enum SINK_HNDL_OPTIONS
{
MULT_INSTANCES_ALLOWED = 1,
};

class ThdStore;

// manages an RAII mutex-protected raw pointer to g3log's handle
// note: std::scoped_lock is not moveable
template<typename Protected_t>
class LockedObj {
  public:
    LockedObj() = delete;
    LockedObj(const LockedObj&) = delete;
    LockedObj(LockedObj &&to_move) noexcept: p_hndl(to_move.p_hndl), raiiLock(std::move(to_move.raiiLock)){};
    LockedObj(std::mutex &to_lock): raiiLock(to_lock) {p_hndl = nullptr;};
    Protected_t p_hndl;
  private:
    std::unique_lock<std::mutex> raiiLock;
};


// ------------------------------------------------------------------------------
//
// Class to interact with the returned std::future from python
// so that we can join g3log's worker threads
//
// ------------------------------------------------------------------------------

class PyFutPass // Passkey Pattern : key for sinks
    {
    private:
       PyFutPass() = default;
       ~PyFutPass() = default;
       template<typename> friend class PyFuture;
       
       // give access to:
       friend class SysLogSnkHndl;    
       friend class LogRotateSnkHndl; 
       friend class ClrTermSnkHndl;
       friend class FD_Mnger;
    };

// note: my first thought was to make a virtual class with join(), and have every specilization
// inherit fromt that virtual, but exporting it to python is not really simple.
template <typename delayed_t>
class PyFuture
{    
public: // passkeyed
    PyFuture() = delete;
    PyFuture(PyFutPass) {};
    void take_fut(std::shared_future<delayed_t> &&for_python, PyFutPass) {_ThdFut = std::move(for_python);};
    std::shared_future<delayed_t> get_copy(PyFutPass) {return _ThdFut;};
    
public:    
    void join() {
        if(!_ThdFut.valid()) throw std::future_error(std::future_errc::no_state);
        _ThdFut.wait();
       };
        
private:
    std::shared_future<delayed_t> _ThdFut;
};



// ------------------------------------------------------------------------------
//
//  Classes for the file descriptor input to the logger
//
// ------------------------------------------------------------------------------

class FD_Mnger;

// forwards the data read from one pipe to g3log
// object lifetime: created before the worker thread starts, deleted at the same time as the worker thread ends.
// destroy() : starts the destruction process (closing file descriptors, shutting-down the worker thread,...)
//             returns a future so that another thread can wait for its destruction to be finished.
class fd_listener
{
public:
  class Pass // Passkey Pattern
    {
    private:
        Pass() = default;
        ~Pass() = default;
        friend g3::FD_Mnger;
    };
public:
  fd_listener(const fd_listener &) = delete;
  fd_listener(int fd_w, int fd_r, fd_listener::Pass);
  void join(fd_listener::Pass) {thd_ret.wait();};
  
  std::shared_future<void> destroy();
  
private:
  std::shared_future<void> thd_ret; // created just before starting the thread
  int fd_writer;
  int fd_reader;
  std::thread _worker;
  void read_on_fd(std::promise<void>);
};
    
// manages the file descriptor listeners                  
class FD_Mnger // file descriptor manager
{
public:
  class constrPass // Passkey Pattern
    {
    private:
        constrPass() = default;
        ~constrPass() = default;
        friend g3::ifaceLogWorker;
    };
    
public:
  FD_Mnger(const FD_Mnger &) = delete;
  FD_Mnger(FD_Mnger::constrPass) {};
  ~FD_Mnger();
  int new_pipe(); // returns the writing end of a new pipe. lock + unlock of mutex
  PyFuture<void> async_remove(int fd); // deletes a pipe. lock + unlock of mutex

private:
  std::mutex _lock; // protects all the datastructures herein
  std::map<int, std::shared_ptr<g3::fd_listener>> _fd_to_listener;
  std::vector<std::shared_ptr<g3::fd_listener>> _final_join; // already destroyed, must be joined before exiting the process.
};


// ------------------------------------------------------------------------------
//
// This class is providing a common (py & c++) interface for the unique g3log logger instance.
//
// ------------------------------------------------------------------------------

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
          
          class g3::LockedObj<g3::SinkHandle<g3logSinkCls> *> access(sinkkey_t key);
          
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
          size_t get_size(); // lock + unlock of mutex
        private:
          friend class ifaceLogWorker::SinkHndlAccess<g3logSinkCls, ClbkType, g3logMsgMvr, pySinkCls>;
          Name_Mnger(){};
          std::mutex _lock; // protects all the datastructures herein
          std::map<std::string, sinkkey_t> _name_to_key;
          // note: no key to name here: key management should be done elsewhere. ( Ptr_Mnger )
        };
    
    private:
      friend class ifaceLogWorker; // can only be constructed in ifaceLogWorker
      SinkHndlAccess(uint32_t options): _options(options) {};
      
      uint32_t _options; // bitmask (MULT_INSTANCES_ALLOWED...)
      
      friend class SysLogSnkHndl;
      friend class LogRotateSnkHndl; 
      friend class ClrTermSnkHndl; 
      
      Ptr_Mnger _g3logPtrs;
      Name_Mnger _userNames;
      /*
      // parameter storage for sink creation:
      // better be explicit about storage, so I let
      // this template for information only:
      //template<typename T, typename U=T>
      //U store(T&& dat) {return std::forward<T>(dat);};
          
      // simply copy the data
      std::string store(std::string content) {
          //std::cerr << "string  STORE CALLED" << std::endl;
          return std::string(content);
          };
          
      // copy the data manually, and keep ownership until the end of the program.
      // not that bad, because you usually don't add a huge number of sinks, and these are
      // stored only on sink creation.
      // TODO: remove memory leak
      const char * store(const char *content) {
          //std::cerr << "const char * STORE CALLED" << content << std::endl;
          size_t len = strlen(content);
          char* persist = new char[len + 1];
          memcpy(persist, content, len+1);
          return persist;
         }
         
         ==> not needed : currently all constructors copy the data
         */
      
    }; // class SinkHndlAccess

public:
  // typedefs of message mover functions:
  typedef void (g3::SyslogSink::* SyslogMvr_t)(g3::LogMessageMover) ;
  typedef void (LogRotate::* LogRotateMvr_t)(std::string) ;
  typedef void (g3::ColorTermSink::* ClrTermMvr_t)(g3::LogMessageMover) ;
  
  // types for the specialized sink interfaces of ifaceLogWorker:
  using SysLogSinkIface_t = ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, SyslogMvr_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>;
  using LogRotateSinkIface_t = ifaceLogWorker::SinkHndlAccess<LogRotate, LogRotateMvr_t, &LogRotate::save, g3::LogRotateSnkHndl>;
  using ClrTermSinkIface_t = ifaceLogWorker::SinkHndlAccess<g3::ColorTermSink, ClrTermMvr_t, &g3::ColorTermSink::ReceiveLogMessage, g3::ClrTermSnkHndl>;
  
public:
  // Interfaces to the sinks.
  // each sink type has its own interface class here.
  // Access is thread safe, but contains locks.
  SysLogSinkIface_t SysLogSinks; // TODO VERY URGENT : don't allow creation of more than one syslog sink TODO
  LogRotateSinkIface_t LogRotateSinks;
  ClrTermSinkIface_t ClrTermSinks;
   
  // scope_lifetime on first call:
  //  - when set to false (default), the interface remains alive until the program exits. 
  //  - when set to true, the interface will be destoyed when the user releases his last shared_ptr. 
  // That parameter is only used on the first call, any subsequent call will ignore this argument.
  static std::shared_ptr<ifaceLogWorker> get_ifaceLogWorker(bool scope_lifetime = false);

  // an alternative interface to the logger: 
  // this was created for interoperability with other existing loggers that can take a
  // file descriptor, or a FILE *, as destination for the strings to log.
  FD_Mnger fd_iface;  
  
  // may be useful for debug purposes:
  void print_addr(){ {std::cout << singleton._instance.lock().get() << std::endl;} }  
    
public: 
  ifaceLogWorker(const ifaceLogWorker &) = delete;
  ifaceLogWorker &operator=(const ifaceLogWorker &) = delete;
  //~ifaceLogWorker() {std::cerr << "ifaceLogWorker deleted" << std::endl;};

private:
  ifaceLogWorker(): SysLogSinks(0), LogRotateSinks(MULT_INSTANCES_ALLOWED), ClrTermSinks(MULT_INSTANCES_ALLOWED), fd_iface(FD_Mnger::constrPass()) {};
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
  std::shared_ptr<ThdStore> pStore; // TODO : make it private : proxy it somehow
  friend class SysLogSnkHndl;  // for pStore access
  friend class LogRotateSnkHndl; 
  friend class ClrTermSnkHndl; 
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
  friend class ClrTermSnkHndl;
  
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
  PyFuture<void> setLogHeader(const char* change);
  PyFuture<void> echoToStderr(); // enables the Linux extension LOG_PERROR
  PyFuture<void> muteStderr(); // opposite of echoToStderr()
  
// from syslog(3) : The argument ident in the call of openlog() is probably stored as-is. Thus, if the string it points to is changed, syslog() may start prepending the changed string, and if the string it points to ceases to exist, the results are undefined. 
// --> we have to store the string in a long-term storage
// => no longer required after PR #79 in g3sinks
  PyFuture<void> setIdentity(std::string& id);
  PyFuture<void> setFacility(int facility);
  PyFuture<void> setOption(int option);
  PyFuture<void> setLevelMap(std::map<int, int> const& m);

  PyFuture<void> setLevel(SyslogSink::LogLevel level, int syslevel);

public:
  SysLogSnkHndl() = delete;
  //SysLogSnkHndl(const SysLogSnkHndl &) = delete;
  SysLogSnkHndl &operator=(const SysLogSnkHndl &) = delete;
  
private:
  friend ifaceLogWorker::SysLogSinkIface_t;
  SysLogSnkHndl(std::shared_ptr<ifaceLogWorker> pworker, sinkkey_t key) : cmmnSinkHndl(pworker, key) {};
}; // SysLogSnkHndl
    
    
class LogRotateSnkHndl: private cmmnSinkHndl
{
public:
  PyFuture<void> save(std::string& logEnty);
  std::string changeLogFile(const std::string& log_directory, const std::string& new_name="");
  std::string logFileName();
  PyFuture<void> setMaxArchiveLogCount(int max_size);
  int getMaxArchiveLogCount();
  PyFuture<void> setFlushPolicy(size_t flush_policy); // 0: never (system auto flush), 1 ... N: every n times
  PyFuture<void> flush();
  PyFuture<void> setMaxLogSize(int max_file_size_in_bytes);
  int getMaxLogSize();
  
public:
  LogRotateSnkHndl() = delete;
  //LogRotateSnkHndl(const LogRotateSnkHndl &) = delete;
  LogRotateSnkHndl &operator=(const LogRotateSnkHndl &) = delete;
  
private:
  friend ifaceLogWorker::LogRotateSinkIface_t;
  LogRotateSnkHndl(std::shared_ptr<ifaceLogWorker> pworker, sinkkey_t key) : cmmnSinkHndl(pworker, key)  {};
}; // LogRotateSnkHndl  

    
    
class ClrTermSnkHndl: private cmmnSinkHndl
{
public:
  // Sink methods go here... (implemented in sinks.cpp)
  PyFuture<void> mute();
  PyFuture<void> unmute();
    
public:
  ClrTermSnkHndl() = delete;
  ClrTermSnkHndl &operator=(const ClrTermSnkHndl &) = delete;
  
private:
  friend ifaceLogWorker::ClrTermSinkIface_t;
  ClrTermSnkHndl(std::shared_ptr<ifaceLogWorker> pworker, sinkkey_t key) : cmmnSinkHndl(pworker, key) {};
}; // ClrTermSnkHndl   
    
} // g3
