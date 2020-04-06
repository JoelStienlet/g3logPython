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
int fd_autoclean = pIface -> fd_iface.new_pipe(); // rely on g3logBindings to do the cleanup

const char *test_string = "hello world to file descriptor!";
write(fd, test_string, strlen(test_string) +1 );

const char more_test_strings[] = "test of a multiple output string," "\x0" "this should be written on a separate output";
write(fd, more_test_strings, sizeof(more_test_strings) );

const char *test_string_2 = "this is written to file descriptor that will be cleaned-up automatically when ending the process.";
write(fd_autoclean, test_string_2, strlen(test_string_2) +1 );

// do the cleanup of the first file descriptor "fd":
g3::PyFuture<void> to_join = pIface -> fd_iface.async_remove(fd);
to_join.join();

// the other file descriptor "fd_autoclean" will be cleaned-up on destruction of of the logger iface instance.

return 0;
}
