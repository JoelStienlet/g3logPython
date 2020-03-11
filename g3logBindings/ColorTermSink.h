#pragma once
#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>

// inspired by the code snippets of g3sinks
namespace g3 {
    
class ColorTermSink {
public:
// Linux xterm color
// http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  enum FG_Color {YELLOW = 33, RED = 31, GREEN=32, WHITE = 37};

  ColorTermSink(): _mute(false) {};
  
  void ReceiveLogMessage(g3::LogMessageMover logEntry);

  void mute();
  void unmute();
  
private:
  FG_Color GetColor(const LEVELS level) const {
     if (level.value == WARNING.value) { return YELLOW; }
     if (level.value == DEBUG.value) { return GREEN; }
     if (g3::internal::wasFatal(level)) { return RED; }

     return WHITE;
  }
  
  bool _mute;
};

} // g3
