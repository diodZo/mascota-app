#pragma once
#include <string>
#include <algorithm>
namespace pet {
class NameEditor {
 std::string value_,default_;unsigned limit_=16,cursor_=39;
public:
 inline static const std::string Alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -";
 static constexpr unsigned Delete=38,Finish=39,Back=40,Count=41;
 void start(std::string name,unsigned limit){limit_=limit;default_=name.substr(0,limit);value_=default_;cursor_=Finish;}
 void navigate(bool right){cursor_=(cursor_+(right?1:Count-1))%Count;}
 // Returns 1=finish, -1=back, 0=editing.
 int accept(){if(cursor_==Finish){if(value_.find_first_not_of(' ')==std::string::npos)value_=default_;return 1;}if(cursor_==Back)return -1;if(cursor_==Delete){if(!value_.empty())value_.pop_back();}else if(value_.size()<limit_)value_+=Alphabet[cursor_];return 0;}
 const std::string& value()const{return value_;}unsigned cursor()const{return cursor_;}
 std::string token()const{return cursor_==Delete?"BORRAR":cursor_==Finish?"LISTO":cursor_==Back?"VOLVER":Alphabet[cursor_]==' '?"ESPACIO":std::string(1,Alphabet[cursor_]);}
};
}
