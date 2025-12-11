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

	Ranger::WriteDefaultProfileToJSON(cachedJSON);
	Leader::WriteDefaultProfileToJSON(cachedJSON);
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
json ConfigLoader::GetStyleMults() {
	if (cachedJSON.empty())
		cachedJSON = LoadConfig();

	if (!cachedJSON.contains("roles"))
		return json::object();

	return cachedJSON["roles"];
}