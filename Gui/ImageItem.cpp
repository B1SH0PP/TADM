#include "ImageItem.h"

ImageItem::ImageItem(QWidget* parent) : QGraphicsPixmapItem(nullptr), m_isFineTuning(false) {}

void ImageItem::StartFineTuning() {
    m_isFineTuning = true;
}

void ImageItem::createDotCursor() {
    int size = 16;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);

    int dotSize = 4;

    painter.drawEllipse((size - dotSize) / 2, (size - dotSize) / 2, dotSize, dotSize);

    m_dotCursor = QCursor(pixmap);
}

void ImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    if (m_isFineTuning) {
        //setCursor(Qt::BlankCursor);  // Equivalent to hiding
    } else {
        setCursor(Qt::CrossCursor);
    }

    QPointF mousePosition = event->pos();
    int R, G, B;
    int x, y;
    x = mousePosition.x();
    y = mousePosition.y();
    if (mousePosition.x() < 0) {
        x = 0;
    }
    if (mousePosition.y() < 0) {
        y = 0;
    }

    pixmap().toImage().pixelColor(x, y).getRgb(&R, &G, &B);
    QString InfoVal = QString(" W:%1,H:%2 | X:%3,Y:%4 | R:%5,G:%6,B:%7")
                              .arg(QString::number(m_w))
                              .arg(QString::number(m_h))
                              .arg(QString::number(x))
                              .arg(QString::number(y))
                              .arg(QString::number(R))
                              .arg(QString::number(G))
                              .arg(QString::number(B));
    emit RGBValue(InfoVal);

    QGraphicsPixmapItem::hoverMoveEvent(event);
}