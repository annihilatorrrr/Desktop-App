#pragma once

#include <Windows.h>
#include <assert.h>

#include "FontResources.h"
#include "ImageResources.h"
#include "MainWindow.h"

#include "../installer/installer_base.h"

class Application
{
public:
    Application(HINSTANCE hInstance, int nCmdShow, bool isAutoUpdateMode,
        bool isSilent, bool noDrivers, bool noAutoStart, bool isFactoryReset,
        const std::wstring& installPath);
    virtual ~Application();

    bool init(int windowCenterX, int windowCenterY);
    int exec();

    HINSTANCE getInstance() { return hInstance_; }
    int getCmdShow() { return (isSilent_ ? SW_HIDE : nCmdShow_); }

    ImageResources* getImageResources() { assert(imageResources_ != NULL); return imageResources_; }
    FontResources* getFontResources() { assert(fontResources_ != NULL); return fontResources_; }

    InstallerBase* getInstaller() { return installer_.get(); }

    std::wstring getPreviousInstallPath();

	std::wstring getPreviousInstallPath();

private:
    ULONG_PTR gdiplusToken_;
    HINSTANCE hInstance_;
    const int nCmdShow_;
    const bool isAutoUpdateMode_;
    const bool isSilent_;

    MainWindow* mainWindow_;
    ImageResources* imageResources_;
    FontResources* fontResources_;
    std::unique_ptr<InstallerBase> installer_;

    void installerCallback(unsigned int progress, INSTALLER_CURRENT_STATE state);
};

extern Application* g_application;