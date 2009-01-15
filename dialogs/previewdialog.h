#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QTimer>
#include "../tools/screenshot.h"

class QHBoxLayout;
class QProgressBar;
class QHttp;
class PreviewDialog : public QDialog
{
  Q_OBJECT

public:
    PreviewDialog(QWidget *parent, Screenshot::Options options);

private slots:
    void kill();
    void effect(int frame);
    void confirm();
    void screenshareFinished(int id, bool error);

protected:
    bool event(QEvent *event);

signals:
    void result(bool result, QString fileName);

private:
  QTimer deathTimer;
  Screenshot mScreenshot;
  bool mConfirmed;
  QHBoxLayout *mButtonLayout;
  QProgressBar *mProgressBar;
  QHttp *mHttp;

};

#endif // PREVIEWDIALOG_H
