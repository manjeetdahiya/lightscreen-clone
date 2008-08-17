/********************************************************************************
** Form generated from reading ui file 'optionsdialog.ui'
**
** Created: Mon Aug 11 04:40:56 2008
**      by: Qt User Interface Compiler version 4.4.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_OPTIONSDIALOG_H
#define UI_OPTIONSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "../widgets/hotkeywidget.h"

QT_BEGIN_NAMESPACE

class Ui_OptionsDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *generalTab;
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLineEdit *targetLineEdit;
    QLabel *label_2;
    QPushButton *browsePushButton;
    QLabel *label;
    QComboBox *formatComboBox;
    QLabel *label_6;
    QSpinBox *delaySpinBox;
    QHBoxLayout *hboxLayout;
    QLineEdit *prefixLineEdit;
    QComboBox *namingComboBox;
    QPushButton *flipPrefixPushButton;
    QCheckBox *startupCheckBox;
    QCheckBox *startupHideCheckBox;
    QWidget *hotkeyTab;
    QHBoxLayout *horizontalLayout_3;
    QGridLayout *gridLayout1;
    QCheckBox *screenCheckBox;
    HotkeyWidget *screenHotkeyWidget;
    QCheckBox *windowCheckBox;
    HotkeyWidget *windowHotkeyWidget;
    QCheckBox *areaCheckBox;
    HotkeyWidget *areaHotkeyWidget;
    QFrame *frame;
    QCheckBox *openCheckBox;
    HotkeyWidget *openHotkeyWidget;
    QCheckBox *directoryCheckBox;
    HotkeyWidget *directoryHotkeyWidget;
    QWidget *moreTab;
    QVBoxLayout *verticalLayout;
    QCheckBox *trayCheckBox;
    QCheckBox *hideCheckBox;
    QCheckBox *optiPngCheckBox;
    QCheckBox *vistaGlassCheckBox;
    QCheckBox *dxScreenCheckBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QCheckBox *messageCheckBox;
    QCheckBox *playSoundCheckBox;
    QGridLayout *gridLayout2;
    QSlider *qualitySlider;
    QLabel *label_10;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *OptionsDialog)
    {
    if (OptionsDialog->objectName().isEmpty())
        OptionsDialog->setObjectName(QString::fromUtf8("OptionsDialog"));
    OptionsDialog->resize(295, 264);
    verticalLayout_2 = new QVBoxLayout(OptionsDialog);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    tabWidget = new QTabWidget(OptionsDialog);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    generalTab = new QWidget();
    generalTab->setObjectName(QString::fromUtf8("generalTab"));
    generalTab->setGeometry(QRect(0, 0, 271, 188));
    vboxLayout = new QVBoxLayout(generalTab);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label_4 = new QLabel(generalTab);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_4, 0, 0, 1, 1);

    targetLineEdit = new QLineEdit(generalTab);
    targetLineEdit->setObjectName(QString::fromUtf8("targetLineEdit"));
    targetLineEdit->setStyleSheet(QString::fromUtf8(""));

    gridLayout->addWidget(targetLineEdit, 0, 1, 1, 1);

    label_2 = new QLabel(generalTab);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    browsePushButton = new QPushButton(generalTab);
    browsePushButton->setObjectName(QString::fromUtf8("browsePushButton"));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/icons/OpenFolder")), QIcon::Normal, QIcon::Off);
    browsePushButton->setIcon(icon);
    browsePushButton->setAutoDefault(false);

    gridLayout->addWidget(browsePushButton, 0, 2, 1, 1);

    label = new QLabel(generalTab);
    label->setObjectName(QString::fromUtf8("label"));
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label, 2, 0, 1, 1);

    formatComboBox = new QComboBox(generalTab);
    formatComboBox->setObjectName(QString::fromUtf8("formatComboBox"));

    gridLayout->addWidget(formatComboBox, 2, 1, 1, 1);

    label_6 = new QLabel(generalTab);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_6, 3, 0, 1, 1);

    delaySpinBox = new QSpinBox(generalTab);
    delaySpinBox->setObjectName(QString::fromUtf8("delaySpinBox"));
    delaySpinBox->setAccelerated(true);
    delaySpinBox->setMaximum(32767);

    gridLayout->addWidget(delaySpinBox, 3, 1, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    prefixLineEdit = new QLineEdit(generalTab);
    prefixLineEdit->setObjectName(QString::fromUtf8("prefixLineEdit"));

    hboxLayout->addWidget(prefixLineEdit);

    namingComboBox = new QComboBox(generalTab);
    namingComboBox->setObjectName(QString::fromUtf8("namingComboBox"));

    hboxLayout->addWidget(namingComboBox);


    gridLayout->addLayout(hboxLayout, 1, 1, 1, 1);

    flipPrefixPushButton = new QPushButton(generalTab);
    flipPrefixPushButton->setObjectName(QString::fromUtf8("flipPrefixPushButton"));
    QIcon icon1;
    icon1.addPixmap(QPixmap(QString::fromUtf8(":/icons/PrefixLeft")), QIcon::Normal, QIcon::Off);
    flipPrefixPushButton->setIcon(icon1);
    flipPrefixPushButton->setCheckable(true);
    flipPrefixPushButton->setAutoDefault(false);

    gridLayout->addWidget(flipPrefixPushButton, 1, 2, 1, 1);


    vboxLayout->addLayout(gridLayout);

    startupCheckBox = new QCheckBox(generalTab);
    startupCheckBox->setObjectName(QString::fromUtf8("startupCheckBox"));

    vboxLayout->addWidget(startupCheckBox);

    startupHideCheckBox = new QCheckBox(generalTab);
    startupHideCheckBox->setObjectName(QString::fromUtf8("startupHideCheckBox"));

    vboxLayout->addWidget(startupHideCheckBox);

    tabWidget->addTab(generalTab, QString());
    hotkeyTab = new QWidget();
    hotkeyTab->setObjectName(QString::fromUtf8("hotkeyTab"));
    hotkeyTab->setGeometry(QRect(8, 28, 271, 188));
    horizontalLayout_3 = new QHBoxLayout(hotkeyTab);
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    gridLayout1 = new QGridLayout();
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    screenCheckBox = new QCheckBox(hotkeyTab);
    screenCheckBox->setObjectName(QString::fromUtf8("screenCheckBox"));

    gridLayout1->addWidget(screenCheckBox, 0, 0, 1, 1);

    screenHotkeyWidget = new HotkeyWidget(hotkeyTab);
    screenHotkeyWidget->setObjectName(QString::fromUtf8("screenHotkeyWidget"));
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(screenHotkeyWidget->sizePolicy().hasHeightForWidth());
    screenHotkeyWidget->setSizePolicy(sizePolicy);
    screenHotkeyWidget->setCheckable(false);

    gridLayout1->addWidget(screenHotkeyWidget, 0, 1, 1, 1);

    windowCheckBox = new QCheckBox(hotkeyTab);
    windowCheckBox->setObjectName(QString::fromUtf8("windowCheckBox"));

    gridLayout1->addWidget(windowCheckBox, 1, 0, 1, 1);

    windowHotkeyWidget = new HotkeyWidget(hotkeyTab);
    windowHotkeyWidget->setObjectName(QString::fromUtf8("windowHotkeyWidget"));
    windowHotkeyWidget->setCheckable(false);

    gridLayout1->addWidget(windowHotkeyWidget, 1, 1, 1, 1);

    areaCheckBox = new QCheckBox(hotkeyTab);
    areaCheckBox->setObjectName(QString::fromUtf8("areaCheckBox"));

    gridLayout1->addWidget(areaCheckBox, 2, 0, 1, 1);

    areaHotkeyWidget = new HotkeyWidget(hotkeyTab);
    areaHotkeyWidget->setObjectName(QString::fromUtf8("areaHotkeyWidget"));
    areaHotkeyWidget->setCheckable(false);

    gridLayout1->addWidget(areaHotkeyWidget, 2, 1, 1, 1);

    frame = new QFrame(hotkeyTab);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);

    gridLayout1->addWidget(frame, 3, 0, 1, 2);

    openCheckBox = new QCheckBox(hotkeyTab);
    openCheckBox->setObjectName(QString::fromUtf8("openCheckBox"));

    gridLayout1->addWidget(openCheckBox, 4, 0, 1, 1);

    openHotkeyWidget = new HotkeyWidget(hotkeyTab);
    openHotkeyWidget->setObjectName(QString::fromUtf8("openHotkeyWidget"));
    openHotkeyWidget->setCheckable(false);

    gridLayout1->addWidget(openHotkeyWidget, 4, 1, 1, 1);

    directoryCheckBox = new QCheckBox(hotkeyTab);
    directoryCheckBox->setObjectName(QString::fromUtf8("directoryCheckBox"));

    gridLayout1->addWidget(directoryCheckBox, 5, 0, 1, 1);

    directoryHotkeyWidget = new HotkeyWidget(hotkeyTab);
    directoryHotkeyWidget->setObjectName(QString::fromUtf8("directoryHotkeyWidget"));
    directoryHotkeyWidget->setCheckable(false);

    gridLayout1->addWidget(directoryHotkeyWidget, 5, 1, 1, 1);


    horizontalLayout_3->addLayout(gridLayout1);

    tabWidget->addTab(hotkeyTab, QString());
    moreTab = new QWidget();
    moreTab->setObjectName(QString::fromUtf8("moreTab"));
    moreTab->setGeometry(QRect(0, 0, 271, 188));
    verticalLayout = new QVBoxLayout(moreTab);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    trayCheckBox = new QCheckBox(moreTab);
    trayCheckBox->setObjectName(QString::fromUtf8("trayCheckBox"));

    verticalLayout->addWidget(trayCheckBox);

    hideCheckBox = new QCheckBox(moreTab);
    hideCheckBox->setObjectName(QString::fromUtf8("hideCheckBox"));

    verticalLayout->addWidget(hideCheckBox);

    optiPngCheckBox = new QCheckBox(moreTab);
    optiPngCheckBox->setObjectName(QString::fromUtf8("optiPngCheckBox"));

    verticalLayout->addWidget(optiPngCheckBox);

    vistaGlassCheckBox = new QCheckBox(moreTab);
    vistaGlassCheckBox->setObjectName(QString::fromUtf8("vistaGlassCheckBox"));

    verticalLayout->addWidget(vistaGlassCheckBox);

    dxScreenCheckBox = new QCheckBox(moreTab);
    dxScreenCheckBox->setObjectName(QString::fromUtf8("dxScreenCheckBox"));

    verticalLayout->addWidget(dxScreenCheckBox);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    label_3 = new QLabel(moreTab);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    horizontalLayout->addWidget(label_3);

    messageCheckBox = new QCheckBox(moreTab);
    messageCheckBox->setObjectName(QString::fromUtf8("messageCheckBox"));

    horizontalLayout->addWidget(messageCheckBox);

    playSoundCheckBox = new QCheckBox(moreTab);
    playSoundCheckBox->setObjectName(QString::fromUtf8("playSoundCheckBox"));

    horizontalLayout->addWidget(playSoundCheckBox);


    verticalLayout->addLayout(horizontalLayout);

    gridLayout2 = new QGridLayout();
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    qualitySlider = new QSlider(moreTab);
    qualitySlider->setObjectName(QString::fromUtf8("qualitySlider"));
    qualitySlider->setMaximum(100);
    qualitySlider->setValue(100);
    qualitySlider->setOrientation(Qt::Horizontal);

    gridLayout2->addWidget(qualitySlider, 0, 2, 1, 1);

    label_10 = new QLabel(moreTab);
    label_10->setObjectName(QString::fromUtf8("label_10"));

    gridLayout2->addWidget(label_10, 0, 1, 1, 1);


    verticalLayout->addLayout(gridLayout2);

    tabWidget->addTab(moreTab, QString());

    verticalLayout_2->addWidget(tabWidget);

    buttonBox = new QDialogButtonBox(OptionsDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::RestoreDefaults);

    verticalLayout_2->addWidget(buttonBox);

    label_4->setBuddy(targetLineEdit);
    label_2->setBuddy(prefixLineEdit);
    label->setBuddy(formatComboBox);
    label_6->setBuddy(delaySpinBox);
    label_3->setBuddy(messageCheckBox);
    label_10->setBuddy(qualitySlider);
    QWidget::setTabOrder(tabWidget, targetLineEdit);
    QWidget::setTabOrder(targetLineEdit, browsePushButton);
    QWidget::setTabOrder(browsePushButton, prefixLineEdit);
    QWidget::setTabOrder(prefixLineEdit, namingComboBox);
    QWidget::setTabOrder(namingComboBox, flipPrefixPushButton);
    QWidget::setTabOrder(flipPrefixPushButton, formatComboBox);
    QWidget::setTabOrder(formatComboBox, delaySpinBox);
    QWidget::setTabOrder(delaySpinBox, startupCheckBox);
    QWidget::setTabOrder(startupCheckBox, startupHideCheckBox);
    QWidget::setTabOrder(startupHideCheckBox, screenCheckBox);
    QWidget::setTabOrder(screenCheckBox, screenHotkeyWidget);
    QWidget::setTabOrder(screenHotkeyWidget, windowCheckBox);
    QWidget::setTabOrder(windowCheckBox, windowHotkeyWidget);
    QWidget::setTabOrder(windowHotkeyWidget, areaCheckBox);
    QWidget::setTabOrder(areaCheckBox, areaHotkeyWidget);
    QWidget::setTabOrder(areaHotkeyWidget, openCheckBox);
    QWidget::setTabOrder(openCheckBox, openHotkeyWidget);
    QWidget::setTabOrder(openHotkeyWidget, directoryCheckBox);
    QWidget::setTabOrder(directoryCheckBox, directoryHotkeyWidget);
    QWidget::setTabOrder(directoryHotkeyWidget, trayCheckBox);
    QWidget::setTabOrder(trayCheckBox, hideCheckBox);
    QWidget::setTabOrder(hideCheckBox, optiPngCheckBox);
    QWidget::setTabOrder(optiPngCheckBox, vistaGlassCheckBox);
    QWidget::setTabOrder(vistaGlassCheckBox, dxScreenCheckBox);
    QWidget::setTabOrder(dxScreenCheckBox, messageCheckBox);
    QWidget::setTabOrder(messageCheckBox, playSoundCheckBox);
    QWidget::setTabOrder(playSoundCheckBox, qualitySlider);
    QWidget::setTabOrder(qualitySlider, buttonBox);

    retranslateUi(OptionsDialog);
    QObject::connect(buttonBox, SIGNAL(rejected()), OptionsDialog, SLOT(reject()));

    tabWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(OptionsDialog);
    } // setupUi

    void retranslateUi(QDialog *OptionsDialog)
    {
    OptionsDialog->setWindowTitle(QApplication::translate("OptionsDialog", "Options - Lightscreen", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("OptionsDialog", "Directory:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    label_2->setToolTip(QApplication::translate("OptionsDialog", "The prefix for the screenshot file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    label_2->setText(QApplication::translate("OptionsDialog", "Filename:", 0, QApplication::UnicodeUTF8));
    browsePushButton->setText(QString());

#ifndef QT_NO_TOOLTIP
    label->setToolTip(QApplication::translate("OptionsDialog", "The file format for the screenshot", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    label->setText(QApplication::translate("OptionsDialog", "Format:", 0, QApplication::UnicodeUTF8));
    formatComboBox->clear();
    formatComboBox->insertItems(0, QStringList()
     << QApplication::translate("OptionsDialog", "PNG", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("OptionsDialog", "JPG", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("OptionsDialog", "BMP", 0, QApplication::UnicodeUTF8)
    );

#ifndef QT_NO_WHATSTHIS
    formatComboBox->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS

    label_6->setText(QApplication::translate("OptionsDialog", "Delay:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_WHATSTHIS
    delaySpinBox->setWhatsThis(QApplication::translate("OptionsDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\">Selecting anything other than 0 in this option will cause the program to <span style=\" font-weight:600;\">wait</span> that amount of seconds before taking the screenshot.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS

    delaySpinBox->setSpecialValueText(QApplication::translate("OptionsDialog", "no delay", 0, QApplication::UnicodeUTF8));
    delaySpinBox->setSuffix(QApplication::translate("OptionsDialog", " seconds", 0, QApplication::UnicodeUTF8));
    delaySpinBox->setPrefix(QString());

#ifndef QT_NO_WHATSTHIS
    prefixLineEdit->setWhatsThis(QApplication::translate("OptionsDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\">The prefix will be inserted before the <span style=\" font-style:italic;\">Naming</span> in the screenshot file and it is usually used to distinguish files.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\">It <span style=\" font-style:italic;\">can</span> be left blank.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS

    namingComboBox->clear();
    namingComboBox->insertItems(0, QStringList()
     << QApplication::translate("OptionsDialog", "(number)", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("OptionsDialog", "(timestamp)", 0, QApplication::UnicodeUTF8)
    );

#ifndef QT_NO_WHATSTHIS
    namingComboBox->setWhatsThis(QApplication::translate("OptionsDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\">The naming is inserted after the prefix and is what makes each screenshot file unique to avoid overwriting.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-weight:600;\">Numeric</span>: inserts a number in sequence, <span style=\" font-style:italic;\">1, 2, 3</span>.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-weight:600;\">Timestamp</span>: inserts a unique number (a Unix timestamp) that corresponds with the"
        " current time.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS


#ifndef QT_NO_WHATSTHIS
    startupCheckBox->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS

    startupCheckBox->setText(QApplication::translate("OptionsDialog", "&Start Lightscreen with Windows", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_WHATSTHIS
    startupHideCheckBox->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS

    startupHideCheckBox->setText(QApplication::translate("OptionsDialog", "&Hide the main window on startup.", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(generalTab), QApplication::translate("OptionsDialog", "General", 0, QApplication::UnicodeUTF8));
    screenCheckBox->setText(QApplication::translate("OptionsDialog", "Capture the screen", 0, QApplication::UnicodeUTF8));
    screenHotkeyWidget->setText(QString());
    windowCheckBox->setText(QApplication::translate("OptionsDialog", "Capture a window", 0, QApplication::UnicodeUTF8));
    windowHotkeyWidget->setText(QString());
    areaCheckBox->setText(QApplication::translate("OptionsDialog", "Capture a screen area", 0, QApplication::UnicodeUTF8));
    areaHotkeyWidget->setText(QString());
    openCheckBox->setText(QApplication::translate("OptionsDialog", "Open the program window", 0, QApplication::UnicodeUTF8));
    openHotkeyWidget->setText(QString());
    directoryCheckBox->setText(QApplication::translate("OptionsDialog", "Open the directory", 0, QApplication::UnicodeUTF8));
    directoryHotkeyWidget->setText(QString());
    tabWidget->setTabText(tabWidget->indexOf(hotkeyTab), QApplication::translate("OptionsDialog", "Hotkeys", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_WHATSTHIS
    trayCheckBox->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS

    trayCheckBox->setText(QApplication::translate("OptionsDialog", "Sho&w a system tray icon.", 0, QApplication::UnicodeUTF8));
    hideCheckBox->setText(QApplication::translate("OptionsDialog", "&Hide Lightscreen while taking a screenshot.", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    optiPngCheckBox->setToolTip(QApplication::translate("OptionsDialog", "Opens a new process called OptiPNG which dramatically reduces the file size.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    optiPngCheckBox->setText(QApplication::translate("OptionsDialog", "Optimize PNG screenshots.", 0, QApplication::UnicodeUTF8));
    vistaGlassCheckBox->setText(QApplication::translate("OptionsDialog", "Use transparent windows.", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    dxScreenCheckBox->setToolTip(QApplication::translate("OptionsDialog", "DirectX screenshots can be useful for capturing some games which would not work using the normal method.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    dxScreenCheckBox->setText(QApplication::translate("OptionsDialog", "Use Direct&X for screenshots (experimental).", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("OptionsDialog", "Notify with:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_WHATSTHIS
    messageCheckBox->setWhatsThis(QApplication::translate("OptionsDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Shows a completion message once the screenshot is saved, clicking this message takes you to the directory where the screenshot was saved.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS

    messageCheckBox->setText(QApplication::translate("OptionsDialog", "Tray icon Popup", 0, QApplication::UnicodeUTF8));
    playSoundCheckBox->setText(QApplication::translate("OptionsDialog", "&Sound cue", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_WHATSTHIS
    qualitySlider->setWhatsThis(QApplication::translate("OptionsDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">This slider goes from 0 to 100. 100 being the highest quality and 0 the lowest.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Quality is related to file size and of course to readability and overall quality of the image.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS

    label_10->setText(QApplication::translate("OptionsDialog", "Screenshot Quality:", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(moreTab), QApplication::translate("OptionsDialog", "Options", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(OptionsDialog);
    } // retranslateUi

};

namespace Ui {
    class OptionsDialog: public Ui_OptionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONSDIALOG_H
