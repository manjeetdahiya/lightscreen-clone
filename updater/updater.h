#ifndef UPDATER_H_
#define UPDATER_H_

#include <QObject>
#include <QHttp>

class Updater : public QObject
{
  Q_OBJECT
public:
	Updater(QObject *parent = 0);

	bool check();
	
signals:
  void done(bool result); 
	
private slots:
  void httpDone(bool result);

private:
  QHttp mHttp;

};

#endif /*UPDATER_H_*/
