#include "decoytrafficgroup.h"

#include "dpiscalemanager.h"
#include "languagecontroller.h"
#include "graphicresources/imageresourcessvg.h"

namespace PreferencesWindow {

DecoyTrafficGroup::DecoyTrafficGroup(ScalableGraphicsObject *parent, const QString &desc, const QString &descUrl)
  : PreferenceGroup(parent, desc, descUrl)
{
    setFlags(flags() | QGraphicsItem::ItemClipsChildrenToShape | QGraphicsItem::ItemIsFocusable);

    checkBoxEnable_ = new ToggleItem(this);
    checkBoxEnable_->setIcon(ImageResourcesSvg::instance().getIndependentPixmap("preferences/DECOY_ICON"));
    connect(checkBoxEnable_, &ToggleItem::stateChanged, this, &DecoyTrafficGroup::onCheckBoxStateChanged);
    addItem(checkBoxEnable_);


    comboBox_ = new ComboBoxItem(this);
    comboBox_->setCaptionFont(FontDescr(12, false));
    connect(comboBox_, &ComboBoxItem::currentItemChanged, this, &DecoyTrafficGroup::onDecoyTrafficOptionChanged);
    addItem(comboBox_);

    updateMode();

    connect(&LanguageController::instance(), &LanguageController::languageChanged, this, &DecoyTrafficGroup::onLanguageChanged);
    onLanguageChanged();
}

void DecoyTrafficGroup::setDecoyTrafficSettings(const types::DecoyTrafficSettings &decoyTrafficSettings)
{
    if (decoyTrafficSettings_ != decoyTrafficSettings) {
        decoyTrafficSettings_ = decoyTrafficSettings;
        checkBoxEnable_->setState(decoyTrafficSettings_.isEnabled());
        comboBox_->setCurrentItem((int)decoyTrafficSettings_.volume());
        updateMode();
    }
}

void DecoyTrafficGroup::onCheckBoxStateChanged(bool isChecked)
{
    decoyTrafficSettings_.setEnabled(isChecked);
    updateMode();
    emit decoyTrafficSettingsChanged(decoyTrafficSettings_);
}

void DecoyTrafficGroup::updateMode()
{
    if (!decoyTrafficSettings_.isEnabled()) {
        hideItems(indexOf(comboBox_), indexOf(comboBox_));
    } else  {
        showItems(indexOf(comboBox_), indexOf(comboBox_));
    }
}

void DecoyTrafficGroup::onDecoyTrafficOptionChanged(QVariant v)
{
    if ((int)decoyTrafficSettings_.volume() != v.toInt())
    {
        decoyTrafficSettings_.setVolume((types::DecoyTrafficVolume)v.toInt());
        emit decoyTrafficSettingsChanged(decoyTrafficSettings_);
    }
}

void DecoyTrafficGroup::hideOpenPopups()
{
    comboBox_->hideMenu();
}

void DecoyTrafficGroup::onLanguageChanged()
{
    checkBoxEnable_->setCaption(tr("Decoy Traffic"));
    setDescription(tr("This is an experimental feature that attempts to combat traffic correlation attacks on adversarial networks." \
                      "When enabled, the app will generate random activity over the tunnel, and upload and download random data at chosen intervals."));

    comboBox_->setLabelCaption(tr("Fake Traffic Volume"));
    QList<QPair<QString, QVariant>> list;
    list.push_back(qMakePair(tr("Low"), (int)types::DecoyTrafficVolume::kLow));
    list.push_back(qMakePair(tr("Medium"), (int)types::DecoyTrafficVolume::kMedium));
    list.push_back(qMakePair(tr("High"), (int)types::DecoyTrafficVolume::kHigh));

    comboBox_->setItems(list, (int)decoyTrafficSettings_.volume());
}

}
