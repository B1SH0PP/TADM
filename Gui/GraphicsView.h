#pragma once
// std
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <format>
#include <filesystem>
#include <regex>
// Qt
#include <QGraphicsView>
#include <QObject>
#include <QBoxLayout>
#include <QLabel>
#include <qevent.h>
#include <QMutexLocker>
#include <QSplitter>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QTimer>
#include <QPointF>
#include <QRectF>
#include <QListWidget>
#include <QFileInfo>
#include <QSpacerItem>
#include <QString>
// OpenCV
#include <opencv2/opencv.hpp>
// own
#include "ImageItem.h"
#include "RectItem.h"
#include "FineTuning.h"
// WinKit
#include <direct.h>

/**
 * 设计思路: `_DRAW`和`m_isDrawRect`
 * `_DRAW`控制当前这个视窗是否具有画框的必要 (功能)
 * `m_isDrawRect`控制窗口画框状态的启停 (开始画框/结束画框).
 */

struct SWITCH {
    SWITCH() {
        _DRAW        = false;
        _FINE_TUNING = false;
        _ADD_MASK    = false;
        _REMOVE_MASK = false;
    }

    bool _DRAW        : 1;  // 1bit
    bool _FINE_TUNING : 1;
    bool _ADD_MASK    : 1;
    bool _REMOVE_MASK : 1;
};

struct IMGwithRECT {
    QPointF point;  // TopLeft pos
    QRectF rect;    // Rect size

    cv::Mat Mat0;  // (CV_8UC3) Rect截取原图
    cv::Mat Mat1;  // (CV_8UC1) (二值化) 轮廓
    cv::Mat Mat2;  // (CV_8UC4) 掩膜+Rect截取原图 (通道顺序:RGBA (蓝绿色的))
    cv::Mat Mat3;  // (CV_8UC4) Mat1轮廓凸包, 单独的红色透明掩膜 (其余像素全为(0,0,0,0))
    cv::Mat Mat4;  // (CV_8UC1) (二值化) 缺陷凸包 (即最终导出的黑白掩膜图)
};

enum class EXPORT_CLASS
{
    GOOD       = 1,
    CRACK      = 2,
    GLUE_STRIP = 3,
    LIQUID     = 4,
    SCRATCH    = 5,
    COLOR      = 6,
    COMBINED   = 7
};

/**
 * 简单介绍下: `QGraphicsRectItem` 和 `QRect`/`QRectF`
 *            `QRect`用于表示矩形的几何数据，而`QGraphicsRectItem`用于在图形场景中显示和操作矩形图形项.
 *
 * 常规用法:
 *    (QGraphicsRectItem*) m_selectionRect->setRect
 *    (QGraphicsRectItem*) m_selectionRect = scene()->addRect(_);
 */

class GraphicsView : public QGraphicsView {
    Q_OBJECT
 public:
    GraphicsView(QWidget* parent                   = 0,
                 bool sDRAW                        = false,
                 std::pair<QImage, QImage> FT_pair = std::make_pair<QImage, QImage>(QImage(), QImage()));
    ~GraphicsView();

    void SetImage(const QImage& image, const QString& materialDir);

    void SwitchImage(const QImage& image);

    QImage captureImage(const QString& materialDir);

    std::pair<QImage, QImage> captureImageFineTuning(QRect rect);

    void Clear();

    inline void GenerateContour(const cv::Mat& croppedImage = cv::Mat());

    void AddMask(const QPoint& ItemPoint);

    void SpliceMask();

    void RemoveMask(const QPoint& ItemPoint);

 protected:
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;


 signals:
    void addRect(QRectF Rect);
    void CannyThresholdChanged(int8_t LorH, size_t CannyThresholdValue);
    void RefreshViewBox2();

 public slots:
    void Center();

    void Zoom(double scaleFactor = 1);

    void Translate(QPointF delta);

    void fitFrame();

    void lowThresholdValueChangeEvent(int value);  // @query - 好像Qt槽函数不能内联

    void highThresholdValueChangeEvent(int value);

    // @query - 这里不能内联, 不清楚我没事, 不然会报 "LINK2019"
    //inline void GenerateConvexHull();

    void GenerateConvexHull();

    void StartFineTuning();

    void StartAddMask();

    void StartRemoveMask();

    void ExportMask();

    void NextImage();

    void LastImage();

    void ExportClassChanged(int id);

 private:
    void setBackground(bool enabled = true, bool invertColor = false);

    inline QString getSuffix(const QString& qstr);

    void clearItem();

    void invertPixel(int x, int y);

    void restorePixel(int x, int y);

    void updatePixmap() { this->m_imageItem->setPixmap(QPixmap::fromImage(m_updateImg)); }

 public:
    bool m_isTranslate;
    bool m_isDrawRect;

    static std::string m_dataParentPath;
    static QString m_currentImg;

    QPoint m_lastMousePos;  // 用来记录鼠标移动事件的起点坐标
    double m_zoomValue = 1;

    QGraphicsScene* m_scene;  // 场景
    ImageItem* m_imageItem;   // 图像元素

    QPixmap m_image;  // 视觉窗口所显示的图像
    QImage m_qimage;
    QPixmap m_tilePixmap = QPixmap(36, 36);  // 背景图片方格

    QWidget* m_posInfoWidget;  // 视觉窗口左上角，用于显示鼠标位置已经对应位置像素灰度值的窗口
    QLabel* m_grayValueLabel;  // 显示灰度值的标签
    QLabel* m_ImgNameLabel;    // 显示图像名的标签

    QGraphicsRectItem* m_selectionRect;  // 截图框
    RectItem* m_mySelectionRect;

    SWITCH* m_s;

    static size_t lowThreshold;
    static size_t highThreshold;

    // 一张图上所有`Rect`及其数据
    static std::vector<IMGwithRECT*> m_Rect_vec;

    // @query - 不知道为什么下面这样定义容器就会报 "std::pair<std::pair<QPointF, QRectF>, IMG*>: 没有重载函数可以转换所有参数类型" 的错, 不清楚原因.
    //          个人感觉是`std::pair`模板底层的缘故.
    // static std::map<QString, std::vector<std::pair<std::pair<QPointF, QRectF>, IMG*>>> m_ImgWithRects_map;

    // 所有图以及其下对应的所有`Rect`和图像
    static std::map<QString, std::vector<IMGwithRECT*>> m_ImgWithRects_map;

    // 父类指针 (设计模式: 反向指针)
    // 好像有问题 !!!
    // FineTuning* m_FT;

    // @{ Fine-tuning
    // 微调的图像数据, (<原图, 凸包掩膜>) (供微调窗口类`class FineTuning`下的本类对象使用)
    std::pair<QImage, QImage> m_FT_pair;

    QImage m_originImg;

    QImage m_updateImg;

    QColor m_lastPosPixel;

    QPoint m_lastPos = QPoint(-1, -1);
    // @} Fine-tuning

    QPoint crossPosition;

    static QRect m_FTRect;

    static cv::Mat m_FTMask;

    std::vector<RectItem*> m_RectItems_vec;

 private:
    bool Q_Pressed;

    bool m_isMouseLeftPressed = false;

    static EXPORT_CLASS m_ec;
};
