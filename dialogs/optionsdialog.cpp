#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QUrl>

#include "optionsdialog.h"
#include "../engines/hotkeyengine.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
  QDialog(parent)
{
  ui.setupUi(this);

  setModal(true);

  QSettings settings;

  // Importing settings from previous versions.
  if (settings.contains("application/optionsLastTab"))
  {
    settings.remove("application");
    settings.setValue("actions/screen/alt", settings.value("hotkey/modifiers/alt", false).toBool());
    settings.setValue("actions/screen/ctrl", settings.value("hotkey/modifiers/ctrl", false).toBool());
    settings.setValue("actions/screen/shift", settings.value("hotkey/modifiers/shift", false).toBool());
    settings.setValue("actions/screen/index", settings.value("hotkey/key", 0).toInt());
    settings.remove("hotkey");
    settings.remove("options/mode");
  }

  if (!(QSettings().contains("options/tray")))
  { // If there are no settings, get rid of the cancel button so that the user is forced to save settings.
    ui.buttonBox->clear();
    ui.buttonBox->addButton(QDialogButtonBox::Ok);
  }

  connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*))   , this, SLOT(dialogButtonClicked(QAbstractButton*)));

  connect(ui.flipPrefixPushButton, SIGNAL(toggled(bool))   , this, SLOT(flipButtonToggled(bool)));

  connect(ui.screenCheckBox, SIGNAL(toggled(bool))   , ui.screenHotkeyWidget, SLOT(setEnabled(bool)));
  connect(ui.areaCheckBox  , SIGNAL(toggled(bool))   , ui.areaHotkeyWidget  , SLOT(setEnabled(bool)));
  connect(ui.windowCheckBox, SIGNAL(toggled(bool))   , ui.windowHotkeyWidget, SLOT(setEnabled(bool)));
  connect(ui.openCheckBox  , SIGNAL(toggled(bool))   , ui.openHotkeyWidget  , SLOT(setEnabled(bool)));
  connect(ui.directoryCheckBox, SIGNAL(toggled(bool)), ui.directoryHotkeyWidget, SLOT(setEnabled(bool)));

  loadSettings();
  //updatePreview();

  connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));

  connect(ui.startupCheckBox, SIGNAL(stateChanged(int)), this, SLOT(startupRelatedStateChange(int)));
  connect(ui.trayCheckBox, SIGNAL(stateChanged(int)), this, SLOT(trayRelatedStateChange(int)));

  connect(ui.browsePushButton, SIGNAL(clicked()), this, SLOT(browse()));

  startupRelatedStateChange(ui.startupCheckBox->checkState());
  trayRelatedStateChange(ui.trayCheckBox->checkState());

  if (QSysInfo::WindowsVersion != QSysInfo::WV_VISTA)
  {
    ui.vistaGlassCheckBox->setVisible(false);
    ui.vistaGlassCheckBox->setChecked(false);
  }

}

/*
 * Slots
 */

void OptionsDialog::accepted()
{
  if (hotkeyCollision())
  {
    QMessageBox::critical(0, tr("Hotkey conflict"),
        tr("You have assigned the same hotkeys to more than one action."));
    return;
  }

  saveSettings();
  accept();
}

void OptionsDialog::browse()
{
  QString fileName = QFileDialog::getExistingDirectory(this,
      tr("Select where you want to save the screenshots"),
      ui.targetLineEdit->text());

  if (fileName.isEmpty())
    return;

  ui.targetLineEdit->setText(fileName);
}

