#pragma once
#include <windows.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>
#include "../../core/animation/library.hpp"
namespace desktop {
using Microsoft::WRL::ComPtr;
inline void require(HRESULT hr,const char* message){if(FAILED(hr))throw std::runtime_error(message);}
struct Bitmap {
 HBITMAP normal=nullptr,mirror=nullptr;int width=0,height=0;
 ~Bitmap(){if(normal)DeleteObject(normal);if(mirror)DeleteObject(mirror);}
};
class SpriteStore {
 std::filesystem::path root_;ComPtr<IWICImagingFactory> factory_;
 std::map<std::string,std::unique_ptr<Bitmap>> images_;
 static HBITMAP dib(int w,int h,void** pixels){BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=w;info.bmiHeader.biHeight=-h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;auto image=CreateDIBSection(nullptr,&info,DIB_RGB_COLORS,pixels,nullptr,0);if(!image)throw std::runtime_error("Cannot allocate sprite bitmap");return image;}
 Bitmap& load(const std::string& filename){
  auto found=images_.find(filename);if(found!=images_.end())return *found->second;
  auto path=std::filesystem::u8path(filename);if(path.is_absolute())throw std::runtime_error("Sprite paths must be relative to assets root");for(const auto& part:path)if(part=="..")throw std::runtime_error("Sprite path escapes assets root");
  ComPtr<IWICBitmapDecoder> decoder;require(factory_->CreateDecoderFromFilename((root_/path).c_str(),nullptr,GENERIC_READ,WICDecodeMetadataCacheOnLoad,&decoder),"Cannot load sprite image (check configured root and manifest)");
  ComPtr<IWICBitmapFrameDecode> frame;require(decoder->GetFrame(0,&frame),"Cannot decode sprite frame");UINT w=0,h=0;require(frame->GetSize(&w,&h),"Cannot read image size");if(!w||!h||w>4096||h>4096||w%64||h%64)throw std::runtime_error("Sprite sheet dimensions must be multiples of 64, max 4096");
  ComPtr<IWICFormatConverter> convert;require(factory_->CreateFormatConverter(&convert),"Cannot create PNG converter");require(convert->Initialize(frame.Get(),GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone,nullptr,0,WICBitmapPaletteTypeCustom),"Cannot convert sprite alpha");
  std::vector<unsigned char> pixels(size_t(w)*h*4);require(convert->CopyPixels(nullptr,w*4,UINT(pixels.size()),pixels.data()),"Cannot read sprite pixels");
  bool transparent=false,visible=false;for(size_t i=3;i<pixels.size();i+=4){transparent|=pixels[i]<255;visible|=pixels[i]>0;}if(!transparent||!visible)throw std::runtime_error("Sprite must contain transparency and visible pixels");
  auto image=std::make_unique<Bitmap>();image->width=int(w);image->height=int(h);void* data=nullptr;image->normal=dib(int(w),int(h),&data);CopyMemory(data,pixels.data(),pixels.size());image->mirror=dib(int(w),int(h),&data);
  auto dest=static_cast<unsigned char*>(data);for(UINT y=0;y<h;++y)for(UINT x=0;x<w;++x)CopyMemory(dest+(size_t(y)*w+x)*4,pixels.data()+(size_t(y)*w+w-1-x)*4,4);
  auto& result=*image;images_.emplace(filename,std::move(image));return result;
 }
public:
 explicit SpriteStore(std::filesystem::path root):root_(std::move(root)){require(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&factory_)),"Cannot initialize Windows image decoder");}
 void preload(const pet::Library& library){for(const auto& clip:library.all())for(const auto& f:clip.frames){auto& image=load(f.image);if(f.x>image.width-f.width||f.y>image.height-f.height)throw std::runtime_error("Frame outside sprite sheet");}}
 void draw(HDC target,const pet::SpriteFrame& f,int x,int y,bool mirror){auto& image=load(f.image);auto dc=CreateCompatibleDC(target);if(!dc)throw std::runtime_error("Cannot create sprite DC");auto previous=SelectObject(dc,mirror?image.mirror:image.normal);BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};BOOL ok=AlphaBlend(target,x+(mirror?-f.dx:f.dx),y+f.dy,64,64,dc,mirror?image.width-f.x-64:f.x,f.y,64,64,blend);SelectObject(dc,previous);DeleteDC(dc);if(!ok)throw std::runtime_error("Cannot blend sprite");}
};
}
