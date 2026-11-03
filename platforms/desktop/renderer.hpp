#pragma once
#include "sprite_store.hpp"
#include "pixel_font.hpp"
#include "../../core/pet/model.hpp"
namespace desktop {
inline constexpr int LogicalSize=240,Scale=3;
inline constexpr COLORREF Ink=RGB(62,62,81),Purple=RGB(113,86,137),Cream=RGB(255,246,218);
inline void rect(HDC dc,int x,int y,int w,int h,COLORREF c){HBRUSH brush=CreateSolidBrush(c);RECT r{x,y,x+w,y+h};FillRect(dc,&r,brush);DeleteObject(brush);}
inline void text(HDC dc,const std::string& s,int x,int y,COLORREF c=Ink){for(char ch:s){for(const auto& g:glyphs)if(g.key==ch){for(int yy=0;yy<7;++yy)for(int xx=0;xx<5;++xx)if(g.rows[yy]&(1<<(4-xx)))rect(dc,x+xx,y+yy,1,1,c);break;}x+=6;}}
inline void centered(HDC dc,const std::string& s,int y,COLORREF c=Ink){text(dc,s,(240-int(s.size())*6+1)/2,y,c);}
inline void heart(HDC dc,int x,int y,COLORREF c){rect(dc,x+1,y,3,2,c);rect(dc,x+5,y,3,2,c);rect(dc,x,y+2,9,3,c);rect(dc,x+1,y+5,7,1,c);rect(dc,x+2,y+6,5,1,c);rect(dc,x+3,y+7,3,1,c);rect(dc,x+4,y+8,1,1,c);}
inline void icon(HDC dc,unsigned kind,int x,int y,COLORREF c){const COLORREF cutout=c==Cream?Purple:Cream;
 if(kind==0){rect(dc,x,y+6,20,4,c);rect(dc,x+3,y+10,14,5,c);rect(dc,x+6,y+15,8,2,c);rect(dc,x+5,y+1,2,3,c);rect(dc,x+11,y,2,4,c);}
 if(kind==1){rect(dc,x+2,y+3,16,12,c);rect(dc,x,y+6,20,6,c);rect(dc,x+5,y+6,2,6,cutout);rect(dc,x+3,y+8,6,2,cutout);rect(dc,x+13,y+7,2,2,cutout);rect(dc,x+15,y+10,2,2,cutout);}
 if(kind==2){rect(dc,x+3,y,12,17,c);rect(dc,x,y+4,18,10,c);rect(dc,x+8,y,10,12,cutout);rect(dc,x+15,y+2,2,2,c);}
 if(kind==3){rect(dc,x,y+8,20,5,c);rect(dc,x+3,y+13,14,3,c);rect(dc,x+2,y+3,4,4,c);rect(dc,x+9,y,4,4,c);rect(dc,x+15,y+4,3,3,c);}
 if(kind==4){rect(dc,x+2,y+2,16,15,c);rect(dc,x+8,y+5,4,9,cutout);rect(dc,x+5,y+8,10,3,cutout);}
 if(kind==5){rect(dc,x+1,y+12,4,5,c);rect(dc,x+8,y+7,4,10,c);rect(dc,x+15,y+1,4,16,c);}
}
class Renderer {
 HDC surface_=nullptr;HBITMAP bitmap_=nullptr;HGDIOBJ previous_=nullptr;SpriteStore& sprites_;
public:
 explicit Renderer(SpriteStore& sprites):sprites_(sprites){surface_=CreateCompatibleDC(nullptr);BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=240;info.bmiHeader.biHeight=-240;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;void* data=nullptr;bitmap_=CreateDIBSection(surface_,&info,DIB_RGB_COLORS,&data,nullptr,0);if(!surface_||!bitmap_){if(bitmap_)DeleteObject(bitmap_);if(surface_)DeleteDC(surface_);throw std::runtime_error("Cannot create 240x240 renderer");}previous_=SelectObject(surface_,bitmap_);}
 ~Renderer(){SelectObject(surface_,previous_);DeleteObject(bitmap_);DeleteDC(surface_);}
 void render(const pet::Model& model){auto dc=surface_;
  rect(dc,0,0,240,240,Cream);rect(dc,0,0,240,27,Purple);text(dc,model.name(),9,10,Cream);heart(dc,164,8,model.percent(pet::Stat::Health)<model.config().lowPercent?RGB(213,204,219):RGB(250,183,167));text(dc,"BEBE",184,10,Cream);
  rect(dc,8,34,224,140,Ink);rect(dc,10,36,220,136,RGB(218,235,226));
  // Pixel room: wallpaper, framed window, shelf and a plant.
  for(int y=42;y<132;y+=13)for(int x=17;x<228;x+=16)rect(dc,x,y,1,2,RGB(188,212,199));
  rect(dc,22,48,53,53,RGB(153,132,118));rect(dc,25,51,47,47,RGB(173,214,231));
  rect(dc,55,56,9,9,RGB(255,229,151));rect(dc,29,68,19,5,RGB(239,248,240));rect(dc,34,65,9,3,RGB(239,248,240));
  rect(dc,25,84,47,14,RGB(137,182,147));rect(dc,46,51,3,47,RGB(153,132,118));rect(dc,25,75,47,3,RGB(153,132,118));
  rect(dc,170,78,44,4,RGB(153,132,118));rect(dc,177,67,10,11,RGB(228,154,136));rect(dc,181,57,3,10,RGB(103,151,112));rect(dc,174,58,7,4,RGB(103,151,112));rect(dc,184,54,6,4,RGB(103,151,112));
  rect(dc,10,145,220,27,RGB(202,173,143));rect(dc,10,142,220,3,RGB(153,132,118));
  for(int y=148;y<173;y+=10){rect(dc,10,y,220,1,RGB(182,151,128));for(int x=24+(y%3)*13;x<230;x+=48)rect(dc,x,y,1,(172-y<10?172-y:10),RGB(182,151,128));}
  rect(dc,model.x()+10,153,44,3,RGB(174,146,130));
  sprites_.draw(dc,model.sprite(),model.x(),94,model.mirrored());
  if(model.sleeping())text(dc,"Z",model.x()+54,93-int(model.frame())*3,Purple);
  if(model.screen()!=pet::Screen::Room){
   rect(dc,17,41,206,116,Ink);rect(dc,19,43,202,112,Cream);
   if(model.screen()==pet::Screen::Stats){
    centered(dc,"MIS NECESIDADES",48,Purple);
    const char* labels[]={"SACIEDAD","FELICIDAD","ENERGIA","HIGIENE","SALUD"};
    for(unsigned i=0;i<5;++i){int y=62+int(i)*17;auto stat=pet::Stat(i);text(dc,labels[i],27,y);std::string value=std::to_string(int(std::lround(model.stats().value(stat))));text(dc,value,207-int(value.size())*6,y);
     rect(dc,27,y+9,180,3,RGB(222,211,190));rect(dc,27,y+9,int(model.percent(stat)*1.8),3,model.percent(stat)<model.config().lowPercent?RGB(198,115,105):RGB(113,153,120));}
   }else{
    centered(dc,"MI SALUD",49,Purple);centered(dc,model.healthSummary(),67);centered(dc,"SALUD "+std::to_string(int(std::lround(model.percent(pet::Stat::Health))))+" / 100",85);
    centered(dc,"COMIDA LIMPIEZA Y SUENO",106);centered(dc,"ME AYUDAN A RECUPERARME",119);
   }
   centered(dc,"ENTER PARA VOLVER",146,Purple);
  }
  rect(dc,11,160,218,11,RGB(245,233,205));centered(dc,model.message(),162,RGB(114,91,85));
  for(unsigned i=0;i<6;++i){int x=10+int(i)*37;bool selected=i==model.selected();rect(dc,x,182,35,31,selected?Purple:RGB(231,220,196));if(selected){rect(dc,x+1,183,33,29,Purple);}icon(dc,i,x+7,189,selected?Cream:Ink);}
  centered(dc,std::string("< ")+pet::ActionNames[model.selected()]+" >",218);
  centered(dc,model.screen()!=pet::Screen::Room?"ENTER CIERRA EL PANEL":pet::ActionDescriptions[model.selected()],231,RGB(101,88,103));
 }
 void present(HDC target){SetStretchBltMode(target,COLORONCOLOR);if(!StretchBlt(target,0,0,LogicalSize*Scale,LogicalSize*Scale,surface_,0,0,LogicalSize,LogicalSize,SRCCOPY))throw std::runtime_error("Cannot present screen");}
};
}
