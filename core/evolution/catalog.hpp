#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <cmath>
#include <string>

namespace pet::evolution {
enum class Species : std::uint8_t { DuckRabbit = 0, CatChicken, WalkingFish, Pony, ChatGPTJunior };
enum class Stage : std::uint8_t { Baby = 1, Child, Teen, Adult };

struct SpeciesInfo {
    Species id;
    std::string_view key;
    std::string_view displayName;
    std::array<std::string_view,4> stageNames;
    std::string_view defaultName;
    std::string_view symbol;
};
inline constexpr std::array<SpeciesInfo,5> Catalog{{
    {Species::DuckRabbit,"pato_conejo","PATO CONEJO",{"BEBE","INFANTIL","JUVENIL","ADULTO MAGICO"},"PATO CONEJO","PC"},
    {Species::CatChicken,"gata_gallina","GATA GALLINA",{"BEBE","INFANTIL","JUVENIL","ADULTA MAJESTUOSA"},"GATA GALLINA","GG"},
    {Species::WalkingFish,"pez_caminante","PEZ CAMINANTE",{"BEBE","INFANTIL","EXPLORADOR","GUARDIAN ACUATICO"},"PEZ","PEZ"},
    {Species::Pony,"poni","PONI ADORABLE",{"BEBE","INFANTIL","JUVENIL MAGICO","ADULTO ALADO"},"PONI","PONI"},
    {Species::ChatGPTJunior,"chatgpt_junior","CHATGPT JUNIOR",{"MINIROBOT","ROBOT CURIOSO","ROBOT EXPLORADOR","MAGO TECNOLOGICO"},"JUNIOR","JR"}
}};
inline constexpr bool validSpecies(Species value) noexcept { return static_cast<unsigned>(value)<Catalog.size(); }
inline constexpr bool validStage(Stage value) noexcept { return value>=Stage::Baby && value<=Stage::Adult; }
inline const SpeciesInfo& speciesInfo(Species value) {
    if (!validSpecies(value)) throw std::invalid_argument("Unknown pet species");
    return Catalog[static_cast<unsigned>(value)];
}
inline Species speciesFromKey(std::string_view key) {
    for (const auto& entry : Catalog) if (entry.key==key) return entry.id;
    throw std::invalid_argument("Unknown pet species key");
}
inline std::string_view stageName(Stage stage) {
    if(!validStage(stage)) throw std::invalid_argument("Unknown evolution stage");
    constexpr std::array<std::string_view,4> names={"BEBE","INFANTIL","JUVENIL","ADULTO"};
    return names[static_cast<unsigned>(stage)-1];
}
inline std::string_view stageName(Species species,Stage stage){
    if(!validStage(stage))throw std::invalid_argument("Unknown evolution stage");
    return speciesInfo(species).stageNames[unsigned(stage)-1];
}
inline std::string clipKey(Species species,unsigned stage,const std::string& animation){
    if(!validStage(Stage(stage)))throw std::invalid_argument("Unknown stage");
    return std::string(speciesInfo(species).key)+"."+std::to_string(stage)+"."+animation;
}
struct Requirement {
    double minimumAgeMinutes;
    std::uint32_t minimumExperience;
    double minimumCare;
    std::uint32_t minimumActivities=0;
    double minimumHappiness=0;
};
struct Rules {
    std::array<Requirement,3> thresholds{{{1440,100,30,3,30},{4320,350,45,8,40},{10080,800,60,16,50}}};
    void validate() const {
        for(unsigned i=0;i<thresholds.size();++i){const auto& r=thresholds[i];
            if(!std::isfinite(r.minimumAgeMinutes)||r.minimumAgeMinutes<0||r.minimumAgeMinutes>5256000||!std::isfinite(r.minimumCare)||r.minimumCare<0||r.minimumCare>100||!std::isfinite(r.minimumHappiness)||r.minimumHappiness<0||r.minimumHappiness>100||r.minimumExperience>1000000000||r.minimumActivities>1000000000)throw std::invalid_argument("Invalid evolution requirement");
            if(i&&(r.minimumAgeMinutes<thresholds[i-1].minimumAgeMinutes||r.minimumExperience<thresholds[i-1].minimumExperience||r.minimumActivities<thresholds[i-1].minimumActivities))throw std::invalid_argument("Evolution requirements must be nondecreasing");
        }
    }
};
struct Progress {
    Stage stage=Stage::Baby;
    double ageMinutes=0;
    std::uint32_t experience=0;
    double care=60;
    std::uint32_t activities=0;
};
inline bool canEvolve(const Progress& p,const Rules& rules,double happiness=100){
    rules.validate();
    if(!validStage(p.stage)||!std::isfinite(p.ageMinutes)||p.ageMinutes<0||!std::isfinite(p.care)||p.care<0||p.care>100||!std::isfinite(happiness)||happiness<0||happiness>100)throw std::invalid_argument("Invalid evolution progress");
    if(p.stage==Stage::Adult)return false;
    const auto& r=rules.thresholds[unsigned(p.stage)-1];
    return p.ageMinutes>=r.minimumAgeMinutes&&p.experience>=r.minimumExperience&&p.care>=r.minimumCare&&p.activities>=r.minimumActivities&&happiness>=r.minimumHappiness;
}
inline bool evolve(Progress& p,const Rules& rules,double happiness=100){
    if(!canEvolve(p,rules,happiness))return false;
    p.stage=Stage(unsigned(p.stage)+1);return true;
}
inline constexpr uint64_t MaxTimeMs=315360000000ULL; // Ten years; saturating counters.
}
