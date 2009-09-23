#ifndef SCREENSHOTMANAGER_H
#define SCREENSHOTMANAGER_H

#include <QObject>
#include <QList>

#include "screenshot.h"

class ScreenshotManager : public QObject
{
  Q_OBJECT

public:
  enum State
  {
    Idle  = 0,
    Busy = 1,
    Disabled  = 2
  };

public:
  ScreenshotManager(QObject *parent);
  static ScreenshotManager *instance();

  void setCount(const unsigned int count){ mCount = count; }
  unsigned int count() const {return mCount;}

public slots:
  void take(Screenshot::Options options);
  void askConfirmation();
  void cleanup();

signals:
  void confirm(Screenshot* screenshot);
  void windowCleanup(Screenshot::Options options);

private:
  static ScreenshotManager* mInstance;
  int mCount; // Concurrent screenshot count.

};

#endif // SCREENSHOTMANAGER_H
