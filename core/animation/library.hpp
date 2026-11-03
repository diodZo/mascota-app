#pragma once
#include "player.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <istream>
#include <algorithm>
namespace pet {
struct SpriteFrame {
 std::string image; int x=0,y=0,width=64,height=64,dx=0,dy=0;
};
struct Animation {
 std::string name; unsigned fps=4; bool loop=true; std::vector<SpriteFrame> frames;
 Clip clip()const{return {unsigned(frames.size()),fps,loop};}
};
class Library {
 std::vector<Animation> clips_;
public:
 void add(Animation a){Player::validate(a.clip());if(a.name.empty()||std::any_of(clips_.begin(),clips_.end(),[&](const Animation& c){return c.name==a.name;}))throw std::runtime_error("Duplicate or empty animation name");clips_.push_back(std::move(a));}
 const Animation& get(const std::string& name)const{for(const auto& c:clips_)if(c.name==name)return c;throw std::runtime_error("Missing animation: "+name);}
 const std::vector<Animation>& all()const{return clips_;}
 static Library load(std::istream& in){
  Library result;std::string line;Animation current;bool active=false,header=false;
  while(std::getline(in,line)){
   std::istringstream row(line);std::string kind;if(!(row>>kind)||kind[0]=='#')continue;
   if(!header){int version=0;if(kind!="petanim"||!(row>>version)||version!=1)throw std::runtime_error("Unsupported animation format");header=true;}
   else if(kind=="clip") {if(active)throw std::runtime_error("Unclosed clip");int loop=0;current={};if(!(row>>current.name>>current.fps>>loop)||(loop!=0&&loop!=1))throw std::runtime_error("Invalid clip header");current.loop=loop!=0;active=true;}
   else if(kind=="frame") {SpriteFrame f;if(!active||!(row>>std::quoted(f.image)>>f.x>>f.y>>f.width>>f.height>>f.dx>>f.dy))throw std::runtime_error("Invalid frame");if(f.image.empty()||f.x<0||f.y<0||f.width!=64||f.height!=64||f.dx< -64||f.dx>64||f.dy< -64||f.dy>64)throw std::runtime_error("Frame must use 64x64 cells and bounded offsets");current.frames.push_back(f);if(current.frames.size()>256)throw std::runtime_error("Too many frames");}
   else if(kind=="end") {if(!active)throw std::runtime_error("Unexpected end");result.add(current);active=false;}
   else throw std::runtime_error("Unknown animation directive: "+kind);
   std::string extra;if(row>>extra)throw std::runtime_error("Unexpected animation data");
  }
  if(!header||active||result.all().empty())throw std::runtime_error("Incomplete animation file");return result;
 }
};
class AnimationManager {
 const Library& library_; const Animation* current_; Player player_;
public:
 explicit AnimationManager(const Library& l,const std::string& initial):library_(l),current_(&l.get(initial)),player_(current_->clip()){}
 void play(const std::string& name,bool restart=false){const auto& next=library_.get(name);if(restart||current_!=&next){player_.play(next.clip());current_=&next;}}
 void advance(unsigned ms){player_.advance(ms);}
 bool takeCompletion(){return player_.takeCompletion();}
 const SpriteFrame& sprite()const{return current_->frames.at(player_.frame());}
 const std::string& state()const{return current_->name;}
 unsigned frame()const{return player_.frame();}
};
inline Library defaultLibrary(){
 Library l;for(const auto& name:{"idle","happy","walk","sad","sleep"}){Animation a;a.name=name;a.fps= name==std::string("sleep")?1:name==std::string("sad")?2:name==std::string("idle")?4:8;a.loop=name!=std::string("happy");unsigned count=name==std::string("idle")?16: name==std::string("happy")?6:name==std::string("sleep")?2:name==std::string("sad")?3:4;for(unsigned i=0;i<count;++i){SpriteFrame f;f.image="placeholder.png";f.x=int(i)*64;a.frames.push_back(f);}l.add(a);}return l;
}
}
