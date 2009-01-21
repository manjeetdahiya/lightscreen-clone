
#ifndef OS_H_
#define OS_H_

#include <QWidget>

struct QPixmap;
struct QPoint;
struct QString;

namespace os
{ //TODO: Comment functions, order alphabetically (in .cpp too)
  void aeroGlass(QWidget *target);
  void addToRecentDocuments(QString fileName);
  QPixmap cursor();
  QPixmap dxScreen();
  QPixmap grabWindow(WId winId);
  void translate(QString language);
}


#endif /*OS_H_*/
