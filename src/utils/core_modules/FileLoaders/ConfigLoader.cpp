#include "ConfigLoader.h"

void IniDefaultJSON()
{
	namespace fs = std::filesystem;

	cachedJSON["General"]["ScanDistance"] = 5000;
	cachedJSON["General"]["SecondsPerCycle"] = 30;
	cachedJSON["General"]["SkippedCycleAmount"] = 1;
	cachedJSON["General"]["MinimumActors"] = 3;
	cachedJSON["SideFeatures"]["RangerTakeCover"] = true;
	cachedJSON["SideFeatures"]["VaguardReplaceRanger"] = true;
	cachedJSON["SideFeatures"]["Hesitation"]["Duration"] = 5;
	cachedJSON["SideFeatures"]["Hesitation"]["InstantKillTime"] = 3;

	Ranger::WriteDefaultProfileToJSON(cachedJSON);
	Leader::WriteDefaultProfileToJSON(cachedJSON);
	Caster::WriteDefaultProfileToJSON(cachedJSON);
	Vanguard::WriteDefaultProfileToJSON(cachedJSON);

	if (!fs::exists(ConfigLoader::filePath))
	{
		// Write to file
		fs::create_directories("Data/SKSE/Plugins/");
		std::ofstream file("Data/SKSE/Plugins/PuppeteerConfig.json");

		if (file.is_open()) {
			file << cachedJSON.dump(4); // pretty print with indent = 4
			file.close();
		}
	}
}

bool ConfigLoader::IfConfigChanged()
{
	std::filesystem::file_time_type currentWriteTime{};
	try {
		currentWriteTime = std::filesystem::last_write_time(filePath);
	}
	catch (...) {
		logger::error("[Puppeteer] Failed to read file modification time.");
		return false;
	}

	if (currentWriteTime != lastModifiedTime)
	{
		lastModifiedTime = currentWriteTime;
		return true;
	}


	return false;
}

json ConfigLoader::LoadConfig()
{
	IniDefaultJSON();

	// Reload JSON
	std::ifstream config(filePath);
	if (config.is_open()) {
		try {
			cachedJSON = json::parse(config);
			//lastModifiedTime = currentWriteTime;
			consoleUtils::Log("[Puppeteer] Config reloaded successfully.");
		}
		catch (const json::parse_error& e) {
			consoleUtils::Log("[Puppeteer] JSON parse error: {}", e.what());
		}
	}
	else {
		consoleUtils::Log("[Puppeteer] Failed to open config file.");
	}

	return cachedJSON;
}

template <typename T>
T GetValue(const std::string& section,
	const std::string& key,
	const T& defaultValue)
{
	if (cachedJSON.empty())
		cachedJSON = ConfigLoader::LoadConfig();

	if (!cachedJSON.contains(section))
		return defaultValue;

	if (!cachedJSON[section].contains(key))
		return defaultValue;

	if (cachedJSON[section][key].is_null())
	{
		consoleUtils::Log("[Puppeteer] Invalid setting '{}:{}' — using default {}",
			section, key, defaultValue);
		return defaultValue;
	}

	return cachedJSON[section].value(key, defaultValue);
}

int  ConfigLoader::GetScanDistance() { return GetValue<int>("General", "ScanDistance", 5000); }
int  ConfigLoader::GetSecondsPerCycle() { return GetValue<int>("General", "SecondsPerCycle", 30); }
int  ConfigLoader::GetSkipCyclesPerCycle() { return GetValue<int>("General", "SkippedCycleAmount", 1); }
int  ConfigLoader::GetMinimumActors() { return GetValue<int>("General", "MinimumActors", 3); }
bool ConfigLoader::GetEnabledLogs() { return GetValue<bool>("General", "enableLogging", true); }
int  ConfigLoader::GetGlobalConfidenceDownChance() { return GetValue<int>("SideFeatures", "GlobalConfidenceDownChance", -1); }
json ConfigLoader::GetRolesInfo() {
	if (cachedJSON.empty())
		cachedJSON = LoadConfig();

	if (!cachedJSON.contains("roles"))
		return json::object();

	return cachedJSON["roles"];
}
bool ConfigLoader::GetRangTakeCoverFeature() { return GetValue<bool>("SideFeatures", "RangerTakeCover", true); }
bool ConfigLoader::GetVangReplaceRang() { return GetValue<bool>("SideFeatures", "VanguardReplaceRanger", true); }
int ConfigLoader::GetDeathHesitationDuration() 
{ 
	if (cachedJSON.empty())
		cachedJSON = ConfigLoader::LoadConfig();

	if (!cachedJSON.contains("SideFeatures"))
		return 6; //5 seconds

	if (!cachedJSON["SideFeatures"].contains("Hesitation"))
		return 6;

	if (!cachedJSON["SideFeatures"]["Hesitation"].contains("ReactionDuration"))
		return 6;

	if (cachedJSON["SideFeatures"]["Hesitation"]["ReactionDuration"].is_null())
	{
		consoleUtils::Log("[Puppeteer] Invalid setting '{}:{}:{}' — using default {}",
			"SideFeatures", "Hesitation", "ReactionDuration", 6);
		return 6;
	}

	return cachedJSON["SideFeatures"]["Hesitation"].value("ReactionDuration", 6);
}
int ConfigLoader::GetInstantKillTime()
{
	if (cachedJSON.empty())
		cachedJSON = ConfigLoader::LoadConfig();

	if (!cachedJSON.contains("SideFeatures"))
		return 4; //5 seconds

	if (!cachedJSON["SideFeatures"].contains("Hesitation"))
		return 4;

	if (!cachedJSON["SideFeatures"]["Hesitation"].contains("InstantKillTime"))
		return 4;

	if (cachedJSON["SideFeatures"]["Hesitation"]["InstantKillTime"].is_null())
	{
		consoleUtils::Log("[Puppeteer] Invalid setting '{}:{}:{}' — using default {}",
			"SideFeatures", "Hesitation", "InstantKillTime", 4);
		return 4;
	}

	return cachedJSON["SideFeatures"]["Hesitation"].value("InstantKillTime", 4);
}

