#pragma once
#include "../pet/stats.hpp"
#include <cstdint>
#include <optional>
#include <iomanip>
#include <locale>
namespace pet {
struct Snapshot {
 std::string name="PATO CONEJO";unsigned stage=1;std::array<double,5> stats{};int lastAction=-1;int64_t savedAt=0;bool sleeping=false;
 void validate(const Config& config)const{
  if(name.empty()||name.size()>16||stage!=1||lastAction< -1||lastAction>5||savedAt<0||savedAt>4000000000000LL)throw std::runtime_error("Unsupported saved pet data");
  for(char ch:name)if(!((ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')||ch==' '||ch=='-'))throw std::runtime_error("Pet name must use A-Z, digits, spaces or hyphens");
  Stats s(config);s.values=stats;s.validate(config);
 }
};
inline std::string encode(const Snapshot& s,const Config& c){s.validate(c);std::ostringstream out;out.imbue(std::locale::classic());out<<std::setprecision(17)<<"{\n  \"version\": 1,\n  \"name\": "<<std::quoted(s.name)<<",\n  \"stage\": "<<s.stage<<",\n  \"stats\": [";for(size_t i=0;i<5;++i)out<<(i?", ":"")<<s.stats[i];out<<"],\n  \"last_action\": "<<s.lastAction<<",\n  \"saved_at\": "<<s.savedAt<<",\n  \"sleeping\": "<<(s.sleeping?"true":"false")<<"\n}\n";return out.str();}
// A deliberately small, strict JSON schema; rejects unknown/duplicate fields.
class SnapshotReader {
 std::istream& in_;
 void expect(char wanted){char got=0;if(!(in_>>got)||got!=wanted)throw std::runtime_error("Invalid save JSON");}
 std::string string(){in_>>std::ws;if(in_.peek()!='"')throw std::runtime_error("Expected JSON string");std::string s;if(!(in_>>std::quoted(s)))throw std::runtime_error("Invalid JSON string");return s;}
 double number(){double n=0;if(!(in_>>n)||!std::isfinite(n))throw std::runtime_error("Invalid save number");return n;}
 int64_t integer(){double n=number();if(std::floor(n)!=n||n< -1||n>4000000000000.0)throw std::runtime_error("Invalid save integer");return int64_t(n);}
public:
 explicit SnapshotReader(std::istream& in):in_(in){in_.imbue(std::locale::classic());}
 Snapshot read(const Config& c){Snapshot result;std::set<std::string> seen;expect('{');
  while(true){std::string key=string();if(!seen.insert(key).second)throw std::runtime_error("Duplicate save key");expect(':');
   if(key=="version"){if(integer()!=1)throw std::runtime_error("Unsupported save version");}
   else if(key=="name")result.name=string();
   else if(key=="stage"){auto n=integer();if(n!=1)throw std::runtime_error("Unsupported saved stage");result.stage=unsigned(n);}
   else if(key=="stats"){expect('[');for(size_t i=0;i<5;++i){if(i)expect(',');result.stats[i]=number();}expect(']');}
   else if(key=="last_action"){auto n=integer();if(n< -1||n>5)throw std::runtime_error("Invalid saved action");result.lastAction=int(n);}
   else if(key=="saved_at")result.savedAt=integer();
   else if(key=="sleeping"){in_>>std::ws;std::string token;for(int i=0;i<4;++i){char ch=0;if(!in_.get(ch))throw std::runtime_error("Invalid JSON boolean");token+=ch;}if(token=="true")result.sleeping=true;else if(token=="fals"){char ch=0;if(!in_.get(ch)||ch!='e')throw std::runtime_error("Invalid JSON boolean");result.sleeping=false;}else throw std::runtime_error("Invalid JSON boolean");}
   else throw std::runtime_error("Unknown save key");
   in_>>std::ws;char next=0;if(!in_.get(next))throw std::runtime_error("Incomplete save JSON");if(next=='}')break;if(next!=',')throw std::runtime_error("Invalid save separator");
  }
  if(seen.size()!=7)throw std::runtime_error("Missing save fields");in_>>std::ws;if(in_.peek()!=std::char_traits<char>::eof())throw std::runtime_error("Extra save data");result.validate(c);return result;
 }
};
struct LoadResult {std::optional<Snapshot> snapshot;bool recovered=false;};
class IStorage {
public:virtual ~IStorage()=default;virtual LoadResult load(const Config& config)=0;virtual void save(const Snapshot& snapshot,const Config& config)=0;
};
}
