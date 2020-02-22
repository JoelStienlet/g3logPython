#include <iostream>  




#include <mutex>
#include <atomic>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "g3log/loglevels.hpp"
#include <g3log/logcapture.hpp>
#include "/usr/local/include/g3sinks/syslogsink.hpp"
#include "/usr/local/include/g3sinks/LogRotate.h"
#include "g3logPython.h"
#include "intern_log.h"

// level_val : enum pyLEVEL
void g3::receivelog(const char *file, int line, const char* functionname, int level_val, const char *message)
{
    
switch(level_val) {
    case (int)g3::pyLEVEL::pyDEBUG: {
        const LEVELS &level = DEBUG;
        LogCapture(file, line, functionname, level).stream() << message;
        break; }
    case (int)g3::pyLEVEL::pyINFO: {
        const LEVELS &level = INFO;
        LogCapture(file, line, functionname, level).stream() << message;
        break; }
    case (int)g3::pyLEVEL::pyWARNING: {
        const LEVELS &level = WARNING;
        LogCapture(file, line, functionname, level).stream() << message;
        break; }
    case (int)g3::pyLEVEL::pyFATAL: {
        const LEVELS &level = FATAL;
        LogCapture(file, line, functionname, level).stream() << message;
        break; }
    default: {
        const LEVELS &level = WARNING;
        LogCapture(file, line, functionname, level).stream() << "invalid level " << level_val;
        LogCapture(file, line, functionname, level).stream() << message;
        break; }
    }
}

