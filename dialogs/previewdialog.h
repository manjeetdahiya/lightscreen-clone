#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QTimer>
#include "../tools/screenshot.h"

class PreviewDialog : public QDialog
{
  Q_OBJECT

public:
    PreviewDialog(QWidget *parent, Screenshot::Options options);

private slots:
    void kill();
    void effect(int frame);
    void confirm();

protected:
    bool event(QEvent *event);

signals:
    void result(bool result, QString fileName);

private:
  QTimer deathTimer;
  Screenshot mScreenshot;
  bool mConfirmed;

};

#endif // PREVIEWDIALOG_H
