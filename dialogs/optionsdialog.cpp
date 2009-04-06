#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QUrl>

#if defined(Q_WS_WIN)
  #include <windows.h>
#endif

#include "optionsdialog.h"
#include "../tools/os.h"

#include "../updater/updater.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
  QDialog(parent)
{
  ui.setupUi(this);
  setModal(true);

  if (os::aeroGlass(this))
  {
    ui.tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #898c95; border-radius: 5px; background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:0.988636, y2:0.608, stop:0 rgba(235, 235, 235, 255), stop:1 rgba(255, 255, 255, 255)); }  QTabWidget::tab-bar { left: 5px; }");
    layout()->setMargin(3);
    resize(minimumSizeHint());
  }

#if !defined(Q_WS_WIN)
  ui.playSoundCheckBox->setVisible(false);
  ui.playSoundCheckBox->setChecked(false);
  ui.cursorCheckBox->setVisible(false);
  ui.cursorCheckBox->setChecked(false);
#endif

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

  initConnections();
  loadSettings();
}

void OptionsDialog::initConnections()
{
  connect(ui.buttonBox              , SIGNAL(clicked(QAbstractButton*)), this    , SLOT(dialogButtonClicked(QAbstractButton*)));
  connect(ui.buttonBox              , SIGNAL(accepted())               , this    , SLOT(accepted()));
  connect(ui.buttonBox              , SIGNAL(rejected())               , this    , SLOT(rejected()));
  connect(ui.flipPrefixPushButton   , SIGNAL(toggled(bool))            , this    , SLOT(flipButtonToggled(bool)));
  connect(ui.browsePushButton       , SIGNAL(clicked())                , this    , SLOT(browse()));
  connect(ui.aboutPushButton        , SIGNAL(clicked())                , parent(), SLOT(showAbout()));
  connect(ui.checkUpdatesPushButton , SIGNAL(clicked())                , this    , SLOT(checkUpdatesNow()));

  connect(ui.screenCheckBox   , SIGNAL(toggled(bool)), ui.screenHotkeyWidget   , SLOT(setEnabled(bool)));
  connect(ui.areaCheckBox     , SIGNAL(toggled(bool)), ui.areaHotkeyWidget     , SLOT(setEnabled(bool)));
  connect(ui.windowCheckBox   , SIGNAL(toggled(bool)), ui.windowHotkeyWidget   , SLOT(setEnabled(bool)));
  connect(ui.openCheckBox     , SIGNAL(toggled(bool)), ui.openHotkeyWidget     , SLOT(setEnabled(bool)));
  connect(ui.directoryCheckBox, SIGNAL(toggled(bool)), ui.directoryHotkeyWidget, SLOT(setEnabled(bool)));
  connect(ui.saveAsCheckBox   , SIGNAL(toggled(bool)), ui.targetLineEdit       , SLOT(setDisabled(bool)));
  connect(ui.saveAsCheckBox   , SIGNAL(toggled(bool)), ui.browsePushButton     , SLOT(setDisabled(bool)));
  connect(ui.saveAsCheckBox   , SIGNAL(toggled(bool)), ui.directoryLabel       , SLOT(setDisabled(bool)));
  connect(ui.startupCheckBox  , SIGNAL(toggled(bool)), ui.startupHideCheckBox  , SLOT(setEnabled(bool)));
  connect(ui.qualitySlider    , SIGNAL(valueChanged(int)), ui.qualityValueLabel, SLOT(setNum(int)));
  connect(ui.trayCheckBox     , SIGNAL(toggled(bool)), ui.messageCheckBox      , SLOT(setEnabled(bool)));

  connect(ui.moreInformationLabel, SIGNAL(linkActivated(QString))      , this, SLOT(link(QString)));
  connect(ui.languageComboBox    , SIGNAL(currentIndexChanged(QString)), this, SLOT(languageChange(QString)));
}
/*
 * Slots
 */

void OptionsDialog::accepted()
{
  if (hotkeyCollision())
  {
    QMessageBox::critical(this, tr("Hotkey conflict"), tr("You have assigned the same hotkeys to more than one action."));
    return;
  }

  if (ui.prefixLineEdit->text().contains(QRegExp("[?:\\\\/*\"<>|]")))
  {
    QMessageBox::critical(this, tr("Filename character error"), tr("The filename can't contain any of the following characters: ? : \\ / * \" < > |"));
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
  Updater::instance()->checkWithFeedback();
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
    loadSettings();
  }
}

void OptionsDialog::flipButtonToggled(bool checked)
{
  setUpdatesEnabled(false);

  ui.filenameLayout->removeWidget(ui.prefixLineEdit);
  ui.filenameLayout->removeWidget(ui.namingComboBox);

  if (checked)
  {
    ui.flipPrefixPushButton->setIcon(QIcon(":/icons/PrefixRight"));
    ui.filenameLayout->addWidget(ui.namingComboBox);
    ui.filenameLayout->addWidget(ui.prefixLineEdit);
  }
  else
  {
    ui.flipPrefixPushButton->setIcon(QIcon(":/icons/PrefixLeft"));
    ui.filenameLayout->addWidget(ui.prefixLineEdit);
    ui.filenameLayout->addWidget(ui.namingComboBox);
  }

  setUpdatesEnabled(true); // Avoids flicker
}

