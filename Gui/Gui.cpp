#include "GUI.h"

static QString materialDir;

GUI::GUI(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::GUIClass()),
      m_viewBox1(new GraphicsView(this, true)),
      m_viewBox2(new GraphicsView(this)),
      m_viewBox3(new GraphicsView(this)) {
    ui->setupUi(this);

    //ui->MainToolBar->addWidget(ui->OpenButton);
    //ui->MainToolBar->addWidget(ui->SaveButton);
    //ui->MainToolBar->addWidget(ui->Left);
    //ui->MainToolBar->addWidget(ui->Right);

    ui->viewBox_1->layout()->addWidget(m_viewBox1);
    ui->viewBox_2->layout()->addWidget(m_viewBox2);
    ui->viewBox_3->layout()->addWidget(m_viewBox3);

    // QTableWidget
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setColumnCount(1);
    QStringList headers;
    headers << "Rect Size";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setStyleSheet(
            "QTableWidget::item { border: 1px solid black; }"  // 设置单元格的边框
            "QTableWidget { gridline-color: black; }"          // 设置表格的分割线颜色
            "QHeaderView::section { border: 1px solid red;}"   // 设置表头的分割线颜色
    );
    ui->tableWidget->setColumnWidth(0, 235);  // 列宽像素

    // QSlider
    ui->CannyLowThresholdSlider->setRange(0, 100);
    ui->CannyLowThresholdSlider->setSingleStep(10);
    ui->CannyLowThresholdSlider->setValue(int(GraphicsView::lowThreshold));
    ui->CannyLowThresholdLabel->setText(QString("CannyLowThreshold = %1").arg(int(GraphicsView::lowThreshold)));
    ui->CannyHighThresholdSlider->setRange(50, 300);
    ui->CannyHighThresholdSlider->setSingleStep(10);
    ui->CannyHighThresholdSlider->setValue(int(GraphicsView::highThreshold));
    ui->CannyHighThresholdLabel->setText(QString("CannyHighThreshold = %1").arg(int(GraphicsView::highThreshold)));

    ui->FineTuningButton->setStyleSheet("QToolButton { background-color: red; color: white; }");

    // QRadioButton
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->RB1, 1);
    buttonGroup->addButton(ui->RB2, 2);
    buttonGroup->addButton(ui->RB3, 3);
    buttonGroup->addButton(ui->RB4, 4);
    buttonGroup->addButton(ui->RB5, 5);
    buttonGroup->addButton(ui->RB6, 6);
    buttonGroup->addButton(ui->RB7, 7);

    connect(ui->OpenButton, &QToolButton::clicked, this, &GUI::setImage);
    connect(ui->SaveButton, &QToolButton::clicked, this, &GUI::saveImage);
    connect(m_viewBox1, &GraphicsView::addRect, this, &GUI::addRect);
    connect(m_viewBox2, &GraphicsView::addRect, this, &GUI::addRect);
    connect(m_viewBox3, &GraphicsView::addRect, this, &GUI::addRect);
    connect(ui->CannyLowThresholdSlider, &QSlider::valueChanged, m_viewBox3, &GraphicsView::lowThresholdValueChangeEvent);
    connect(ui->CannyHighThresholdSlider, &QSlider::valueChanged, m_viewBox3, &GraphicsView::highThresholdValueChangeEvent);
    connect(m_viewBox3, &GraphicsView::CannyThresholdChanged, this, &GUI::refreshMask);
    connect(ui->ConvexHullButton, &QToolButton::clicked, m_viewBox2, &GraphicsView::GenerateConvexHull);
    connect(ui->FineTuningButton, &QToolButton::clicked, this, &GUI::StartFineTuning);
    connect(m_viewBox2, &GraphicsView::RefreshViewBox2, this, &GUI::RefreshViewBox2);
    connect(ui->ExportButton, &QToolButton::clicked, m_viewBox2, &GraphicsView::ExportMask);
    connect(ui->RightButton, &QToolButton::clicked, m_viewBox1, &GraphicsView::NextImage);
    connect(ui->LeftButton, &QToolButton::clicked, m_viewBox1, &GraphicsView::LastImage);
    connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), m_viewBox1, &GraphicsView::ExportClassChanged);
}

