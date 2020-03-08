//
//  implementation of class ThdStore 
//
// Here we solve the problem of data-lifetime when passing data to g3log's threads.
// The objects (ex: a string) have to stay alive until the worker thread has finished using them.
// The idea is to make a deep-copy of the data, and store them here in a global store for as long as needed.
// When a worker thread from g3log has finished, the corresponding data can be freed.
// This is done asynchronously, in a "cleanup-thread".
// 
//

#include "intern_log.h"

#include <chrono>
#include <thread>

namespace g3 {

// join the cleanup-thread
void ThdStore::sendTerm_n_join()
{
    terminate_thd = true;
    // TODO: wake-up the thread faster than its timeout (enventfd ?)
    cleanupThd.join();
}

void ThdStore::start_thread()
{
    cleanupThd = std::thread(&g3::ThdStore::ThdWorker, this);
}

// cleanup thread worker function
void ThdStore::ThdWorker()
{
    bool all_empty = false;
    
    // note: after terminate_thd has been set to true, 
    // normally no additional data will be added to the list.
    while(!terminate_thd && !all_empty) 
      {
        std::this_thread::sleep_for (std::chrono::seconds(1));
        all_empty = true; // set to false if any elements are found
        
        { // raii mutex lock
        std::lock_guard<std::mutex> lock(_ShdLstLck);
        if(_SharedList.size() > 0) { // note: size() is constant time since c++11
            std::list<std::shared_ptr<StoredIface>>::iterator it;
            it = _CleanupList.end();
            _CleanupList.splice (it, _SharedList);
            all_empty = false;
           }
        } // mutex
       
       _CleanupList.remove_if([](std::shared_ptr<StoredIface> p_data){return p_data.get() -> has_finished() == std::future_status::ready;});
       if(_CleanupList.size() > 0) all_empty = false; // we have to wait some longer. do one more iteration.
      }
}

} // g3
