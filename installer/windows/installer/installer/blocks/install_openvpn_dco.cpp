#include "install_openvpn_dco.h"

#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QString>

#include "../installer_base.h"
#include "../installer_utils.h"
#include "../settings.h"
#include "../../../utils/applicationinfo.h"
#include "../../../utils/logger.h"

#include "types/global_consts.h"

using namespace std;

InstallOpenVPNDCO::InstallOpenVPNDCO(double weight) : IInstallBlock(weight, L"openvpndco", false)
{
}

int InstallOpenVPNDCO::executeStep()
{
    DWORD buildNum = InstallerUtils::getOSBuildNumber();
    if (buildNum < kMinWindowsBuildNumberForOpenVPNDCO) {
        Log::instance().out(
            L"WARNING: OS version is not compatible with the OpenVPN DCO driver.  Windows 10 build %lu or newer is required"
            L" to use this driver.", kMinWindowsBuildNumberForOpenVPNDCO);
        return -ERROR_OTHER;
    }

    const QString installPath = QString::fromStdWString(Settings::instance().getPath());
    const QString program = QString("\"%1\\devcon.exe\"").arg(installPath);
    const QString infFile = QString("openvpndco\\win%2\\ovpn-dco.inf").arg(buildNum >= kWindows11BuildNumber ? 11 : 10);

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.setWorkingDirectory(installPath);
    process.start(program, QStringList() << "dp_add" << infFile);
    process.waitForFinished();
    QByteArray appOutput = process.readAll();

    if (process.exitCode() != 0) {
        Log::instance().out(L"InstallOpenVPNDCO: devcon.exe returned exit code %d", process.exitCode());
        Log::instance().out(L"InstallOpenVPNDCO: devcon.exe output (%hs)", appOutput.constData());
        return -ERROR_OTHER;
    }

    // Parse the OEM identifier from the output and store it for use during uninstall.
    QRegularExpression re("oem\\d+.inf");
    const QRegularExpressionMatch match = re.match(appOutput);
    if (!match.hasMatch()) {
        Log::instance().out(L"InstallOpenVPNDCO: failed to find OEM indentifier in devcon.exe output (%hs)", appOutput.constData());
        return -ERROR_OTHER;
    }

    QString adapterOEMIdentifier = match.captured(0);
    QSettings reg(QString::fromStdWString(ApplicationInfo::installerRegistryKey()), QSettings::NativeFormat);
    reg.setValue("ovpnDCODriverOEMIdentifier", adapterOEMIdentifier);

    Log::instance().out(L"OpenVPN DCO driver (%s) successfully added to the Windows driver store", adapterOEMIdentifier.toStdWString().c_str());

    return 100;
}
