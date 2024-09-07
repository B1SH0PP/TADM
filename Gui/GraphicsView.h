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

    cv::Mat Mat0;  // (CV_8UC3) Rect capture original image
    cv::Mat Mat1;  // (CV_8UC1) (Binary) Contour
    cv::Mat Mat2;  // (CV_8UC4) Mask + Rect captures the original image (channel order: RGBA (blue-green))
    cv::Mat Mat3;  // (CV_8UC4) Mat1 contour convex hull, single red transparent mask (the rest of the pixels are all (0,0,0,0))
    cv::Mat Mat4;  // (CV_8UC1) (Binary) Defect convex hull (i.e. the final exported black and white mask image)
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

    void lowThresholdValueChangeEvent(int value);

    void highThresholdValueChangeEvent(int value);

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

    QPoint m_lastMousePos;
    double m_zoomValue = 1;

    QGraphicsScene* m_scene;
    ImageItem* m_imageItem;

    QPixmap m_image;
    QImage m_qimage;
    QPixmap m_tilePixmap = QPixmap(36, 36);

    QWidget* m_posInfoWidget;
    QLabel* m_grayValueLabel;
    QLabel* m_ImgNameLabel;

    QGraphicsRectItem* m_selectionRect;
    RectItem* m_mySelectionRect;

    SWITCH* m_s;

    static size_t lowThreshold;
    static size_t highThreshold;

    static std::vector<IMGwithRECT*> m_Rect_vec;


    static std::map<QString, std::vector<IMGwithRECT*>> m_ImgWithRects_map;

    // @{ Fine-tuning
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
