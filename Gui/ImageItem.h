#pragma once
// Qt
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QPainter>
// std
#include <queue>

/*
* @brief - 其实和`ROI`元素是同一类东西，都是在视觉窗口内显示的元素item
*          通过鼠标点选获取当前灰度值.
*/
class ImageItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

 public:
    explicit ImageItem(QWidget* parent = nullptr);

    //void switchPixel(int x, int y, bool _switch);

    void StartFineTuning();

 signals:
    void RGBValue(QString InfoVal);

    void XYPosition(int x, int y);

 private:
    //void invertPixel(int x, int y);

    //void restorePixel(int x, int y);

    //void updatePixmap() { setPixmap(QPixmap::fromImage(m_updateImg)); }

    void createDotCursor();

 protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

    //virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    //virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

 public:
    bool m_isFineTuning;
    int m_w;
    int m_h;
    //QImage m_originImg;
    //QImage m_updateImg;
    //QColor m_lastPosPixel;
    //QPoint m_lastPos = QPoint(-1, -1);

    //std::queue<QColor> m_pixel_que;

 private:
    QCursor m_dotCursor;
};