void OptionsDialog::dialogButtonClicked(QAbstractButton *button)
{
   if (ui.buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
   {
     QMessageBox msgBox;
     msgBox.setWindowTitle(tr("Lightscreen"));
     msgBox.setText(tr("Restoring the default options will cause you to lose all of your current configuration."));
     msgBox.setIcon(QMessageBox::Warning);

     QPushButton *restoreButton     = msgBox.addButton(tr("Restore"), QMessageBox::ActionRole);
     QPushButton *dontRestoreButton = msgBox.addButton(tr("Don't Restore"), QMessageBox::ActionRole);

     msgBox.exec();

     Q_UNUSED(restoreButton)

     if (msgBox.clickedButton() == dontRestoreButton)
         return;

     QSettings().clear();
     accept();
   }
}

void OptionsDialog::flipButtonToggled(bool checked)
{
  setUpdatesEnabled(false);
  ui.hboxLayout->removeWidget(ui.prefixLineEdit);
  ui.hboxLayout->removeWidget(ui.namingComboBox);

  if (checked)
  {
    ui.flipPrefixPushButton->setIcon(QIcon(":/icons/PrefixRight"));
    ui.hboxLayout->addWidget(ui.namingComboBox);
    ui.hboxLayout->addWidget(ui.prefixLineEdit);
  }
  else
  {
    ui.flipPrefixPushButton->setIcon(QIcon(":/icons/PrefixLeft"));
    ui.hboxLayout->addWidget(ui.prefixLineEdit);
    ui.hboxLayout->addWidget(ui.namingComboBox);
  }
  setUpdatesEnabled(true);// Avoids flicker
}

void OptionsDialog::openUrl(QString url)
{
  QDesktopServices::openUrl(QUrl(url));
}

void OptionsDialog::saveSettings()
{
  QSettings settings;

  settings.beginGroup("file");
  settings.setValue("format", ui.formatComboBox->currentIndex());
  settings.setValue("prefix", ui.prefixLineEdit->text());
  settings.setValue("naming", ui.namingComboBox->currentIndex());
  settings.setValue("target", ui.targetLineEdit->text());
  settings.endGroup();

  settings.beginGroup("options");
  settings.setValue("startup", ui.startupCheckBox->isChecked());
  settings.setValue("startupHide", ui.startupHideCheckBox->isChecked());
  settings.setValue("hide", ui.hideCheckBox->isChecked());
  settings.setValue("delay", ui.delaySpinBox->value());
  settings.setValue("flip", ui.flipPrefixPushButton->isChecked());
  settings.setValue("tray", ui.trayCheckBox->isChecked());
  settings.setValue("message", ui.messageCheckBox->isChecked());
  settings.setValue("quality", ui.qualitySlider->value());
  settings.setValue("vistaGlass", ui.vistaGlassCheckBox->isChecked());
  settings.setValue("optipng", ui.optiPngCheckBox->isChecked());
  settings.setValue("playSound", ui.playSoundCheckBox->isChecked());
  settings.setValue("dxScreen", ui.dxScreenCheckBox->isChecked());
  settings.endGroup();

  settings.beginGroup("actions");

  settings.beginGroup(tr("screen"));
  settings.setValue("enabled", ui.screenCheckBox->isChecked());
  settings.setValue("ctrl", ui.screenHotkeyWidget->hotkey().ctrl);
  settings.setValue("alt", ui.screenHotkeyWidget->hotkey().alt);
  settings.setValue("shift", ui.screenHotkeyWidget->hotkey().shift);
  settings.setValue("index", HotkeyEngine::keyToIndex(ui.screenHotkeyWidget->hotkey().key));
  settings.endGroup();

  settings.beginGroup(tr("window"));
  settings.setValue("enabled", ui.windowCheckBox->isChecked());
  settings.setValue("ctrl", ui.windowHotkeyWidget->hotkey().ctrl);
  settings.setValue("alt", ui.windowHotkeyWidget->hotkey().alt);
  settings.setValue("shift", ui.windowHotkeyWidget->hotkey().shift);
  settings.setValue("index", HotkeyEngine::keyToIndex(ui.windowHotkeyWidget->hotkey().key));
  settings.endGroup();

  settings.beginGroup(tr("area"));
  settings.setValue("enabled", ui.areaCheckBox->isChecked());
  settings.setValue("ctrl", ui.areaHotkeyWidget->hotkey().ctrl);
  settings.setValue("alt", ui.areaHotkeyWidget->hotkey().alt);
  settings.setValue("shift", ui.areaHotkeyWidget->hotkey().shift);
  settings.setValue("index", HotkeyEngine::keyToIndex(ui.areaHotkeyWidget->hotkey().key));
  settings.endGroup();

  settings.beginGroup(tr("open"));
  settings.setValue("enabled", ui.openCheckBox->isChecked());
  settings.setValue("ctrl", ui.openHotkeyWidget->hotkey().ctrl);
  settings.setValue("alt", ui.openHotkeyWidget->hotkey().alt);
  settings.setValue("shift", ui.openHotkeyWidget->hotkey().shift);
  settings.setValue("index", HotkeyEngine::keyToIndex(ui.openHotkeyWidget->hotkey().key));
  settings.endGroup();

  settings.beginGroup(tr("directory"));
  settings.setValue("enabled", ui.directoryCheckBox->isChecked());
  settings.setValue("ctrl", ui.directoryHotkeyWidget->hotkey().ctrl);
  settings.setValue("alt", ui.directoryHotkeyWidget->hotkey().alt);
  settings.setValue("shift", ui.directoryHotkeyWidget->hotkey().shift);
  settings.setValue("index", HotkeyEngine::keyToIndex(ui.directoryHotkeyWidget->hotkey().key));
  settings.endGroup();

  settings.endGroup();
}

void OptionsDialog::startupRelatedStateChange(int state)
{
  ui.startupHideCheckBox->setEnabled(state);

  if (!state)
    ui.startupHideCheckBox->setChecked(false);
}

void OptionsDialog::trayRelatedStateChange(int state)
{
  ui.messageCheckBox->setEnabled(state);

  if (!state)
  {
    ui.messageCheckBox->setChecked(false);
  }
}

/*
 * Private
 */
void OptionsDialog::loadSettings()
{
  QSettings settings;

  settings.beginGroup("file");
  ui.formatComboBox->setCurrentIndex(settings.value("format", 1).toInt());
  ui.prefixLineEdit->setText(settings.value("prefix", "screenshot.").toString());
  ui.namingComboBox->setCurrentIndex(settings.value("naming", 0).toInt());
  ui.targetLineEdit->setText(settings.value("target").toString());
  settings.endGroup();

  settings.beginGroup("options");
  ui.startupCheckBox->setChecked(settings.value("startup").toBool());
  ui.startupHideCheckBox->setChecked(settings.value("startupHide").toBool());
  ui.hideCheckBox->setChecked(settings.value("hide").toBool());
  ui.delaySpinBox->setValue(settings.value("delay", 0).toInt());
  ui.flipPrefixPushButton->setChecked(settings.value("flip", false).toBool());
  ui.trayCheckBox->setChecked(settings.value("tray", true).toBool());
  ui.messageCheckBox->setChecked(settings.value("message").toBool());
  ui.qualitySlider->setValue(settings.value("quality", 100).toInt());
  ui.vistaGlassCheckBox->setChecked(settings.value("vistaGlass", true).toBool());
  ui.playSoundCheckBox->setChecked(settings.value("playSound", false).toBool());
  ui.dxScreenCheckBox->setChecked(settings.value("dxScreen", false).toBool());

  ui.optiPngCheckBox->setChecked(settings.value("optipng", true).toBool());

  if (QFile::exists("optipng.exe")) //TODO: Change value when cross-platform
  {
    ui.optiPngCheckBox->setEnabled(true);
  }
  else
  {
    ui.optiPngCheckBox->setEnabled(false);
  }

  settings.endGroup();

  settings.beginGroup("actions");

  int index;
  HotkeyEngine::Hotkey screenHotkey;
  HotkeyEngine::Hotkey areaHotkey;
  HotkeyEngine::Hotkey windowHotkey;
  HotkeyEngine::Hotkey openHotkey;
  HotkeyEngine::Hotkey directoryHotkey;

  // This toggle is for the first run
  ui.screenCheckBox->toggle();
  ui.areaCheckBox->toggle();
  ui.windowCheckBox->toggle();
  ui.openCheckBox->toggle();
  ui.directoryCheckBox->toggle();

  settings.beginGroup(tr("screen"));
  ui.screenCheckBox->setChecked(settings.value("enabled", true).toBool());
  screenHotkey.ctrl = settings.value("ctrl", false).toBool();
  screenHotkey.alt = settings.value("alt", false).toBool();
  screenHotkey.shift = settings.value("shift", false).toBool();
  index = settings.value("index", 0).toInt();
  screenHotkey.key = HotkeyEngine::indexToKey(index);
  settings.endGroup();

  settings.beginGroup(tr("area"));
  ui.areaCheckBox->setChecked(settings.value("enabled", false).toBool());
  areaHotkey.ctrl = settings.value("ctrl", true).toBool();
  areaHotkey.alt = settings.value("alt", false).toBool();
  areaHotkey.shift = settings.value("shift", false).toBool();
  index = settings.value("index", 0).toInt();
  areaHotkey.key = HotkeyEngine::indexToKey(index);
  settings.endGroup();

  settings.beginGroup(tr("window"));
  ui.windowCheckBox->setChecked(settings.value("enabled", false).toBool());
  windowHotkey.ctrl = settings.value("ctrl", false).toBool();
  windowHotkey.alt = settings.value("alt", true).toBool();
  windowHotkey.shift = settings.value("shift", false).toBool();
  index = settings.value("index", 0).toInt();
  windowHotkey.key = HotkeyEngine::indexToKey(index);
  settings.endGroup();

  settings.beginGroup(tr("open"));
  ui.openCheckBox->setChecked(settings.value("enabled", false).toBool());
  openHotkey.ctrl = settings.value("ctrl", true).toBool();
  openHotkey.alt = settings.value("alt", false).toBool();
  openHotkey.shift = settings.value("shift", false).toBool();
  index = settings.value("index", HotkeyEngine::keyToIndex(HotkeyEngine::KPrior)).toInt();
  openHotkey.key = HotkeyEngine::indexToKey(index);
  settings.endGroup();

  settings.beginGroup(tr("directory"));
  ui.directoryCheckBox->setChecked(settings.value("enabled", false).toBool());
  directoryHotkey.ctrl = settings.value("ctrl", false).toBool();
  directoryHotkey.alt = settings.value("alt", false).toBool();
  directoryHotkey.shift = settings.value("shift", true).toBool();
  index = settings.value("index", HotkeyEngine::keyToIndex(HotkeyEngine::KPrior)).toInt();
  directoryHotkey.key = HotkeyEngine::indexToKey(index);
  settings.endGroup();

  settings.endGroup();

  ui.screenHotkeyWidget->setHotkey(screenHotkey);
  ui.areaHotkeyWidget->setHotkey(areaHotkey);
  ui.windowHotkeyWidget->setHotkey(windowHotkey);
  ui.openHotkeyWidget->setHotkey(openHotkey);
  ui.directoryHotkeyWidget->setHotkey(directoryHotkey);
}

bool OptionsDialog::hotkeyCollision()
{
  // Check for hotkey collision

  if (ui.screenCheckBox->isChecked())
  {
    if (ui.screenHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey() && ui.areaCheckBox->isChecked())
      return true;

    if (ui.screenHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey() && ui.windowCheckBox->isChecked())
      return true;

    if (ui.screenHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey() && ui.openCheckBox->isChecked())
      return true;

    if (ui.screenHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey() && ui.directoryCheckBox->isChecked())
              return true;
  }

  if (ui.areaCheckBox->isChecked())
  {
    if (ui.areaHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey() && ui.screenCheckBox->isChecked())
      return true;

    if (ui.areaHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey() && ui.windowCheckBox->isChecked())
      return true;

    if (ui.areaHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey() && ui.openCheckBox->isChecked())
      return true;

    if (ui.areaHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey() && ui.directoryCheckBox->isChecked())
      return true;
  }

  if (ui.windowCheckBox->isChecked())
  {
    if (ui.windowHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey() && ui.screenCheckBox->isChecked())
      return true;

    if (ui.windowHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey() && ui.areaCheckBox->isChecked())
      return true;

    if (ui.windowHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey() && ui.openCheckBox->isChecked())
      return true;

    if (ui.windowHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey() && ui.directoryCheckBox->isChecked())
          return true;
  }

  if (ui.openCheckBox->isChecked())
  {
    if (ui.openHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey() && ui.screenCheckBox->isChecked())
      return true;

    if (ui.openHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey() && ui.areaCheckBox->isChecked())
      return true;

    if (ui.openHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey() && ui.windowCheckBox->isChecked())
      return true;

    if (ui.openHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey() && ui.directoryCheckBox->isChecked())
      return true;

  }

  if (ui.directoryCheckBox->isChecked())
  {
    if (ui.directoryHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey() && ui.screenCheckBox->isChecked())
      return true;

    if (ui.directoryHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey() && ui.areaCheckBox->isChecked())
      return true;

    if (ui.directoryHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey() && ui.windowCheckBox->isChecked())
      return true;

    if (ui.directoryHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey() && ui.openCheckBox->isChecked())
      return true;
  }

  return false;
}

