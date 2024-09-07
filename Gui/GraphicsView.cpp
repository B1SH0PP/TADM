#include "graphicsView.h"
#define ZOOMMAX 1920
#define ZOOMMIN 0.02

std::vector<IMGwithRECT*> GraphicsView::m_Rect_vec;
std::map<QString, std::vector<IMGwithRECT*>> GraphicsView::m_ImgWithRects_map;
size_t GraphicsView::lowThreshold          = 50;
size_t GraphicsView::highThreshold         = 150;
std::string GraphicsView::m_dataParentPath = "";
QString GraphicsView::m_currentImg         = "";
cv::Mat GraphicsView::m_FTMask             = cv::Mat();
QRect GraphicsView::m_FTRect               = QRect();
EXPORT_CLASS GraphicsView::m_ec;

// @TODO - Here, the switching between the two states is still unified with a function. Later, it will be unified like the `FineTuning` mode is
//         switched using `StartFineTuning()`. It is too stupid to specify the `sDRAW` state directly in the constructor during construction.
// @brief - Create a view window object.
// @param sDRAW -
// @param FT_pair - In fine-tuning mode, a pair of fine-tuning image materials (<original image to be fine-tuned, convex hull mask to be fine-tuned>)
//                  are passed in when creating the view object (GraphicsView).
GraphicsView::GraphicsView(QWidget* parent, bool sDRAW, std::pair<QImage, QImage> FT_pair)
    : QGraphicsView(parent),
      m_isTranslate(false),
      m_isDrawRect(false),
      m_scene(new QGraphicsScene()),
      m_imageItem(new ImageItem()),
      m_selectionRect(nullptr),
      m_mySelectionRect(nullptr),
      m_s(new SWITCH()),
      m_FT_pair(FT_pair) {
    m_scene->addItem(m_imageItem);
    setScene(m_scene);

    m_s->_DRAW = sDRAW;

    /* Turn off the horizontal/vertical drag bars of the view window */
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setRenderHint(QPainter::Antialiasing);  // Rendering Anti-Aliasing

    setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    /* Used to display the mouse coordinates and the grayscale value of the image in the upper left corner of the visual window */
    m_grayValueLabel = new QLabel(this);
    m_grayValueLabel->setObjectName("GrayValue_X");
    m_grayValueLabel->setStyleSheet("color:rgb(200,255,200); background-color:rgba(50,50,50,160); font: Microsoft YaHei;font-size: 15px;");
    m_grayValueLabel->setVisible(true);
    m_imageItem->setAcceptHoverEvents(true);
    m_grayValueLabel->setFixedWidth(702);
    m_grayValueLabel->setText(" W:0,H:0 | X:0,Y:0 | R:0,G:0,B:0");

    /* Display image name */
    if (m_s->_DRAW) {
        m_ImgNameLabel = new QLabel(this);
        m_ImgNameLabel->setObjectName("ImgName");
        m_ImgNameLabel->setStyleSheet("color:rgb(200,255,200); background-color:rgba(50,50,50,160); font: Microsoft YaHei;font-size: 15px;");
        m_ImgNameLabel->setVisible(true);
        m_ImgNameLabel->setText("Current Image: ----");
    }

    /* Display Area Window */
    m_posInfoWidget = new QWidget(this);
    m_posInfoWidget->setFixedHeight(20);
    m_posInfoWidget->setGeometry(0, 0, 900, 25);
    m_posInfoWidget->setStyleSheet("background-color:rgba(0,0,0,0);");
    auto fLayout = new QHBoxLayout(m_posInfoWidget);
    fLayout->setSpacing(0);
    fLayout->setContentsMargins(0, 0, 0, 0);
    fLayout->addWidget(m_grayValueLabel);
    fLayout->addStretch();
    fLayout->addWidget(m_ImgNameLabel);
    //fLayout->addStretch();

    /* Initialization threshold */
    emit CannyThresholdChanged(0, GraphicsView::lowThreshold);
    emit CannyThresholdChanged(1, GraphicsView::highThreshold);

    connect(m_imageItem, &ImageItem::RGBValue, this, [&](QString InfoVal) { m_grayValueLabel->setText(InfoVal); });

    /* Setting the Background */
    setBackground();
    centerOn(0, 0);
}

