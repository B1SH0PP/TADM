// Qt
#include <QGraphicsRectItem>
#include <QStyleOptionGraphicsItem>
#include <QStyleOption>
#include <QCursor>
#include <QPainter>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>
// std
#include <iostream>

const Qt::CursorShape handleCursors[] = {
        Qt::SizeFDiagCursor,
        Qt::SizeVerCursor,
        Qt::SizeBDiagCursor,
        Qt::SizeHorCursor,
        Qt::SizeFDiagCursor,
        Qt::SizeVerCursor,
        Qt::SizeBDiagCursor,
        Qt::SizeHorCursor,
};

const QString mapCursors[] = {"",
                              ":/QtGuiApplication4/Resources/rotate_top_left.png",
                              ":/QtGuiApplication4/Resources/rotate_top_middle.png",
                              ":/QtGuiApplication4/Resources/rotate_top_right.png",
                              ":/QtGuiApplication4/Resources/rotate_middle_right.png",
                              ":/QtGuiApplication4/Resources/rotate_bottom_right.png",
                              ":/QtGuiApplication4/Resources/rotate_bottom_middle.png",
                              ":/QtGuiApplication4/Resources/rotate_bottom_left.png",
                              ":/QtGuiApplication4/Resources/rotate_middle_left.png"};

class RectItem : public QGraphicsRectItem {
    enum MOUSEHANDLE {
        handleNone = 0,
        handleTopLeft = 1,
        handleTopMiddle = 2,
        handleTopRight = 3,
        handleMiddleRight = 4,
        handleBottomRight = 5,
        handleBottomMiddle = 6,
        handleBottomLeft = 7,
        handleMiddleLeft = 8,
    };

    enum MOUSEROTATEHANDLE {
        handleRotateNone = 0,
        handleRotateTopLeft = 1,
        handleRotateTopMiddle = 2,
        handleRotateTopRight = 3,
        handleRotateMiddleRight = 4,
        handleRotateBottomRight = 5,
        handleRotateBottomMiddle = 6,
        handleRotateBottomLeft = 7,
        handleRotateMiddleLeft = 8,
    };

    const float c_handle_size = 8.0;
    const float c_handle_space = -4.0;

    const float c_rotate_tolerance = 20.0;
    const int c_handle_cursors_size = 8;  // handleCursors[] size
    const int c_rotate_cursors_size = 9;  // MOUSEROTATEHANDLE size
    const QSize c_rotate_cursor_size = QSize(20, 20);

 public:
    RectItem(QGraphicsItem* parent = Q_NULLPTR);
    ~RectItem();

    // Returns the shape of this item as a QPainterPath in local coordinates.
    QPainterPath shape() const override;

    // Returns the bounding rect of the shape (including the resize handles).
    QRectF boundingRect() const override;

    void updateHandlesPos();

    bool isHover();

    // point is scene coordinate
    QCursor getRotateCursor(const QPointF& point);

    // set point for start rorate
    // @note point is scene coordinate
    void setRotateStart(const QPointF& point);

    // set point for end rorate
    // @note point is scene coordinate
    void setRotateEnd(const QPointF& point);

 protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) Q_DECL_OVERRIDE;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);

    //  Executed when the mouse leaves the shape (NOT PRESSED).
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    // Executed when the mouse moves over the shape (NOT PRESSED).
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

    //  Executed when the mouse is being moved over the item while being pressed.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    // Executed when the mouse is pressed on the item.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    // Executed when the mouse is released from the item.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    // Executed when the key is pressed on the item.
    void keyPressEvent(QKeyEvent* event);

 private:
    //  Returns the resize handle below the given point.
    MOUSEHANDLE handleAt(const QPointF& point);
    //  Perform shape interactive resize.
    void interactiveResize(const QPointF& mousePos);

    // the length2 with point1 and point2
    float getLength2(const QPointF& point1, const QPointF& point2);

 public:
    static std::vector<RectItem*> m_objectAddresses_vec;  //

 private:
    std::map<MOUSEROTATEHANDLE, QPointF> m_points;
    std::map<MOUSEROTATEHANDLE, QCursor> m_cursorRotate;
    std::map<MOUSEHANDLE, QRectF> m_handles;
    MOUSEHANDLE m_handle;
    QCursor m_cursor;
    QPointF m_mousePressPos;
    QRectF m_mousePressRect;

    QPointF m_mouseRotateStart;
    float m_fLastAngle;

    MOUSEHANDLE m_bhandleSelected;

    bool m_isHover;
};
