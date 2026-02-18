#pragma once

#include <Windows.h>
#include <string>

class WireGuardController final
{
public:
    static WireGuardController &instance()
    {
        static WireGuardController wc;
        return wc;
    }

    bool installService(bool isAmneziaWG);
    bool configure(const std::wstring &config);
    bool deleteService();

    UINT getStatus(ULONG64 &lastHandshake, ULONG64 &txBytes, ULONG64 &rxBytes) const;

private:
    bool isInitialized_ = false;
    bool isAmneziaWG_ = false;
    std::wstring exeName_;

private:
    explicit WireGuardController();
    std::wstring configFile() const;

    void getKernelDriverStatus(ULONG64& lastHandshake, ULONG64& txBytes, ULONG64& rxBytes) const;
    HANDLE getKernelInterfaceHandle() const;

    void getAmneziaWGStatus(ULONG64& lastHandshake, ULONG64& txBytes, ULONG64& rxBytes) const;
};
