
#include <iostream>  

#include <mutex>
#include <atomic>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "g3log/loglevels.hpp"
#include "/usr/local/include/g3sinks/syslogsink.hpp"
#include "/usr/local/include/g3sinks/LogRotate.h"
#include "g3logPython.h"
#include "intern_log.h"

#include <pybind11/pybind11.h>


PYBIND11_MODULE(_g3logPython, m)
{
m.attr("g3DEBUG")   = pybind11::int_((int)g3::pyLEVEL::pyDEBUG);
m.attr("g3INFO")    = pybind11::int_((int)g3::pyLEVEL::pyINFO);
m.attr("g3WARNING") = pybind11::int_((int)g3::pyLEVEL::pyWARNING);
m.attr("g3FATAL")   = pybind11::int_((int)g3::pyLEVEL::pyFATAL);

pybind11::class_<g3::SysLogSnkHndl>(m, "SysLogSnkHndl")
    .def("setLogHeader", &g3::SysLogSnkHndl::setLogHeader)
    .def("setIdentity",  &g3::SysLogSnkHndl::setIdentity)
    .def("echoToStderr", &g3::SysLogSnkHndl::echoToStderr);    
    
pybind11::class_<g3::LogRotateSnkHndl>(m, "LogRotateSnkHndl")
    .def("setMaxArchiveLogCount", &g3::LogRotateSnkHndl::setMaxArchiveLogCount);
   
    
pybind11::class_<g3::ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3::g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>>(m, "SysLogSinkHndlAccess")
    .def("new_Sink", 
         &g3::ifaceLogWorker::SinkHndlAccess<g3::SyslogSink, g3::g3logMsgMvrcall_t, &g3::SyslogSink::syslog, g3::SysLogSnkHndl>::new_Sink<const char*>,
         "creates a syslog sink");
    
pybind11::class_<g3::ifaceLogWorker::SinkHndlAccess<LogRotate, g3::g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>>(m, "LogRotateSinkHndlAccess")
    .def("new_Sink", 
         &g3::ifaceLogWorker::SinkHndlAccess<LogRotate, g3::g3logRotateMsgMvrcall_t, &LogRotate::save, g3::LogRotateSnkHndl>::new_Sink<const std::string&, const std::string&>,
         "creates a LogRotate sink");
    
pybind11::class_<g3::ifaceLogWorker, std::shared_ptr<g3::ifaceLogWorker>>(m, "ifaceLogWorker")
    .def_readonly("SysLogSinks", 
                  &g3::ifaceLogWorker::SysLogSinks, 
                  "syslog handle manager", 
                  pybind11::return_value_policy::reference_internal)
    .def_readonly("LogRotateSinks", 
                  &g3::ifaceLogWorker::LogRotateSinks, 
                  "LogRotate handle manager", 
                  pybind11::return_value_policy::reference_internal);
       
m.def("get_ifaceLogWorker", &g3::ifaceLogWorker::get_ifaceLogWorker, "access the log worker instance", pybind11::arg("scope_lifetime") = false);

m.def("receivelog", &g3::receivelog, "send log message to g3log");

}


