#pragma once
// Qt
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QSplitter>
#include <QToolBar>
#include <QSlider>
#include "ui_Gui.h"
#include <QRadioButton>
#include <QButtonGroup>
// OpenCV
#include <opencv2/opencv.hpp>
// own
#include "imageItem.h"
#include "graphicsView.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class GUIClass;
};

QT_END_NAMESPACE

class GUI : public QMainWindow {
    Q_OBJECT

 public:
    GUI(QWidget* parent = nullptr);
    ~GUI();

 private:
    void setImage();
    void saveImage();
    QImage Mat2QImage(const cv::Mat& mat);

 public slots:
    void addRect(QRectF Rect);
    void refreshMask(int8_t LorH, size_t CannyThresholdValue);
    void StartFineTuning();
    void RefreshViewBox2();

 private:
    Ui::GUIClass* ui;
    GraphicsView* m_viewBox1;
    GraphicsView* m_viewBox2;
    GraphicsView* m_viewBox3;
};
