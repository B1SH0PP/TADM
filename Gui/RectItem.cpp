#include "RectItem.h"

std::vector<RectItem*> RectItem::m_objectAddresses_vec;

RectItem::RectItem(QGraphicsItem* parent)
    : QGraphicsRectItem(parent),
      m_points(),
      m_cursorRotate(),
      m_handles(),
      m_handle(MOUSEHANDLE::handleNone),
      m_cursor(Qt::ArrowCursor),
      m_mousePressPos(),
      m_mousePressRect(),
      m_mouseRotateStart(),
      m_fLastAngle(0.0),
      m_bhandleSelected(MOUSEHANDLE::handleNone),
      m_isHover(false) {
    setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);

    m_handles.insert(std::make_pair(MOUSEHANDLE::handleTopLeft, QRectF()));
    m_handles.insert(std::make_pair(MOUSEHANDLE::handleTopMiddle, QRectF()));
    m_handles.insert(std::make_pair(MOUSEHANDLE::handleTopRight, QRectF()));
    m_handles.insert(std::make_pair(MOUSEHANDLE::handleMiddleLeft, QRectF()));
    m_handles.insert(std::make_pair(MOUSEHANDLE::handleMiddleRight, QRectF()));
    m_handles.insert(std::make_pair(MOUSEHANDLE::handleBottomLeft, QRectF()));
    m_handles.insert(std::make_pair(MOUSEHANDLE::handleBottomMiddle, QRectF()));
    m_handles.insert(std::make_pair(MOUSEHANDLE::handleBottomRight, QRectF()));

    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateTopLeft, QPointF()));
    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateTopMiddle, QPointF()));
    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateTopRight, QPointF()));
    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateMiddleRight, QPointF()));
    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateBottomRight, QPointF()));
    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateBottomMiddle, QPointF()));
    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateBottomLeft, QPointF()));
    m_points.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateMiddleLeft, QPointF()));

    m_cursorRotate.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateTopLeft,
                                         QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateTopLeft]).scaled(c_rotate_cursor_size))));
    m_cursorRotate.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateTopMiddle,
                                         QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateTopMiddle]).scaled(c_rotate_cursor_size))));
    m_cursorRotate.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateTopRight,
                                         QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateTopRight]).scaled(c_rotate_cursor_size))));
    m_cursorRotate.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateMiddleRight,
                                         QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateMiddleRight]).scaled(c_rotate_cursor_size))));
    m_cursorRotate.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateBottomRight,
                                         QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateBottomRight]).scaled(c_rotate_cursor_size))));
    m_cursorRotate.insert(
            std::make_pair(MOUSEROTATEHANDLE::handleRotateBottomMiddle,
                           QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateBottomMiddle]).scaled(c_rotate_cursor_size))));
    m_cursorRotate.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateBottomLeft,
                                         QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateBottomLeft]).scaled(c_rotate_cursor_size))));
    m_cursorRotate.insert(std::make_pair(MOUSEROTATEHANDLE::handleRotateMiddleLeft,
                                         QCursor(QPixmap(mapCursors[MOUSEROTATEHANDLE::handleRotateMiddleLeft]).scaled(c_rotate_cursor_size))));

    updateHandlesPos();

    // 存储 Rect 对象地址
    m_objectAddresses_vec.push_back(this);
}

RectItem::~RectItem() {}

QPainterPath RectItem::shape() const {
    QPainterPath path = QPainterPath();
    path.addRect(this->rect());
    if (this->isSelected()) {
        for (auto shape : m_handles)
            path.addEllipse(shape.second);
    }

    return path;
}

QRectF RectItem::boundingRect() const {
    auto o = c_handle_size + c_handle_space;
    return this->rect().adjusted(-o, -o, o, o);
}

