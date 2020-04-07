
#include <g3logBindings/g3logBindings.h>
#include <g3log/g3log.hpp>

#include <pybind11/pybind11.h>

#include "pandabase.h"
#include "multiplexStream.h"
#include "pnotify.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

// https://docs.panda3d.org/1.10/cpp/reference/panda3d.core.Notify
// https://www.panda3d.org/reference/cxx/MultiplexStream.php 
// Pofstream -> alias of OFileStream
// https://github.com/panda3d/panda3d/blob/5f7809469c04047b64b2b998c8251239a826c9df/panda/src/downloader/multiplexStreamBuf.cxx
// https://github.com/panda3d/panda3d/blob/371e60c768cfd1a1383c5b05f1cab2078014ccea/panda/src/downloader/multiplexStreamBuf.h
// https://github.com/panda3d/panda3d/blob/371e60c768cfd1a1383c5b05f1cab2078014ccea/panda/src/downloader/multiplexStream.I
// 
void set_g3log_as_panda3d_backend()
{
// get a pointer to the interface singleton:
std::shared_ptr<g3::ifaceLogWorker> pIface = g3::ifaceLogWorker::get_ifaceLogWorker();
errno = 0;
int pipefd = pIface -> fd_iface.new_pipe();
FILE *file_from_fd = fdopen(pipefd,"w");
if(file_from_fd == NULL) {
    LOG(WARNING) << "unable to make FILE from file descriptor.";
    LOG(WARNING) << strerror(errno);
  } else {  
   MultiplexStream *p_mx = new MultiplexStream();
   Notify().ptr() -> set_ostream_ptr(p_mx, true); // libpandaexpress.so  true: delete later
   p_mx -> add_stdio_file(file_from_fd, true); // --> will close FILE*, thus also pipefd
  }
}

void set_cerr_as_panda3d_backend()
{
Notify().ptr() -> set_ostream_ptr(NULL, false); // NULL --> default cerr
}

PYBIND11_MODULE(cppP3Dcalls, m) {
    m.doc() = "g3logPython example using the file descriptor to ensure interoperability with the logger from another library";
    m.def("set_g3log_as_panda3d_backend", &set_g3log_as_panda3d_backend, "changes the default output stream from panda3d");
    m.def("set_cerr_as_panda3d_backend", &set_cerr_as_panda3d_backend, "changes the output stream from panda3d: back to default cerr");
}
