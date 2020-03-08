

#include "g3logBindings.h"
//#include "intern_log.h"

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

