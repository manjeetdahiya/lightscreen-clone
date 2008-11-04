#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QUrl>

#include <QDebug>

#include "optionsdialog.h"
#include "../tools/os.h"
#include "../updater/updater.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
  QDialog(parent)
{
  ui.setupUi(this);

  setModal(true);

#ifndef Q_WS_WIN
  ui.playSoundCheckBox->setVisible(false);
  ui.playSoundCheckBox->setChecked(false);
#endif

  QSettings settings;

  if (!settings.contains("options/tray"))
  { // If there are no settings, get rid of the cancel button so that the user is forced to save them
    ui.buttonBox->clear();
    ui.buttonBox->addButton(QDialogButtonBox::Ok);
  }

  connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonClicked(QAbstractButton*)));

  connect(ui.flipPrefixPushButton, SIGNAL(toggled(bool)), this, SLOT(flipButtonToggled(bool)));

  connect(ui.aboutPushButton       , SIGNAL(clicked()), parent, SLOT(showAbout()));
  connect(ui.checkUpdatesPushButton, SIGNAL(clicked()), this  , SLOT(checkUpdatesNow()));

  connect(ui.screenCheckBox   , SIGNAL(toggled(bool)), ui.screenHotkeyWidget   , SLOT(setEnabled(bool)));
  connect(ui.areaCheckBox     , SIGNAL(toggled(bool)), ui.areaHotkeyWidget     , SLOT(setEnabled(bool)));
  connect(ui.windowCheckBox   , SIGNAL(toggled(bool)), ui.windowHotkeyWidget   , SLOT(setEnabled(bool)));
  connect(ui.openCheckBox     , SIGNAL(toggled(bool)), ui.openHotkeyWidget     , SLOT(setEnabled(bool)));
  connect(ui.directoryCheckBox, SIGNAL(toggled(bool)), ui.directoryHotkeyWidget, SLOT(setEnabled(bool)));

  connect(ui.translateLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));

  // Getting the language entries
  QDir lang(QCoreApplication::applicationDirPath() + "/lang", "*.qm");
  lang.setFilter(QDir::Files);

  QStringList entries;

  entries << "English"; // Set here and not the .ui because it the items would get reset when calling retranslateUi

  foreach (QString entry, lang.entryList())
  {
    entry.chop(3); // Remove the ".em"
    entry[0] = entry.at(0).toUpper(); // Make first char uppercase
    entries << entry;
  }

  ui.languageComboBox->addItems(entries);

  connect(ui.languageComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(languageChange(QString)));

  loadSettings();

  connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));

  connect(ui.startupCheckBox, SIGNAL(stateChanged(int)), this, SLOT(startupRelatedStateChange(int)));
  connect(ui.trayCheckBox   , SIGNAL(stateChanged(int)), this, SLOT(trayRelatedStateChange(int)));

  connect(ui.browsePushButton, SIGNAL(clicked()), this, SLOT(browse()));

  startupRelatedStateChange(ui.startupCheckBox->checkState());
  trayRelatedStateChange(ui.trayCheckBox->checkState());
}

/*
 * Slots
 */

void OptionsDialog::accepted()
{
  if (hotkeyCollision())
  {
    QMessageBox::critical(0, tr("Hotkey conflict"), tr("You have assigned the same hotkeys to more than one action."));
    return;
  }

  saveSettings();
  accept();
}

void OptionsDialog::browse()
{
  QString fileName = QFileDialog::getExistingDirectory(this, tr("Select where you want to save the screenshots"), ui.targetLineEdit->text());

  if (fileName.isEmpty())
    return;

  ui.targetLineEdit->setText(fileName);
}

void OptionsDialog::checkUpdatesNow()
{
  Updater::instance()->check();
}