void RectItem::updateHandlesPos() {
    auto s = c_handle_size;
    auto b = boundingRect();

    m_handles[MOUSEHANDLE::handleTopLeft] = QRectF(b.left(), b.top(), s, s);
    m_handles[MOUSEHANDLE::handleTopMiddle] = QRectF(b.center().x() - s / 2, b.top(), s, s);
    m_handles[MOUSEHANDLE::handleTopRight] = QRectF(b.right() - s, b.top(), s, s);
    m_handles[MOUSEHANDLE::handleMiddleLeft] = QRectF(b.left(), b.center().y() - s / 2, s, s);
    m_handles[MOUSEHANDLE::handleMiddleRight] = QRectF(b.right() - s, b.center().y() - s / 2, s, s);
    m_handles[MOUSEHANDLE::handleBottomLeft] = QRectF(b.left(), b.bottom() - s, s, s);
    m_handles[MOUSEHANDLE::handleBottomMiddle] = QRectF(b.center().x() - s / 2, b.bottom() - s, s, s);
    m_handles[MOUSEHANDLE::handleBottomRight] = QRectF(b.right() - s, b.bottom() - s, s, s);
}

bool RectItem::isHover() {
    return m_isHover;
}

QCursor RectItem::getRotateCursor(const QPointF& point) {
    if (m_isHover == true || !isSelected())
        return QCursor();

    if (boundingRect().contains(mapFromScene(point)))
        return QCursor();

    auto srcRect = rect();
    auto frameRect = srcRect.adjusted(-c_rotate_tolerance, -c_rotate_tolerance, c_rotate_tolerance, c_rotate_tolerance);

    QPointF innerPoint = mapFromScene(point);

    if (!frameRect.contains(innerPoint))
        return QCursor();

    m_points[MOUSEROTATEHANDLE::handleRotateTopLeft] = srcRect.topLeft();
    m_points[MOUSEROTATEHANDLE::handleRotateTopMiddle] = QPointF(srcRect.center().x(), srcRect.top());
    m_points[MOUSEROTATEHANDLE::handleRotateTopRight] = srcRect.topRight();
    m_points[MOUSEROTATEHANDLE::handleRotateMiddleRight] = QPointF(srcRect.right(), srcRect.center().y());
    m_points[MOUSEROTATEHANDLE::handleRotateBottomRight] = srcRect.bottomRight();
    m_points[MOUSEROTATEHANDLE::handleRotateBottomMiddle] = QPointF(srcRect.center().x(), srcRect.bottom());
    m_points[MOUSEROTATEHANDLE::handleRotateBottomLeft] = srcRect.bottomLeft();
    m_points[MOUSEROTATEHANDLE::handleRotateMiddleLeft] = QPointF(srcRect.left(), srcRect.center().y());

    auto ret = MOUSEROTATEHANDLE::handleRotateNone;
    float l = 3.4028235E38;
    for (auto& iter : m_points) {
        auto length = getLength2(iter.second, innerPoint);
        if (length < l) {
            l = length;
            ret = iter.first;
        }
    }

    if (ret == MOUSEROTATEHANDLE::handleRotateNone)
        return QCursor();
    float angle = this->rotation() + 22.5;
    while (angle >= 360.0)
        angle -= 360;
    ret = MOUSEROTATEHANDLE(((int)ret + (int)(angle / 45)) % c_rotate_cursors_size);
    if (ret == MOUSEROTATEHANDLE::handleRotateNone)
        ret = MOUSEROTATEHANDLE::handleRotateTopLeft;

    return m_cursorRotate[ret];
}

void RectItem::setRotateStart(const QPointF& point) {
    m_mouseRotateStart = point;
    m_fLastAngle = rotation();
}

void RectItem::setRotateEnd(const QPointF& point) {
    QPointF ori = mapToScene(transformOriginPoint());
    QPointF v1 = m_mouseRotateStart - ori;
    QPointF v2 = point - ori;

    float angle = std::atan2f(v2.y(), v2.x()) - std::atan2f(v1.y(), v1.x());

    angle = m_fLastAngle + angle * 180 / 3.1415926;

    // angle = [0,360)
    while (angle < 0.0)
        angle += 360;
    while (angle >= 360.0)
        angle -= 360;

    setRotation(angle);
}

void RectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QStyleOptionGraphicsItem op;

    if (widget == nullptr)
        op = *option;
    else
        op.initFrom(widget);

    if (option->state & QStyle::State_Selected)
        op.state = QStyle::State_None;

    QGraphicsRectItem::paint(painter, &op, widget);

    if (isSelected() == true) {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(QBrush(QColor(255, 255, 255, 255)));
        painter->setPen(QPen(QColor(0x293a56ff), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        for (auto it : m_handles) {
            if (m_bhandleSelected == MOUSEHANDLE::handleNone || it.first == m_bhandleSelected)
                painter->drawRect(it.second);
        }
    }
}

void RectItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    QGraphicsRectItem::hoverEnterEvent(event);
    m_isHover = true;
}

void RectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    setCursor(Qt::ArrowCursor);
    QGraphicsRectItem::hoverLeaveEvent(event);
    m_isHover = false;
}

void RectItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    if (isSelected()) {
        m_handle = handleAt(event->pos());

        if (MOUSEHANDLE::handleNone == m_handle)
            m_cursor = Qt::SizeAllCursor;
        else {
            float angle = this->rotation() + 22.5;
            while (angle >= 360.0)
                angle -= 360;
            // choose the right cursor
            m_cursor = handleCursors[((int)m_handle + (int)(angle / 45) - 1) % c_handle_cursors_size];
        }
        setCursor(m_cursor);
    }
    QGraphicsRectItem::hoverMoveEvent(event);
}

void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (MOUSEHANDLE::handleNone != m_bhandleSelected) {

        QPointF tempPoint = event->pos();
        QPointF tempPoint1 = mapToScene(event->pos());

        interactiveResize(event->pos());
        //interactiveResize(mapToScene(event->pos()));

    } else
        QGraphicsRectItem::mouseMoveEvent(event);
}

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    // @1 - 这里发现, 如果点击的不是图像左上角点 (场景原点) 则`m_bhandleSelected`使用`mapToScene()`后会不同, 若是左上角点则一致 (handleTopLeft = 1)
    m_bhandleSelected = handleAt(event->pos());
    //m_bhandleSelected = handleAt(mapToScene(event->pos()));
    if (MOUSEHANDLE::handleNone != m_bhandleSelected) {
        m_mousePressPos = event->pos();
        //m_mousePressPos = mapToScene(event->pos());
        m_mousePressRect = boundingRect();
    }
    QGraphicsRectItem::mousePressEvent(event);
}

void RectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    auto rr = this->rect();
    auto angle = qDegreesToRadians(this->rotation());

    auto p1 = rr.center();
    auto origin = this->transformOriginPoint();
    QPointF p2 = QPointF(0, 0);

    p2.setX(origin.x() + qCos(angle) * (p1.x() - origin.x()) - qSin(angle) * (p1.y() - origin.y()));
    p2.setY(origin.y() + qSin(angle) * (p1.x() - origin.x()) + qCos(angle) * (p1.y() - origin.y()));

    auto diff = p1 - p2;
    this->setRect(rr.adjusted(-diff.x(), -diff.y(), -diff.x(), -diff.y()));
    setTransformOriginPoint(this->rect().center());

    updateHandlesPos();

    m_bhandleSelected = MOUSEHANDLE::handleNone;
    m_mousePressPos = QPointF();
    m_mousePressRect = QRectF();
    this->update();
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void RectItem::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Up:
            this->moveBy(0, -1);
            event->setAccepted(true);
            break;
        case Qt::Key_Down:
            this->moveBy(0, 1);
            event->setAccepted(true);
            break;
        case Qt::Key_Left:
            this->moveBy(-1, 0);
            event->setAccepted(true);
            break;
        case Qt::Key_Right:
            this->moveBy(1, 0);
            event->setAccepted(true);
            break;
        default:
            event->setAccepted(false);
            break;
    }
    //QGraphicsRectItem::keyPressEvent(event);
}

RectItem::MOUSEHANDLE RectItem::handleAt(const QPointF& point) {
    for (auto it : m_handles) {
        if (it.second.contains(point))
            return it.first;
    }
    return MOUSEHANDLE::handleNone;
}