GraphicsView::~GraphicsView() {
    m_scene->deleteLater();
    delete m_imageItem;
}

// @brief - Set the image for the visual window, which is an external public interface
void GraphicsView::SetImage(const QImage& image, const QString& materialDir) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    m_qimage         = image.copy();
    m_image          = QPixmap::fromImage(image);
    m_imageItem->m_w = m_image.width();
    m_imageItem->m_h = m_image.height();
    m_imageItem->setPixmap(m_image);

    QPoint newCenter(m_image.width() / 2, m_image.height() / 2);
    this->centerOn(newCenter);

    fitFrame();
    //show();

    /* set image name */
    if (m_s->_DRAW && !m_s->_FINE_TUNING) {
        m_dataParentPath = std::filesystem::path(materialDir.toStdString()).parent_path().string();
        m_currentImg     = getSuffix(materialDir);
        m_ImgNameLabel->setText(QString("Current Image: %1").arg(m_currentImg));
    }
}

// @brief - Different from `SetImage()`, this function is not centered, and is used to quickly switch and compare two images.
void GraphicsView::SwitchImage(const QImage& image) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    m_qimage         = image.copy();
    m_image          = QPixmap::fromImage(image);
    m_imageItem->m_w = m_image.width();
    m_imageItem->m_h = m_image.height();
    m_imageItem->setPixmap(m_image);

    QPoint newCenter(m_image.width() / 2, m_image.height() / 2);
    //this->centerOn(newCenter);

    // fitFrame();
    // show();
}

QImage GraphicsView::captureImage(const QString& materialDir) {
    if (m_ImgWithRects_map.size() == 0)
        return QImage();
    QRectF rect = m_Rect_vec[m_Rect_vec.size() - 1]->rect;

    cv::Mat image = cv::imread(materialDir.toStdString());
    if (image.empty()) {
        return QImage();
    }

    cv::Rect roi(static_cast<int>(rect.left()), static_cast<int>(rect.top()), static_cast<int>(rect.width()), static_cast<int>(rect.height()));
    roi                                     = roi & cv::Rect(0, 0, image.cols, image.rows);
    cv::Mat croppedImage                    = image(roi);
    // @TODO - For now, the top image is considered the newest image by default, and the backtracking function has not yet been implemented.
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat0 = croppedImage;

    GenerateContour(croppedImage);

    // cv::Mat -> QImage
    // @TODO - There seems to be something wrong with the color, not consistent with the original picture.
    QImage qimg(croppedImage.data, croppedImage.cols, croppedImage.rows, croppedImage.step, QImage::Format_RGB888);

    // BGR -> RGB
    return qimg.rgbSwapped().copy();  // `.copy()` avoid shallow copies
}

std::pair<QImage, QImage> GraphicsView::captureImageFineTuning(QRect rect) {
    // (1). Mat3
    cv::Mat imageOri = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.clone();
    //cv::cvtColor(imageOri, imageOri, cv::COLOR_BGR2RGB);
    assert(!imageOri.empty() && "captureImageFineTuning() imageOri is empty!");
    // (2). Mat2
    cv::Mat image = m_Rect_vec[m_Rect_vec.size() - 1]->Mat2.clone();
    cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
    assert(!image.empty() && "captureImageFineTuning() image is empty!");

    cv::Rect roi(static_cast<int>(rect.left()), static_cast<int>(rect.top()), static_cast<int>(rect.width()), static_cast<int>(rect.height()));
    // (1). Mat1
    roi                   = roi & cv::Rect(0, 0, imageOri.cols, imageOri.rows);
    cv::Mat croppedImage0 = imageOri(roi);
    // (2). Mat2
    roi                   = roi & cv::Rect(0, 0, image.cols, image.rows);
    cv::Mat croppedImage1 = image(roi);

    // cv::Mat -> QImage
    // @TODO - There seems to be something wrong with the color, not consistent with the original picture.
    QImage qimg0(croppedImage0.data, croppedImage0.cols, croppedImage0.rows, croppedImage0.step, QImage::Format_RGB888);
    QImage qimg1(croppedImage1.data, croppedImage1.cols, croppedImage1.rows, croppedImage1.step, QImage::Format_RGBA8888);

    // BGR -> RGB
    return std::make_pair<QImage, QImage>(qimg0.rgbSwapped().copy(), qimg1.rgbSwapped().copy());
}

