#include "../core/evolution/catalog.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
void require(bool success){ if(!success) throw std::runtime_error("Evolution test failed"); }
template<typename Callable> void requireReject(Callable f){ bool failed=false;try{f();}catch(const std::invalid_argument&){failed=true;}require(failed); }
int main(){
    using namespace pet::evolution;
    require(Catalog.size()==5);
    for (const auto& entry:Catalog){require(speciesFromKey(entry.key)==entry.id);require(!entry.displayName.empty());}
    require(speciesInfo(Species::DuckRabbit).key=="pato_conejo");
    require(stageName(Stage::Baby)=="BEBE" && stageName(Stage::Adult)=="ADULTO");
    requireReject([]{speciesFromKey("unknown");});
    requireReject([]{speciesInfo(static_cast<Species>(99));});
    requireReject([]{stageName(static_cast<Stage>(0));});
    Rules rules;rules.thresholds={{{60,20,30},{240,100,45},{720,300,60}}};rules.validate(); Progress p;
    require(!canEvolve(p,rules));
    p.ageMinutes=60;p.experience=20;p.care=29;require(!canEvolve(p,rules));
    p.care=30;require(evolve(p,rules)&&p.stage==Stage::Child);
    require(!evolve(p,rules));
    p.ageMinutes=240;p.experience=100;p.care=45;require(evolve(p,rules)&&p.stage==Stage::Teen);
    p.ageMinutes=720;p.experience=300;p.care=60;require(evolve(p,rules)&&p.stage==Stage::Adult);
    require(!canEvolve(p,rules)&&!evolve(p,rules));
    Rules production;require(production.thresholds[0].minimumAgeMinutes==1440&&production.thresholds[2].minimumExperience==800);
    Progress complete{Stage::Baby,10080,800,100,20};
    auto missing=complete;missing.ageMinutes=0;require(!canEvolve(missing,production));missing=complete;missing.experience=0;require(!canEvolve(missing,production));missing=complete;missing.care=0;require(!canEvolve(missing,production));missing=complete;missing.activities=0;require(!canEvolve(missing,production));require(!canEvolve(complete,production,0));require(evolve(complete,production)&&complete.stage==Stage::Child);
    for(const auto& entry:Catalog)for(unsigned stage=1;stage<=4;++stage)require(!stageName(entry.id,Stage(stage)).empty());
    Progress bad;bad.stage=static_cast<Stage>(5);requireReject([&]{canEvolve(bad,rules);});
    Rules invalid;invalid.thresholds[1].minimumAgeMinutes=1;requireReject([&]{invalid.validate();});
    invalid=Rules{};invalid.thresholds[0].minimumCare=101;requireReject([&]{invalid.validate();});
    std::cout<<"PASS: five species, four stages, safe thresholds, step-by-step evolution, invalid data\n";
}
