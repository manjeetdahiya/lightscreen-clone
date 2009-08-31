#ifndef WINDOWPICKER_H
#define WINDOWPICKER_H

#include <QWidget>

class QLabel;
class QRubberBand;
class WindowPicker : public QWidget
{
  Q_OBJECT

public:
    WindowPicker();

signals:
    void pixmap(QPixmap pixmap);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent*);
    void focusOutEvent(QFocusEvent*);

private:
    QLabel *mWindowLabel;
    QRubberBand *mIndicator;
};

#endif // WINDOWPICKER_H
