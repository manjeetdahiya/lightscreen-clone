#ifndef UPDATER_H_
#define UPDATER_H_

#include <QObject>
#include <QHttp>
#include <QFile>

class Updater : public QObject
{
  Q_OBJECT

public:  
  enum State
  {
    Idle  = 0,
    Check = 1,
    VersionData = 2,
    Download  = 3
  };

  enum Result
  {
    Error = 0,
    NoVersion = 1,
    NewVersion = 2,
    MajorUpgrade = 3
  };

  Updater();

	static Updater *instance();

signals:
  void checkDone(Updater::Result result);
  void downloadDone(bool error);
  void downloading(QString fileName);
  void canceled(bool reminder);

public slots:
  void check();
  void check(QObject *receiver);
  void download();
  void disable();
  void cancel();
  void abort();

private slots:
  void httpDone(bool error);

private:
  QHttp mHttp;
  QFile mDownload;
  State mState;
  QString mHost;
  static Updater* mInstance;

};

#endif /*UPDATER_H_*/
