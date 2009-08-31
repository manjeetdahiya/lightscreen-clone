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

#include "../tools/screenshot.h"

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;

class AreaSelector : public QDialog
{
    Q_OBJECT
public:
    AreaSelector(QWidget* parent, QPixmap pixmap, Screenshot::Options options);
    ~AreaSelector();
    QPixmap pixmap;

protected slots:
    void displayHelp();
    void grabRect();
    void cancel();
    void animationTick(int frame);

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
    void showEvent( QShowEvent* e );

    void updateHandles();

    QRegion handleMask() const;
    QPoint limitPointToRect( const QPoint &p, const QRect &r ) const;

    Screenshot::Options mScreenshotOptions;
    QPoint mMousePos;
    QRect  mSelection;
    bool   mMouseDown;
    bool   mNewSelection;
    const int mHandleSize;
    QRect *mMouseOverHandle;
    QPoint mDragStartPoint;
    QRect  mSelectionBeforeDrag;
    QTimer mIdleTimer;
    bool mShowHelp;
    bool mGrabbing;
    int  mOverlayAlpha;


    // naming convention for handles
    // T top, B bottom, R Right, L left
    // 2 letters: a corner
    // 1 letter: the handle on the middle of the corresponding side
    QRect mTLHandle, mTRHandle, mBLHandle, mBRHandle;
    QRect mLHandle, mTHandle, mRHandle, mBHandle;

    QVector<QRect*> mHandles;
    QWidget        *mAcceptWidget;
};

#endif
