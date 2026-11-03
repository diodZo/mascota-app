#pragma once
#include <filesystem>
#include <fstream>
#include <windows.h>
#include <stdexcept>
namespace desktop {
class Profiles {
 std::filesystem::path directory_;
public:
 explicit Profiles(std::filesystem::path directory):directory_(std::move(directory)){}
 std::filesystem::path active()const{
  std::ifstream in(directory_/L"active-profile.txt");std::string name;if(std::getline(in,name)&&!name.empty()&&name.size()<64&&name.find("..") ==std::string::npos&&name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.")==std::string::npos){auto candidate=directory_/name;auto backup=candidate;backup+=L".bak";if(std::filesystem::exists(candidate)||std::filesystem::exists(backup))return candidate;}
  return directory_/L"pet.json";
 }
 std::filesystem::path next()const{for(unsigned i=2;i<100000;++i){auto path=directory_/("pet-"+std::to_string(i)+".json");auto backup=path;backup+=L".bak";if(!std::filesystem::exists(path)&&!std::filesystem::exists(backup))return path;}throw std::runtime_error("No free save profile");}
 void remember(const std::filesystem::path& path){std::filesystem::create_directories(directory_);auto temp=directory_/L"active-profile.txt.tmp";{std::ofstream out(temp);out<<path.filename().u8string()<<'\n';out.flush();if(!out)throw std::runtime_error("Cannot remember active pet");}if(!MoveFileExW(temp.c_str(),(directory_/L"active-profile.txt").c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))throw std::runtime_error("Cannot update active pet");}
};
}
