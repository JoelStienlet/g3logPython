
#include "ColorTermSink.h"

namespace g3 {
    
void ColorTermSink::ReceiveLogMessage(g3::LogMessageMover logEntry) 
{
 auto level = logEntry.get()._level;
 auto color = GetColor(level);

 if(!_mute) std::cerr << "\033[" << color << "m" << logEntry.get().toString() << "\033[m";
}
  
void ColorTermSink::mute() 
{
_mute = true;
}
  
void ColorTermSink::unmute() 
{
_mute = false;
}
  
} // g3
