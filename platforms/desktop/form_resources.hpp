#pragma once
#include "sprite_store.hpp"
#include "../../core/evolution/catalog.hpp"
#include <fstream>
#include <optional>
namespace desktop {
struct FormResources {
 pet::Library library;std::vector<std::string> missing;
 static FormResources load(const std::filesystem::path& root,SpriteStore& sprites,const std::filesystem::path& overrideManifest={}){
  FormResources result;
  auto tryLoad=[&](const std::filesystem::path& path)->std::optional<pet::Library>{try{std::ifstream in(path);if(!in)return {};return pet::Library::load(in);}catch(const std::exception& e){result.missing.push_back(path.u8string()+": "+e.what());return {};}};
  for(const auto& species:pet::evolution::Catalog)for(unsigned stage=1;stage<=4;++stage){
   auto directory=root/L"assets/characters"/std::filesystem::u8path(species.key)/("stage_0"+std::to_string(stage));
   auto original=tryLoad(!overrideManifest.empty()&&species.id==pet::evolution::Species::DuckRabbit&&stage==1?root/overrideManifest:directory/L"pet.anim");
   auto fallback=tryLoad(directory/L"placeholder/pet.anim");auto builtIn=pet::defaultLibrary();
   for(const auto* name:{"idle","walk","happy","sad","eat","sleep"}){
    std::optional<pet::Animation> selected;
    auto accept=[&](const std::optional<pet::Library>& source){if(!source)return false;auto clip=source->find(name);if(!clip||clip->loop!=(std::string(name)!="happy"&&std::string(name)!="eat"))return false;try{pet::Library one;one.add(*clip);sprites.preload(one);selected=*clip;return true;}catch(const std::exception& e){result.missing.push_back(std::string(species.key)+"/"+std::to_string(stage)+"/"+name+": "+e.what());return false;}};
    if(!accept(original)){result.missing.push_back(std::string(species.key)+"/stage_0"+std::to_string(stage)+"/"+name+": pending original; using provisional resource");if(!accept(fallback)){selected=builtIn.get(name);for(auto& f:selected->frames){f.image.clear();f.x=f.y=0;}result.missing.push_back("Emergency symbol placeholder: "+std::string(species.key)+"/"+std::to_string(stage)+"/"+name);}}
    selected->name=pet::evolution::clipKey(species.id,stage,name);result.library.add(*selected);
   }
  }
  // Backward-compatible unqualified entries for tests and initial player construction.
  for(const auto* name:{"idle","walk","happy","sad","eat","sleep"}){auto clip=result.library.get(pet::evolution::clipKey(pet::evolution::Species::DuckRabbit,1,name));clip.name=name;result.library.add(clip);}
  return result;
 }
};
}
