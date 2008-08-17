/********************************************************************************
** Form generated from reading ui file 'lightscreenwindow.ui'
**
** Created: Mon Aug 11 04:40:56 2008
**      by: Qt User Interface Compiler version 4.4.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_LIGHTSCREENWINDOW_H
#define UI_LIGHTSCREENWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LightscreenWindowClass
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hboxLayout;
    QToolButton *screenshotPushButton;
    QVBoxLayout *vboxLayout;
    QPushButton *optionsPushButton;
    QPushButton *hidePushButton;
    QPushButton *quitPushButton;

    void setupUi(QDialog *LightscreenWindowClass)
    {
    if (LightscreenWindowClass->objectName().isEmpty())
        LightscreenWindowClass->setObjectName(QString::fromUtf8("LightscreenWindowClass"));
    LightscreenWindowClass->resize(179, 103);
    verticalLayout = new QVBoxLayout(LightscreenWindowClass);
    verticalLayout->setSpacing(6);
    verticalLayout->setMargin(6);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    screenshotPushButton = new QToolButton(LightscreenWindowClass);
    screenshotPushButton->setObjectName(QString::fromUtf8("screenshotPushButton"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(screenshotPushButton->sizePolicy().hasHeightForWidth());
    screenshotPushButton->setSizePolicy(sizePolicy);
    screenshotPushButton->setPopupMode(QToolButton::InstantPopup);

    hboxLayout->addWidget(screenshotPushButton);

    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    optionsPushButton = new QPushButton(LightscreenWindowClass);
    optionsPushButton->setObjectName(QString::fromUtf8("optionsPushButton"));
    optionsPushButton->setFlat(false);

    vboxLayout->addWidget(optionsPushButton);

    hidePushButton = new QPushButton(LightscreenWindowClass);
    hidePushButton->setObjectName(QString::fromUtf8("hidePushButton"));
    hidePushButton->setFlat(false);

    vboxLayout->addWidget(hidePushButton);

    quitPushButton = new QPushButton(LightscreenWindowClass);
    quitPushButton->setObjectName(QString::fromUtf8("quitPushButton"));
    quitPushButton->setFlat(false);

    vboxLayout->addWidget(quitPushButton);


    hboxLayout->addLayout(vboxLayout);


    verticalLayout->addLayout(hboxLayout);


    retranslateUi(LightscreenWindowClass);
    QObject::connect(quitPushButton, SIGNAL(clicked()), LightscreenWindowClass, SLOT(accept()));

    QMetaObject::connectSlotsByName(LightscreenWindowClass);
    } // setupUi

    void retranslateUi(QDialog *LightscreenWindowClass)
    {
    LightscreenWindowClass->setWindowTitle(QApplication::translate("LightscreenWindowClass", "Lightscreen", 0, QApplication::UnicodeUTF8));
    screenshotPushButton->setText(QApplication::translate("LightscreenWindowClass", "Screeenshot", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    optionsPushButton->setToolTip(QApplication::translate("LightscreenWindowClass", "Configure Lightscreen", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_WHATSTHIS
    optionsPushButton->setWhatsThis(QApplication::translate("LightscreenWindowClass", "Opens the configuration dialog", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS

    optionsPushButton->setText(QApplication::translate("LightscreenWindowClass", "&Options", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    hidePushButton->setToolTip(QApplication::translate("LightscreenWindowClass", "Hide Lightscreen ", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_WHATSTHIS
    hidePushButton->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS

    hidePushButton->setText(QApplication::translate("LightscreenWindowClass", "&Hide", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    quitPushButton->setToolTip(QString());
#endif // QT_NO_TOOLTIP

    quitPushButton->setText(QApplication::translate("LightscreenWindowClass", "&Quit", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(LightscreenWindowClass);
    } // retranslateUi

};

namespace Ui {
    class LightscreenWindowClass: public Ui_LightscreenWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIGHTSCREENWINDOW_H
