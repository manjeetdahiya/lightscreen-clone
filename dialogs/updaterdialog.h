#ifndef UPDATERDIALOG_H
#define UPDATERDIALOG_H

#include <QProgressDialog>
#include "../updater/updater.h"

class QProgressBar;
class QDialogButtonBox;
class QLabel;
class QAbstractButton;
class UpdaterDialog : public QDialog
{
  Q_OBJECT

public:
  UpdaterDialog(bool silent);

signals:
  void download();
  void disable();
  void cancel();

public slots:
  void checkDone(Updater::Result result);
  void downloadDone(bool error);
  void downloading(QString file);
  void progressBar(int value, int maximum);
  void setLabelText(QString text);
  void yes();
  void button(QAbstractButton* button);

private slots:
  void link(QString url);

private:
  QProgressBar     *mProgressBar;
  QDialogButtonBox *mButtonBox;
  QLabel           *mLabel;
  QLabel           *mProgressLabel;
  bool              mSilent;
};

#endif // UPDATERDIALOG_H
