#include "wireguardcustomconfig.h"
#include "utils/log/categories.h"
#include "utils/log/clean_sensitive_info.h"

#include <QFileInfo>
#include <QSettings>

namespace customconfigs {

CUSTOM_CONFIG_TYPE WireguardCustomConfig::type() const
{
    return CUSTOM_CONFIG_WIREGUARD;
}

QString WireguardCustomConfig::name() const
{
    return name_;
}

QString WireguardCustomConfig::nick() const
{
    return nick_;
}

QString WireguardCustomConfig::filename() const
{
    return filename_;
}

QStringList WireguardCustomConfig::hostnames() const
{
    QStringList list;
    if (!endpointHostname_.isEmpty())
        list << endpointHostname_;
    return list;
}

bool WireguardCustomConfig::isAllowFirewallAfterConnection() const
{
    return isAllowFirewallAfterConnection_;
}

bool WireguardCustomConfig::isCorrect() const
{
    return errMessage_.isEmpty();
}

QString WireguardCustomConfig::getErrorForIncorrect() const
{
    return errMessage_;
}

QSharedPointer<WireGuardConfig> WireguardCustomConfig::getWireGuardConfig(const QString &endpointIp) const
{
    auto *config = new WireGuardConfig(privateKey_, ipAddress_, dnsAddress_, publicKey_,
                                       presharedKey_, endpointIp + endpointPort_, allowedIps_);
    if (obfuscationParams_.isValid()) {
        config->setAmneziawgParam(obfuscationParams_);
    }
    return QSharedPointer<WireGuardConfig>(config);
}

// static
ICustomConfig *WireguardCustomConfig::makeFromFile(const QString &filepath)
{
    WireguardCustomConfig *config = new WireguardCustomConfig();
    QFileInfo fi(filepath);
    config->name_ = fi.completeBaseName();
    config->filename_ = fi.fileName();
    config->loadFromFile(filepath);  // here the config can change to incorrect
    config->validate();
    return config;
}

void WireguardCustomConfig::loadFromFile(const QString &filepath)
{
    QSettings file(filepath, QSettings::IniFormat);
    switch (file.status()) {
    default:
    case QSettings::AccessError:
        qDebug(LOG_CUSTOM_OVPN) << "Failed to open file" << Utils::cleanSensitiveInfo(filepath);
        errMessage_ = QObject::tr("Failed to open file");
        return;
    case QSettings::FormatError:
        qDebug(LOG_CUSTOM_OVPN) << "Failed to parse file" << Utils::cleanSensitiveInfo(filepath);
        errMessage_ = QObject::tr("Invalid config format");
        return;
    case QSettings::NoError:
        // File exists and format is correct.
        break;
    }

    auto groups = file.childGroups();
    if (groups.indexOf("Interface") < 0) {
        errMessage_ = QObject::tr("Missing \"Interface\" section");
        return;
    }
    file.beginGroup("Interface");
    privateKey_ = file.value("PrivateKey").toString();
    ipAddress_ = WireGuardConfig::stripIpv6Address(file.value("Address").toStringList());
    dnsAddress_ = WireGuardConfig::stripIpv6Address(file.value("DNS").toStringList());

    obfuscationParams_ = api_responses::AmneziawgUnblockParam();
    if (file.contains("Jc"))
        obfuscationParams_.jc = file.value("Jc").toInt();
    if (file.contains("Jmin"))
        obfuscationParams_.jmin = file.value("Jmin").toInt();
    if (file.contains("Jmax"))
        obfuscationParams_.jmax = file.value("Jmax").toInt();
    if (file.contains("S1"))
        obfuscationParams_.s1 = file.value("S1").toInt();
    if (file.contains("S2"))
        obfuscationParams_.s2 = file.value("S2").toInt();
    if (file.contains("S3"))
        obfuscationParams_.s3 = file.value("S3").toInt();
    if (file.contains("S4"))
        obfuscationParams_.s4 = file.value("S4").toInt();
    if (file.contains("H1"))
        obfuscationParams_.h1 = file.value("H1").toString();
    if (file.contains("H2"))
        obfuscationParams_.h2 = file.value("H2").toString();
    if (file.contains("H3"))
        obfuscationParams_.h3 = file.value("H3").toString();
    if (file.contains("H4"))
        obfuscationParams_.h4 = file.value("H4").toString();
    if (file.contains("I1"))
        obfuscationParams_.iValues.append(file.value("I1").toString());
    if (file.contains("I2"))
        obfuscationParams_.iValues.append(file.value("I2").toString());
    if (file.contains("I3"))
        obfuscationParams_.iValues.append(file.value("I3").toString());
    if (file.contains("I4"))
        obfuscationParams_.iValues.append(file.value("I4").toString());
    if (file.contains("I5"))
        obfuscationParams_.iValues.append(file.value("I5").toString());

    file.endGroup();

    if (groups.indexOf("Peer") < 0) {
        errMessage_ = QObject::tr("Missing \"Peer\" section");
        return;
    }
    file.beginGroup("Peer");
    publicKey_ = file.value("PublicKey").toString();
    presharedKey_ = file.value("PresharedKey").toString();
    allowedIps_ =
        WireGuardConfig::stripIpv6Address(file.value("AllowedIPs", "0.0.0.0/0").toStringList());
    if (!allowedIps_.contains("/0"))
        isAllowFirewallAfterConnection_ = false;
    QStringList endpointParts = file.value("Endpoint").toString().split(":");
    endpointHostname_ = endpointParts[0];
    endpointPort_.clear();
    endpointPortNumber_ = 0;
    if (endpointParts.size() > 1) {
        endpointPortNumber_ = endpointParts[1].toUInt();
        if (endpointPortNumber_ > 0)
            endpointPort_ = QString(":%1").arg(endpointPortNumber_);
    }
    file.endGroup();
    nick_ = endpointHostname_;
}

void WireguardCustomConfig::validate()
{
    // If already incorrect, skip further validation.
    if (!errMessage_.isEmpty())
        return;

    // Some fields are required.
    if (privateKey_.isEmpty())
        errMessage_ = QObject::tr("Missing \"PrivateKey\" in the \"Interface\" section");
    else if (ipAddress_.isEmpty())
        errMessage_ = QObject::tr("Missing \"Address\" in the \"Interface\" section");
    else if (dnsAddress_.isEmpty())
        errMessage_ = QObject::tr("Missing \"DNS\" in the \"Interface\" section");
    else if (publicKey_.isEmpty())
        errMessage_ = QObject::tr("Missing \"PublicKey\" in the \"Peer\" section");
    else if (endpointHostname_.isEmpty())
        errMessage_ = QObject::tr("Missing \"Endpoint\" in the \"Peer\" section");
}

} //namespace customconfigs

