
#include <g3logBindings/g3logBindings.h>
#include <g3log/g3log.hpp>

#include <pybind11/pybind11.h>


void log_something()
{
 static int cnter = 0;
 LOG(DEBUG) << "Hello N°" << cnter << " from c++!";   
 cnter ++;
}

void log_string(std::string to_log)
{
 LOG(DEBUG) << "logged in .cpp: " << to_log;
}


void add_syslog()
{
// get a pointer to the interface singleton:
std::shared_ptr<g3::ifaceLogWorker> pIface = g3::ifaceLogWorker::get_ifaceLogWorker(); 
// add the new sink:
pIface -> SysLogSinks.new_Sink("journald sink","g3logPython_05c++");
}

PYBIND11_MODULE(cppLib, m) {
    m.doc() = "g3logPython example of a mixed C++/python code";
    
    m.def("log_something", &log_something, "A function which uses g3log in C++");
    m.def("log_string", &log_string, "logs a string from C++");    
    m.def("add_syslog", &add_syslog, "add a new sink from C++");    
    
}