void GraphicsView::Clear() {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    m_imageItem->setPixmap(QPixmap());
}

// @brief - Rewrite the mouse wheel scroll event function.
//          Mainly relies on the Zoom() method.
void GraphicsView::wheelEvent(QWheelEvent* event) {
    QPoint scrollAmount = event->angleDelta();
    if ((scrollAmount.y() > 0) && (m_zoomValue >= ZOOMMAX))
        return;
    else if ((scrollAmount.y() < 0) && (m_zoomValue <= ZOOMMIN))
        return;

    scrollAmount.y() > 0 ? Zoom(1.1) : Zoom(0.9);
}

// @brief - Override mouse move events
//          Mainly relies on the `Translate()` method.
void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());

    QPointF ScenePointF = mapToScene(event->pos());
    QPoint ItemPoint =
            QPoint(static_cast<int>(m_imageItem->mapFromScene(ScenePointF).x()), static_cast<int>(m_imageItem->mapFromScene(ScenePointF).y()));

    if (item && item->type() != QGraphicsPixmapItem::Type && item->type() != QGraphicsRectItem::Type) {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    // Qt::MouseButton a = event->button();

    if (m_isDrawRect) {
        if (m_mySelectionRect) {
            QRectF newRect(mapToScene(m_lastMousePos), mapToScene(event->pos()));
            m_mySelectionRect->setRect(newRect.normalized());
        }
    } else if (m_isTranslate) {
        QPointF mouseDelta = event->pos() - m_lastMousePos;
        Translate(mouseDelta);
        m_lastMousePos = event->pos();
    } else if (!m_originImg.isNull() && ItemPoint.x() < m_originImg.width() && ItemPoint.y() < m_originImg.height()) {
        // (1).
        if (m_lastPos != QPoint(-1, -1)) {
            restorePixel(m_lastPos.x(), m_lastPos.y());
        }

        // (2).
        invertPixel(ItemPoint.x(), ItemPoint.y());

        // (3).
        m_lastPos = QPoint(ItemPoint.x(), ItemPoint.y());
    }

    if (m_s->_ADD_MASK && m_isMouseLeftPressed) {
        AddMask(ItemPoint);
    } else if (m_s->_REMOVE_MASK && m_isMouseLeftPressed) {
        RemoveMask(ItemPoint);
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());

    QPointF ScenePointF = mapToScene(event->pos());
    QPoint ItemPoint =
            QPoint(static_cast<int>(m_imageItem->mapFromScene(ScenePointF).x()), static_cast<int>(m_imageItem->mapFromScene(ScenePointF).y()));


    if (item && (item->type() != QGraphicsPixmapItem::Type)) {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton && m_s->_DRAW == true) {
        m_isTranslate  = false;
        m_isDrawRect   = true;
        m_lastMousePos = event->pos();

        if (m_selectionRect) {
            scene()->removeItem(m_selectionRect);
            delete m_selectionRect;
            m_selectionRect = nullptr;
        }

        /* 1.Naive */
        //QPen pen(Qt::red);
        //pen.setStyle(Qt::DashLine);
        //m_selectionRect = scene()->addRect(QRectF(mapToScene(m_lastMousePos), QSize()), pen, QBrush(Qt::transparent));

        /* 2.Custom */
        m_mySelectionRect = new RectItem();
        m_RectItems_vec.emplace_back(m_mySelectionRect);
        auto it = m_ImgWithRects_map.find(m_currentImg);
        if (it == m_ImgWithRects_map.end() && m_s->_FINE_TUNING == false) {
            //std::vector<IMGwithRECT*> temp_ImgWithRect_vec;
            IMGwithRECT* _imgrect = new IMGwithRECT();
            m_Rect_vec.emplace_back(_imgrect);
            m_ImgWithRects_map[m_currentImg] = m_Rect_vec;

        } else if (m_s->_FINE_TUNING == false) {
            IMGwithRECT* _imgrect = new IMGwithRECT();
            m_Rect_vec.emplace_back(_imgrect);

            // m_Rect_vec[m_Rect_vec.size() - 1]->point = m_mySelectionRect->rect().topLeft();
        }

        // m_mySelectionRect->setPos(mapToScene(m_lastMousePos).x(),
        //                           mapToScene(m_lastMousePos).y());
        m_scene->addItem(m_mySelectionRect);
        QPen pen;
        if (m_s->_FINE_TUNING != true)
            pen.setColor(Qt::red);
        else
            pen.setColor(Qt::blue);
        pen.setStyle(Qt::DashLine);
        m_mySelectionRect->setPen(pen);

    } else if (event->button() == Qt::LeftButton && m_s->_ADD_MASK) {
        m_isMouseLeftPressed = true;

        AddMask(ItemPoint);
        updatePixmap();
    } else if (event->button() == Qt::RightButton) {
        m_isTranslate  = true;
        m_isDrawRect   = false;
        m_lastMousePos = event->pos();
    } else if (event->button() == Qt::LeftButton && m_s->_REMOVE_MASK) {
        m_isMouseLeftPressed = true;

        RemoveMask(ItemPoint);
        updatePixmap();
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());
    if (item && item->type() != QGraphicsPixmapItem::Type && item->type() != QGraphicsRectItem::Type) {
        QGraphicsView::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton && m_mySelectionRect && m_s->_DRAW == true && m_s->_FINE_TUNING == false) {
        m_isDrawRect = false;

        //IMGwithRECT* _imgrect = new IMGwithRECT();
        QPointF Point                            = m_mySelectionRect->rect().topLeft();
        QRectF Rect                              = m_mySelectionRect->rect();
        m_Rect_vec[m_Rect_vec.size() - 1]->point = Point;
        m_Rect_vec[m_Rect_vec.size() - 1]->rect  = Rect;
        //m_Rect_vec.emplace_back(_imgrect);

        emit GraphicsView::addRect(m_mySelectionRect->rect());

        m_mySelectionRect = nullptr;
    } else if (event->button() == Qt::LeftButton && m_mySelectionRect && m_s->_DRAW == true && m_s->_FINE_TUNING == true) {

        m_isDrawRect = false;
        m_FTRect     = m_mySelectionRect->rect().toRect();

        if (m_Rect_vec.empty() || m_Rect_vec[m_Rect_vec.size() - 1]->Mat2.empty()) {
            return;
        }
        FineTuning* ft                                   = new FineTuning(nullptr, captureImageFineTuning(m_FTRect));
        ft->m_FineTuningBox->m_imageItem->m_isFineTuning = true;
        ft->show();
        ft->setAttribute(Qt::WA_DeleteOnClose);
        connect(ft, &FineTuning::destroyed, this, &GraphicsView::SpliceMask);

    } else if (event->button() == Qt::LeftButton && (m_s->_ADD_MASK || m_s->_REMOVE_MASK)) {
        m_isMouseLeftPressed = false;
    } else if (event->button() == Qt::RightButton) {
        m_isTranslate = false;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event) {
    fitFrame();
    Center();
    QGraphicsView::mouseDoubleClickEvent(event);
}

void GraphicsView::paintEvent(QPaintEvent* event) {
    QPainter painter(this->viewport());
    painter.drawTiledPixmap(QRect(QPoint(0, 0), QPoint(this->width(), this->height())), m_tilePixmap);
    QGraphicsView::paintEvent(event);
}

void GraphicsView::resizeEvent(QResizeEvent* event) {
    QSize currentviewBoxSize = this->viewport()->size();

    m_posInfoWidget->setGeometry(0, 0, currentviewBoxSize.width() * 0.98, 25);

    fitFrame();
    Center();

    QGraphicsView::resizeEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Q && !Q_Pressed) {
        Q_Pressed = true;
        SwitchImage(m_FT_pair.first);
    }

    QGraphicsView::keyPressEvent(event);
}

