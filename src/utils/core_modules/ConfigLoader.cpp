#include "ConfigLoader.h"

void IniDefaultJSON()
{
	//using namespace ConfigLoader;
	cachedJSON["General"]["ScanDistance"] = 5000;
	cachedJSON["General"]["SecondsPerCycle"] = 30;
	cachedJSON["General"]["SkippedCycleAmount"] = 1;
	cachedJSON["General"]["MinimumActors"] = 2;
	cachedJSON["SideFeatures"]["RangerTakeCover"] = true;
	cachedJSON["SideFeatures"]["VaguardReplaceRanger"] = true;

	Ranger::WriteDefaultProfileToJSON(cachedJSON);
	Leader::WriteDefaultProfileToJSON(cachedJSON);
	Vanguard::WriteDefaultProfileToJSON(cachedJSON);

	// Write to file
	std::filesystem::create_directories("Data/SKSE/Plugins/");
	std::ofstream file("Data/SKSE/Plugins/PuppeteerConfig.json");

	if (file.is_open()) {
		file << cachedJSON.dump(4); // pretty print with indent = 4
		file.close();
	}
}

json ConfigLoader::LoadConfigIfChanged()
{
	namespace fs = std::filesystem;
	if (!fs::exists(filePath)) {
		IniDefaultJSON();
		lastModifiedTime = fs::last_write_time(filePath);
	}

	fs::file_time_type currentWriteTime{};
	try {
		currentWriteTime = fs::last_write_time(filePath);
	}
	catch (...) {
		logger::error("[Puppeteer] Failed to read file modification time.");
		return cachedJSON;
	}

	// Reload only if changed
	if (currentWriteTime != lastModifiedTime) {
		std::ifstream config(filePath);
		if (config.is_open()) {
			try {
				cachedJSON = json::parse(config);
				lastModifiedTime = currentWriteTime;
				consoleUtils::Log("[Puppeteer] Config reloaded successfully.");
			}
			catch (const json::parse_error& e) {
				consoleUtils::Log("[Puppeteer] JSON parse error: {}", e.what());
			}
		}
		else {
			consoleUtils::Log("[Puppeteer] Failed to open config file.");
		}
	}

	return cachedJSON;
}

int ConfigLoader::GetSecondsPerCycle()
{
	if (cachedJSON.empty())
	{
		cachedJSON = LoadConfigIfChanged();
	}
	
	if (cachedJSON.contains("General"))
	{
		if (consoleUtils::TriggerOnce("INVALID_SETTING", (
			!cachedJSON["General"].contains("SecondsPerCycle") ||
			cachedJSON["General"]["SecondsPerCycle"].is_null()
		)))
			consoleUtils::Log("[Puppeteer] SecondsPerCycle invalid! Reverting to default value: 30\n\tPlease check again for typo or incorrect values!");

		return cachedJSON["General"].value("SecondsPerCycle", 30);
	}
	
	return 30;
}

int ConfigLoader::GetScanDistance()
{
	if (cachedJSON.empty())
	{
		cachedJSON = LoadConfigIfChanged();
	}

	cachedJSON = LoadConfigIfChanged();

	if (cachedJSON.contains("General"))
	{
		if (consoleUtils::TriggerOnce("INVALID_SETTING", (
			!cachedJSON["General"].contains("ScanDistance") ||
			cachedJSON["General"]["ScanDistance"].is_null()
		)))
			consoleUtils::Log("[Puppeteer] ScanDistance invalid! Reverting to default value: 5000\n\tPlease check again for typo or incorrect values!");
		
		return cachedJSON["General"].value("ScanDistance", 5000);
	}

	return 5000;
}

int ConfigLoader::GetMinimumActors()
{
	if (cachedJSON.empty())
	{
		cachedJSON = LoadConfigIfChanged();
	}

	if (cachedJSON.contains("General"))
	{
		if (consoleUtils::TriggerOnce("INVALID_SETTING", (
			!cachedJSON["General"].contains("MinimumActors") ||
			cachedJSON["General"]["MinimumActors"].is_null()
		)))
			consoleUtils::Log("[Puppeteer] MinimumActors invalid! Reverting to default value: 3\n\tPlease check again for typo or incorrect values!");
		
		return cachedJSON["General"].value("MinimumActors", 3);
	}

	return 3;
}

int ConfigLoader::GetSkipCyclesPerCycle()
{
	if (cachedJSON.empty())
	{
		cachedJSON = LoadConfigIfChanged();
	}

	if (cachedJSON.contains("General"))
	{
		if (consoleUtils::TriggerOnce("INVALID_SETTING",(
			!cachedJSON["General"].contains("SkippedCycleAmount") ||
			cachedJSON["General"]["SkippedCycleAmount"].is_null()
		)))
			consoleUtils::Log("[Puppeteer] SkippedCycleAmount invalid! Reverting to default value: 1\n\tPlease check again for typo or incorrect values!");
		
		return cachedJSON["General"].value("SkippedCycleAmount", 1);
	}

	return 1;
}

bool ConfigLoader::GetEnabledLogs()
{
	if (cachedJSON.empty())
	{
		cachedJSON = LoadConfigIfChanged();
	}

	if (cachedJSON.contains("General"))
	{
		if (consoleUtils::TriggerOnce("INVALID_SETTING", (
			!cachedJSON["General"].contains("enableLogging") ||
			cachedJSON["General"]["enableLogging"].is_null()
			)))
			consoleUtils::Log("[Puppeteer] enableLogging invalid! Reverting to default value: 0\n[Puppeteer]Please check again for typo or incorrect values!");
		
		return cachedJSON["General"].value("enableLogging", false);
	}


	return false;
}

