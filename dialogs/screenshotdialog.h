#ifndef SCREENSHOTDIALOG_H
#define SCREENSHOTDIALOG_H

#include <QDialog>
#include <QPoint>

class Screenshot;
class QScrollArea;
class QLabel;
class ScreenshotDialog : public QDialog
{
public:
    ScreenshotDialog(Screenshot *screenshot, QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QScrollArea *mScrollArea;
    QLabel *mLabel;
    QPoint mMousePos;
};

#endif // SCREENSHOTDIALOG_H
