#include "ImageItem.h"

ImageItem::ImageItem(QWidget* parent) : QGraphicsPixmapItem(nullptr), m_isFineTuning(false) {}

//void ImageItem::switchPixel(int x, int y, bool _switch) {
//QColor invertedColor;
////QColor color = m_pixel_que.front();
//switch (_switch) {
//    case true:
//        // 这里为了醒目, 采取 "取反"
//        invertedColor = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());  // `alpha`通道不变
//        break;
//    case false:
//        invertedColor = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());  // `alpha`通道不变
//        break;
//}

//m_qimg.setPixelColor(x, y, invertedColor);
//this->setPixmap(QPixmap::fromImage(m_qimg));
//}

void ImageItem::StartFineTuning() {
    m_isFineTuning = true;
}

//void ImageItem::invertPixel(int x, int y) {
//    // @1 - 因为是4通道图, 不能用`QRgb`, 改用`QColor`
//    // 先保存当前像素值, 以供后期恢复
//    m_lastPosPixel = m_originImg.pixelColor(x, y);
//
//    // QColor invertedPixel = QColor(72, 201, 165, m_lastPosPixel.alpha());
//    QColor invertedPixel = QColor(255 - m_lastPosPixel.red(), 255 - m_lastPosPixel.green(), 255 - m_lastPosPixel.blue(), m_lastPosPixel.alpha());
//    m_updateImg.setPixelColor(x, y, invertedPixel);
//    updatePixmap();
//}
//
//void ImageItem::restorePixel(int x, int y) {
//    m_updateImg.setPixelColor(x, y, m_lastPosPixel);
//    updatePixmap();
//}

void ImageItem::createDotCursor() {
    int size = 16;  // 光标图像的大小
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);  // 填充透明色

    QPainter painter(&pixmap);
    painter.setBrush(Qt::black);  // 选择黑色画刷
    painter.setPen(Qt::NoPen);    // 不需要边框线

    int dotSize = 4;  // 点的大小
    // 在图像中心绘制一个小圆点
    painter.drawEllipse((size - dotSize) / 2, (size - dotSize) / 2, dotSize, dotSize);

    m_dotCursor = QCursor(pixmap);  // 创建自定义光标
}

//void ImageItem::AddPixelToMask(const QPoint& pos) {}

void ImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    if (m_isFineTuning) {
        //setCursor(Qt::BlankCursor);  // 相当于隐藏
    } else {
        setCursor(Qt::CrossCursor);  // 十字
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

    //if (!m_originImg.isNull() && x < m_originImg.width() && y < m_originImg.height()) {
    //    // (1).先恢复上一个像素
    //    if (m_lastPos != QPoint(-1, -1)) {
    //        restorePixel(m_lastPos.x(), m_lastPos.y());
    //    }
    //    // (2).重置突出显示当前像素
    //    invertPixel(x, y);
    //    // (3).记录当前坐标, 以供(1)中恢复
    //    m_lastPos = QPoint(x, y);
    //}

    //if (!m_qimg.isNull()) {
    //    /* 取出像素值, 并置为强调色 */
    //    //m_pixel_que.push(m_qimg.pixelColor(x, y));
    //    switchPixel(x, y, true);
    //}

    pixmap().toImage().pixelColor(x, y).getRgb(&R, &G, &B);
    QString InfoVal = QString(" W:%1,H:%2 | X:%3,Y:%4 | R:%5,G:%6,B:%7")  // 字符串整体替换
                              .arg(QString::number(m_w))
                              .arg(QString::number(m_h))
                              .arg(QString::number(x))
                              .arg(QString::number(y))
                              .arg(QString::number(R))
                              .arg(QString::number(G))
                              .arg(QString::number(B));
    emit RGBValue(InfoVal);

    // 上送父类
    QGraphicsPixmapItem::hoverMoveEvent(event);
}

//void ImageItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
//    QPointF mousePosition = event->pos();
//    int x = mousePosition.x();
//    int y = mousePosition.y();
//
//    if (event->button() == Qt::LeftButton) {
//        m_isMouseLeftPressed = true;
//    }
//
//    // 上送父类
//    QGraphicsPixmapItem::mousePressEvent(event);
//}
//
//void ImageItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
//    QPointF mousePosition = event->pos();
//    int x = mousePosition.x();
//    int y = mousePosition.y();
//
//    if (m_isMouseLeftPressed) {
//        AddMask(x, y);
//    }
//
//    // 上送父类
//    QGraphicsPixmapItem::mouseMoveEvent(event);
//}
//
//void ImageItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
//    if (event->button() == Qt::LeftButton) {
//        // 持续按住状态关闭
//        m_isMouseLeftPressed = false;
//    }
//
//    // 上送父类
//    QGraphicsPixmapItem::mouseReleaseEvent(event);
//}
