#pragma once
#include "catalog.hpp"
#include <istream>
#include <sstream>
#include <set>
#include <array>
namespace pet::evolution {
struct Settings {
 Rules rules;uint64_t cooldownMs=60000,careWindowMs=1800000;
 double significantGain=5,initialCare=60;
 unsigned maxNameLength=16;
 std::array<unsigned,6> xp={10,12,8,10,0,0};
 void validate()const{rules.validate();if(cooldownMs<1000||cooldownMs>86400000||careWindowMs<1000||careWindowMs>2592000000ULL||!std::isfinite(significantGain)||significantGain<=0||significantGain>100||!std::isfinite(initialCare)||initialCare<0||initialCare>100||maxNameLength<1||maxNameLength>16)throw std::invalid_argument("Invalid evolution settings");for(auto x:xp)if(x>1000)throw std::invalid_argument("XP award too large");}
 static Settings load(std::istream& in){Settings s;std::set<std::string> seen;std::string line;while(std::getline(in,line)){std::istringstream row(line);std::string key;if(!(row>>key)||key[0]=='#')continue;
  if(key=="stage"){unsigned stage=0;if(!(row>>stage)||stage<2||stage>4||!seen.insert("stage"+std::to_string(stage)).second)throw std::invalid_argument("Invalid or repeated evolution stage");auto& t=s.rules.thresholds[stage-2];if(!(row>>t.minimumAgeMinutes>>t.minimumExperience>>t.minimumCare>>t.minimumActivities>>t.minimumHappiness))throw std::invalid_argument("Invalid stage requirement");}
  else {if(!seen.insert(key).second)throw std::invalid_argument("Duplicate evolution setting");
   if(key=="cooldown_ms")row>>s.cooldownMs;else if(key=="care_window_ms")row>>s.careWindowMs;else if(key=="significant_gain")row>>s.significantGain;else if(key=="initial_care")row>>s.initialCare;else if(key=="max_name_length")row>>s.maxNameLength;else if(key=="xp_food")row>>s.xp[0];else if(key=="xp_play")row>>s.xp[1];else if(key=="xp_sleep")row>>s.xp[2];else if(key=="xp_clean")row>>s.xp[3];else throw std::invalid_argument("Unknown evolution setting: "+key);if(!row)throw std::invalid_argument("Invalid evolution number");
  }std::string extra;if(row>>extra)throw std::invalid_argument("Extra evolution data");
 }s.validate();return s;}
};
struct Event {Species species;Stage previous;Stage current;};
}