void OptionsDialog::dialogButtonClicked(QAbstractButton *button)
{
  if (ui.buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Lightscreen - Options"));
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

  setUpdatesEnabled(true); // Avoids flicker
}

void OptionsDialog::languageChange(QString language)
{
  os::translate(language);
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
  settings.setValue("enabled", ui.fileGroupBox->isChecked());
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
  settings.setValue("playSound", ui.playSoundCheckBox->isChecked());
  // We save the explicit string because addition/removal of language files can cause it to change
  settings.setValue("language", ui.languageComboBox->currentText());
  // This settings is inverted because the first iteration of the Updater did not have a settings but instead relied on the messagebox choice of the user.
  settings.setValue("disableUpdater", !ui.updaterCheckBox->isChecked());
  settings.setValue("preview", ui.previewCheckBox->isChecked());

  // Advanced
  settings.setValue("disableHideAlert", !ui.warnHideCheckBox->isChecked());
  settings.setValue("dxScreen", ui.dxScreenCheckBox->isChecked());
  settings.setValue("clipboard", ui.clipboardCheckBox->isChecked());
  settings.setValue("optipng", ui.optiPngCheckBox->isChecked());
  settings.setValue("currentMonitor", ui.currentMonitorCheckBox->isChecked());
  settings.endGroup();

  settings.beginGroup("actions");

  settings.beginGroup("screen");
  settings.setValue("enabled", ui.screenCheckBox->isChecked());
  settings.setValue("hotkey", ui.screenHotkeyWidget->hotkey());
  settings.endGroup();

  settings.beginGroup("area");
  settings.setValue("enabled", ui.areaCheckBox->isChecked());
  settings.setValue("hotkey", ui.areaHotkeyWidget->hotkey());
  settings.endGroup();

  settings.beginGroup("window");
  settings.setValue("enabled", ui.windowCheckBox->isChecked());
  settings.setValue("hotkey", ui.windowHotkeyWidget->hotkey());
  settings.endGroup();

  settings.beginGroup("open");
  settings.setValue("enabled", ui.openCheckBox->isChecked());
  settings.setValue("hotkey", ui.openHotkeyWidget->hotkey());
  settings.endGroup();

  settings.beginGroup("directory");
  settings.setValue("enabled", ui.directoryCheckBox->isChecked());
  settings.setValue("hotkey", ui.directoryHotkeyWidget->hotkey());
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
    ui.messageCheckBox->setChecked(false);
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
  ui.targetLineEdit->setText(settings.value("target", QDir::homePath() + "/screenshots").toString()); // Defaults to $HOME$/screenshots
  ui.fileGroupBox->setChecked(settings.value("enabled", true).toBool());
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
  ui.playSoundCheckBox->setChecked(settings.value("playSound", false).toBool());
  ui.updaterCheckBox->setChecked(!settings.value("disableUpdater", false).toBool());
  ui.previewCheckBox->setChecked(settings.value("preview", false).toBool());

  // Advanced
  ui.clipboardCheckBox->setChecked(settings.value("clipboard", true).toBool());
  ui.dxScreenCheckBox->setChecked(settings.value("dxScreen", false).toBool());
  ui.optiPngCheckBox->setChecked(settings.value("optipng", true).toBool());
  ui.warnHideCheckBox->setChecked(!settings.value("disableHideAlert", false).toBool());
  ui.currentMonitorCheckBox->setChecked(settings.value("currentMonitor", false).toBool());

  if (QFile::exists("optipng.exe")) //TODO: Change value when cross-platform
  {
    ui.optiPngCheckBox->setEnabled(true);
  }
  else
  {
    ui.optiPngCheckBox->setEnabled(false);
    ui.optiPngCheckBox->setText(ui.optiPngCheckBox->text() + tr(" (File not found)"));
  }

  QString lang = settings.value("language").toString();
  int index = ui.languageComboBox->findText(lang, Qt::MatchExactly | Qt::MatchCaseSensitive);

  if (index == -1)
    index = 0; // If the data is invalid then revert to the default language (English)

  ui.languageComboBox->setCurrentIndex(index);

  settings.endGroup();

  settings.beginGroup("actions");

  // This toggle is for the first run
  ui.screenCheckBox->toggle();
  ui.areaCheckBox->toggle();
  ui.windowCheckBox->toggle();
  ui.openCheckBox->toggle();
  ui.directoryCheckBox->toggle();

  settings.beginGroup("screen");
  ui.screenCheckBox->setChecked(settings.value("enabled", true).toBool());
  ui.screenHotkeyWidget->setHotkey(settings.value("hotkey", QKeySequence(Qt::Key_Print)).value<QKeySequence> ());
  settings.endGroup();

  settings.beginGroup("area");
  ui.areaCheckBox->setChecked(settings.value("enabled").toBool());
  ui.areaHotkeyWidget->setHotkey(settings.value("hotkey", QKeySequence(Qt::CTRL + Qt::Key_Print)).value<QKeySequence> ());
  settings.endGroup();

  settings.beginGroup("window");
  ui.windowCheckBox->setChecked(settings.value("enabled").toBool());
  ui.windowHotkeyWidget->setHotkey(settings.value("hotkey", QKeySequence(Qt::SHIFT + Qt::Key_Print)).value<QKeySequence> ());
  settings.endGroup();

  settings.beginGroup("open");
  ui.openCheckBox->setChecked(settings.value("enabled").toBool());
  ui.openHotkeyWidget->setHotkey(settings.value("hotkey", QKeySequence(Qt::CTRL+ Qt::Key_PageUp)).value<QKeySequence> ());
  settings.endGroup();

  settings.beginGroup("directory");
  ui.directoryCheckBox->setChecked(settings.value("enabled").toBool());
  ui.directoryHotkeyWidget->setHotkey(settings.value("hotkey", QKeySequence(Qt::SHIFT + Qt::Key_PageUp)).value<QKeySequence> ());
  settings.endGroup();

  settings.endGroup();
}

bool OptionsDialog::hotkeyCollision()
{
  // Check for hotkey collision (there's probably a better way to do this...=)

  if (ui.screenCheckBox->isChecked())
  {
    if (ui.screenHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey()
        && ui.areaCheckBox->isChecked())
      return true;

    if (ui.screenHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey()
        && ui.windowCheckBox->isChecked())
      return true;

    if (ui.screenHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey()
        && ui.openCheckBox->isChecked())
      return true;

    if (ui.screenHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey()
        && ui.directoryCheckBox->isChecked())
      return true;
  }

  if (ui.areaCheckBox->isChecked())
  {
    if (ui.areaHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey()
        && ui.screenCheckBox->isChecked())
      return true;

    if (ui.areaHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey()
        && ui.windowCheckBox->isChecked())
      return true;

    if (ui.areaHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey()
        && ui.openCheckBox->isChecked())
      return true;

    if (ui.areaHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey()
        && ui.directoryCheckBox->isChecked())
      return true;
  }

  if (ui.windowCheckBox->isChecked())
  {
    if (ui.windowHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey()
        && ui.screenCheckBox->isChecked())
      return true;

    if (ui.windowHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey()
        && ui.areaCheckBox->isChecked())
      return true;

    if (ui.windowHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey()
        && ui.openCheckBox->isChecked())
      return true;

    if (ui.windowHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey()
        && ui.directoryCheckBox->isChecked())
      return true;
  }

  if (ui.openCheckBox->isChecked())
  {
    if (ui.openHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey()
        && ui.screenCheckBox->isChecked())
      return true;

    if (ui.openHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey()
        && ui.areaCheckBox->isChecked())
      return true;

    if (ui.openHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey()
        && ui.windowCheckBox->isChecked())
      return true;

    if (ui.openHotkeyWidget->hotkey() == ui.directoryHotkeyWidget->hotkey()
        && ui.directoryCheckBox->isChecked())
      return true;

  }

  if (ui.directoryCheckBox->isChecked())
  {
    if (ui.directoryHotkeyWidget->hotkey() == ui.screenHotkeyWidget->hotkey()
        && ui.screenCheckBox->isChecked())
      return true;

    if (ui.directoryHotkeyWidget->hotkey() == ui.areaHotkeyWidget->hotkey()
        && ui.areaCheckBox->isChecked())
      return true;

    if (ui.directoryHotkeyWidget->hotkey() == ui.windowHotkeyWidget->hotkey()
        && ui.windowCheckBox->isChecked())
      return true;

    if (ui.directoryHotkeyWidget->hotkey() == ui.openHotkeyWidget->hotkey()
        && ui.openCheckBox->isChecked())
      return true;
  }

  return false;
}

void OptionsDialog::changeEvent(QEvent* event)
{
  if (event->type() == QEvent::LanguageChange)
  { // TODO: Resize to minimum logical size?
    ui.retranslateUi(this);
  }

  QDialog::changeEvent(event);
}
