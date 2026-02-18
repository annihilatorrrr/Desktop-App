#include "amneziawgunblockparams.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace api_responses {

static QString getHValue(const QJsonObject &obj, const QString &key)
{
    // Temporary method until backend changes the H-values to be strings rather than integers.

    if (!obj.contains(key)) {
        return QString();
    }

    const auto hValue = obj.value(key);
    if (hValue.isString()) {
        return hValue.toString();
    }

    return QString::number(hValue.toInt());
}

bool AmneziawgUnblockParam::isValid() const
{
    // We do not check title or countries, as a WG custom config with AmneziaWG params will not have these.
    return (jc > 0) || (jmin > 0) || (jmax > 0) || (s1 > 0) || (s2 > 0) || (s3 > 0) || (s4 > 0) ||
           (!h1.isEmpty()) || (!h2.isEmpty()) || (!h3.isEmpty()) || (!h4.isEmpty()) || (!iValues.isEmpty());
}

void AmneziawgUnblockParam::setDefault()
{
    // TODO: JDRM talk to Vlad to figure out what acceptable defaults are when we cannot reach the unblock params API.
}

AmneziawgUnblockParams::AmneziawgUnblockParams(const std::string &json)
{
    if (json.empty()) {
        return;
    }

    QJsonParseError errCode;
    auto doc = QJsonDocument::fromJson(QByteArray(json.c_str()), &errCode);
    if (errCode.error != QJsonParseError::ParseError::NoError) {
        return;
    }

    auto jsonObject = doc.object();
    if (!jsonObject.contains("data")) {
        return;
    }

    auto jsonData = jsonObject["data"].toObject();

    if (!jsonData.contains("params")) {
        return;
    }

    auto jsonParamsArray = jsonData["params"].toArray();
    for (const QJsonValue &value : std::as_const(jsonParamsArray)) {
        AmneziawgUnblockParam param;
        QJsonObject obj = value.toObject();

        // Parse required fields
        if (!obj.contains("title")) {
            continue;
        }

        param.title = obj["title"].toString();

        if (obj.contains("countries")) {
            const QJsonArray jsonCountries = obj["countries"].toArray();
            for (const QJsonValue &country : jsonCountries) {
                param.countries.append(country.toString());
            }
        }

        // Documentation: https://github.com/amnezia-vpn/amneziawg-go?tab=readme-ov-file#message-paddings
        param.jc = obj.value("Jc").toInt(0);
        param.jmin = obj.value("Jmin").toInt(0);
        param.jmax = obj.value("Jmax").toInt(0);
        param.s1 = obj.value("S1").toInt(0);
        param.s2 = obj.value("S2").toInt(0);
        param.s3 = obj.value("S3").toInt(0);
        param.s4 = obj.value("S4").toInt(0);
        param.h1 = getHValue(obj, "H1");
        param.h2 = getHValue(obj, "H2");
        param.h3 = getHValue(obj, "H3");
        param.h4 = getHValue(obj, "H4");

        for (int i = 1; i <= 5; ++i) {
            QString key = QString("I%1").arg(i);
            if (obj.contains(key)) {
                QString value = obj.value(key).toString();
                if (!value.isEmpty()) {
                    param.iValues.append(value);
                }
            }
        }

        params_.append(param);
    }
}

QStringList AmneziawgUnblockParams::presets() const
{
    QStringList presets;
    for (const auto &param : std::as_const(params_)) {
        presets.append(param.title);
    }
    return presets;
}

AmneziawgUnblockParam AmneziawgUnblockParams::getUnblockParamForPreset(const QString &preset)
{
    AmneziawgUnblockParam param;

    // We'll have an empty preset if the user has not yet selected one.  E.g. anticensorship was auto-enabled.
    if (!preset.isEmpty()) {
        for (const auto &entry : std::as_const(params_)) {
            if (entry.title == preset) {
                param = entry;
                break;
            }
        }
    }

    // If we didn't locate the preset, or it is empty, select the first parameter set in the list.  If we do not have
    // any (e.g. the API is blocked or hasn't somehow completed yet) use a default set.
    if (!param.isValid()) {
        if (params_.isEmpty()) {
            param.setDefault();
        } else {
            param = params_.first();
        }
    }

    return param;
}

QDebug operator<<(QDebug debug, const AmneziawgUnblockParam &param)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "AmneziawgUnblockParam("
                    << "title=" << param.title
                    << ", countries=" << param.countries
                    << ", jc=" << param.jc
                    << ", jmin=" << param.jmin
                    << ", jmax=" << param.jmax
                    << ", s1=" << param.s1
                    << ", s2=" << param.s2
                    << ", s3=" << param.s3
                    << ", s4=" << param.s4
                    << ", h1=" << param.h1
                    << ", h2=" << param.h2
                    << ", h3=" << param.h3
                    << ", h4=" << param.h4
                    << ", iValues=" << param.iValues
                    << ")";
    return debug;
}

} // namespace api_responses
