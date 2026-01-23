#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>
#import <Security/Security.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include "clear_wifi_history.h"
#include "../utils.h"

bool ClearWiFiHistory::clear()
{
    spdlog::info("Starting WiFi history cleanup for macOS");
    
    bool success = true;
    
    // Get current connected network before clearing
    std::string currentSSID = getCurrentConnectedSSID();
    if (!currentSSID.empty()) {
        spdlog::debug("Current connected WiFi network: {}", currentSSID);
    } else {
        spdlog::debug("No WiFi network currently connected");
    }
    
    // Remove all preferred networks (except current)
    // Note: This does NOT disconnect from current network
    success &= removeAllPreferredNetworks(currentSSID);

    // Clear password from keychain (except current)
    success &= clearWiFiPasswordsFromKeychain(currentSSID);
    
    // Clear logs and diagnostics
    success &= clearWiFiDiagnosticLogs();
    
    if (success) {
        spdlog::info("WiFi history cleanup completed successfully");
    } else {
        spdlog::warn("WiFi history cleanup completed with some failures");
    }
    
    return success;
}

std::string ClearWiFiHistory::getCurrentConnectedSSID()
{
    std::string currentSSID;
    
    // First, get the WiFi interface name
    std::string wifiInterface;
    int ifaceResult = Utils::executeCommand("sh",
        {"-c", "networksetup -listallhardwareports | awk '/Wi-Fi|AirPort/{getline; print $NF}'"},
        &wifiInterface);
    
    if (ifaceResult != 0 || wifiInterface.empty()) {
        spdlog::warn("Failed to determine WiFi interface");
        return currentSSID;
    }
    
    // Trim whitespace from interface name
    wifiInterface.erase(wifiInterface.find_last_not_of(" \n\r\t") + 1);
    
    if (wifiInterface.empty()) {
        spdlog::warn("No WiFi interface found");
        return currentSSID;
    }
    
    // Get SSID using ipconfig getsummary
    std::string ipconfigOutput;
    std::vector<std::string> args = {"getsummary", wifiInterface};
    int ipconfigResult = Utils::executeCommand("ipconfig", args, &ipconfigOutput);
    
    if (ipconfigResult == 0 && !ipconfigOutput.empty()) {
        // Look for "SSID : NetworkName" in the output
        size_t ssidPos = ipconfigOutput.find(" SSID : ");
        if (ssidPos != std::string::npos) {
            ssidPos += 8; // skip " SSID : "
            size_t endPos = ipconfigOutput.find("\n", ssidPos);
            if (endPos != std::string::npos) {
                currentSSID = ipconfigOutput.substr(ssidPos, endPos - ssidPos);
                // Trim whitespace
                currentSSID.erase(currentSSID.find_last_not_of(" \n\r\t") + 1);
            }
        }
    }
    
    return currentSSID;
}

bool ClearWiFiHistory::removeAllPreferredNetworks(const std::string &currentSSID)
{
    spdlog::debug("Removing all preferred wireless networks except current");
    
    auto interfaces = getWiFiInterfaces();
    bool success = true;
    
    if (interfaces.empty()) {
        spdlog::warn("No WiFi interfaces found");
        return true; // Not a failure - just no WiFi hardware
    }
    
    for (const auto &iface : interfaces) {
        spdlog::debug("Clearing networks for interface: {}", iface);
        
        // Get list of all preferred networks
        std::string networksOutput;
        int listResult = Utils::executeCommand("networksetup",
            {"-listpreferredwirelessnetworks", iface}, &networksOutput);
        
        if (listResult != 0) {
            spdlog::warn("Failed to list preferred networks for interface: {}", iface);
            continue;
        }
        
        // Parse network list and remove each network except current
        // Format: "Preferred networks on en0:\n\tNetworkName1\n\tNetworkName2"
        std::istringstream stream(networksOutput);
        std::string line;
        int removedCount = 0;
        int skippedCount = 0;
        
        while (std::getline(stream, line)) {
            // Skip header line
            if (line.find("Preferred networks on") != std::string::npos) {
                continue;
            }
            
            // Network names are prefixed with tab
            if (line.empty() || line[0] != '\t') {
                continue;
            }
            
            // Extract network name (remove leading tab)
            std::string networkName = line.substr(1);
            // Trim trailing whitespace
            networkName.erase(networkName.find_last_not_of(" \n\r\t") + 1);
            
            if (networkName.empty()) {
                continue;
            }
            
            // Skip if this is the current connected network
            if (!currentSSID.empty() && networkName == currentSSID) {
                spdlog::debug("Skipping current connected network: {}", networkName);
                skippedCount++;
                continue;
            }
            
            // Remove this network
            spdlog::info("Removing network: {}", networkName);
            int removeResult = Utils::executeCommand("networksetup",
                {"-removepreferredwirelessnetwork", iface, networkName});
            
            if (removeResult == 0) {
                removedCount++;
                spdlog::debug("Successfully removed network: {}", networkName);
            } else {
                spdlog::warn("Failed to remove network: {}", networkName);
                success = false;
            }
        }
        
        spdlog::debug("Removed {} network(s), kept {} network(s) for interface: {}", 
                     removedCount, skippedCount, iface);
    }
    
    return success;
}

