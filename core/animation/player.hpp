#pragma once
#include <cstdint>
#include <stdexcept>
namespace pet {
struct Clip { unsigned frames; unsigned fps; bool loop; };
class Player {
 Clip clip_; uint64_t elapsed_=0; bool completion_=false;
public:
 explicit Player(Clip c):clip_(c){validate(c);}
 static void validate(Clip c){if(!c.frames||!c.fps||c.fps>120)throw std::invalid_argument("Invalid clip");}
 void play(Clip c){validate(c);clip_=c;elapsed_=0;completion_=false;}
 void advance(uint32_t ms){bool before=finished();elapsed_+=ms;if(!before&&finished())completion_=true;}
 unsigned frame()const {auto n=elapsed_*clip_.fps/1000;return clip_.loop?unsigned(n%clip_.frames):unsigned(n>=clip_.frames?clip_.frames-1:n);}
 bool finished()const{return !clip_.loop && elapsed_*clip_.fps/1000>=clip_.frames;}
 bool takeCompletion(){bool result=completion_;completion_=false;return result;}
};
}
