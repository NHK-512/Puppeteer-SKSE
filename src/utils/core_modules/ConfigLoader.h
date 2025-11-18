#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())
using namespace nlohmann;

static std::filesystem::file_time_type lastModifiedTime = std::filesystem::file_time_type::min();
static json cachedJSON = json::object();

namespace ConfigLoader
{
	const std::string filePath = "Data/SKSE/Plugins/PuppeteerConfig.json";

	//Gets general data from config JSON in JSON format
	json LoadConfigIfChanged();

	int GetSecondsPerCycle();

	int GetScanDistance();

	int GetMinimumActors();

	int GetSkipCyclesPerCycle();
}