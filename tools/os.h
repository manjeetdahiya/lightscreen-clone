
#ifndef OS_H_
#define OS_H_

#include <QObject>

struct QPixmap;
struct QPoint;
namespace os
{ //TODO: Comment functions
  QPixmap dxScreen();
  QPixmap grabWindow(WId winId);
  QPixmap cursor();
  QPoint mousePosition();
  void translate(QString language);
  void vistaGlass(QWidget *target);

}


#endif /*OS_H_*/
