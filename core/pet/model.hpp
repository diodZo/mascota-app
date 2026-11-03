#pragma once
#include "../animation/library.hpp"
#include "../persistence/snapshot.hpp"
namespace pet {
enum class Button {Left, Accept, Right};
enum class Screen {Room, Health, Stats};
inline constexpr std::array<const char*,6> ActionNames={"COMIDA","JUEGOS","DORMIR","HIGIENE","SALUD","ESTADISTICAS"};
inline constexpr std::array<const char*,6> ActionDescriptions={"UN BOCADO PARA CRECER","JUGAR Y SONREIR","DESCANSAR O DESPERTAR","UN BANO DE BURBUJAS","COMO ME ENCUENTRO","VER MIS NECESIDADES"};
class Model {
 Config config_;Stats stats_;Library library_;AnimationManager animation_;
 unsigned selected_=0,stateMs_=0,messageMs_=0;int x1000_=88000,lastAction_=-1;bool mirrored_=false,movingLeft_=false,sleeping_=false;
 Screen screen_=Screen::Room;std::string feedback_,name_="PATO CONEJO";unsigned stage_=1;
 void play(const std::string& state,bool restart=false){if(restart||animation_.state()!=state){animation_.play(state,true);stateMs_=0;}}
 bool needy()const{return stats_.percent(Stat::Hunger,config_)<config_.lowPercent||stats_.percent(Stat::Happiness,config_)<config_.lowPercent||stats_.percent(Stat::Hygiene,config_)<config_.lowPercent||stats_.percent(Stat::Health,config_)<config_.lowPercent;}
 void rest(){play(sleeping_?"sleep":needy()?"sad":"idle");}
 void feedback(const char* text){feedback_=text;messageMs_=3500;}
 void wake(){sleeping_=false;}
 void step(unsigned ms){
  stateMs_+=ms;if(messageMs_<=ms)messageMs_=0;else messageMs_-=ms;
  double minutes=ms/60000.0;
  for(unsigned i=0;i<5;++i)if(i!=unsigned(Stat::Energy))stats_.change(Stat(i),-config_.ranges[i].decay*minutes,config_);
  stats_.change(Stat::Energy,(sleeping_?config_.sleepGain:-config_.ranges[2].decay)*minutes,config_);
  bool healthyCare=stats_.percent(Stat::Hunger,config_)>=50&&stats_.percent(Stat::Hygiene,config_)>=50;
  if(healthyCare)stats_.change(Stat::Health,config_.healthRecovery*minutes,config_);
  else if(stats_.percent(Stat::Hunger,config_)<config_.lowPercent||stats_.percent(Stat::Hygiene,config_)<config_.lowPercent)stats_.change(Stat::Health,-config_.neglectDamage*minutes,config_);
  if(sleeping_&&stats_.percent(Stat::Energy,config_)>=config_.wakePercent){wake();feedback("YA TENGO ENERGIA");play("happy",true);}
  else if(!sleeping_&&stats_.percent(Stat::Energy,config_)<=config_.autoSleepPercent){sleeping_=true;feedback("TENGO SUENO");play("sleep",true);}
  animation_.advance(ms);
  if(sleeping_){play("sleep");return;}
  if(animation_.takeCompletion()){rest();return;}
  if(animation_.state()=="happy")return;
  if(needy()){play("sad");return;}
  if(animation_.state()=="sad"||animation_.state()=="sleep"){rest();return;}
  if(animation_.state()=="walk"){
   x1000_+=(movingLeft_?-1:1)*int(ms)*18;if(x1000_<=28000){x1000_=28000;movingLeft_=false;}if(x1000_>=148000){x1000_=148000;movingLeft_=true;}mirrored_=movingLeft_;if(stateMs_>=2400)rest();
  }else if(stateMs_>=8000&&stats_.percent(Stat::Energy,config_)>config_.lowPercent){movingLeft_=!movingLeft_;mirrored_=movingLeft_;play("walk");}
 }
public:
 Model():Model(defaultLibrary(),Config{}){}
 explicit Model(Library lib,Config config=Config{}):config_(config),stats_(config_),library_(std::move(lib)),animation_(library_,"idle"){
  config_.validate();for(const auto* clip:{"idle","happy","walk","sad","sleep"})library_.get(clip);
  if(library_.get("happy").loop||!library_.get("idle").loop||!library_.get("walk").loop||!library_.get("sad").loop||!library_.get("sleep").loop)throw std::runtime_error("Invalid required animation loop modes");
 }
 Model(const Model&)=delete;Model& operator=(const Model&)=delete;
 void tick(unsigned ms){while(ms){unsigned part=(std::min)(ms,10u);ms-=part;step(part);}}
 void press(Button b){
  if(b!=Button::Accept){screen_=Screen::Room;selected_=(selected_+(b==Button::Left?5:1))%6;return;}
  if(screen_!=Screen::Room){screen_=Screen::Room;return;}
  lastAction_=int(selected_);
  switch(selected_){
  case 0:wake();stats_.change(Stat::Hunger,config_.foodGain,config_);stats_.change(Stat::Happiness,3,config_);feedback("QUE RICO! GRACIAS");play("happy",true);break;
  case 1:
   if(stats_.percent(Stat::Energy,config_)<=config_.autoSleepPercent){sleeping_=true;feedback("PRIMERO VOY A DORMIR");play("sleep",true);break;}
   wake();stats_.change(Stat::Happiness,config_.playGain,config_);stats_.change(Stat::Energy,-config_.playEnergyCost,config_);stats_.change(Stat::Hunger,-4,config_);stats_.change(Stat::Hygiene,-3,config_);feedback("ME ENCANTA JUGAR");play("happy",true);break;
  case 2:sleeping_=!sleeping_;feedback(sleeping_?"DULCES SUENOS":"HOLA OTRA VEZ");if(sleeping_)play("sleep",true);else rest();break;
  case 3:wake();stats_.change(Stat::Hygiene,config_.cleanGain,config_);stats_.change(Stat::Happiness,5,config_);feedback("GRACIAS POR CUIDARME");play("happy",true);break;
  case 4:screen_=Screen::Health;break;
  case 5:screen_=Screen::Stats;break;
  }
 }
 void activate(unsigned index){if(index>=6)throw std::invalid_argument("Invalid action");screen_=Screen::Room;selected_=index;press(Button::Accept);}
 std::string message()const{
  if(messageMs_)return feedback_;if(sleeping_)return "DULCES SUENOS";
  if(stats_.percent(Stat::Hunger,config_)<config_.lowPercent)return "TENGO HAMBRE";
  if(stats_.percent(Stat::Energy,config_)<config_.lowPercent)return "TENGO SUENO";
  if(stats_.percent(Stat::Hygiene,config_)<config_.lowPercent)return "QUIERO UN BANO";
  if(stats_.percent(Stat::Health,config_)<config_.lowPercent)return "NECESITO CUIDADOS";
  if(stats_.percent(Stat::Happiness,config_)<config_.lowPercent)return "QUIERO JUGAR";
  return stats_.percent(Stat::Happiness,config_)>=80?"ESTOY FELIZ":"GRACIAS POR VENIR";
 }
 std::string healthSummary()const{return sleeping_?"ESTOY DESCANSANDO":percent(Stat::Energy)<config_.lowPercent?"NECESITO DESCANSAR":needy()?"NECESITO CUIDADOS":"ME ENCUENTRO BIEN";}
 Snapshot snapshot(int64_t now)const{Snapshot s;s.name=name_;s.stage=stage_;s.stats=stats_.values;s.lastAction=lastAction_;s.savedAt=now;s.sleeping=sleeping_;return s;}
 void restore(const Snapshot& s){s.validate(config_);name_=s.name;stage_=s.stage;stats_.values=s.stats;lastAction_=s.lastAction;sleeping_=s.sleeping;screen_=Screen::Room;messageMs_=0;rest();}
 const Stats& stats()const{return stats_;}const Config& config()const{return config_;}
 double percent(Stat s)const{return stats_.percent(s,config_);}
 unsigned frame()const{return animation_.frame();}unsigned selected()const{return selected_;}int lastAction()const{return lastAction_;}
 bool mirrored()const{return mirrored_;}bool sleeping()const{return sleeping_;}int x()const{return x1000_/1000;}
 Screen screen()const{return screen_;}const std::string& name()const{return name_;}unsigned stage()const{return stage_;}
 const std::string& state()const{return animation_.state();}const SpriteFrame& sprite()const{return animation_.sprite();}
 void notify(const char* message){feedback(message);}
};
}

