#include <QDateTime>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QTextBrowser>
#include <QTimer>
#include <QUrl>

#if defined(Q_WS_WIN)
  #include <windows.h>
#endif

#include <QDebug>

#include "optionsdialog.h"
#include "../tools/os.h"
#include "../tools/customnaming.h"

#include "../updater/updater.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
  QDialog(parent)
{
  ui.setupUi(this);

  if (os::aeroGlass(this))
  { // The standard QTabBar looks odd when full aero transparency is enabled.
    setStyleSheet("QTabBar::tab:selected { background: white; margin-top: 2px; padding-right: -1px; padding: 0; border: 1px solid gray; border-bottom: none; }");
    ui.tabWidget->setTabText(ui.tabWidget->currentIndex(), "  "+tr("General")+"  "); //Fake padding.
  }

  setModal(true);

#if !defined(Q_WS_WIN)
  ui.playSoundCheckBox->setVisible(false);
  ui.playSoundCheckBox->setChecked(false);
  ui.cursorCheckBox->setVisible(false);
  ui.cursorCheckBox->setChecked(false);
  ui.startupCheckBox->setVisible(false);
  ui.startupCheckBox->setChecked(false);
  ui.optiPngCheckBox->setVisible(false);
  ui.optiPngCheckBox->setChecked(false);
#endif

  QSettings settings;

  if (!settings.contains("options/tray"))
  { // If there are no settings, get rid of the cancel button so that the user is forced to save them
    ui.buttonBox->clear();
    ui.buttonBox->addButton(QDialogButtonBox::Ok);

    // Move the first option window to the center of the screen, since windows usually positions it in a random location since it has no visible parent.
    move(QApplication::desktop()->screen(QApplication::desktop()->primaryScreen())->rect().center()-QPoint(height()/2, width()/2));
  }

  connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonClicked(QAbstractButton*)));

  connect(ui.customPushButton    , SIGNAL(toggled(bool)), this, SLOT(customButtonToggled(bool)));
  connect(ui.customHelpPushButton, SIGNAL(clicked()), this, SLOT(showCustomHelp()));

  connect(ui.aboutPushButton        , SIGNAL(clicked()), parent, SLOT(showAbout()));
  connect(ui.checkUpdatesPushButton , SIGNAL(clicked()), this  , SLOT(checkUpdatesNow()));

  connect(ui.screenCheckBox   , SIGNAL(toggled(bool)), ui.screenHotkeyWidget   , SLOT(setEnabled(bool)));
  connect(ui.areaCheckBox     , SIGNAL(toggled(bool)), ui.areaHotkeyWidget     , SLOT(setEnabled(bool)));
  connect(ui.windowCheckBox   , SIGNAL(toggled(bool)), ui.windowHotkeyWidget   , SLOT(setEnabled(bool)));
  connect(ui.openCheckBox     , SIGNAL(toggled(bool)), ui.openHotkeyWidget     , SLOT(setEnabled(bool)));
  connect(ui.directoryCheckBox, SIGNAL(toggled(bool)), ui.directoryHotkeyWidget, SLOT(setEnabled(bool)));

  // Getting the language entries
  QDir lang(QCoreApplication::applicationDirPath() + "/lang", "*.qm");
  lang.setFilter(QDir::Files);

  QStringList entries;
  QString entry;

  entries << "English"; // Set here and not the .ui because it the items would get reset when calling retranslateUi

  foreach (entry, lang.entryList())
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

  if (ui.customPushButton->isChecked() && !CustomNaming::isValid(ui.customLineEdit->text()))
  {
    QMessageBox::critical(0, tr("Custom Naming Error"), tr("The custom naming string you entered is invalid."));
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

void OptionsDialog::changePage()
{

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

void OptionsDialog::customButtonToggled(bool checked)
{
  setUpdatesEnabled(false);

  // I'm sure there's a better way to do this..

  ui.hboxLayout->removeWidget(ui.prefixLineEdit);
  ui.hboxLayout->removeWidget(ui.namingComboBox);
  ui.hboxLayout->removeWidget(ui.customLineEdit);
  ui.hboxLayout->removeWidget(ui.customHelpPushButton);

  ui.prefixLineEdit->setVisible(true);
  ui.namingComboBox->setVisible(true);
  ui.customLineEdit->setVisible(true);
  ui.customHelpPushButton->setVisible(true);

  if (checked)
  {
    ui.hboxLayout->addWidget(ui.customLineEdit);
    ui.hboxLayout->addWidget(ui.customHelpPushButton);

    ui.prefixLineEdit->setVisible(false);
    ui.namingComboBox->setVisible(false);
  }
  else
  {
    ui.hboxLayout->addWidget(ui.prefixLineEdit);
    ui.hboxLayout->addWidget(ui.namingComboBox);

    ui.customLineEdit->setVisible(false);
    ui.customHelpPushButton->setVisible(false);
  }

  setUpdatesEnabled(true); // Avoids flicker
}

void OptionsDialog::languageChange(QString language)
{
  os::translate(language);
}

void OptionsDialog::saveSettings()
{
  QSettings settings;

  settings.beginGroup("file");
  settings.setValue("format", ui.formatComboBox->currentIndex());
  settings.setValue("prefix", ui.prefixLineEdit->text());
  settings.setValue("naming", ui.namingComboBox->currentIndex());
  settings.setValue("custom", ui.customPushButton->isChecked());
  settings.setValue("customString", ui.customLineEdit->text());
  settings.setValue("target", ui.targetLineEdit->text());
  settings.setValue("enabled", ui.fileGroupBox->isChecked());
  settings.endGroup();

  settings.beginGroup("options");
  settings.setValue("startup", ui.startupCheckBox->isChecked());
  settings.setValue("startupHide", ui.startupHideCheckBox->isChecked());
  settings.setValue("hide", ui.hideCheckBox->isChecked());
  settings.setValue("delay", ui.delaySpinBox->value());
  settings.setValue("tray", ui.trayCheckBox->isChecked());
  settings.setValue("message", ui.messageCheckBox->isChecked());
  settings.setValue("quality", ui.qualitySlider->value());
  settings.setValue("playSound", ui.playSoundCheckBox->isChecked());
  // We save the explicit string because addition/removal of language files can cause it to change
  settings.setValue("language", ui.languageComboBox->currentText());
  // This settings is inverted because the first iteration of the Updater did not have a settings but instead relied on the messagebox choice of the user.
  settings.setValue("disableUpdater", !ui.updaterCheckBox->isChecked());
  settings.setValue("magnify", ui.magnifyCheckBox->isChecked());
  settings.setValue("cursor", ui.cursorCheckBox->isChecked());
  settings.setValue("saveAs", ui.saveAsCheckBox->isChecked());

  // Advanced
  settings.setValue("disableHideAlert", !ui.warnHideCheckBox->isChecked());
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

void OptionsDialog::showCustomHelp()
{
  QDialog helpDialog;
  helpDialog.setWindowTitle("Custon Naming Information");
  helpDialog.setMinimumSize(400, 400);

  QTextBrowser help(&helpDialog);
  help.setHtml(ui.customLineEdit->whatsThis());

  QHBoxLayout layout;
  layout.addWidget(&help);

  layout.setMargin(0);
  layout.setContentsMargins(0, 0, 0, 0);

  helpDialog.setLayout(&layout);

  helpDialog.exec();
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
  ui.customPushButton->setChecked(settings.value("custom", false).toBool());
  ui.customLineEdit->setText(settings.value("customString", "screenshot./number/").toString());
  ui.targetLineEdit->setText(settings.value("target", QDir::homePath() + "/screenshots").toString()); // Defaults to $HOME$/screenshots
  ui.fileGroupBox->setChecked(settings.value("enabled", true).toBool());
  settings.endGroup();

  settings.beginGroup("options");
  ui.startupCheckBox->setChecked(settings.value("startup").toBool());
  ui.startupHideCheckBox->setChecked(settings.value("startupHide").toBool());
  ui.hideCheckBox->setChecked(settings.value("hide").toBool());
  ui.delaySpinBox->setValue(settings.value("delay", 0).toInt());
  ui.trayCheckBox->setChecked(settings.value("tray", true).toBool());
  ui.messageCheckBox->setChecked(settings.value("message").toBool());
  ui.qualitySlider->setValue(settings.value("quality", 100).toInt());
  ui.playSoundCheckBox->setChecked(settings.value("playSound", false).toBool());
  ui.updaterCheckBox->setChecked(!settings.value("disableUpdater", false).toBool());
  ui.magnifyCheckBox->setChecked(settings.value("magnify", false).toBool());
  ui.cursorCheckBox->setChecked(settings.value("cursor", false).toBool());
  ui.saveAsCheckBox->setChecked(settings.value("saveAs", false).toBool());

  customButtonToggled(ui.customPushButton->isChecked());

  // Advanced
  ui.clipboardCheckBox->setChecked(settings.value("clipboard", true).toBool());
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

#if defined(Q_WS_WIN)
bool OptionsDialog::winEvent(MSG *message, long *result)
{

  if (message->message == WM_KEYUP)
  {
    int vk = message->wParam;

    if (vk == VK_SNAPSHOT)
    {
      QFlags<Qt::KeyboardModifier> keyboardModifiers;

      if (GetAsyncKeyState(VK_CONTROL))
        keyboardModifiers = keyboardModifiers | Qt::ControlModifier;
      if (GetAsyncKeyState(VK_SHIFT))
        keyboardModifiers = keyboardModifiers | Qt::ShiftModifier;
      if (GetAsyncKeyState(VK_LMENU))
        keyboardModifiers = keyboardModifiers | Qt::AltModifier;

      HotkeyWidget *widget = 0;

      if (ui.screenHotkeyWidget->hasFocus())
        widget = ui.screenHotkeyWidget;
      if (ui.areaHotkeyWidget->hasFocus())
        widget = ui.areaHotkeyWidget;
      if (ui.windowHotkeyWidget->hasFocus())
        widget = ui.windowHotkeyWidget;
      if (ui.openHotkeyWidget->hasFocus())
        widget = ui.openHotkeyWidget;

      if (widget)
        QCoreApplication::postEvent(widget, new QKeyEvent(QEvent::KeyPress, Qt::Key_Print, keyboardModifiers));
    }
  }

  return QDialog::winEvent(message, result);
}
#endif
