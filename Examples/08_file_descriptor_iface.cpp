/*
 
  This example tests the file descriptor interface for interoperability with other log systems.
  
 */

#include <g3logBindings/g3logBindings.h>
#include <g3log/g3log.hpp>

#include <unistd.h>
#include <string.h>

int main(void)
{
// we start the logger with the interface provided by the bindings:
std::shared_ptr<g3::ifaceLogWorker> pIface = g3::ifaceLogWorker::get_ifaceLogWorker();   
pIface -> ClrTermSinks.new_Sink("color term sink");

LOG(DEBUG) << "message using g3log's interface";

int fd = pIface -> fd_iface.new_pipe();

const char *test_string = "hello world to file descriptor!";
write(fd, test_string, strlen(test_string) +1 );

g3::PyFuture<void> to_join = pIface -> fd_iface.async_remove(fd);
to_join.join();

sleep(1);

return 0;
}
