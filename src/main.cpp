//#include "PCH.h"

//struct PlayerCharacter_Update {
//    static void thunk(RE::PlayerCharacter* player, float delta) {
//        func(player, delta);
//        AIManager::Initialize();
//    }
//
//    static inline REL::Relocation<decltype(thunk)> func;    
//    static inline auto idx = 0xAD;
//};

extern "C" DLLEXPORT bool SKSEPlugin_Load(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);

    //SKSE::GetMessagingInterface()->RegisterListener("SKSE", SKSEMessageHandler);
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {

        if (message->type == SKSE::MessagingInterface::kDataLoaded)
        {
            //RE::ConsoleLog::GetSingleton()->Print("Puppeteer plugin has successfully loaded.");
            AIManager::Initialize();
            //MainLoop::Hook::Install();
        }

        });

    return true;
}