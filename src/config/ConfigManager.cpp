#define DEBUG true

#include "ConfigManager.h"
#include "ConfigBuilder.h"

void ConfigManager::loadConfig() {
    ConfigManager::config = ConfigBuilder()
            .setIsDebug(DEBUG)
            .setImageCacheSizeBytes(1024 * 1024 * 100)
            .build();
}

Config* ConfigManager::getOrLoadConfig() {
    if (!ConfigManager::configLoaded) {
        loadConfig();
    }

    return ConfigManager::config;
}

ConfigManager::ConfigManager() {
    ConfigManager::configLoaded = false;
}
