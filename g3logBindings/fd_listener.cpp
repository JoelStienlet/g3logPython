
#include "g3logBindings.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

std::shared_future<void> g3::fd_listener::destroy()
{
close(fd_writer); // will cause the thread to close
fd_writer = -1; // note: fd_reader will be closed by the thread.
return thd_ret;
}

void g3::fd_listener::read_on_fd(std::promise<void> to_join)
{   
const int buf_size = 1024; // real size = buf_size+1  
ssize_t n_read;
std::unique_ptr<char[]> buf(new char[buf_size]);
    
do{
   n_read = read(fd_reader, buf.get(), buf_size); // read(2)
   if(n_read > 0) {
       std::stringstream strout;
       ssize_t offset = 0;
       while(offset < n_read) {
           if(buf[offset] == 0) {
               LOG(INFO) << strout.str();
               strout.str();
             } else {
               strout.put(buf[offset]);
             }
           offset ++;
          }
       }
 }while(n_read > 0);

close(fd_reader);
fd_reader = -1;
to_join.set_value();
}

g3::fd_listener::fd_listener(int fd_w, int fd_r, fd_listener::constrPass)
{
fd_writer = fd_w;
fd_reader = fd_r;

// start the thread:
std::promise<void> to_join;
thd_ret = to_join.get_future();
_worker =  std::thread(&g3::fd_listener::read_on_fd, this, std::move(to_join));
_worker.detach();
}

int g3::FD_Mnger::new_pipe()
{
int ret;
int pipefd[2];
ret = pipe2(pipefd, O_CLOEXEC); // pipe(2)
if(ret != 0) {
   LOG(WARNING) << strerror(errno);
   throw std::runtime_error("pipe2");
   }
   
try
{ 
  // raii mutex lock
  std::lock_guard<std::mutex> lock(_lock);
  // cannot use make_shared here, because of private constructor
  std::shared_ptr<fd_listener> p_listener(new fd_listener(pipefd[1], pipefd[0], fd_listener::constrPass() ));
  _fd_to_listener.insert({pipefd[1], p_listener});
} catch (...) {
    LOG(WARNING) << "failed to insert pipe fd in map";
    close(pipefd[0]);
    close(pipefd[1]);
    throw std::runtime_error("fd_insert");
  }
    
return pipefd[1];    
}

g3::PyFuture<void> g3::FD_Mnger::async_remove(int fd)
{
std::shared_ptr<g3::fd_listener> p_to_del;
{ 
  // raii mutex lock
  std::lock_guard<std::mutex> lock(_lock);
  p_to_del = _fd_to_listener[fd];
  _fd_to_listener.erase(fd);
  _final_join.push_back(p_to_del);
}

std::shared_future<void> thd_ret = p_to_del.get() -> destroy();

PyFutPass pass; // Passkey Pattern
PyFuture<void> fut_for_py(pass);
fut_for_py.take_fut(std::move(thd_ret), pass);
return fut_for_py;
}

/*
//
// although async_remove() returns a future that allows the user to wait for the listener to finish,
// we have no guarantee that he will do so.
// But: when exiting the process, all threads have to be cleaned-up.
// 
// -> call async_remove() on all elements in _fd_to_listener, then join all threads in _final_join.
// 
g3::FD_Mnger::~FD_Mnger()
{
    // TODO
}
*/