void RectItem::interactiveResize(const QPointF& mousePos) {
    auto offset = c_handle_size + c_handle_space;
    auto bRect = boundingRect();
    auto rr = this->rect();
    auto diff = QPointF(0, 0);

    prepareGeometryChange();

    if (m_bhandleSelected == MOUSEHANDLE::handleTopLeft) {
        auto fromX = m_mousePressRect.left();
        auto fromY = m_mousePressRect.top();
        auto toX = fromX + mousePos.x() - m_mousePressRect.x();
        auto toY = fromY + mousePos.y() - m_mousePressRect.y();

        if (!(toX - fromX >= rr.width() || toY - fromY >= rr.height())) {
            diff.setX(toX - fromX);
            diff.setY(toY - fromY);
            bRect.setLeft(toX);
            bRect.setTop(toY);
            rr.setLeft(bRect.left() + offset);
            rr.setTop(bRect.top() + offset);
            this->setRect(rr);
        }
    } else if (m_bhandleSelected == MOUSEHANDLE::handleTopMiddle) {
        auto fromY = m_mousePressRect.top();
        auto toY = fromY + mousePos.y() - m_mousePressPos.y();

        if (!(toY - fromY >= rr.height())) {
            diff.setY(toY - fromY);
            bRect.setTop(toY);
            rr.setTop(bRect.top() + offset);
            this->setRect(rr);
        }
    } else if (m_bhandleSelected == MOUSEHANDLE::handleTopRight) {
        auto fromX = m_mousePressRect.right();
        auto fromY = m_mousePressRect.top();
        auto toX = fromX + mousePos.x() - m_mousePressPos.x();
        auto toY = fromY + mousePos.y() - m_mousePressPos.y();

        if (!(fromX - toX >= rr.width() || toY - fromY >= rr.height())) {
            diff.setX(toX - fromX);
            diff.setY(toY - fromY);
            bRect.setRight(toX);
            bRect.setTop(toY);
            rr.setRight(bRect.right() - offset);
            rr.setTop(bRect.top() + offset);
            this->setRect(rr);
        }
    } else if (m_bhandleSelected == MOUSEHANDLE::handleMiddleLeft) {
        auto fromX = m_mousePressRect.left();
        auto toX = fromX + mousePos.x() - m_mousePressPos.x();

        if (!(toX - fromX >= rr.width())) {
            diff.setX(toX - fromX);
            bRect.setLeft(toX);
            rr.setLeft(bRect.left() + offset);
            this->setRect(rr);
        }
    } else if (m_bhandleSelected == MOUSEHANDLE::handleMiddleRight) {
        auto fromX = m_mousePressRect.right();
        auto toX = fromX + mousePos.x() - m_mousePressPos.x();

        if (!(fromX - toX >= rr.width())) {
            diff.setX(toX - fromX);
            bRect.setRight(toX);
            rr.setRight(bRect.right() - offset);
            this->setRect(rr);
        }
    } else if (m_bhandleSelected == MOUSEHANDLE::handleBottomLeft) {
        auto fromX = m_mousePressRect.left();
        auto fromY = m_mousePressRect.bottom();
        auto toX = fromX + mousePos.x() - m_mousePressPos.x();
        auto toY = fromY + mousePos.y() - m_mousePressPos.y();

        if (!(toX - fromX >= rr.width() || fromY - toY >= rr.height())) {
            diff.setX(toX - fromX);
            diff.setY(toY - fromY);
            bRect.setLeft(toX);
            bRect.setBottom(toY);
            rr.setLeft(bRect.left() + offset);
            rr.setBottom(bRect.bottom() - offset);
            this->setRect(rr);
        }
    } else if (m_bhandleSelected == MOUSEHANDLE::handleBottomMiddle) {
        auto fromY = m_mousePressRect.bottom();
        auto toY = fromY + mousePos.y() - m_mousePressPos.y();

        if (!(fromY - toY >= rr.height())) {
            diff.setY(toY - fromY);
            bRect.setBottom(toY);
            rr.setBottom(bRect.bottom() - offset);
            this->setRect(rr);
        }
    } else if (m_bhandleSelected == MOUSEHANDLE::handleBottomRight) {
        auto fromX = m_mousePressRect.right();
        auto fromY = m_mousePressRect.bottom();
        auto toX = fromX + mousePos.x() - m_mousePressPos.x();
        auto toY = fromY + mousePos.y() - m_mousePressPos.y();

        if (!(fromX - toX >= rr.width() || fromY - toY >= rr.height())) {
            diff.setX(toX - fromX);
            diff.setY(toY - fromY);
            bRect.setRight(toX);
            bRect.setBottom(toY);
            rr.setRight(bRect.right() - offset);
            rr.setBottom(bRect.bottom() - offset);
            this->setRect(rr);
        }
    }
    updateHandlesPos();
}

float RectItem::getLength2(const QPointF& point1, const QPointF& point2) {
    return (point1.x() - point2.x()) * (point1.x() - point2.x()) + (point1.y() - point2.y()) * (point1.y() - point2.y());
}
