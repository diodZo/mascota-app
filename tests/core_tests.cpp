#include "../core/pet/model.hpp"
#include <iostream>
#include <stdexcept>
void check(bool ok){if(!ok)throw std::runtime_error("Test failed");}
template<class F>void rejects(F fn){bool rejected=false;try{fn();}catch(const std::exception&){rejected=true;}check(rejected);}
int main(){
 // Original Player checks retained.
 pet::Player loop({4,4,true});check(loop.frame()==0);loop.advance(249);check(loop.frame()==0);loop.advance(1);check(loop.frame()==1);loop.advance(750);check(loop.frame()==0);check(!loop.takeCompletion());
 pet::Player once({2,2,false});once.advance(5000);check(once.frame()==1&&once.finished());check(once.takeCompletion());check(!once.takeCompletion());once.advance(1000);check(!once.takeCompletion());once.play({2,4,false});check(once.frame()==0&&!once.finished());once.advance(500);check(once.takeCompletion());
 rejects([]{pet::Player bad({0,4,true});});rejects([]{pet::Player bad({4,0,true});});
 // Six actions now have gameplay effects rather than demonstration-only reactions.
 pet::Model m;m.press(pet::Button::Left);check(m.selected()==5);m.press(pet::Button::Right);check(m.selected()==0);
 double food=m.stats().value(pet::Stat::Hunger);m.press(pet::Button::Accept);check(m.lastAction()==0&&m.stats().value(pet::Stat::Hunger)>food&&m.state()=="happy");m.tick(750);check(m.state()=="idle");m.tick(250);check(m.frame()==1);
 double happiness=m.stats().value(pet::Stat::Happiness),energy=m.stats().value(pet::Stat::Energy);m.activate(1);check(m.stats().value(pet::Stat::Happiness)>happiness&&m.stats().value(pet::Stat::Energy)<energy&&m.state()=="happy");m.tick(750);check(m.state()=="idle");
 m.tick(8000);check(m.state()=="walk");int start=m.x();m.tick(500);check(m.x()<start&&m.mirrored());m.tick(1900);check(m.state()=="idle");
 double hygiene=m.stats().value(pet::Stat::Hygiene);m.activate(3);check(m.stats().value(pet::Stat::Hygiene)>hygiene);
 m.activate(2);check(m.sleeping()&&m.state()=="sleep");energy=m.stats().value(pet::Stat::Energy);m.tick(10000);check(m.stats().value(pet::Stat::Energy)>energy);m.activate(2);check(!m.sleeping());
 m.activate(4);check(m.screen()==pet::Screen::Health);m.press(pet::Button::Accept);check(m.screen()==pet::Screen::Room);m.activate(5);check(m.screen()==pet::Screen::Stats);m.press(pet::Button::Left);check(m.selected()==4&&m.screen()==pet::Screen::Room);
 auto lib=pet::defaultLibrary();pet::AnimationManager manager(lib,"idle");manager.advance(250);check(manager.frame()==1);manager.play("happy");manager.advance(750);check(manager.takeCompletion());check(!manager.takeCompletion());manager.play("idle");check(manager.frame()==0);rejects([&]{manager.play("missing");});
 std::istringstream valid("petanim 1\nclip idle 8 1\nframe \"image with spaces.png\" 64 0 64 64 -2 -3\nend\n");auto parsed=pet::Library::load(valid);check(parsed.get("idle").frames[0].image=="image with spaces.png"&&parsed.get("idle").frames[0].dy==-3);
 for(const auto* text:{"petanim 9","petanim 1\nclip bad 0 1\nend", "petanim 1\nclip bad 4 1\nframe a 0 0 32 64 0 0\nend", "petanim 1\nclip bad 4 1", "petanim 1\nunknown", "petanim 1\nend"})rejects([&]{std::istringstream in(text);pet::Library::load(in);});
 // Same elapsed time produces the same model state with coarse or fine tick calls.
 pet::Model coarse,fine;coarse.tick(11000);for(int i=0;i<1100;++i)fine.tick(10);check(coarse.x()==fine.x()&&coarse.state()==fine.state()&&coarse.frame()==fine.frame());
 // Passive changes and state-dependent messages.
 pet::Model timed;auto before=timed.stats().values;timed.tick(60000);check(timed.stats().value(pet::Stat::Hunger)<before[0]);check(timed.stats().value(pet::Stat::Happiness)<before[1]);check(timed.stats().value(pet::Stat::Energy)<before[2]);check(timed.stats().value(pet::Stat::Hygiene)<before[3]);check(timed.stats().value(pet::Stat::Health)>before[4]);
 auto low=timed.snapshot(123);low.stats[0]=5;timed.restore(low);check(timed.state()=="sad"&&timed.message()=="TENGO HAMBRE");timed.activate(0);timed.tick(750);check(timed.state()=="idle");
 low=timed.snapshot(124);low.stats[2]=5;timed.restore(low);timed.tick(10);check(timed.sleeping()&&timed.state()=="sleep");timed.tick(240000);check(!timed.sleeping());
 for(unsigned i=0;i<200;++i){timed.activate(0);timed.activate(3);}for(size_t i=0;i<5;++i)check(timed.stats().values[i]>=0&&timed.stats().values[i]<=100);
 pet::Config custom;custom.ranges[0]={10,20,12,3};pet::Model different(pet::defaultLibrary(),custom);different.activate(0);check(different.stats().value(pet::Stat::Hunger)==14.5);
 std::istringstream settings("stat hambre 10 20 12 3\nfood_gain 25\n");check(pet::Config::load(settings).ranges[0].maximum==20);
 rejects([]{std::istringstream bad("stat hambre 10 0 1 2");pet::Config::load(bad);});rejects([]{std::istringstream bad("food_gain 2\nfood_gain 3");pet::Config::load(bad);});
 pet::Model needs;auto needy=needs.snapshot(0);needy.stats[3]=5;needs.restore(needy);check(needs.state()=="sad"&&needs.message()=="QUIERO UN BANO");needs.activate(3);needs.tick(750);check(needs.state()=="idle");needy=needs.snapshot(0);needy.stats[1]=5;needs.restore(needy);check(needs.message()=="QUIERO JUGAR");needs.activate(1);needs.activate(1);needs.tick(750);check(needs.state()=="idle");
 pet::Config recovery;recovery.healthRecovery=100;pet::Model recovering(pet::defaultLibrary(),recovery);auto unwell=recovering.snapshot(0);unwell.stats[4]=0;recovering.restore(unwell);check(recovering.state()=="sad");recovering.tick(16000);check(recovering.stats().value(pet::Stat::Health)>25&&recovering.state()!="sad");
 auto unchanged=needs.stats().values;needs.activate(4);check(needs.stats().values==unchanged);needs.activate(5);check(needs.stats().values==unchanged);
 auto saved=timed.snapshot(123456);saved.name="NUBE";std::istringstream json(pet::encode(saved,timed.config()));auto decoded=pet::SnapshotReader(json).read(timed.config());check(decoded.name=="NUBE"&&decoded.savedAt==123456&&decoded.lastAction==3&&decoded.stats==saved.stats);pet::Model restored;restored.restore(decoded);check(restored.name()=="NUBE"&&restored.stats().values==saved.stats);
 saved.sleeping=true;std::istringstream asleepJson(pet::encode(saved,timed.config()));auto asleep=pet::SnapshotReader(asleepJson).read(timed.config());check(asleep.sleeping);restored.restore(asleep);check(restored.state()=="sleep");
 auto encoded=pet::encode(saved,timed.config());for(const auto& bad:{encoded.substr(0,encoded.size()/2),std::string("{}"),encoded+"trailing"})rejects([&]{std::istringstream input(bad);pet::SnapshotReader(input).read(timed.config());});
 auto invalid=encoded;invalid.replace(invalid.find("\"version\": 1"),12,"\"version\": 9");rejects([&]{std::istringstream input(invalid);pet::SnapshotReader(input).read(timed.config());});
 std::cout<<"PASS: animation regressions, six actions, stats and bounds, sleep/recovery, sad/happy/walk, panels, config, JSON round-trip, invalid saves, deterministic time\n";
}