void GraphicsView::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Q && Q_Pressed) {
        Q_Pressed = false;
        SwitchImage(m_FT_pair.second);
    }

    QGraphicsView::keyReleaseEvent(event);
}

void GraphicsView::Center() {
    this->centerOn(m_imageItem->pixmap().width() / 2, m_imageItem->pixmap().height() / 2);
}

void GraphicsView::Zoom(double scaleFactor) {
    m_zoomValue *= scaleFactor;
    this->scale(scaleFactor, scaleFactor);
}

void GraphicsView::Translate(QPointF delta) {
    int w = viewport()->rect().width();
    int h = viewport()->rect().height();

    qreal scaleX = transform().m11();
    qreal scaleY = transform().m22();

    QPoint newCenter(w / 2. - delta.x(), h / 2. - delta.y());
    this->centerOn(mapToScene(newCenter));
}

void GraphicsView::fitFrame() {
    if (this->width() < 1 || m_image.width() < 1)
        return;

    double winWidth    = this->width();
    double winHeight   = this->height();
    double ScaleWidth  = (m_image.width() + 1) / winWidth;
    double ScaleHeight = (m_image.height() + 1) / winHeight;
    double s_temp      = ScaleWidth >= ScaleHeight ? 1 / ScaleWidth : 1 / ScaleHeight;
    double scale       = s_temp / m_zoomValue;
    if ((scale >= ZOOMMAX) || (scale <= ZOOMMIN))
        return;

    Zoom(scale);
    m_zoomValue = s_temp;
}

