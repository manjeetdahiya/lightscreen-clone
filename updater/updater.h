#ifndef UPDATER_H_
#define UPDATER_H_

#include <QObject>
#include <QPointer>
#include <QNetworkAccessManager>
#include <QFile>

class QNetworkReply;
class Updater : public QObject
{
  Q_OBJECT

public:  
  enum State
  {
    Idle  = 0,
    Check = 1,
    VersionData = 2,
    Redirect = 3,
    Download  = 4
  };

  enum Result
  {
    Error = 0,
    NoVersion = 1,
    NewVersion = 2,
    MajorUpgrade = 3
  };

  Updater();

  QString changelog();
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
  void abort();

private slots:
  void requestFinished(QNetworkReply* reply);
  void downloadFinished();
  void downloadReadyRead();

private:
  QPointer<QNetworkAccessManager> mNetwork;
  QPointer<QObject> mController;
  QPointer<QNetworkReply> mDownload;
  QFile mDownloadFile;

  State mState;
  static Updater* mInstance;

};

#endif /*UPDATER_H_*/
