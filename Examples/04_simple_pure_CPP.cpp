/*
 
  This example is in C++ only.
  
  Note that for a C++ -only project you can use g3log alone, but the bindings will give you a different interface.
  
 */

#include <g3logBindings/g3logBindings.h>
#include <g3log/g3log.hpp>

int main(void)
{
// we start the logger with the interface provided by the bindings:
std::shared_ptr<g3::ifaceLogWorker> pIface = g3::ifaceLogWorker::get_ifaceLogWorker();   
pIface -> ClrTermSinks.new_Sink("color term sink");

// Juste use the LOG macro defined in g3log:
LOG(DEBUG) << "Hello world from cpp!";
LOG(INFO) << "some information...";

return 0;
}