void GraphicsView::lowThresholdValueChangeEvent(int value) {
    GraphicsView::lowThreshold = value;
    GenerateContour();
    emit CannyThresholdChanged(0, lowThreshold);
}

void GraphicsView::highThresholdValueChangeEvent(int value) {
    GraphicsView::highThreshold = value;
    GenerateContour();
    emit CannyThresholdChanged(1, highThreshold);
}

void GraphicsView::GenerateConvexHull() {
    if (m_Rect_vec.size() == 0 || m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.empty() || m_Rect_vec[m_Rect_vec.size() - 1]->Mat1.empty()) {
        return;
    }
    cv::Mat inputMat = m_Rect_vec[m_Rect_vec.size() - 1]->Mat1.clone();
    cv::Mat RectMat  = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.clone();

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(inputMat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<std::vector<cv::Point>> hulls(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        cv::convexHull(contours[i], hulls[i]);
    }


    cv::Mat mask(inputMat.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));
    cv::Scalar fillColor(0, 0, 255, 100);
    for (size_t i = 0; i < hulls.size(); i++) {
        cv::drawContours(mask, hulls, (int)i, fillColor, cv::FILLED);
    }
    cv::cvtColor(RectMat, RectMat, cv::COLOR_BGR2BGRA);  // 1 Channel -> 4 Channel (BGRA)
    cv::addWeighted(RectMat, 1.0, mask, 1.0, 0.0, RectMat);
    cv::cvtColor(RectMat, RectMat, cv::COLOR_BGRA2RGBA);
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat3 = mask.clone();
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat2 = RectMat.clone();
    SetImage(QImage(RectMat.data, RectMat.cols, RectMat.rows, RectMat.step, QImage::Format_RGBA8888), m_currentImg);
    //cv::imshow("", RectMat);
}

void GraphicsView::StartFineTuning() {
    static bool state = false;
    if (!state) {
        state                   = true;
        this->m_s->_FINE_TUNING = true;
        this->m_s->_DRAW        = true;
    } else if (state) {
        state                   = true;
        this->m_s->_FINE_TUNING = false;
        this->m_s->_DRAW        = false;
    }
}

void GraphicsView::StartAddMask() {
    static bool state = false;

    if (state == false) {
        state                = true;
        this->m_s->_ADD_MASK = true;
    } else if (state == true) {
        state                = false;
        this->m_s->_ADD_MASK = false;
    }
}

void GraphicsView::StartRemoveMask() {
    static bool state = false;

    if (state == false) {
        state                   = true;
        this->m_s->_REMOVE_MASK = true;
    } else if (state == true) {
        state                   = false;
        this->m_s->_REMOVE_MASK = false;
    }
}

