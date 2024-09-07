#include "FineTuning.h"

// @1 - Calling process: class GraphicsView -> mouseReleaseEvent() -> FineTuning()
FineTuning::FineTuning(QWidget* parent, const std::pair<QImage, QImage>& FTImage_pair)
    : QWidget(parent),
      ui(new Ui::FineTuningClass()),
      m_FineTuningBox(new GraphicsView(this, false, FTImage_pair)),
      m_FTImageWithOriMask(FTImage_pair.second.copy()) {
    ui->setupUi(this);

    ui->FineTuningWidget->layout()->addWidget(m_FineTuningBox);

    m_FineTuningBox->SetImage(FTImage_pair.second, "");
    m_FineTuningBox->m_originImg = FTImage_pair.second.copy();
    m_FineTuningBox->m_updateImg = m_FineTuningBox->m_originImg.copy();
    //m_FineTuningBox->m_imageItem->setCursor(m_FineTuningBox->m_imageItem->m_dotCursor);

    ui->AddMask->setStyleSheet("QToolButton { background-color: red; color: white; }");
    ui->RemoveMask->setStyleSheet("QToolButton { background-color: red; color: white; }");

    connect(ui->AddMask, &QToolButton::clicked, this, &FineTuning::StartAddMask);
    connect(ui->RemoveMask, &QToolButton::clicked, this, &FineTuning::StartRemoveMask);
}

FineTuning::~FineTuning() {
    delete ui;
}

void FineTuning::StartAddMask() {
    static bool state = false;
    if (!state) {
        state = true;
        ui->AddMask->setStyleSheet("QToolButton { background-color: green; color: white; }");
    } else if (state) {
        state = false;
        ui->AddMask->setStyleSheet("QToolButton { background-color: red; color: white; }");
    }
    m_FineTuningBox->StartAddMask();

    // @TODO - close RemoveMask
    // ...
}

void FineTuning::StartRemoveMask() {
    static bool state = false;
    if (!state) {
        state = true;
        ui->RemoveMask->setStyleSheet("QToolButton { background-color: green; color: white; }");
    } else if (state) {
        state = false;
        ui->RemoveMask->setStyleSheet("QToolButton { background-color: red; color: white; }");
    }
    m_FineTuningBox->StartRemoveMask();

    // @TODO - close AddMask
    // ...
}

void FineTuning::closeEvent(QCloseEvent* event) {
    // Check the style sheet to determine the current button state
    // case: AddMask
    if (ui->AddMask->styleSheet().contains("background-color: green") && ui->RemoveMask->styleSheet().contains("background-color: red")) {
        QImage qimg1 = this->m_FineTuningBox->m_FT_pair.first;
        QImage qimg2 = this->m_FineTuningBox->m_FT_pair.second;
        cv::Mat mat0(qimg2.height(), qimg2.width(), CV_8UC4, (void*)qimg2.bits(), qimg2.bytesPerLine());
        cv::cvtColor(mat0, mat0, cv::COLOR_RGBA2BGRA);
        cv::Mat alphaChannel;
        cv::extractChannel(mat0, alphaChannel, 3);
        for (int y = 0; y < alphaChannel.rows; ++y) {
            for (int x = 0; x < alphaChannel.cols; ++x) {
                uchar& pixel = alphaChannel.at<uchar>(y, x);
                if (pixel == 255) {
                    pixel = 0;  // 255 -> 0
                } else if (pixel == 100) {
                    pixel = 255;  // 0~255 -> 255
                }
            }
        }
        GraphicsView::m_FTMask = alphaChannel;
    }
    // case: RemoveMask
    else if (ui->AddMask->styleSheet().contains("background-color: red") && ui->RemoveMask->styleSheet().contains("background-color: green")) {
        QImage qimg2 = this->m_FineTuningBox->m_FT_pair.second;
        cv::Mat mat1(qimg2.height(), qimg2.width(), CV_8UC4, (void*)qimg2.bits(), qimg2.bytesPerLine());
        cv::cvtColor(mat1, mat1, cv::COLOR_RGBA2BGRA);

        cv::Mat mat2(m_FTImageWithOriMask.height(),
                     m_FTImageWithOriMask.width(),
                     CV_8UC4,
                     (void*)m_FTImageWithOriMask.bits(),
                     m_FTImageWithOriMask.bytesPerLine());
        cv::cvtColor(mat2, mat2, cv::COLOR_RGBA2BGRA);


        cv::Mat diff, diffGray, binaryResult;
        cv::absdiff(mat1, mat2, diff);
        std::vector<cv::Mat> channels;
        cv::split(diff, channels);
        diffGray = channels[0] + channels[1] + channels[2] + channels[3];
        cv::threshold(diffGray, binaryResult, 0, 255, cv::THRESH_BINARY_INV);
        GraphicsView::m_FTMask = binaryResult;
    }

    // clear the previous Rect when closing
    assert(RectItem::m_objectAddresses_vec.size() != 0);
    delete RectItem::m_objectAddresses_vec[RectItem::m_objectAddresses_vec.size() - 1];
    RectItem::m_objectAddresses_vec.pop_back();

    QWidget::closeEvent(event);
}