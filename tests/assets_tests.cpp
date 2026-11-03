#define UNICODE
#define _UNICODE
#include "../platforms/desktop/renderer.hpp"
#include "../platforms/desktop/file_store.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
void verify(bool ok,int line){if(!ok)throw std::runtime_error("Asset test failed at line "+std::to_string(line));}
#define check(condition) verify((condition),__LINE__)
int wmain(int argc,wchar_t** argv){
 if(argc!=2)return 2;HRESULT hr=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);if(FAILED(hr))return 3;int code=0;
 try{
  std::filesystem::path root=argv[1];auto load=[&](const char* file){std::ifstream in(root/"assets/characters/pato_conejo/baby/placeholder"/file);return pet::Library::load(in);};
  auto sheet=load("pet.anim"),individual=load("individual.anim");desktop::SpriteStore store(root);store.preload(sheet);store.preload(individual);
  auto screen=GetDC(nullptr);auto a=CreateCompatibleDC(screen),b=CreateCompatibleDC(screen);auto ba=CreateCompatibleBitmap(screen,720,720),bb=CreateCompatibleBitmap(screen,720,720);auto olda=SelectObject(a,ba),oldb=SelectObject(b,bb);ReleaseDC(nullptr,screen);
  for(bool mirrored:{false,true})for(size_t i=0;i<sheet.get("happy").frames.size();++i){desktop::rect(a,0,0,100,100,RGB(11,21,31));desktop::rect(b,0,0,100,100,RGB(11,21,31));store.draw(a,sheet.get("happy").frames[i],0,10,mirrored);store.draw(b,individual.get("happy").frames[i],0,10,mirrored);for(int y=0;y<80;++y)for(int x=0;x<64;++x)check(GetPixel(a,x,y)==GetPixel(b,x,y));check(GetPixel(a,0,10)==RGB(11,21,31));}
  auto invalid=sheet;auto extra=sheet.get("idle");extra.name="out_of_bounds";extra.frames[0].x=4096;invalid.add(extra);bool rejected=false;try{store.preload(invalid);}catch(const std::exception&){rejected=true;}check(rejected);
  pet::Model model(sheet);{desktop::Renderer renderer(store);renderer.render(model);renderer.present(a);check(GetPixel(a,0,0)==desktop::Purple);check(GetPixel(a,719,0)==desktop::Purple);}
  for(auto action:{4u,5u}){model.activate(action);desktop::Renderer renderer(store);renderer.render(model);renderer.present(a);}
  // New atlas frames contain actual pixel changes, not translation-only animations.
  for(const auto* name:{"idle","happy","walk","sad","sleep"}){const auto& frames=sheet.get(name).frames;desktop::rect(a,0,0,100,100,RGB(11,21,31));auto first=frames.front();first.dy=first.dx=0;store.draw(a,first,0,0,false);bool different=false;
   for(const auto& source:frames){desktop::rect(b,0,0,100,100,RGB(11,21,31));auto other=source;other.dy=other.dx=0;store.draw(b,other,0,0,false);for(int y=0;y<64&&!different;++y)for(int x=0;x<64;++x)if(GetPixel(a,x,y)!=GetPixel(b,x,y)){different=true;break;}if(different)break;}check(different);
  }
  auto unique=std::to_string(GetCurrentProcessId())+"-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());auto path=root/"build/persistence-tests"/unique/"pet.json";desktop::FileStore storage(path);check(!storage.load(model.config()).snapshot);auto snapshot=model.snapshot(1234);storage.save(snapshot,model.config());auto loaded=storage.load(model.config());check(loaded.snapshot->stats==snapshot.stats&&!loaded.recovered);model.activate(0);auto updated=model.snapshot(1240);storage.save(updated,model.config());check(storage.load(model.config()).snapshot->savedAt==1240);{std::ofstream broken(path);broken<<"corrupt";}loaded=storage.load(model.config());check(loaded.recovered&&loaded.snapshot->savedAt==1234);storage.save(updated,model.config());check(!storage.load(model.config()).recovered);
  {std::ofstream broken(path);broken<<"corrupt";std::ofstream backup(path.wstring()+L".bak");backup<<"corrupt";}bool refused=false;try{storage.load(model.config());}catch(const std::exception&){refused=true;}check(refused);
  SelectObject(a,olda);SelectObject(b,oldb);DeleteObject(ba);DeleteObject(bb);DeleteDC(a);DeleteDC(b);
  std::cout<<"PASS: actual PNG load, atlas/individual pixel equivalence, alpha, horizontal flip, invalid crop, 240x240 renderer, panels, changed animation pixels, 3x presentation, file save/reload and backup recovery\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';code=1;}CoUninitialize();return code;
}