void OptionsDialog::languageChange(QString language)
{
  os::translate(language);
}

void OptionsDialog::link(QString url)
{
  QDesktopServices::openUrl(url);
}

void OptionsDialog::rejected()
{
  languageChange(QSettings().value("options/language").toString()); // Revert language to default.
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

/*
 * Private
 */
void OptionsDialog::loadSettings()
{
  QSettings settings;

  if (!settings.contains("file/format"))
  { // If there are no settings, get rid of the cancel button so that the user is forced to save them
    ui.buttonBox->clear();
    ui.buttonBox->addButton(QDialogButtonBox::Ok);

    // Move the first option window to the center of the screen, since Windows usually positions it in a random location since it has no visible parent.
    if (!(static_cast<QWidget*>(parent())->isVisible()))
      move(qApp->desktop()->screen(qApp->desktop()->primaryScreen())->rect().center()-QPoint(height()/2, width()/2));
  }

  ui.startupCheckBox->toggle();
  ui.trayCheckBox->toggle();

  settings.beginGroup("file");
  ui.formatComboBox->setCurrentIndex(settings.value("format", 1).toInt());
  ui.prefixLineEdit->setText(settings.value("prefix", "screenshot.").toString());
  ui.namingComboBox->setCurrentIndex(settings.value("naming", 0).toInt());
  ui.targetLineEdit->setText(settings.value("target", os::getDocumentsPath() + QDir::separator() + "screenshots").toString()); // Defaults to $HOME$/screenshots
  ui.fileGroupBox->setChecked(settings.value("enabled", true).toBool());
  settings.endGroup();

  settings.beginGroup("options");
  ui.startupCheckBox->setChecked(settings.value("startup", false).toBool());
  ui.startupHideCheckBox->setChecked(settings.value("startupHide", false).toBool());
  ui.hideCheckBox->setChecked(settings.value("hide", true).toBool());
  ui.delaySpinBox->setValue(settings.value("delay", 0).toInt());
  ui.flipPrefixPushButton->setChecked(settings.value("flip", false).toBool());
  ui.trayCheckBox->setChecked(settings.value("tray", true).toBool());
  ui.messageCheckBox->setChecked(settings.value("message").toBool());
  ui.qualitySlider->setValue(settings.value("quality", 100).toInt());
  ui.playSoundCheckBox->setChecked(settings.value("playSound", false).toBool());
  ui.updaterCheckBox->setChecked(!settings.value("disableUpdater", false).toBool());
  ui.magnifyCheckBox->setChecked(settings.value("magnify", false).toBool());
  ui.cursorCheckBox->setChecked(settings.value("cursor", false).toBool());
  ui.saveAsCheckBox->setChecked(settings.value("saveAs", false).toBool());

  // Advanced
  ui.clipboardCheckBox->setChecked(settings.value("clipboard", false).toBool());
  ui.optiPngCheckBox->setChecked(settings.value("optipng", true).toBool());
  ui.warnHideCheckBox->setChecked(!settings.value("disableHideAlert", false).toBool());
  ui.currentMonitorCheckBox->setChecked(settings.value("currentMonitor", false).toBool());

#if defined(Q_WS_WIN)
  if (QFile::exists("optipng.exe"))
  {
    ui.optiPngCheckBox->setEnabled(true);
  }
  else
  {
    ui.optiPngCheckBox->setEnabled(false);
    ui.optiPngCheckBox->setText(ui.optiPngCheckBox->text() + tr(" (OptiPNG not found)"));
  }
#endif

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
  ui.windowHotkeyWidget->setHotkey(settings.value("hotkey", QKeySequence(Qt::ALT + Qt::Key_Print)).value<QKeySequence> ());
  settings.endGroup();

  settings.beginGroup("open");
  ui.openCheckBox->setChecked(settings.value("enabled").toBool());
  ui.openHotkeyWidget->setHotkey(settings.value("hotkey", QKeySequence(Qt::CTRL + Qt::Key_PageUp)).value<QKeySequence> ());
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

bool OptionsDialog::event(QEvent* event)
{
  if (event->type() == QEvent::LanguageChange)
  {
    ui.retranslateUi(this);
    resize(minimumSizeHint());
  }

  return QDialog::event(event);
}

#if defined(Q_WS_WIN)
// Qt does not send the print screen key as a regular QKeyPress event, so we must use the Windows API
bool OptionsDialog::winEvent(MSG *message, long *result)
{
  if (message->message == WM_KEYUP || message->message == WM_SYSKEYUP)
  {
    if (message->wParam == VK_SNAPSHOT)
        qApp->postEvent(qApp->focusWidget(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Print,  qApp->keyboardModifiers()));
  }

  return QDialog::winEvent(message, result);
}
#endif
