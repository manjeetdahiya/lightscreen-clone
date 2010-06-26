#ifndef WINDOWPICKER_H
#define WINDOWPICKER_H

#include <QWidget>

class QLabel;
class QRect;
class WindowPicker : public QWidget
{
  Q_OBJECT

public:
    WindowPicker();

signals:
    void pixmap(QPixmap pixmap);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent*);
    void focusOutEvent(QFocusEvent*);

private:
    QPixmap mCrosshair;
    QLabel *mWindowLabel;
    QLabel *mWindowIcon;
    QLabel *mCrosshairLabel;
    bool mTaken;

};

#endif // WINDOWPICKER_H