GUI::~GUI() {
    delete ui;
}

void GUI::setImage() {
    /* 选择素材路径 */
    const QString dlgTitle   = "Select material File";
    const QString dlgOpenDir = "../data";
    const QString dlgFilter  = QLatin1String(";;");
    // QString* dlgPtrSelFilter = &result.lastIoSettings.selectedFilter;
    materialDir              = QFileDialog::getOpenFileName(this, dlgTitle, dlgOpenDir, dlgFilter);

    if (materialDir == QString())
        return;
    QImage originImage(materialDir);
    m_viewBox1->SetImage(originImage, materialDir);
}

void GUI::saveImage() {
    if (materialDir == "" || GraphicsView::m_Rect_vec.size() == 0)
        return;

    QImage croppedPixmap = m_viewBox1->captureImage(materialDir);
    m_viewBox2->SetImage(croppedPixmap, materialDir);

    // @TODO - 这里初步设计为 "按下Save按钮" 就同时显示2幅画面.
    m_viewBox3->SetImage(QImage(GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.data,
                                GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.cols,
                                GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.rows,
                                GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.step,
                                QImage::Format_Grayscale8),
                         materialDir);
}

QImage GUI::Mat2QImage(const cv::Mat& mat) {
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (mat.type() == CV_8UC1) {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3) {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    // 8-bits unsigned, NO. OF CHANNELS = 4
    else if (mat.type() == CV_8UC4) {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image;
    } else {
        return QImage();
    }
}

//void GUI::GetConvexHull() {
//    m_viewBox2->GenerateConvexHull();
//}

//void GUI::GetFineTuning() {
//    m_viewBox2->StartFineTuning();
//}

// @brief - 添加`Rect`数据到`QTableWidget`中.
void GUI::addRect(QRectF Rect) {
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
    QTableWidgetItem* item = new QTableWidgetItem(QString("(%1, %2), (%3, %4)")
                                                          .arg(static_cast<int>(Rect.topLeft().x()))
                                                          .arg(static_cast<int>(Rect.topLeft().y()))
                                                          .arg(static_cast<int>(Rect.width()))
                                                          .arg(static_cast<int>(Rect.height())));
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 2, 0, item);
}

void GUI::refreshMask(int8_t LorH, size_t CannyThresholdValue) {
    if (materialDir == "" || GraphicsView::m_Rect_vec.size() == 0) {
        // 即使没有图移动`sliter`控件, 也刷新`Threshold`
        switch (LorH) {
            case 0:
                ui->CannyLowThresholdLabel->setText(QString("CannyLowThreshold = %1").arg(QString::number(CannyThresholdValue)));
                break;
            case 1:
                ui->CannyHighThresholdLabel->setText(QString("CannyHighThreshold = %1").arg(QString::number(CannyThresholdValue)));
                break;
        }

        return;
    }

    switch (LorH) {
        case 0:
            ui->CannyLowThresholdLabel->setText(QString("CannyLowThreshold = %1").arg(QString::number(CannyThresholdValue)));
            break;
        case 1:
            ui->CannyHighThresholdLabel->setText(QString("CannyHighThreshold = %1").arg(QString::number(CannyThresholdValue)));
            break;
    }

    m_viewBox3->SetImage(QImage(GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.data,
                                GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.cols,
                                GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.rows,
                                GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat1.step,
                                QImage::Format_Grayscale8),
                         materialDir);
}

void GUI::StartFineTuning() {
    static bool state = false;
    if (!state) {
        ui->FineTuningButton->setStyleSheet("QToolButton { background-color: green; color: white; }");
        state = true;
        m_viewBox2->StartFineTuning();
    } else if (state) {
        ui->FineTuningButton->setStyleSheet("QToolButton { background-color: red; color: white; }");
        state = false;
        m_viewBox2->StartFineTuning();
    }
}

void GUI::RefreshViewBox2() {
    cv::Mat mat = GraphicsView::m_Rect_vec[GraphicsView::m_Rect_vec.size() - 1]->Mat2;
    QImage qimg(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA8888);
    m_viewBox2->SetImage(qimg, "111");
}
