#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shellapi.h>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <memory>
#include "renderer.hpp"
#include "file_store.hpp"
#include "form_resources.hpp"
#include "profiles.hpp"
namespace {
bool runtimeFailed=false;
std::unique_ptr<pet::Model> model;
std::unique_ptr<desktop::Renderer> renderer;
std::unique_ptr<pet::IStorage> storage;
std::unique_ptr<desktop::Profiles> profiles;std::filesystem::path currentSave;
bool rememberProfile=true;
unsigned autoSaveMs=0,pendingSaveMs=0;
bool pendingSave=false;
int64_t wallTime(){return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
void save(){if(!model->hasPet()||model->screen()==pet::Screen::Start)return;storage->save(model->snapshot(wallTime()),model->config());if(rememberProfile)profiles->remember(currentSave);autoSaveMs=0;pendingSave=false;}
void requestSave(){pendingSave=true;pendingSaveMs=1000;}
void handleRequests(){
 if(model->takeNewPetRequest()){auto bak=currentSave;bak+=L".bak";if(std::filesystem::exists(currentSave)||std::filesystem::exists(bak))currentSave=profiles->next();storage=std::make_unique<desktop::FileStore>(currentSave);save();}
 if(model->takeEvolutionEvent()){requestSave();pendingSaveMs=0;}
 if(model->takeEvolutionSaveRequest()){requestSave();pendingSaveMs=0;}
}
using Clock=std::chrono::steady_clock;auto previous=Clock::now();
void error(HWND hwnd,const std::exception& e){MessageBoxA(hwnd,e.what(),"Mascota virtual - error",MB_OK|MB_ICONERROR);}
void input(pet::Button b,HWND hwnd){model->press(b);if(b==pet::Button::Accept)requestSave();handleRequests();InvalidateRect(hwnd,nullptr,FALSE);}
LRESULT CALLBACK procedure(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 try{switch(msg){
 case WM_CREATE:previous=Clock::now();if(!SetTimer(hwnd,1,16,nullptr))return -1;return 0;
 case WM_TIMER:{auto now=Clock::now();auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(now-previous).count();previous=now;unsigned elapsed=unsigned(std::clamp<long long>(ms,0,1000));model->tick(elapsed);handleRequests();if(model->hasPet()&&model->screen()!=pet::Screen::Start)autoSaveMs+=elapsed;
  if(pendingSave)pendingSaveMs=pendingSaveMs<=elapsed?0:pendingSaveMs-elapsed;
  if(autoSaveMs>=30000||(pendingSave&&pendingSaveMs==0)){try{save();}catch(const std::exception&){model->notify("NO SE PUDO GUARDAR");autoSaveMs=0;pendingSave=false;}}
  InvalidateRect(hwnd,nullptr,FALSE);return 0;}
 case WM_KEYDOWN:if(lp&(1LL<<30))return 0;if(wp==VK_LEFT)input(pet::Button::Left,hwnd);if(wp==VK_RIGHT)input(pet::Button::Right,hwnd);if(wp==VK_RETURN)input(pet::Button::Accept,hwnd);return 0;
 case WM_LBUTTONDOWN:{int x=LOWORD(lp)/desktop::Scale,y=HIWORD(lp)/desktop::Scale;if(model->screen()==pet::Screen::Start||model->screen()==pet::Screen::Species||model->screen()==pet::Screen::Name){if(y>=180&&y<235)input(x<80?pet::Button::Left:x>=160?pet::Button::Right:pet::Button::Accept,hwnd);}else if(y>=182&&y<213&&x>=10&&x<230){unsigned index=unsigned((x-10)/37);if(index<6){model->activate(index);requestSave();handleRequests();InvalidateRect(hwnd,nullptr,FALSE);}}SetFocus(hwnd);return 0;}
 case WM_ERASEBKGND:return 1;
 case WM_PAINT:{PAINTSTRUCT ps;auto dc=BeginPaint(hwnd,&ps);try{renderer->render(*model);renderer->present(dc);}catch(...){EndPaint(hwnd,&ps);throw;}EndPaint(hwnd,&ps);return 0;}
 case WM_CLOSE:try{handleRequests();save();DestroyWindow(hwnd);}catch(const std::exception& e){error(hwnd,e);}return 0;
 case WM_DESTROY:KillTimer(hwnd,1);PostQuitMessage(0);return 0;
 }}catch(const std::exception& e){runtimeFailed=true;KillTimer(hwnd,1);error(hwnd,e);DestroyWindow(hwnd);return 0;}
 return DefWindowProcW(hwnd,msg,wp,lp);
}
std::filesystem::path defaultRoot(){wchar_t name[32768];DWORD count=GetModuleFileNameW(nullptr,name,32768);if(!count||count==32768)throw std::runtime_error("Cannot locate executable");auto path=std::filesystem::path(name).parent_path();for(int i=0;i<5;++i){if(std::filesystem::exists(path/L"config/gameplay.cfg"))return path;path=path.parent_path();}throw std::runtime_error("Cannot locate assets; pass --assets-root <project directory>");}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int show){
 HRESULT com=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);if(FAILED(com)){MessageBoxW(nullptr,L"No se pudo iniciar el lector de imagenes.",L"Error",MB_OK);return 1;}
 int code=0;
 try{
  std::filesystem::path root,savePath,manifest,evolutionConfig=L"config/evolution.cfg";bool forceNew=false;
  int argc=0;auto argv=CommandLineToArgvW(GetCommandLineW(),&argc);if(!argv)throw std::runtime_error("Cannot read arguments");
  try{for(int i=1;i<argc;++i){std::wstring option=argv[i];if(option==L"--new-game"){forceNew=true;}else if((option==L"--assets-root"||option==L"--animation"||option==L"--save"||option==L"--evolution-config")&&i+1<argc){if(option==L"--assets-root")root=argv[++i];else if(option==L"--save")savePath=argv[++i];else if(option==L"--evolution-config")evolutionConfig=argv[++i];else manifest=argv[++i];}else throw std::runtime_error("Usage: virtual_pet.exe [--assets-root directory] [--animation manifest] [--save save.json] [--new-game] [--evolution-config config]");}}catch(...){LocalFree(argv);throw;}LocalFree(argv);
  if(root.empty())root=defaultRoot();
  std::ifstream cfg(root/L"config/gameplay.cfg");if(!cfg)throw std::runtime_error("Cannot open config/gameplay.cfg");auto config=pet::Config::load(cfg);
  std::ifstream evolutionFile(root/evolutionConfig);if(!evolutionFile)throw std::runtime_error("Cannot open evolution config");auto evolution=pet::evolution::Settings::load(evolutionFile);
  desktop::SpriteStore sprites(root);auto resources=desktop::FormResources::load(root,sprites,manifest);
  std::error_code logError;std::filesystem::create_directories(root/L"build",logError);std::ofstream log(root/L"build/missing-assets.log");for(const auto& message:resources.missing)log<<message<<'\n';
  model=std::make_unique<pet::Model>(std::move(resources.library),config,evolution);
  rememberProfile=savePath.empty();profiles=std::make_unique<desktop::Profiles>(rememberProfile?root/L"saves":std::filesystem::absolute(savePath).parent_path());
  currentSave=rememberProfile?profiles->active():savePath;storage=std::make_unique<desktop::FileStore>(currentSave);
  if(forceNew)model->beginSetup();else{auto restored=storage->load(config);if(restored.snapshot){model->restore(*restored.snapshot);auto now=wallTime();if(restored.snapshot->savedAt>0&&now>restored.snapshot->savedAt)model->recordOffline(uint64_t((std::min)(now-restored.snapshot->savedAt,int64_t(31536000)))*1000);model->showStartMenu();if(restored.recovered)model->notify("GUARDADO RECUPERADO");}else model->beginSetup();}
  renderer=std::make_unique<desktop::Renderer>(sprites);
  SetProcessDPIAware();WNDCLASSW wc{};wc.lpfnWndProc=procedure;wc.hInstance=instance;wc.lpszClassName=L"VirtualPetEvolution4";wc.hCursor=LoadCursor(nullptr,IDC_ARROW);if(!RegisterClassW(&wc))throw std::runtime_error("Cannot register window");
  RECT r{0,0,desktop::LogicalSize*desktop::Scale,desktop::LogicalSize*desktop::Scale};DWORD style=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;AdjustWindowRect(&r,style,FALSE);
  auto hwnd=CreateWindowW(wc.lpszClassName,L"Mascota virtual - Mision 4 (sprites provisionales)",style,CW_USEDEFAULT,CW_USEDEFAULT,r.right-r.left,r.bottom-r.top,nullptr,nullptr,instance,nullptr);if(!hwnd)throw std::runtime_error("Cannot create simulator window");ShowWindow(hwnd,show);
  MSG msg{};BOOL status;while((status=GetMessageW(&msg,nullptr,0,0))>0){TranslateMessage(&msg);DispatchMessageW(&msg);}if(status<0||runtimeFailed)code=1;
  renderer.reset();model.reset();storage.reset();
 }catch(const std::exception& e){renderer.reset();model.reset();storage.reset();error(nullptr,e);code=1;}
 CoUninitialize();return code;
}
