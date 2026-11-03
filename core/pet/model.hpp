#pragma once
#include "../animation/library.hpp"
#include "../persistence/snapshot.hpp"
#include "../evolution/settings.hpp"
#include "../ui/name_editor.hpp"
namespace pet {
enum class Button {Left, Accept, Right};
enum class Screen {Room, Health, Stats, Start, Species, Name, Evolution};
inline constexpr std::array<const char*,6> ActionNames={"COMIDA","JUEGOS","DORMIR","HIGIENE","SALUD","ESTADISTICAS"};
inline constexpr std::array<const char*,6> ActionDescriptions={"UN BOCADO PARA CRECER","JUGAR Y SONREIR","DESCANSAR O DESPERTAR","UN BANO DE BURBUJAS","COMO ME ENCUENTRO","VER MIS NECESIDADES"};
class Model {
 Config config_;Stats stats_;Library library_;AnimationManager animation_;
 evolution::Settings settings_;Snapshot progress_;std::string state_="idle";
 NameEditor editor_;unsigned speciesCursor_=0,startChoice_=0,evolutionMs_=0;
 bool hasPet_=true,newPetRequest_=false,saveRequest_=false;
 std::optional<evolution::Event> event_;evolution::Stage previousStage_=evolution::Stage::Baby;
 unsigned selected_=0,stateMs_=0,messageMs_=0;int x1000_=88000,lastAction_=-1;bool mirrored_=false,movingLeft_=false,sleeping_=false;
 Screen screen_=Screen::Room;std::string feedback_,name_="PATO CONEJO";unsigned stage_=1;
 std::string animationKey(const std::string& state)const{auto key=evolution::clipKey(species(),visualStage(),state);return library_.find(key)?key:state;}
 void play(const std::string& state,bool restart=false){auto key=animationKey(state);if(restart||animation_.state()!=key){animation_.play(key,true);stateMs_=0;}state_=state;}
 void startSleep(){if(!sleeping_){progress_.sleepStartEnergy=percent(Stat::Energy);progress_.sleepRewarded=false;}sleeping_=true;}
 void award(unsigned action,double gain){
  if(action>=6||gain+1e-9<settings_.significantGain||progress_.playedMs<progress_.rewardReadyMs[action]||!settings_.xp[action])return;
  progress_.experience=(std::min)(1000000000u,progress_.experience+settings_.xp[action]);
  progress_.activities=(std::min)(1000000000u,progress_.activities+1);
  progress_.rewardReadyMs[action]=progress_.playedMs+settings_.cooldownMs;
 }
 void updateProgress(unsigned ms){
  auto add=[ms](uint64_t& counter){counter=(std::min)(evolution::MaxTimeMs,counter+ms);};
  add(progress_.ageMs);add(progress_.playedMs);add(progress_.careSampleMs);
  double target=0;for(unsigned i=0;i<5;++i)target+=percent(Stat(i))/5;
  progress_.care=std::clamp(progress_.care+(target-progress_.care)*double(ms)/double(settings_.careWindowMs),0.0,100.0);
  if(sleeping_&&!progress_.sleepRewarded&&percent(Stat::Energy)-progress_.sleepStartEnergy+1e-9>=settings_.significantGain){award(2,percent(Stat::Energy)-progress_.sleepStartEnergy);progress_.sleepRewarded=true;}
 }
 void checkEvolution(){
  evolution::Progress p{evolution::Stage(stage_),double(progress_.ageMs)/60000.0,progress_.experience,progress_.care,progress_.activities};
  if(!evolution::evolve(p,settings_.rules,percent(Stat::Happiness)))return;
  previousStage_=evolution::Stage(stage_);stage_=unsigned(p.stage);evolutionMs_=0;screen_=Screen::Evolution;
  event_=evolution::Event{species(),previousStage_,p.stage};saveRequest_=true;play("idle",true);
 }
 void finishSetup(){
  progress_=Snapshot{};progress_.species=evolution::Catalog[speciesCursor_].id;progress_.care=settings_.initialCare;
  stats_=Stats(config_);name_=editor_.value();stage_=1;sleeping_=false;lastAction_=-1;selected_=0;x1000_=88000;mirrored_=movingLeft_=false;
  hasPet_=true;newPetRequest_=true;screen_=Screen::Room;event_.reset();messageMs_=0;play("idle",true);feedback("HOLA! VAMOS A JUGAR");
 }
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
  updateProgress(ms);
  if(sleeping_&&stats_.percent(Stat::Energy,config_)>=config_.wakePercent){wake();feedback("YA TENGO ENERGIA");play("happy",true);}
  else if(!sleeping_&&stats_.percent(Stat::Energy,config_)<=config_.autoSleepPercent){startSleep();feedback("TENGO SUENO");play("sleep",true);}
  animation_.advance(ms);
  if(sleeping_){play("sleep");return;}
  if(animation_.takeCompletion()){rest();return;}
  if(state_=="happy")return;
  if(needy()){play("sad");return;}
  if(state_=="sad"||state_=="sleep"){rest();return;}
  if(state_=="walk"){
   x1000_+=(movingLeft_?-1:1)*int(ms)*18;if(x1000_<=28000){x1000_=28000;movingLeft_=false;}if(x1000_>=148000){x1000_=148000;movingLeft_=true;}mirrored_=movingLeft_;if(stateMs_>=2400)rest();
  }else if(stateMs_>=8000&&stats_.percent(Stat::Energy,config_)>config_.lowPercent){movingLeft_=!movingLeft_;mirrored_=movingLeft_;play("walk");}
 }
public:
 Model():Model(defaultLibrary(),Config{}){}
 explicit Model(Library lib,Config config=Config{},evolution::Settings settings=evolution::Settings{}):config_(config),stats_(config_),library_(std::move(lib)),animation_(library_,"idle"),settings_(settings){
  settings_.validate();progress_.care=settings_.initialCare;config_.validate();for(const auto* clip:{"idle","happy","walk","sad","sleep"})library_.get(clip);
  if(library_.get("happy").loop||!library_.get("idle").loop||!library_.get("walk").loop||!library_.get("sad").loop||!library_.get("sleep").loop)throw std::runtime_error("Invalid required animation loop modes");
 }
 Model(const Model&)=delete;Model& operator=(const Model&)=delete;
 void tick(unsigned ms){
  if(screen_==Screen::Start||screen_==Screen::Species||screen_==Screen::Name){animation_.advance(ms);return;}
  if(screen_==Screen::Evolution){unsigned before=evolutionMs_;evolutionMs_=(std::min)(3500u,evolutionMs_+(std::min)(ms,3500u));if(before<1500&&evolutionMs_>=1500)play("idle",true);animation_.advance(ms);if(evolutionMs_>=3500){screen_=Screen::Room;feedback("FELICIDADES! CRECISTE");rest();}return;}
  while(ms){unsigned part=(std::min)(ms,10u);ms-=part;step(part);}checkEvolution();
 }
 void press(Button b){
  if(screen_==Screen::Evolution)return;
  if(screen_==Screen::Start){if(b!=Button::Accept)startChoice_=1-startChoice_;else if(startChoice_==0){screen_=Screen::Room;rest();}else beginSetup();return;}
  if(screen_==Screen::Species){if(b==Button::Accept){editor_.start(std::string(evolution::Catalog[speciesCursor_].defaultName),settings_.maxNameLength);screen_=Screen::Name;}else{speciesCursor_=(speciesCursor_+(b==Button::Right?1:unsigned(evolution::Catalog.size())-1))%unsigned(evolution::Catalog.size());play("idle",true);}return;}
  if(screen_==Screen::Name){if(b==Button::Accept){int result=editor_.accept();if(result==1)finishSetup();else if(result== -1){screen_=Screen::Species;play("idle",true);}}else editor_.navigate(b==Button::Right);return;}
  if(b!=Button::Accept){screen_=Screen::Room;selected_=(selected_+(b==Button::Left?5:1))%6;return;}
  if(screen_!=Screen::Room){screen_=Screen::Room;return;}
  double beforeHunger=percent(Stat::Hunger),beforeHappy=percent(Stat::Happiness),beforeClean=percent(Stat::Hygiene);
  lastAction_=int(selected_);
  switch(selected_){
  case 0:wake();stats_.change(Stat::Hunger,config_.foodGain,config_);stats_.change(Stat::Happiness,3,config_);feedback("QUE RICO! GRACIAS");play("happy",true);break;
  case 1:
   if(stats_.percent(Stat::Energy,config_)<=config_.autoSleepPercent){startSleep();feedback("PRIMERO VOY A DORMIR");play("sleep",true);break;}
   wake();stats_.change(Stat::Happiness,config_.playGain,config_);stats_.change(Stat::Energy,-config_.playEnergyCost,config_);stats_.change(Stat::Hunger,-4,config_);stats_.change(Stat::Hygiene,-3,config_);feedback("ME ENCANTA JUGAR");play("happy",true);break;
  case 2:if(sleeping_)wake();else startSleep();feedback(sleeping_?"DULCES SUENOS":"HOLA OTRA VEZ");if(sleeping_)play("sleep",true);else rest();break;
  case 3:wake();stats_.change(Stat::Hygiene,config_.cleanGain,config_);stats_.change(Stat::Happiness,5,config_);feedback("GRACIAS POR CUIDARME");play("happy",true);break;
  case 4:screen_=Screen::Health;break;
  case 5:screen_=Screen::Stats;break;
  }
  if(selected_==0)award(0,percent(Stat::Hunger)-beforeHunger);
  if(selected_==1)award(1,percent(Stat::Happiness)-beforeHappy);
  if(selected_==3)award(3,percent(Stat::Hygiene)-beforeClean);
 }
 void activate(unsigned index){if(screen_==Screen::Evolution||screen_==Screen::Start||screen_==Screen::Species||screen_==Screen::Name)return;if(index>=6)throw std::invalid_argument("Invalid action");screen_=Screen::Room;selected_=index;press(Button::Accept);}
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
 Snapshot snapshot(int64_t now)const{Snapshot s=progress_;s.sourceVersion=2;s.name=name_;s.stage=stage_;s.stats=stats_.values;s.lastAction=lastAction_;s.savedAt=now;s.sleeping=sleeping_;return s;}
 void restore(const Snapshot& s){s.validate(config_);progress_=s;if(s.sourceVersion==1)progress_.care=settings_.initialCare;name_=s.name;stage_=s.stage;stats_.values=s.stats;lastAction_=s.lastAction;sleeping_=s.sleeping;screen_=Screen::Room;hasPet_=true;newPetRequest_=false;event_.reset();messageMs_=0;rest();}
 void recordOffline(uint64_t ms){progress_.offlineMs=(std::min)(evolution::MaxTimeMs,progress_.offlineMs+(std::min)(ms,evolution::MaxTimeMs));}
 void showStartMenu(){screen_=Screen::Start;startChoice_=0;}
 void beginSetup(){hasPet_=false;screen_=Screen::Species;speciesCursor_=0;play("idle",true);}
 bool hasPet()const{return hasPet_;}
 bool takeNewPetRequest(){bool result=newPetRequest_;newPetRequest_=false;return result;}
 bool takeEvolutionSaveRequest(){bool result=saveRequest_;saveRequest_=false;return result;}
 std::optional<evolution::Event> takeEvolutionEvent(){auto result=event_;event_.reset();return result;}
 evolution::Species species()const{return screen_==Screen::Species||screen_==Screen::Name?evolution::Catalog[speciesCursor_].id:progress_.species;}
 unsigned visualStage()const{return screen_==Screen::Species||screen_==Screen::Name?1:screen_==Screen::Evolution&&evolutionMs_<1500?unsigned(previousStage_):stage_;}
 unsigned speciesCursor()const{return speciesCursor_;}unsigned startChoice()const{return startChoice_;}
 const NameEditor& nameEditor()const{return editor_;}
 unsigned evolutionElapsed()const{return evolutionMs_;}
 uint32_t experience()const{return progress_.experience;}double care()const{return progress_.care;}
 double ageMinutes()const{return double(progress_.ageMs)/60000;}uint64_t offlineMinutes()const{return progress_.offlineMs/60000;}
 uint32_t activities()const{return progress_.activities;}
 const Stats& stats()const{return stats_;}const Config& config()const{return config_;}
 double percent(Stat s)const{return stats_.percent(s,config_);}
 unsigned frame()const{return animation_.frame();}unsigned selected()const{return selected_;}int lastAction()const{return lastAction_;}
 bool mirrored()const{return mirrored_;}bool sleeping()const{return sleeping_;}int x()const{return x1000_/1000;}
 Screen screen()const{return screen_;}const std::string& name()const{return name_;}unsigned stage()const{return stage_;}
 const std::string& state()const{return state_;}const SpriteFrame& sprite()const{return animation_.sprite();}
 void notify(const char* message){feedback(message);}
};
}

