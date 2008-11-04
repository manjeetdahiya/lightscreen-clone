/*
 * ScreenshotEngine:
 *
 * This class handles the screenshots, access to the screen and options.
 *
 */
#ifndef SCREENSHOTENGINE_H_
#define SCREENSHOTENGINE_H_

#include <QObject>
#include <QString>
#include <QDir>
#include <QPixmap>


class ScreenshotEngine : public QObject
{
  Q_OBJECT

public:
  enum Format
  {
    PNG = 0,
    JPEG,
    BMP,
    TIFF
  };

  enum Naming
  {
    Numeric = 0,
    Timestamp
  };

  enum Mode
  {
    WholeScreen = 0,
    ActiveWindow,
    SelectedArea
  };

  struct Options
  {
    int format;
    QString prefix;
    int naming;
    QDir directory;
    int mode;
    int quality;
    bool flipNaming;
    bool directX;
  };

  struct Result
  {
    Options options;
    bool result;
    QString fileName;
  };

    ScreenshotEngine();

    QPixmap getActiveWindow();
    QString getFileName(Options options);
    char* getFormat(int format);
    QPixmap getSelectedArea();
    QPixmap getWholeScreen(bool directx = false);

    ScreenshotEngine::Result lastScreenshot();

    void setEnabled(bool enabled);
    bool isEnabled();

public slots:
  ScreenshotEngine::Result take(ScreenshotEngine::Options options);

private:
  ScreenshotEngine::Result mLastResult;
  bool mEnabled;

};

#endif /*SCREENSHOTENGINE_H_*/
