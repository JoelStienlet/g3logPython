
// // TODO: to remove: replace memcpy
// #include <cstring>


#include <future>
#include <list>
#include <memory>

namespace g3 {
    
class StoredIface;
//
// See explanations in "store.cpp"
// 
// This class works together with the class StoredForThd.
//
class ThdStore
{
public:
   void store(std::shared_ptr<StoredIface>&& p_newData) {
        std::lock_guard<std::mutex> lock(_ShdLstLck);
        // TODO: check if memory barrier is inserted automatically
        _SharedList.push_back(p_newData);
      }
    
    ThdStore(): terminate_thd(false) {start_thread();};
    ~ThdStore() {sendTerm_n_join();};
    
private:    
      
  // we make two lists: 
  // - one list shared between threads, protected by a mutex.
  // - one list that can only be accessed by the cleanup-thread
  // taking elements from the shared list and inserting them in the cleanup-list is fast ( list::splice() )
  // loops on the cleanup-list is slow (test of all the futures wait_for() )
  std::mutex _ShdLstLck; // for _SharedList
  std::list<std::shared_ptr<StoredIface>> _SharedList;
  std::list<std::shared_ptr<StoredIface>> _CleanupList;
 
  // for the cleanup-thread:
  void start_thread();
  void sendTerm_n_join(); // ask the cleanup thread to terminate and wait until he has finished.
  std::atomic<bool> terminate_thd; // flag to pass the termination order to the thread
  void ThdWorker();
  std::thread cleanupThd;
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




