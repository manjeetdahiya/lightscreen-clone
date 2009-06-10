/*
 * Based on KDE's KSnapshot regiongrabber.h, revision 772337, Copyright 2007 Luca Gugelmann <lucag@student.ethz.ch>
 * released under the GNU LGPL  <http://www.gnu.org/licenses/old-licenses/library.txt>
 */

#ifndef AREASELECTOR_H
#define AREASELECTOR_H

#include <QDialog>
#include <QRegion>
#include <QPoint>
#include <QVector>
#include <QRect>
#include <QTimer>

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;

class AreaSelector : public QDialog
{
    Q_OBJECT
public:
    AreaSelector(QWidget* parent, QPixmap pixmap, bool magnify);
    ~AreaSelector();
    QPixmap pixmap;

protected slots:
    void displayHelp();
    void grabRect();
    void cancel();

signals:
    void regionGrabbed( const QPixmap & );

protected:
    void paintEvent( QPaintEvent* e );
    void resizeEvent( QResizeEvent* e );
    void mousePressEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseDoubleClickEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* e );
    void updateHandles();
    QRegion handleMask() const;
    QPoint limitPointToRect( const QPoint &p, const QRect &r ) const;

    bool magnify;
    QPoint mousePos;
    QRect selection;
    bool mouseDown;
    bool newSelection;
    const int handleSize;
    QRect* mouseOverHandle;
    QPoint dragStartPoint;
    QRect  selectionBeforeDrag;
    QTimer idleTimer;
    bool showHelp;
    bool grabbing;

    // naming convention for handles
    // T top, B bottom, R Right, L left
    // 2 letters: a corner
    // 1 letter: the handle on the middle of the corresponding side
    QRect TLHandle, TRHandle, BLHandle, BRHandle;
    QRect LHandle, THandle, RHandle, BHandle;

    QVector<QRect*> handles;
    QWidget *acceptWidget;
};

#endif
