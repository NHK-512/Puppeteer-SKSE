extern "C" DLLEXPORT bool SKSEPlugin_Load(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);

    //SKSE::GetMessagingInterface()->RegisterListener("SKSE", SKSEMessageHandler);
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {

        if (message->type == SKSE::MessagingInterface::kDataLoaded)
        {
            RE::ConsoleLog::GetSingleton()->Print("Puppeteer plugin has successfully loaded.");
            AIManager::Initialize();
        }

        });

    return true;
}


// NPC-AI-Behavior.cpp : Defines the entry point for the application.
//
// NOTE: REQUIRES fmt.dll in the build output directory to be put in the Game directory for it to run

//void SKSEMessageHandler(SKSE::MessagingInterface::Message* msg) {
//    if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
//        RE::ConsoleLog::GetSingleton()->Print("SkyrimAIPlugin has successfully loaded through SKSE!");
//        AIManager::Initialize();
//    }
//}

//extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
//{
//    a_info->infoVersion = SKSE::PluginInfo::kVersion;
//    //a_info->name = Plugin::NAME.data();
//    //a_info->version = Plugin::VERSION[0];
//
//    //if (a_skse->IsEditor()) {
//    //    logger::critical("Loaded in editor, marking as incompatible"sv);
//    //    return false;
//    //}
//
//    const auto ver = a_skse->RuntimeVersion();
//    if (ver < SKSE::RUNTIME_SSE_1_5_39) {
//        logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
//        return false;
//    }
//
//    return true;
//}

//extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
//    SKSE::PluginVersionData v;
//    v.PluginVersion(1);
//    v.PluginName("SkyrimAIPlugin");
//
//    //v.Author("YourName");
//    v.UsesAddressLibrary(true);
//    v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });
//    v.HasNoStructUse(true);
//
//    return v;
//}();



