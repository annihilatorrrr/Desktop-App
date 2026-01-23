#pragma once
#include <string>

// ClearWiFiHistory - Comprehensive WiFi history cleanup utility for Linux
//
// PURPOSE:
// This class removes all traces of WiFi network history to protect user privacy and location data.
// WiFi network data (SSIDs, MAC addresses, connection timestamps) can be correlated with public
// databases like wigle.net to track user movement history and physical locations over time.
//
// WHAT IS CLEARED:
// 1. NetworkManager WiFi Connections - Saved WiFi profiles in /etc/NetworkManager/system-connections/
// 2. wpa_supplicant Configuration - WiFi network configurations in /etc/wpa_supplicant/
// 3. iwd (iNet Wireless Daemon) - Network profiles in /var/lib/iwd/
// 4. NetworkManager State Files - Recent connection data and timestamps
//
// All operations preserve the currently connected network if detected.

class ClearWiFiHistory
{
public:
    static bool clear();

private:
    static std::string getCurrentConnectedSSID();
    static bool clearNetworkManagerConnections(const std::string &currentSSID);
    static bool clearNetworkManagerState();
    static bool clearWpaSupplicantConfig(const std::string &currentSSID);
    static bool clearIwdProfiles(const std::string &currentSSID);
    static bool clearWiFiJournalLogs();
    
    // ========== Helper Methods ==========

    // Safely delete a file
    // Returns: true if file was deleted or didn't exist, false on error
    static bool deleteFileSafely(const std::string &filePath);
        
    // Check if a file contains WiFi/wireless configuration
    // Used to identify wireless network files
    static bool isWirelessConfigFile(const std::string &filePath);
    
    // Check if a file is a wpa_supplicant configuration file by content
    // Validates by looking for wpa_supplicant-specific directives
    // Parameters:
    //   filePath - Path to file to check
    // Returns: true if file contains wpa_supplicant configuration
    static bool isWpaSupplicantConfigFile(const std::string &filePath);
    
    // Reload NetworkManager to apply changes
    static bool reloadNetworkManager();
};
