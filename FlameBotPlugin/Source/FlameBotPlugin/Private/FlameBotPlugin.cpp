#include "Modules/ModuleManager.h"
#include "FlameBot_API.h"

class FFlameBotPluginModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        FlameBot_Init();
        UE_LOG(LogTemp, Log, TEXT("FlameBot: Motor baslatildi."));
    }

    virtual void ShutdownModule() override
    {
        FlameBot_Destroy();
        UE_LOG(LogTemp, Log, TEXT("FlameBot: Motor kapatildi."));
    }
};

IMPLEMENT_MODULE(FFlameBotPluginModule, FlameBotPlugin)