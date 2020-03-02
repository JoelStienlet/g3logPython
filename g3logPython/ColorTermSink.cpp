
#include "ColorTermSink.h"

namespace g3 {
    
void ColorTermSink::ReceiveLogMessage(g3::LogMessageMover logEntry) 
{
 auto level = logEntry.get()._level;
 auto color = GetColor(level);

 std::cout << "\033[" << color << "m" << logEntry.get().toString() << "\033[m" << std::endl;
}
  
} // g3
