#pragma once
#include <array>
#include <cmath>
#include <string>
#include <sstream>
#include <istream>
#include <set>
#include <stdexcept>
#include <algorithm>
namespace pet {
enum class Stat:unsigned {Hunger, Happiness, Energy, Hygiene, Health};
inline constexpr std::array<const char*,5> StatKeys={"hambre","felicidad","energia","higiene","salud"};
struct Range {double minimum=0,maximum=100,initial=75,decay=2;};
struct Config {
 std::array<Range,5> ranges={Range{0,100,65,3},Range{0,100,70,2},Range{0,100,70,2.5},Range{0,100,75,1.8},Range{0,100,90,0}};
 double lowPercent=25,autoSleepPercent=15,wakePercent=95,sleepGain=25,healthRecovery=1,neglectDamage=1;
 double foodGain=25,playGain=20,playEnergyCost=8,cleanGain=35;
 void validate()const{
  for(const auto& r:ranges)if(!std::isfinite(r.minimum)||!std::isfinite(r.maximum)||!std::isfinite(r.initial)||!std::isfinite(r.decay)||r.minimum< -10000||r.maximum>10000||r.minimum>=r.maximum||r.initial<r.minimum||r.initial>r.maximum||r.decay<0||r.decay>100)throw std::runtime_error("Invalid statistic range or decay");
  for(double v:{lowPercent,autoSleepPercent,wakePercent,sleepGain,healthRecovery,neglectDamage,foodGain,playGain,playEnergyCost,cleanGain})if(!std::isfinite(v)||v<0||v>100)throw std::runtime_error("Invalid gameplay value");
  if(autoSleepPercent>=wakePercent||lowPercent>=wakePercent||sleepGain<=0)throw std::runtime_error("Invalid sleep thresholds");
 }
 static Config load(std::istream& input){Config c;std::set<std::string> seen;std::string line;
  while(std::getline(input,line)){std::istringstream row(line);std::string key;if(!(row>>key)||key[0]=='#')continue;
   if(key=="stat"){std::string name;if(!(row>>name))throw std::runtime_error("Missing statistic name");if(!seen.insert("stat "+name).second)throw std::runtime_error("Duplicate statistic");auto it=std::find(StatKeys.begin(),StatKeys.end(),name);if(it==StatKeys.end())throw std::runtime_error("Unknown statistic");auto& v=c.ranges[size_t(it-StatKeys.begin())];if(!(row>>v.minimum>>v.maximum>>v.initial>>v.decay))throw std::runtime_error("Invalid stat configuration");}
   else {if(!seen.insert(key).second)throw std::runtime_error("Duplicate gameplay setting");double* value=nullptr;
    if(key=="low_percent")value=&c.lowPercent;else if(key=="auto_sleep_percent")value=&c.autoSleepPercent;else if(key=="wake_percent")value=&c.wakePercent;else if(key=="sleep_gain")value=&c.sleepGain;else if(key=="health_recovery")value=&c.healthRecovery;else if(key=="neglect_damage")value=&c.neglectDamage;else if(key=="food_gain")value=&c.foodGain;else if(key=="play_gain")value=&c.playGain;else if(key=="play_energy_cost")value=&c.playEnergyCost;else if(key=="clean_gain")value=&c.cleanGain;else throw std::runtime_error("Unknown gameplay setting: "+key);
    if(!(row>>*value))throw std::runtime_error("Invalid gameplay number");
   }std::string extra;if(row>>extra)throw std::runtime_error("Unexpected gameplay data");
  }c.validate();return c;
 }
};
struct Stats {
 std::array<double,5> values{};
 explicit Stats(const Config& c){for(size_t i=0;i<5;++i)values[i]=c.ranges[i].initial;}
 double value(Stat stat)const{return values[size_t(stat)];}
 double percent(Stat stat,const Config& c)const{const auto& r=c.ranges[size_t(stat)];return 100*(value(stat)-r.minimum)/(r.maximum-r.minimum);}
 void change(Stat stat,double percentDelta,const Config& c){size_t i=size_t(stat);const auto& r=c.ranges[i];values[i]=std::clamp(values[i]+percentDelta*(r.maximum-r.minimum)/100,r.minimum,r.maximum);}
 void validate(const Config& c)const{for(size_t i=0;i<5;++i)if(!std::isfinite(values[i])||values[i]<c.ranges[i].minimum||values[i]>c.ranges[i].maximum)throw std::runtime_error("Saved statistic outside configured range");}
};
}
