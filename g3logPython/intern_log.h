
// TODO: to remove: replace memcpy
#include <cstring>


#include <future>
#include <list>
#include <memory>

namespace g3 {
    
void receivelog(const char *file, int line, const char* functionname, int level_val, const char *message);

enum class pyLEVEL : int
{
    pyDEBUG,
    pyINFO,
    pyWARNING,
    pyFATAL
};



// TODO   TO REMOVE once the generic manager in place //////////////////////
// RAII string type with its deleter (Dltr) included to cross library boundaries 
class DltrStr
{
public:
    DltrStr() = delete;
    // the deleter accompanies the memory block to enable it beeing freed in the library (using a different heap, as on Windows):
    DltrStr(const char * content) {
        size_t len = strlen(content);
        _content = (char *)malloc(len + 1); //std::shared_ptr<char>(new char[len + 1], [](char *b){ delete[](b);} );
        memcpy(_content, content, len+1); };
        
    DltrStr(const std::string& content) {
        size_t len = content.length();
        _content = (char *)malloc(len + 1); // std::shared_ptr<char>(new char[len + 1], [](char *b){ delete[](b);} );
        memcpy(_content, content.c_str(), len+1); };
        
    const char *c_str() {return _content;};
    
private:
    //std::shared_ptr<char> _content;
    char *_content;
};


// ------------------------------------------------------------------------------



class StoredIface;
//
// To solve the problem of lifetime when passing data to g3log threads
// The data have to stay alive until the thread has finished.
// The idea is to make a deep-copy of the data, and store them here
// until the thread has finished.
// 
// This class works together with the class StoredForThd.
//
class ThdStore
{
public:
   void store(std::shared_ptr<StoredIface>&& p_newData) {
        const std::lock_guard<std::mutex> lock(_ShdLstLck);
        // TODO: check if memory barrier is inserted automatically
        _SharedList.push_back(p_newData);
      }
    
private:    
    
  void join(){}; // TODO: join the cleanup-thread
  
  // we make two lists: 
  // - one list shared between threads, protected by a mutex.
  // - one list that can only be accessed by the cleanup-thread
  // taking elements from the shared list and inserting them in the cleanup-list is fast ( list::splice() )
  // loops on the cleanup-list is slow (test of all the futures wait_for() )
  std::mutex _ShdLstLck; // for _SharedList
  std::list<std::shared_ptr<StoredIface>> _SharedList;
  std::list<std::shared_ptr<StoredIface>> _CleanupList;
};

//
// interface to the StoredForThd class:
// -> all stored blocs have a common virtual interface to interact with their thread state,
//    independent of the type of the shared_future<>
class StoredIface
{
public:
   StoredIface() = default;
    
   // checks if the future is available
   virtual std::future_status has_finished() = 0;
};

// see the comment of "class ThdStore" for a description of the problematic.
// 
// This class serves as a base class to wrap the data to store.
// To use it, create a derived class in the handle that will contain the actual data.
// 
template <typename delayed_t>
class StoredForThd: public StoredIface
{
public:
    
    std::future_status has_finished() { return _ThdFut.wait_for(std::chrono::seconds(0)); };
    
    delayed_t get() {return _ThdFut.get();}
    
    void set_future(std::shared_future<delayed_t> &&fut) {_ThdFut = fut;};
    
private:

    std::shared_future<delayed_t> _ThdFut;
};

//
// Helper class for the frequent case where 1 string must be stored
//
class Helper1StrStore: public StoredForThd<void>
{
  public:
    Helper1StrStore() = delete;
    Helper1StrStore(std::string init): Str(init) {};
    const char *c_str() {return Str.c_str();};
  private:
    std::string Str;
};

} // g3