void GraphicsView::ExportMask() {
    auto Enum2String = [](const EXPORT_CLASS& m_ec) -> std::string {
        std::string className;
        switch (m_ec) {
            case EXPORT_CLASS::GOOD:
                return className = "good";
                break;
            case EXPORT_CLASS::CRACK:
                return className = "crack";
                break;
            case EXPORT_CLASS::GLUE_STRIP:
                return className = "glue_strip";
                break;
            case EXPORT_CLASS::LIQUID:
                return className = "liquid";
                break;
            case EXPORT_CLASS::SCRATCH:
                return className = "scratch";
                break;
            case EXPORT_CLASS::COLOR:
                return className = "color";
                break;
            case EXPORT_CLASS::COMBINED:
                return className = "combined";
                break;
            default:
                return "";
        }
    };

    static bool state             = false;
    std::string Path_ground_truth = "../CopperAlloyDatasets/ground_truth/";
    std::string Path_test         = "../CopperAlloyDatasets/test/";
    std::string Path_train        = "../CopperAlloyDatasets/train/";

    if (!state) {
        state = true;
#if defined(_WIN32)
        if (_mkdir(Path_ground_truth.c_str()) == 0 && _mkdir(Path_test.c_str()) == 0 && _mkdir(Path_train.c_str()) == 0) {
            std::cout << "Directory created successfully." << std::endl;
        } else {
            std::cout << "Failed to create directory. Error: " << errno << std::endl;
            if (errno == EEXIST) {
                std::cout << "Directory already exists." << std::endl;
            }
        }
#else
        mkdir(groundTruthPath.c_str(), 0777);
#endif

        for (int i = 1; i <= 8; ++i) {
            std::string str1 = Path_ground_truth + Enum2String(static_cast<EXPORT_CLASS>(i));
            std::string str2 = Path_test + Enum2String(static_cast<EXPORT_CLASS>(i));
            if (_mkdir(str1.c_str()) == 0 && _mkdir(str2.c_str()) == 0) {
                std::cout << "Directory created successfully." << std::endl;
            } else {
                std::cout << "Failed to create directory. Error: " << errno << std::endl;
                if (errno == EEXIST) {
                    std::cout << "Directory already exists." << std::endl;
                }
            }
        }
    }


    Path_ground_truth = Path_ground_truth + Enum2String(m_ec) + "/" + std::format("{:03}", m_Rect_vec.size()) + "_mask.png";
    Path_test         = Path_test + Enum2String(m_ec) + "/" + std::format("{:03}", m_Rect_vec.size()) + ".png";
    if (Enum2String(m_ec) != "good") {
        cv::Mat mat0 = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0;
        cv::Mat mat4 = m_Rect_vec[m_Rect_vec.size() - 1]->Mat4;
        cv::resize(mat0, mat0, cv::Size(640, 640), 0, 0, cv::INTER_LINEAR);
        cv::resize(mat4, mat4, cv::Size(640, 640), 0, 0, cv::INTER_LINEAR);
        cv::threshold(mat4, mat4, 128, 255, cv::THRESH_BINARY);
        cv::imwrite(Path_test, mat0);
        cv::imwrite(Path_ground_truth, mat4);
    } else {
        cv::Mat mat0 = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0;
        cv::resize(mat0, mat0, cv::Size(640, 640), 0, 0, cv::INTER_LINEAR);
        cv::imwrite(Path_train, mat0);
    }
}

void GraphicsView::NextImage() {
    auto nextImage = [](const std::string& fileName) -> std::string {
        std::regex pattern("(\\d+)(_.+)");
        std::smatch match;

        if (std::regex_match(fileName, match, pattern)) {
            int number = std::stoi(match[1]);
            number++;

            std::string newFileName = std::to_string(number) + match[2].str();
            return newFileName;
        }

        return fileName;
    };

    std::string nextImagePath = m_dataParentPath + "/" + nextImage(m_currentImg.toStdString());
    if (nextImagePath.empty())
        return;
    QImage originImage(QString::fromStdString(nextImagePath));
    this->SetImage(originImage, QString::fromStdString(nextImagePath));
}

