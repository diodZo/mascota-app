#pragma once
#include "../pet/stats.hpp"
#include <cstdint>
#include <optional>
#include <iomanip>
#include <locale>
#include "../evolution/catalog.hpp"
namespace pet {
struct Snapshot {
 std::string name="PATO CONEJO";unsigned stage=1;std::array<double,5> stats{};int lastAction=-1;int64_t savedAt=0;bool sleeping=false;
 unsigned sourceVersion=2;
 evolution::Species species=evolution::Species::DuckRabbit;
 uint64_t ageMs=0,playedMs=0,offlineMs=0,careSampleMs=0;
 uint32_t experience=0,activities=0;double care=60,sleepStartEnergy=0;
 std::array<uint64_t,6> rewardReadyMs{};bool sleepRewarded=false;
 void validate(const Config& config)const{
  if(name.empty()||name.size()>16||(stage<1||stage>4)||lastAction< -1||lastAction>5||savedAt<0||savedAt>4000000000000LL)throw std::runtime_error("Unsupported saved pet data");
  for(char ch:name)if(!((ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')||ch==' '||ch=='-'))throw std::runtime_error("Pet name must use A-Z, digits, spaces or hyphens");
  Stats s(config);s.values=stats;s.validate(config);
  if(!evolution::validSpecies(species)||ageMs>evolution::MaxTimeMs||playedMs>evolution::MaxTimeMs||offlineMs>evolution::MaxTimeMs||careSampleMs>playedMs||experience>1000000000||activities>1000000000||!std::isfinite(care)||care<0||care>100||!std::isfinite(sleepStartEnergy)||sleepStartEnergy<0||sleepStartEnergy>100)throw std::runtime_error("Invalid saved progress");
  for(auto t:rewardReadyMs)if(t>evolution::MaxTimeMs+86400000)throw std::runtime_error("Invalid reward cooldown");
 }
};
inline std::string encode(const Snapshot& s,const Config& c){
 s.validate(c);std::ostringstream out;out.imbue(std::locale::classic());out<<std::setprecision(17);
 out<<"{\n  \"version\": 2,\n  \"name\": "<<std::quoted(s.name)<<",\n  \"stage\": "<<s.stage<<",\n  \"stats\": [";
 for(size_t i=0;i<5;++i)out<<(i?", ":"")<<s.stats[i];
 out<<"],\n  \"last_action\": "<<s.lastAction<<",\n  \"saved_at\": "<<s.savedAt<<",\n  \"sleeping\": "<<(s.sleeping?"true":"false")
 <<",\n  \"species\": "<<std::quoted(std::string(evolution::speciesInfo(s.species).key))
 <<",\n  \"age_ms\": "<<s.ageMs<<",\n  \"played_ms\": "<<s.playedMs<<",\n  \"offline_ms\": "<<s.offlineMs
 <<",\n  \"experience\": "<<s.experience<<",\n  \"care\": "<<s.care<<",\n  \"care_sample_ms\": "<<s.careSampleMs
 <<",\n  \"activities\": "<<s.activities<<",\n  \"reward_ready_ms\": [";
 for(size_t i=0;i<6;++i)out<<(i?", ":"")<<s.rewardReadyMs[i];
 out<<"],\n  \"sleep_start_energy\": "<<s.sleepStartEnergy<<",\n  \"sleep_rewarded\": "<<(s.sleepRewarded?"true":"false")<<"\n}\n";return out.str();
}
// A deliberately small, strict JSON schema; rejects unknown/duplicate fields.
class SnapshotReader {
 std::istream& in_;
 void expect(char wanted){char got=0;if(!(in_>>got)||got!=wanted)throw std::runtime_error("Invalid save JSON");}
 std::string string(){in_>>std::ws;if(in_.peek()!='"')throw std::runtime_error("Expected JSON string");std::string s;if(!(in_>>std::quoted(s)))throw std::runtime_error("Invalid JSON string");return s;}
 double number(){double n=0;if(!(in_>>n)||!std::isfinite(n))throw std::runtime_error("Invalid save number");return n;}
 int64_t integer(){double n=number();if(std::floor(n)!=n||n< -1||n>4000000000000.0)throw std::runtime_error("Invalid save integer");return int64_t(n);}
 bool boolean(){in_>>std::ws;std::string token;for(int i=0;i<4;++i){char ch=0;if(!in_.get(ch))throw std::runtime_error("Invalid JSON boolean");token+=ch;}if(token=="true")return true;if(token=="fals"){char ch=0;if(in_.get(ch)&&ch=='e')return false;}throw std::runtime_error("Invalid JSON boolean");}
 uint64_t counter(){auto n=integer();if(n<0)throw std::runtime_error("Negative save counter");return uint64_t(n);}
 uint32_t boundedCount(){auto n=counter();if(n>1000000000)throw std::runtime_error("Save count too large");return uint32_t(n);}
public:
 explicit SnapshotReader(std::istream& in):in_(in){in_.imbue(std::locale::classic());}
 Snapshot read(const Config& c){Snapshot result;std::set<std::string> seen;expect('{');
  while(true){std::string key=string();if(!seen.insert(key).second)throw std::runtime_error("Duplicate save key");expect(':');
   if(key=="version"){auto v=integer();if(v!=1&&v!=2)throw std::runtime_error("Unsupported save version");result.sourceVersion=unsigned(v);}
   else if(key=="name")result.name=string();
   else if(key=="stage"){auto n=integer();if(n<1||n>4)throw std::runtime_error("Unsupported saved stage");result.stage=unsigned(n);}
   else if(key=="stats"){expect('[');for(size_t i=0;i<5;++i){if(i)expect(',');result.stats[i]=number();}expect(']');}
   else if(key=="last_action"){auto n=integer();if(n< -1||n>5)throw std::runtime_error("Invalid saved action");result.lastAction=int(n);}
   else if(key=="saved_at")result.savedAt=integer();
   else if(key=="sleeping")result.sleeping=boolean();
   else if(key=="species")result.species=evolution::speciesFromKey(string());
   else if(key=="age_ms")result.ageMs=counter();
   else if(key=="played_ms")result.playedMs=counter();
   else if(key=="offline_ms")result.offlineMs=counter();
   else if(key=="experience")result.experience=boundedCount();
   else if(key=="care")result.care=number();
   else if(key=="care_sample_ms")result.careSampleMs=counter();
   else if(key=="activities")result.activities=boundedCount();
   else if(key=="reward_ready_ms"){expect('[');for(size_t i=0;i<6;++i){if(i)expect(',');result.rewardReadyMs[i]=counter();}expect(']');}
   else if(key=="sleep_start_energy")result.sleepStartEnergy=number();
   else if(key=="sleep_rewarded")result.sleepRewarded=boolean();
   else throw std::runtime_error("Unknown save key");
   in_>>std::ws;char next=0;if(!in_.get(next))throw std::runtime_error("Incomplete save JSON");if(next=='}')break;if(next!=',')throw std::runtime_error("Invalid save separator");
  }
  if(!seen.count("version"))throw std::runtime_error("Missing save version");
  if(result.sourceVersion==1){
   const std::set<std::string> legacy={"version","name","stage","stats","last_action","saved_at","sleeping"};
   if(seen!=legacy||result.stage!=1)throw std::runtime_error("Invalid legacy save schema");
   result.species=evolution::Species::DuckRabbit;result.sleepStartEnergy=100*(result.stats[2]-c.ranges[2].minimum)/(c.ranges[2].maximum-c.ranges[2].minimum);
  }else if(seen.size()!=18)throw std::runtime_error("Missing save fields");in_>>std::ws;if(in_.peek()!=std::char_traits<char>::eof())throw std::runtime_error("Extra save data");result.validate(c);return result;
 }
};
struct LoadResult {std::optional<Snapshot> snapshot;bool recovered=false;};
class IStorage {
public:virtual ~IStorage()=default;virtual LoadResult load(const Config& config)=0;virtual void save(const Snapshot& snapshot,const Config& config)=0;
};
}
