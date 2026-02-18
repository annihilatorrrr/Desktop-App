#pragma once

#include "preferenceswindow/toggleitem.h"
#include "preferenceswindow/comboboxitem.h"
#include "preferenceswindow/preferencegroup.h"

namespace PreferencesWindow {

class AntiCensorshipGroup : public PreferenceGroup
{
    Q_OBJECT
public:
    explicit AntiCensorshipGroup(ScalableGraphicsObject *parent,
                                  const QString &desc = "",
                                  const QString &descUrl = "");

    void setAntiCensorshipEnabled(bool enabled);
    void setAmneziawgPreset(const QString &preset);
    void setAmneziawgUnblockParams(const QString &activePreset, QStringList presets);
    void setEnabled(bool enabled);

    void setDescription(const QString &desc, const QString &descUrl = "");

signals:
    void antiCensorshipStateChanged(bool enabled);
    void amneziawgPresetChanged(QString preset);

private slots:
    void onCheckBoxStateChanged(bool isChecked);
    void onAmneziawgPresetChanged(const QVariant &value);
    void onLanguageChanged();

private:
    void updateMode();

    ToggleItem *checkBoxEnable_;
    ComboBoxItem *amneziawgPreset_;

    bool isEnabled_ = false;
};

} // namespace PreferencesWindow