void GraphicsView::LastImage() {
    auto nextImage = [](const std::string& fileName) -> std::string {
        std::regex pattern("(\\d+)(_.+)");
        std::smatch match;

        if (std::regex_match(fileName, match, pattern)) {
            int number = std::stoi(match[1]);
            number--;
            if (number < 0)
                number = 0;

            std::string newFileName = std::to_string(number) + match[2].str();
            return newFileName;
        }

        return fileName;
    };

    std::string nextImagePath = m_dataParentPath + "/" + nextImage(m_currentImg.toStdString());
    if (nextImagePath.empty())
        return;
    QImage originImage(QString::fromStdString(nextImagePath));
    this->SetImage(originImage, QString::fromStdString(nextImagePath));
}

void GraphicsView::ExportClassChanged(int id) {
    m_ec = static_cast<EXPORT_CLASS>(id);
}

void GraphicsView::GenerateContour(const cv::Mat& croppedImage) {
    cv::Mat img;
    if (m_Rect_vec.empty() || m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.empty())
        return;
    else if (croppedImage.empty()) {
        img = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.clone();
    } else {
        img = croppedImage.clone();
    }

    cv::GaussianBlur(img, img, cv::Size(5, 5), 1.5);

    cv::Mat edges;
    cv::Canny(img, edges, lowThreshold, highThreshold);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat Contours = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::drawContours(Contours, contours, -1, cv::Scalar(255), cv::FILLED);

    m_Rect_vec[m_Rect_vec.size() - 1]->Mat1 = Contours;
}

void GraphicsView::AddMask(const QPoint& ItemPoint) {
    QRect rect = m_FT_pair.second.rect();

    if (m_FT_pair.second.rect().contains(ItemPoint)) {
        int bytesPerLine = m_FT_pair.second.bytesPerLine();
        int bpp          = m_FT_pair.second.depth() / 8;

        uchar* data  = m_FT_pair.second.bits();
        //uchar* pixel = data + ItemPos.y() * bytesPerLine + ItemPos.x() * bpp;
        uchar* pixel = data + ItemPoint.y() * bytesPerLine + ItemPoint.x() * bpp;

        pixel[0] = 0;    // B
        pixel[1] = 255;  // G
        pixel[2] = 0;    // R
        pixel[3] = 100;
    }
}

void GraphicsView::SpliceMask() {
    QRect rect = m_FTRect;
    //cv::Rect roi(static_cast<int>(rect.left()), static_cast<int>(rect.top()), static_cast<int>(rect.width()), static_cast<int>(rect.height()));

    cv::Mat mat = m_FTMask;

    cv::Mat originMask = m_Rect_vec[m_Rect_vec.size() - 1]->Mat3.clone();
    cv::Mat binaryMask(originMask.size(), CV_8UC1);
    //cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
    assert(!originMask.empty() && "SpliceMask() image is empty!");

    for (int y = 0; y < originMask.rows; ++y) {
        for (int x = 0; x < originMask.cols; ++x) {
            cv::Vec4b& pixel = originMask.at<cv::Vec4b>(y, x);
            if (pixel == cv::Vec4b(0, 0, 0, 0)) {
                binaryMask.at<uchar>(y, x) = 0;
            } else {
                binaryMask.at<uchar>(y, x) = 255;
            }
        }
    }

    int whiteCount = 0;
    int blackCount = 0;
    int rows       = mat.rows;
    int cols       = mat.cols;
    for (int i = 0; i < std::max(rows, cols); ++i) {
        if (i < cols) {
            uchar topPixel = mat.at<uchar>(0, i);
            if (topPixel == 255)
                whiteCount++;
            else if (topPixel == 0)
                blackCount++;

            uchar bottomPixel = mat.at<uchar>(rows - 1, i);
            if (bottomPixel == 255)
                whiteCount++;
            else if (bottomPixel == 0)
                blackCount++;
        }
        if (i < rows) {
            uchar leftPixel = mat.at<uchar>(i, 0);
            if (leftPixel == 255)
                whiteCount++;
            else if (leftPixel == 0)
                blackCount++;

            uchar rightPixel = mat.at<uchar>(i, cols - 1);
            if (rightPixel == 255)
                whiteCount++;
            else if (rightPixel == 0)
                blackCount++;
        }
    }
    if (whiteCount > blackCount) {

        for (int y = 0; y < mat.rows; ++y) {
            for (int x = 0; x < mat.cols; ++x) {
                int binaryMask_x = rect.x() + x;
                int binaryMask_y = rect.y() + y;

                uchar pixelMat1 = mat.at<uchar>(y, x);
                uchar pixelMat2 = binaryMask.at<uchar>(binaryMask_y, binaryMask_x);

                if (pixelMat1 != pixelMat2) {
                    binaryMask.at<uchar>(binaryMask_y, binaryMask_x) = 0;
                }
            }
        }

    } else {
        for (int y = 0; y < mat.rows; ++y) {
            for (int x = 0; x < mat.cols; ++x) {
                int binaryMask_x = rect.x() + x;
                int binaryMask_y = rect.y() + y;

                uchar pixelMat1 = mat.at<uchar>(y, x);
                uchar pixelMat2 = binaryMask.at<uchar>(binaryMask_y, binaryMask_x);

                if (pixelMat1 != pixelMat2) {
                    binaryMask.at<uchar>(binaryMask_y, binaryMask_x) = 255;
                }
            }
        }
    }
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat4 = binaryMask.clone();

    cv::Mat mask(binaryMask.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));
    cv::Scalar fillColor(0, 0, 255, 100);
    for (int y = 0; y < binaryMask.rows; ++y) {
        for (int x = 0; x < binaryMask.cols; ++x) {
            uchar pixel = binaryMask.at<uchar>(y, x);
            if (pixel == 255) {
                mask.at<cv::Vec4b>(y, x) = fillColor;
            }
        }
    }

    m_Rect_vec[m_Rect_vec.size() - 1]->Mat3 = mask;

    cv::Mat Mat0 = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.clone();
    cv::cvtColor(Mat0, Mat0, cv::COLOR_BGR2BGRA);  // 1 Channel -> 4 Channel (BGRA)
    cv::addWeighted(Mat0, 1.0, mask, 1.0, 0.0, Mat0);
    cv::cvtColor(Mat0, Mat0, cv::COLOR_BGRA2RGBA);
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat2 = Mat0;  // RGBA

    emit RefreshViewBox2();
}

