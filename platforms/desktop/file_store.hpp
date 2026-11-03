#pragma once
#include <windows.h>
#include <filesystem>
#include <fstream>
#include "../../core/persistence/snapshot.hpp"
namespace desktop {
class FileStore final:public pet::IStorage {
 std::filesystem::path path_;
 static std::filesystem::path suffix(const std::filesystem::path& p,const wchar_t* s){auto result=p;result+=s;return result;}
 static pet::Snapshot read(const std::filesystem::path& path,const pet::Config& config){
  if(std::filesystem::file_size(path)>65536)throw std::runtime_error("Save file is too large");std::ifstream in(path);if(!in)throw std::runtime_error("Cannot read save file");return pet::SnapshotReader(in).read(config);
 }
 static void write(const std::filesystem::path& path,const std::string& text){
  HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);if(file==INVALID_HANDLE_VALUE)throw std::runtime_error("Cannot create save file");
  DWORD written=0;BOOL okay=WriteFile(file,text.data(),DWORD(text.size()),&written,nullptr);bool flushed=okay&&written==text.size()&&FlushFileBuffers(file);CloseHandle(file);if(!flushed)throw std::runtime_error("Cannot flush save file");
 }
public:
 explicit FileStore(std::filesystem::path path):path_(std::move(path)){}
 pet::LoadResult load(const pet::Config& config)override{
  bool exists=std::filesystem::exists(path_),backup=std::filesystem::exists(suffix(path_,L".bak"));
  if(exists)try{return {read(path_,config),false};}catch(const std::exception&){}
  if(backup)try{return {read(suffix(path_,L".bak"),config),true};}catch(const std::exception&){}
  if(exists||backup)throw std::runtime_error("Cannot restore save or backup. Files were preserved; inspect saves/pet.json and its .bak before starting a new game.");return {};
 }
 void save(const pet::Snapshot& snapshot,const pet::Config& config)override{
  const auto text=pet::encode(snapshot,config);if(!path_.parent_path().empty())std::filesystem::create_directories(path_.parent_path());auto temp=suffix(path_,L".tmp");write(temp,text);
  bool valid=false;if(std::filesystem::exists(path_))try{read(path_,config);valid=true;}catch(const std::exception&){}
  if(valid){auto backupTemp=suffix(path_,L".bak.tmp");if(!CopyFileW(path_.c_str(),backupTemp.c_str(),FALSE)||!MoveFileExW(backupTemp.c_str(),suffix(path_,L".bak").c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))throw std::runtime_error("Cannot update save backup");}
  if(!MoveFileExW(temp.c_str(),path_.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))throw std::runtime_error("Cannot commit save file");
 }
};
}
