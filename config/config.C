
#include "TROOT.h"
#include "TApplication.h"
#include "TGClient.h"
#include "GreenMonster.h"

extern void InitGui();
VoidFuncPtr_t initfuncs[] = {InitGui, 0};
// VoidFuncPtr_t initfuncs[] = {0};
TROOT root("GUI", "GUI test environment", initfuncs);
int main(int argc, char **argv)
{
  TApplication theApp("App", &argc, argv);
  GreenMonster configWin(gClient->GetRoot(), 800, 600);
  configWin.Init();

  theApp.Run();
  return 0;
}