void GraphicsView::RemoveMask(const QPoint& ItemPoint) {
    QRgb pixelValue = m_FT_pair.first.pixel(ItemPoint.x(), ItemPoint.y());

    if (m_FT_pair.second.rect().contains(ItemPoint)) {
        int bytesPerLine = m_FT_pair.second.bytesPerLine();
        int bpp          = m_FT_pair.second.depth() / 8;

        uchar* data  = m_FT_pair.second.bits();
        uchar* pixel = data + ItemPoint.y() * bytesPerLine + ItemPoint.x() * bpp;

        pixel[2] = qBlue(pixelValue);   // B
        pixel[1] = qGreen(pixelValue);  // G
        pixel[0] = qRed(pixelValue);    // R
        pixel[3] = 255;                 // A
    }
}

void GraphicsView::setBackground(bool enabled, bool invertColor) {
    if (enabled) {
        // Prepare background check-board pattern
        m_tilePixmap.fill(invertColor ? QColor(220, 220, 220) : QColor(35, 35, 35));
        QPainter tilePainter(&m_tilePixmap);
        QColor color(50, 50, 50, 255);
        QColor invertedColor(210, 210, 210, 255);
        tilePainter.fillRect(0, 0, 18, 18, invertColor ? invertedColor : color);
        tilePainter.fillRect(18, 18, 18, 18, invertColor ? invertedColor : color);
        tilePainter.end();

    } else {
    }
}

QString GraphicsView::getSuffix(const QString& qstr) {
    return QFileInfo(qstr).fileName();
}

void GraphicsView::clearItem() {}

void GraphicsView::invertPixel(int x, int y) {
    m_lastPosPixel = m_FT_pair.second.pixelColor(x, y);

    QColor invertedPixel = QColor(255 - m_lastPosPixel.red(), 255 - m_lastPosPixel.green(), 255 - m_lastPosPixel.blue(), m_lastPosPixel.alpha());
    m_updateImg.setPixelColor(x, y, invertedPixel);
    updatePixmap();
}

void GraphicsView::restorePixel(int x, int y) {
    m_updateImg = m_FT_pair.second;
    m_updateImg.setPixelColor(x, y, m_lastPosPixel);
    updatePixmap();
}