std::vector<std::string> ClearWiFiHistory::getWiFiInterfaces()
{
    std::vector<std::string> interfaces;
    std::string output;
    
    // Get list of network services
    int result = Utils::executeCommand("networksetup", {"-listallhardwareports"}, &output);
    
    if (result != 0) {
        spdlog::warn("Failed to list hardware ports");
        // Fallback: try common WiFi interfaces
        interfaces.push_back("en0");
        return interfaces;
    }
    
    // Parse output to find WiFi interfaces
    // Format: "Hardware Port: Wi-Fi\nDevice: en0"
    size_t pos = 0;
    while ((pos = output.find("Hardware Port: Wi-Fi", pos)) != std::string::npos) {
        size_t devicePos = output.find("Device: ", pos);
        if (devicePos != std::string::npos) {
            devicePos += 8; // strlen("Device: ")
            size_t endPos = output.find("\n", devicePos);
            if (endPos != std::string::npos) {
                std::string iface = output.substr(devicePos, endPos - devicePos);
                // Trim whitespace
                iface.erase(iface.find_last_not_of(" \n\r\t") + 1);
                if (!iface.empty()) {
                    interfaces.push_back(iface);
                    spdlog::debug("Found WiFi interface: {}", iface);
                }
            }
        }
        pos++;
    }
    
    // Fallback: if no interfaces found, try common ones
    if (interfaces.empty()) {
        spdlog::debug("No WiFi interfaces found in networksetup output, trying en0");
        interfaces.push_back("en0");
    }
    
    return interfaces;
}

bool ClearWiFiHistory::clearWiFiPasswordsFromKeychain(const std::string &currentSSID)
{
    spdlog::debug("Clearing WiFi passwords from keychain except current");
    
    // Get list of all WiFi network SSIDs from all keychains
    // Note: WiFi passwords are typically stored in System.keychain, but we search all keychains
    std::string dumpOutput;
    int dumpResult = Utils::executeCommand("sh",
        {"-c", "security dump-keychain 2>&1 | "
               "grep -B 5 'AirPort network password' | "
               "grep '\"acct\"' | "
               "sed 's/.*\"acct\"<blob>=\"\\(.*\\)\"/\\1/' | "
               "sort -u"},
        &dumpOutput);
    
    if (dumpResult != 0) {
        spdlog::warn("Failed to list WiFi passwords from keychain");
        return false;
    }
    
    if (dumpOutput.empty()) {
        spdlog::debug("No WiFi passwords found in keychain");
        return true;
    }
    
    // Parse SSIDs from output (one per line)
    std::istringstream stream(dumpOutput);
    std::string ssid;
    int deletedCount = 0;
    int skippedCount = 0;
    
    while (std::getline(stream, ssid)) {
        // Trim whitespace
        ssid.erase(ssid.find_last_not_of(" \n\r\t") + 1);
        size_t firstNonSpace = ssid.find_first_not_of(" \t");
        if (firstNonSpace != std::string::npos) {
            ssid = ssid.substr(firstNonSpace);
        }
        
        if (ssid.empty()) {
            continue;
        }
        
        // Skip if this is the current connected network
        if (!currentSSID.empty() && ssid == currentSSID) {
            spdlog::debug("Skipping password for current network: {}", ssid);
            skippedCount++;
            continue;
        }
        
        // Delete this password from all keychains
        // By not specifying a keychain, security will search in all available keychains
        int result = Utils::executeCommand("security",
            {"delete-generic-password",
             "-D", "AirPort network password",
             "-a", ssid},
            nullptr);
        
        if (result == 0) {
            deletedCount++;
            spdlog::debug("Successfully deleted password for: {}", ssid);
        } else {
            spdlog::warn("Failed to delete password for: {}", ssid);
        }
    }
    
    spdlog::debug("Deleted {} WiFi password(s), kept {} password(s)", deletedCount, skippedCount);
    
    return true;
}

bool ClearWiFiHistory::clearWiFiDiagnosticLogs()
{
    spdlog::debug("Clearing WiFi diagnostic logs");
    
    bool success = true;
    
    // Clear various log locations using shell for wildcard expansion
    std::vector<std::string> logCommands = {
        "rm -rf /var/log/wifi.log* 2>/dev/null || true",
        "rm -rf /Library/Logs/DiagnosticReports/WiFi* 2>/dev/null || true",
        "rm -rf /private/var/log/wifi-* 2>/dev/null || true"
    };
    
    for (const auto &cmd : logCommands) {
        int result = Utils::executeCommand("sh", {"-c", cmd});
        if (result == 0) {
            spdlog::debug("Cleared log path with command: {}", cmd);
        } else {
            spdlog::debug("Command completed (files may not exist): {}", cmd);
        }
    }
    
    return success;
}
