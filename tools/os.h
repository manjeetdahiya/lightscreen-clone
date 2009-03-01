
#ifndef OS_H_
#define OS_H_

#include <QWidget>

struct QPixmap;
struct QPoint;
struct QString;

namespace os
{
  // Gives the target widget a full aero glass background
  bool aeroGlass(QWidget *target);
  // Adds the filename to the Windows recent document list (useful for Windows 7 jump lists)
  void addToRecentDocuments(QString fileName);
  // Set the target window as the foreground window (Windows only)
  void setForegroundWindow(QWidget *window);
  // Returns the cursor pixmap in Windows
  QPixmap cursor();
  // Returns the pixmap of the given window id.
  QPixmap grabWindow(WId winId);
  // Translates the ui to the given language name.
  void translate(QString language);
}


#endif /*OS_H_*/
