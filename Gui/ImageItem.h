#pragma once
// Qt
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
#include <QPainter>
// std
#include <queue>

class ImageItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

 public:
    explicit ImageItem(QWidget* parent = nullptr);

    void StartFineTuning();

 signals:
    void RGBValue(QString InfoVal);

    void XYPosition(int x, int y);

 private:
    void createDotCursor();

 protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

 public:
    bool m_isFineTuning;
    int m_w;
    int m_h;

 private:
    QCursor m_dotCursor;
};
