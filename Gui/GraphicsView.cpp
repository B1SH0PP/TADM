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

//int GraphicsView::m_count = 0;

// @TODO - 这里2个状态的切换还是统一用函数来切换吧, 后期改为统一像`FineTuning`模式就是使用`StartFineTuning()`来切换. 这里直接在构造时在构造函数中指定
//         `sDRAW`状态太蠢了.
// @brief - 创建视图窗口对象.
// @param sDRAW -
// @param FT_pair - 在微调模式 (Fine-tuning) 下, 创建视图对象 (GraphicsView) 时一并传入的一对微调图像素材 (<待微调原图, 待微调凸包掩膜>).
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
    m_scene->addItem(m_imageItem);  // 将`QGraphicsPixmapItem`对象添加到 "场景" 中
    setScene(m_scene);              // 设置场景 (Scene) 设置为`QGraphicsView`的当前场景 (View)

    m_s->_DRAW = sDRAW;

    /* 关闭视觉窗口的水平/垂直拖动条 */
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setRenderHint(QPainter::Antialiasing);  // 抗锯齿渲染

    // 这里设置的是 "场景矩形" (画布) 的大小, (INT_MIN / 2, INT_MIN / 2) 设置的是场景左上角坐标, 这里设置的是`win`上
    // 的最小整数宏, 而 (INT_MAX, INT_MAX) 设置的是矩形的 "宽度" 和 "高度" (就是画布的size).
    setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
    // 设置视窗变换锚点为视窗中心 (AnchorViewCenter), 变换锚点即视图变换 (旋转 / 缩放) 时的中心点坐标.
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    // 设置视窗更新模式为 "完全视窗更新" (FullViewportUpdate), 即每次视窗都重新绘制整个区域.
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    /* 用于在视觉窗口左上角显示鼠标坐标以及图像的灰度值 */
    m_grayValueLabel = new QLabel(this);
    m_grayValueLabel->setObjectName("GrayValue_X");
    m_grayValueLabel->setStyleSheet("color:rgb(200,255,200); background-color:rgba(50,50,50,160); font: Microsoft YaHei;font-size: 15px;");
    m_grayValueLabel->setVisible(true);
    m_imageItem->setAcceptHoverEvents(true);
    m_grayValueLabel->setFixedWidth(702);
    m_grayValueLabel->setText(" W:0,H:0 | X:0,Y:0 | R:0,G:0,B:0");  // 初始化

    /* 显示图像名 */
    if (m_s->_DRAW) {
        m_ImgNameLabel = new QLabel(this);
        m_ImgNameLabel->setObjectName("ImgName");
        m_ImgNameLabel->setStyleSheet("color:rgb(200,255,200); background-color:rgba(50,50,50,160); font: Microsoft YaHei;font-size: 15px;");
        m_ImgNameLabel->setVisible(true);
        m_ImgNameLabel->setText("Current Image: ----");  // 初始化
    }

    /* 显示区域窗口 */
    m_posInfoWidget = new QWidget(this);
    m_posInfoWidget->setFixedHeight(20);
    m_posInfoWidget->setGeometry(0, 0, 900, 25);
    m_posInfoWidget->setStyleSheet("background-color:rgba(0,0,0,0);");  // 设置窗口的背景颜色
    // 创建一个应于于`m_posInfoWidget`的水平布局
    auto fLayout = new QHBoxLayout(m_posInfoWidget);
    fLayout->setSpacing(0);
    fLayout->setContentsMargins(0, 0, 0, 0);
    fLayout->addWidget(m_grayValueLabel);
    fLayout->addStretch();
    fLayout->addWidget(m_ImgNameLabel);
    //fLayout->addStretch();

    /* 初始化阈值 */
    emit CannyThresholdChanged(0, GraphicsView::lowThreshold);
    emit CannyThresholdChanged(1, GraphicsView::highThreshold);

    connect(m_imageItem, &ImageItem::RGBValue, this, [&](QString InfoVal) { m_grayValueLabel->setText(InfoVal); });

    /* 设置背景 */
    setBackground();
    // @1 - "场景 (Scene) 坐标" 相当于一张很大的画布, "视图中心" 相当于 "相机视窗". 而这里的`centerOn(0, 0)`作用就是把 "相机视窗" 的中心
    //      对准场景 (画布) 的 (0,0) 点, 这样看到的场景内容就是以 (0,0) 点为中心.
    centerOn(0, 0);
}

GraphicsView::~GraphicsView() {
    m_scene->deleteLater();
    delete m_imageItem;
}

// @brief - 为视觉窗口设置图像，是一个对外公共接口
void GraphicsView::SetImage(const QImage& image, const QString& materialDir) {
    // 加锁
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    m_qimage         = image.copy();
    m_image          = QPixmap::fromImage(image);
    m_imageItem->m_w = m_image.width();
    m_imageItem->m_h = m_image.height();
    m_imageItem->setPixmap(m_image);

    // 设置`scene`中心到图像中点
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

// @brief - 区别与`SetImage()`, 该函数不居中, 用于快速切换比较2张图像.
void GraphicsView::SwitchImage(const QImage& image) {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    m_qimage         = image.copy();
    m_image          = QPixmap::fromImage(image);
    m_imageItem->m_w = m_image.width();
    m_imageItem->m_h = m_image.height();
    m_imageItem->setPixmap(m_image);

    // 设置`scene`中心到图像中点
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
    roi                                     = roi & cv::Rect(0, 0, image.cols, image.rows);  // ROI超出图像边界检查
    cv::Mat croppedImage                    = image(roi);
    // @TODO - 这里暂时就默认按最上面一张为新, 回溯的功能还没实现.
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat0 = croppedImage;
    // 掩膜
    GenerateContour(croppedImage);

    // cv::Mat -> QImage
    // @TODO - 色彩好像有点问题, 与原图不一致.
    QImage qimg(croppedImage.data, croppedImage.cols, croppedImage.rows, croppedImage.step, QImage::Format_RGB888);

    // BGR -> RGB
    return qimg.rgbSwapped().copy();  // `.copy()`避免浅拷贝
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
    roi                   = roi & cv::Rect(0, 0, imageOri.cols, imageOri.rows);  // ROI超出图像边界检查
    cv::Mat croppedImage0 = imageOri(roi);                                       // 截取的原图中要微调的部分
    // (2). Mat2
    roi                   = roi & cv::Rect(0, 0, image.cols, image.rows);
    cv::Mat croppedImage1 = image(roi);  // 截取的原图微调部分加上红色掩膜

    // cv::Mat -> QImage
    // @TODO - 色彩好像有点问题, 与原图不一致.
    QImage qimg0(croppedImage0.data, croppedImage0.cols, croppedImage0.rows, croppedImage0.step, QImage::Format_RGB888);
    QImage qimg1(croppedImage1.data, croppedImage1.cols, croppedImage1.rows, croppedImage1.step, QImage::Format_RGBA8888);

    // BGR -> RGB
    return std::make_pair<QImage, QImage>(qimg0.rgbSwapped().copy(), qimg1.rgbSwapped().copy());  // `.copy()`避免浅拷贝
    //return qimg.copy();  // `.copy()`避免浅拷贝
}

void GraphicsView::Clear() {
    // 加锁
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    m_imageItem->setPixmap(QPixmap());
}

// @brief - 重写鼠标滚轮滚动的事件函数
//          主要依赖于Zoom()方法
void GraphicsView::wheelEvent(QWheelEvent* event) {
    //滚轮的滚动量
    QPoint scrollAmount = event->angleDelta();
    if ((scrollAmount.y() > 0) && (m_zoomValue >= ZOOMMAX))  //最大放大到原始图像的50倍
        return;
    else if ((scrollAmount.y() < 0) && (m_zoomValue <= ZOOMMIN))  //最小缩小到原始图像的50倍
        return;

    // 正值表示滚轮远离使用者,为放大;负值表示朝向使用者,为缩小
    scrollAmount.y() > 0 ? Zoom(1.1) : Zoom(0.9);
}

// @brief - 重写鼠标移动事件
//          主要依赖于`Translate()`方法
void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());

    // @1 - 这里2个坐标是一致的. 因为 "场景坐标系" 和 "项坐标系" 是重合的, 即图像中每个像素在项坐标系中的坐标和其在场景坐标系中的坐标是重合的.
    // @2 - 这里通过`mapToScene()`转换后精度提高了 (QPoint -> QPointF), 反而有利于后续处理.
    QPointF ScenePointF = mapToScene(event->pos());  // 视图坐标 (局部坐标 / 相机视场坐标) -> 场景坐标 (画布坐标)
    // @1 - 这里一定要加一次`static_cast<int>`强转是因为这种强转是 "截断式" 的, 即直接舍弃小数部分, 而不是`toPoint()`的四舍五入式, 这样就不会导致鼠标在
    //      像素右下角 (坐标的小数部分大于`.5`) 时会进位到下一个像素, 像素这种东西不能四舍五入进位.
    // QPoint ItemPoint = m_imageItem->mapFromScene(ScenePointF).toPoint();  // 场景坐标 -> 项坐标 (图像`Item`坐标)
    QPoint ItemPoint =
            QPoint(static_cast<int>(m_imageItem->mapFromScene(ScenePointF).x()), static_cast<int>(m_imageItem->mapFromScene(ScenePointF).y()));

    // 当鼠标的位置不在图像元素上时，则直接上送鼠标移动事件，不做处理
    if (item && item->type() != QGraphicsPixmapItem::Type && item->type() != QGraphicsRectItem::Type) {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    // @query - 好像在`mouseMoveEvent`处理函数中调用`event->button()`返回的枚举类型都是`0`, 检测不到按键类型.
    // Qt::MouseButton a = event->button();

    // case: 左键画框
    if (m_isDrawRect)  // 只有鼠标按下时才会进入
    {
        if (m_mySelectionRect) {
            QRectF newRect(mapToScene(m_lastMousePos), mapToScene(event->pos()));
            m_mySelectionRect->setRect(newRect.normalized());
        }
    }
    // case: 右键拖动图像
    else if (m_isTranslate) {
        // @1 - `m_isTranslate`在鼠标右键按下时, 会被置为true，在右键松开时置为false
        //      因此只有在鼠标拖动图像元素时，才会进入该if分支
        //      当前位置减去鼠标旧位置，得到鼠标的移动量
        QPointF mouseDelta = event->pos() - m_lastMousePos;
        // 根据移动量来转换坐标，实现拖动图像的效果
        Translate(mouseDelta);
        m_lastMousePos = event->pos();  // 更新记录鼠标坐标
    }
    // case: 实现反色光标 (反色光标是要无论增减掩膜模式下都要开启的)
    else if (!m_originImg.isNull() && ItemPoint.x() < m_originImg.width() && ItemPoint.y() < m_originImg.height()) {
        // (1).先恢复上一个像素
        if (m_lastPos != QPoint(-1, -1)) {
            restorePixel(m_lastPos.x(), m_lastPos.y());
        }

        // (2).重置突出显示当前像素
        invertPixel(ItemPoint.x(), ItemPoint.y());

        // (3).记录当前坐标, 以供(1)中恢复
        m_lastPos = QPoint(ItemPoint.x(), ItemPoint.y());
    }

    // case: 添加绿色掩膜
    if (m_s->_ADD_MASK && m_isMouseLeftPressed) {
        //AddMask(event->pos().x(), event->pos().y());
        AddMask(ItemPoint);
    }
    // case: 移除绿色掩膜和红色掩膜
    else if (m_s->_REMOVE_MASK && m_isMouseLeftPressed) {
        RemoveMask(ItemPoint);
    }

    // @1 - 重写事件处理函数时要注意, 必须调用一次父类的处理函数 (上送父类)
    //      已知这样的用处是:
    //      (1).确保事件继续按照正常的流程传递到场景中的 "项 (Item)" 下, 继续触发定义在项下的事件处理函数.
    QGraphicsView::mouseMoveEvent(event);
}

// @brief - 重写鼠标按下事件.
void GraphicsView::mousePressEvent(QMouseEvent* event) {
    // `itemAt()`用来检测鼠标当前位置是否在任何图形项上.
    QGraphicsItem* item = itemAt(event->pos());

    // @1 - 像`mouseMoveEvent()`事件一样, 做一次显式坐标转换.
    QPointF ScenePointF = mapToScene(event->pos());  // 视图坐标 (局部坐标 / 相机视场坐标) -> 场景坐标 (画布坐标)
    // QPoint ItemPoint = m_imageItem->mapFromScene(ScenePointF).toPoint();  // 场景坐标 -> 项坐标 (图像`Item`坐标)
    QPoint ItemPoint =
            QPoint(static_cast<int>(m_imageItem->mapFromScene(ScenePointF).x()), static_cast<int>(m_imageItem->mapFromScene(ScenePointF).y()));


    // `item`检查鼠标位置是否在 "图形对象" 上时;
    // `(item->type() != QGraphicsPixmapItem::Type)`检查该 "图形对象" 的类型不是`QGraphicsPixmapItem`类型;
    if (item && (item->type() != QGraphicsPixmapItem::Type)) {
        // 若不是自定义事件, 则调用父类的处理函数 (默认处理)
        QGraphicsView::mousePressEvent(event);
        return;
    }

    // 左键按下
    if (event->button() == Qt::LeftButton && m_s->_DRAW == true) {
        m_isTranslate  = false;
        m_isDrawRect   = true;
        m_lastMousePos = event->pos();  // 记录Rect左上角坐标

        if (m_selectionRect) {
            scene()->removeItem(m_selectionRect);
            delete m_selectionRect;
            m_selectionRect = nullptr;
        }

        /* 1.Naive */
        //QPen pen(Qt::red);
        //pen.setStyle(Qt::DashLine);
        //// @1 - 使用`addRect()`添加一个`QGraphicsRectItem`对象到场景 (QGraphicsScene) 中, 并用成员属性`m_selectionRect`接收, 后续在`mouseMoveEvent()`中使
        ////      用`setRect()不断更新这个`QGraphicsRectItem`对象的参数.
        //m_selectionRect = scene()->addRect(QRectF(mapToScene(m_lastMousePos), QSize()), pen, QBrush(Qt::transparent));

        /* 2.Custom */
        m_mySelectionRect = new RectItem();
        m_RectItems_vec.emplace_back(m_mySelectionRect);  // 存一下
        auto it = m_ImgWithRects_map.find(m_currentImg);
        if (it == m_ImgWithRects_map.end() && m_s->_FINE_TUNING == false) {  // case: 没有这幅图, 在`_map`中注册当前图名
            //std::vector<IMGwithRECT*> temp_ImgWithRect_vec;
            IMGwithRECT* _imgrect = new IMGwithRECT();
            m_Rect_vec.emplace_back(_imgrect);
            m_ImgWithRects_map[m_currentImg] = m_Rect_vec;

        } else if (m_s->_FINE_TUNING == false) {  // case: 当前图已注册, 在其`vector`中插入新`IMGwithRECT`数据 (代表一个Rect数据)
            IMGwithRECT* _imgrect = new IMGwithRECT();
            m_Rect_vec.emplace_back(_imgrect);

            // @1 - 不用在这刚点击时就插入左上角坐标点, 因为目前`m_mySelectionRect`还是(0,0), 目前选择在鼠标释放时插入.
            // m_Rect_vec[m_Rect_vec.size() - 1]->point = m_mySelectionRect->rect().topLeft();
        }

        // @1 - `mapToScene()`作用是将 "视图坐标" 转为 "场景坐标". 又因为我的图像左上角与场景坐标系原点 (0,0) 重合 (第四象限), 所以这里计算出的场景坐标
        //      与图像坐标系重合 (一致), 即与显示在左上角的鼠标光标坐标值一致, 即是场景坐标又是图像坐标.
        //      而`m_lastMousePos`事件记录的是光标相对 "视图坐标系" 的坐标, 所以需要转换一下.
        // m_mySelectionRect->setPos(mapToScene(m_lastMousePos).x(),
        //                           mapToScene(m_lastMousePos).y());  // 设置矩形项原点 (默认是左上角点) 位于场景中的坐标
        m_scene->addItem(m_mySelectionRect);
        QPen pen;
        if (m_s->_FINE_TUNING != true)
            pen.setColor(Qt::red);
        else
            pen.setColor(Qt::blue);
        pen.setStyle(Qt::DashLine);
        m_mySelectionRect->setPen(pen);

        //m_mySelectionRect->setRect(0, 0, 1, 1);
        //m_mySelectionRect->setTransformOriginPoint(0, 0);
        //m_mySelectionRect->setRotation(0);
    }
    // case: Add_Mask
    else if (event->button() == Qt::LeftButton && m_s->_ADD_MASK) {
        m_isMouseLeftPressed = true;

        // 单点像素添加Mask, 不一定要等`MoveEvent`
        AddMask(ItemPoint);
        updatePixmap();
    }
    // case: 右键按下
    else if (event->button() == Qt::RightButton) {
        m_isTranslate  = true;
        m_isDrawRect   = false;
        m_lastMousePos = event->pos();  // 更新鼠标位置
    }
    // case: Remove_Mask
    else if (event->button() == Qt::LeftButton && m_s->_REMOVE_MASK) {
        m_isMouseLeftPressed = true;

        // 单点像素移除Mask, 不一定要等`MoveEvent`
        RemoveMask(ItemPoint);
        updatePixmap();
    }

    QGraphicsView::mousePressEvent(event);
}

// @brief - 鼠标释放响应函数.
void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());
    if (item && item->type() != QGraphicsPixmapItem::Type && item->type() != QGraphicsRectItem::Type) {
        // 调用父类处理函数, 并结束调用
        QGraphicsView::mouseReleaseEvent(event);
        return;
    }

    // 键释放
    // @1 - `&& selectionRect`防止下文`selectionRect->rect()`取到空指针.
    if (event->button() == Qt::LeftButton && m_mySelectionRect && m_s->_DRAW == true && m_s->_FINE_TUNING == false) {
        // 关闭`Rect`的刷新 (即不再跟随鼠标)
        m_isDrawRect = false;

        /* 存数据 */
        // @1 - 这里 Rect 的坐标是相对显示的图片的, 图片的左上角点为 (0,0) 点, 宽和高就是图像的 w 和 h.
        //      即: 如果画一个从图像左上角起的矩形, 这个矩形的参数存储为: x = 0, y = 0, h = ..., w = ...

        //IMGwithRECT* _imgrect = new IMGwithRECT();
        QPointF Point                            = m_mySelectionRect->rect().topLeft();
        QRectF Rect                              = m_mySelectionRect->rect();
        m_Rect_vec[m_Rect_vec.size() - 1]->point = Point;
        m_Rect_vec[m_Rect_vec.size() - 1]->rect  = Rect;
        //m_Rect_vec.emplace_back(_imgrect);

        // @query - 不知道为什么这里直接用`std::pair`构造函数 (不用`std::make_pair`) 会出错.
        // std::pair<cv::Mat, cv::Mat> MatMatPair(cv::Mat(), cv::Mat());

        emit GraphicsView::addRect(m_mySelectionRect->rect());

        m_mySelectionRect = nullptr;
    } else if (event->button() == Qt::LeftButton && m_mySelectionRect && m_s->_DRAW == true && m_s->_FINE_TUNING == true) {
        // 微调模式: `_DRAW == true` && `_FINE_TUNING == true`
        //          (1).框不会被记录到容器`m_Rect_vec`中;
        //          (2).框不会同步到`QTableWidget`中;

        m_isDrawRect = false;

        //QPoint Point = m_mySelectionRect->rect().topLeft().toPoint();
        m_FTRect = m_mySelectionRect->rect().toRect();

        // @1 - `m_Rect_vec.empty()`是避免没打开图像直接画 "蓝框" 导致容器越界报错.
        //      `|| m_Rect_vec[m_Rect_vec.size() - 1]->Mat2.empty()`是避免没有生成 "红色掩膜" 就画 "蓝框" 导致报错.
        if (m_Rect_vec.empty() || m_Rect_vec[m_Rect_vec.size() - 1]->Mat2.empty()) {
            return;
        }
        FineTuning* ft                                   = new FineTuning(nullptr, captureImageFineTuning(m_FTRect));
        ft->m_FineTuningBox->m_imageItem->m_isFineTuning = true;
        ft->show();
        ft->setAttribute(Qt::WA_DeleteOnClose);  // 关闭窗口就析构 (默认是隐藏)
        connect(ft, &FineTuning::destroyed, this, &GraphicsView::SpliceMask);

    }
    // case: 松开鼠标关闭 AddMask 模式 (即: 停止对鼠标路径着色)
    //       松开鼠标关闭 RemoveMask 模式 (即: 停止擦除鼠标路径掩膜)
    else if (event->button() == Qt::LeftButton && (m_s->_ADD_MASK || m_s->_REMOVE_MASK)) {
        m_isMouseLeftPressed = false;
    }
    // case: 右键释放, 关闭图片移动状态
    else if (event->button() == Qt::RightButton) {
        m_isTranslate = false;
    }

    // 调用父类处理函数
    QGraphicsView::mouseReleaseEvent(event);
}

// @brief - 在视觉窗口上双击鼠标左键，会有图像居中效果，主要依赖于Center()方法.
void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event) {
    // 自适应图像大小至视觉窗口的大小
    fitFrame();
    // 居中显示
    Center();
    QGraphicsView::mouseDoubleClickEvent(event);
}

// @brief - 绘制回调函数, 用于视觉窗口背景绘制
void GraphicsView::paintEvent(QPaintEvent* event) {
    QPainter painter(this->viewport());
    painter.drawTiledPixmap(QRect(QPoint(0, 0), QPoint(this->width(), this->height())), m_tilePixmap);  // 绘制背景
    QGraphicsView::paintEvent(event);
}

void GraphicsView::resizeEvent(QResizeEvent* event) {
    QSize currentviewBoxSize = this->viewport()->size();

    // 根据视窗的宽度动态调整矩形的宽度
    m_posInfoWidget->setGeometry(0, 0, currentviewBoxSize.width() * 0.98, 25);

    fitFrame();
    Center();

    // 调用 (上送) 父类
    QGraphicsView::resizeEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Q && !Q_Pressed) {  // Q键
        Q_Pressed = true;
        SwitchImage(m_FT_pair.first);
        // @1 - 用不着显示触发
        //update();
        //repaint();  // 立即触发重绘
    }

    QGraphicsView::keyPressEvent(event);
}

void GraphicsView::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Q && Q_Pressed) {  // Q键
        Q_Pressed = false;
        SwitchImage(m_FT_pair.second);
        //update();
        //repaint();  // 立即触发重绘
    }

    QGraphicsView::keyReleaseEvent(event);
}

// @brief - 视图居中.
void GraphicsView::Center() {
    // 调用`QGraphicsView`自带的方法`centerOn`, 使视觉窗口的中心位于图像元素的中心点
    // 注意, 重复一遍, 这里并没有移动图像元素自己的坐标, 而是将视图移动到了图像元素的中心点
    this->centerOn(m_imageItem->pixmap().width() / 2, m_imageItem->pixmap().height() / 2);
}

void GraphicsView::Zoom(double scaleFactor) {
    // 记录下当前相对于图像原图的缩放比例，可以记录下当前图像真实放大缩小了多少倍
    // 可以借此来限制图像的最大或最小缩放比例
    m_zoomValue *= scaleFactor;
    // 调用视图类`QGraphicsView`自带的`scale()`缩放方法，来对视图进行缩放，实现放大缩小的功能
    // 缩放的同时，视图里的所有元素也会进行缩放，也就达到了视觉窗口放大缩小的效果
    this->scale(scaleFactor, scaleFactor);
}

// @brief - 当图像被拖动时调用该函数，传入参数为图像的拖动量
void GraphicsView::Translate(QPointF delta) {
    int w = viewport()->rect().width();
    int h = viewport()->rect().height();

    // 获取当前的缩放比例
    qreal scaleX = transform().m11();  // 横向缩放因子
    qreal scaleY = transform().m22();  // 纵向缩放因子

    // 通过拖动量获取新的中心点
    QPoint newCenter(w / 2. - delta.x(), h / 2. - delta.y());  // +0.5为向上取整
    // `mapToScene()`方法会将视图坐标转换为场景坐标，再通过centerOn来改变当前视图在场景中的坐标，实现图像拖动的效果
    // 重复一遍，其实图像的坐标从来没有改变，依然位于场景的(0,0)位置，改变的是视图的坐标
    this->centerOn(mapToScene(newCenter));
}

// @brief 图片自适应方法，根据图像原始尺寸和当前视觉窗口的大小计算出应缩放的尺寸，再根据已经缩放的比例计算还差的缩放比例，
//        补齐应缩放的比例，使得图像和视觉窗口大小相适配
void GraphicsView::fitFrame() {
    if (this->width() < 1 || m_image.width() < 1)
        return;

    //计算缩放比例
    double winWidth    = this->width();
    double winHeight   = this->height();
    double ScaleWidth  = (m_image.width() + 1) / winWidth;
    double ScaleHeight = (m_image.height() + 1) / winHeight;
    double s_temp      = ScaleWidth >= ScaleHeight ? 1 / ScaleWidth : 1 / ScaleHeight;
    double scale       = s_temp / m_zoomValue;
    if ((scale >= ZOOMMAX) || (scale <= ZOOMMIN))  //最大放大到原始图像的50倍
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

    // findContours -> convexHull -> drawContours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(inputMat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<std::vector<cv::Point>> hulls(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        cv::convexHull(contours[i], hulls[i]);
    }

    // 轮廓绘制
    //cv::Mat drawing = cv::Mat::zeros(inputMat.size(), CV_8UC3);
    //for (size_t i = 0; i < hulls.size(); i++) {
    //    cv::drawContours(drawing, hulls, (int)i, cv::Scalar(0, 255, 0), 1);
    //}

    // 掩膜绘制
    cv::Mat mask(inputMat.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));  // 创建透明掩膜 (包含 Alpha 通道)
    cv::Scalar fillColor(0, 0, 255, 100);                            // 淡红色，100 是 alpha 值 (0-255)，表示透明度
    for (size_t i = 0; i < hulls.size(); i++) {
        cv::drawContours(mask, hulls, (int)i, fillColor, cv::FILLED);  // 填充凸包区域
    }
    cv::cvtColor(RectMat, RectMat, cv::COLOR_BGR2BGRA);  // 1 Channel -> 4 Channel (BGRA)
    cv::addWeighted(RectMat, 1.0, mask, 1.0, 0.0, RectMat);
    cv::cvtColor(RectMat, RectMat, cv::COLOR_BGRA2RGBA);
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat3 = mask.clone();     // 保存掩膜
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat2 = RectMat.clone();  // 保存掩膜&Rect
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

    if (!state) {  // case: 只执行一次
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
        mkdir(groundTruthPath.c_str(), 0777);  // Unix/Linux
#endif

        // 创建好所有类别的文件夹
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
    }
    // case: good
    else {
        cv::Mat mat0 = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0;
        cv::resize(mat0, mat0, cv::Size(640, 640), 0, 0, cv::INTER_LINEAR);
        cv::imwrite(Path_train, mat0);
    }


    //switch (m_ec) {
    //    case EXPORT_CLASS::GOOD: {
    //        SaveImage(Enum2String(m_ec));
    //        break;
    //    }

    //    case EXPORT_CLASS::CRACK: {

    //        break;
    //    }
    //}
}

void GraphicsView::NextImage() {
    // 正则表达式提取数字部分
    auto nextImage = [](const std::string& fileName) -> std::string {
        std::regex pattern("(\\d+)(_.+)");
        std::smatch match;

        if (std::regex_match(fileName, match, pattern)) {
            int number = std::stoi(match[1]);
            number++;
            // @TODO - 加一个判断, 检测路径下最大文件数, 防止 number 越界.
            //if (number < 0)
            //    number = 0;

            std::string newFileName = std::to_string(number) + match[2].str();
            return newFileName;
        }

        // 如果没有找到数字，返回原始文件名
        return fileName;
    };

    std::string nextImagePath = m_dataParentPath + "/" + nextImage(m_currentImg.toStdString());
    if (nextImagePath.empty())
        return;
    QImage originImage(QString::fromStdString(nextImagePath));
    this->SetImage(originImage, QString::fromStdString(nextImagePath));
}

void GraphicsView::LastImage() {
    // 正则表达式提取数字部分
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

        // 如果没有找到数字，返回原始文件名
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
    // @1 - 未读图时拖动`Slider`, 会导致bug, 所以这里除了判断`empty()`, 还需额外判断一次`m_Rect_vec.size()`
    // @1 - 下文加上`|| m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.empty()`是为了防止当画了`Rect`框后但是没按下`Save`按钮, 即没有执行`captureImage()`, 没有
    //      将`Rect`框内的图像存入`m_Rect_vec`容器. (可以debug看下, 若不按`Save`按钮`Mat0`是空的).
    if (m_Rect_vec.empty() || m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.empty())
        return;
    else if (croppedImage.empty()) {  // case: 刷新掩膜
        // @1 - `cv::Mat`用等号`=`赋值, 其底层执行的其实是共享数据的指针, 而不是拷贝数据, 所以这里修改了`img`容器内的也会跟着被修改.
        //      所以要最后加个`.clone()`
        img = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.clone();
    } else {  // case: 初次生成掩膜
        img = croppedImage.clone();
    }

    // Gauss
    //auto Gauss_start = std::chrono::high_resolution_clock::now();
    cv::GaussianBlur(img, img, cv::Size(5, 5), 1.5);
    //auto Gauss_end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> Gauss_elapsed = Gauss_end - Gauss_start;  // second
    //std::cout << "Gauss" << Gauss_elapsed.count() << " seconds.\n";

    // Contours
    //auto Canny_start = std::chrono::high_resolution_clock::now();
    cv::Mat edges;
    cv::Canny(img, edges, lowThreshold, highThreshold);
    //auto Canny_end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> Canny_elapsed = Canny_end - Canny_start;  // second
    //std::cout << "Canny" << Canny_elapsed.count() << " seconds.\n";

    std::vector<std::vector<cv::Point>> contours;
    //auto findContours_start = std::chrono::high_resolution_clock::now();
    cv::findContours(edges, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    //auto findContours_end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> findContours_elapsed = findContours_end - findContours_start;  // second
    //std::cout << "findContours" << findContours_elapsed.count() << " seconds.\n";

    // Contours
    cv::Mat Contours = cv::Mat::zeros(img.size(), CV_8UC1);
    //auto drawContours_start = std::chrono::high_resolution_clock::now();
    cv::drawContours(Contours, contours, -1, cv::Scalar(255), cv::FILLED);
    //auto drawContours_end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> drawContours_elapsed = drawContours_end - drawContours_start;  // second
    //std::cout << "drawContours" << drawContours_elapsed.count() << " seconds.\n";

    //for (const auto& contour : contours) {
    //    cv::drawContours(Contours, std::vector<std::vector<cv::Point>>{contour}, -1, cv::Scalar(0), cv::FILLED);
    //}

    // @TODO - 这里暂时就默认按最上面一张为新, 回溯的功能还没实现.
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat1 = Contours;


    //cv::imshow("Contours", Contours);
    //cv::waitKey(0);
}

void GraphicsView::AddMask(const QPoint& ItemPoint) {
    QRect rect = m_FT_pair.second.rect();

    // case: 防止越界
    if (m_FT_pair.second.rect().contains(ItemPoint)) {
        int bytesPerLine = m_FT_pair.second.bytesPerLine();
        int bpp          = m_FT_pair.second.depth() / 8;  // 每像素的字节数 (ie: 24位RGB图像是3字节)

        uchar* data  = m_FT_pair.second.bits();  // 首地址
        //uchar* pixel = data + ItemPos.y() * bytesPerLine + ItemPos.x() * bpp;
        uchar* pixel = data + ItemPoint.y() * bytesPerLine + ItemPoint.x() * bpp;

        pixel[0] = 0;    // B
        pixel[1] = 255;  // G
        pixel[2] = 0;    // R
        pixel[3] = 100;

        //pixel[0] = static_cast<uchar>(pixel[0] * (1 - pixel[3]) + pixel[3]);
        //pixel[1] = static_cast<uchar>(pixel[1] * (1 - pixel[3]) + pixel[3]);
        //pixel[2] = static_cast<uchar>(pixel[2] * (1 - pixel[3]) + pixel[3]);
        //pixel[3] = 100;
    }
    //m_updateImg = m_FT_pair.second;
}

void GraphicsView::SpliceMask() {
    QRect rect = m_FTRect;
    //cv::Rect roi(static_cast<int>(rect.left()), static_cast<int>(rect.top()), static_cast<int>(rect.width()), static_cast<int>(rect.height()));

    cv::Mat mat = m_FTMask;

    cv::Mat originMask = m_Rect_vec[m_Rect_vec.size() - 1]->Mat3.clone();
    cv::Mat binaryMask(originMask.size(), CV_8UC1);
    //cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
    assert(!originMask.empty() && "SpliceMask() image is empty!");

    // 将原本纯红色掩膜转为二值化掩膜方便后续逐像素比较
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

    /* 判断传入的是 Add (黑包白) 还是 Remove (白包黑) */
    int whiteCount = 0;
    int blackCount = 0;
    int rows       = mat.rows;
    int cols       = mat.cols;
    for (int i = 0; i < std::max(rows, cols); ++i) {
        if (i < cols) {
            // 上边
            uchar topPixel = mat.at<uchar>(0, i);
            if (topPixel == 255)
                whiteCount++;
            else if (topPixel == 0)
                blackCount++;

            // 下边
            uchar bottomPixel = mat.at<uchar>(rows - 1, i);
            if (bottomPixel == 255)
                whiteCount++;
            else if (bottomPixel == 0)
                blackCount++;
        }
        if (i < rows) {
            // 左边
            uchar leftPixel = mat.at<uchar>(i, 0);
            if (leftPixel == 255)
                whiteCount++;
            else if (leftPixel == 0)
                blackCount++;

            // 右边
            uchar rightPixel = mat.at<uchar>(i, cols - 1);
            if (rightPixel == 255)
                whiteCount++;
            else if (rightPixel == 0)
                blackCount++;
        }
    }
    if (whiteCount > blackCount) {  // case: RemoveMask, 进行像素或`||`操作
        //std::cout << "RemoveMask mode" << std::endl;
        for (int y = 0; y < mat.rows; ++y) {
            for (int x = 0; x < mat.cols; ++x) {
                // Get corresponding pixel in mat2
                int binaryMask_x = rect.x() + x;
                int binaryMask_y = rect.y() + y;

                uchar pixelMat1 = mat.at<uchar>(y, x);
                uchar pixelMat2 = binaryMask.at<uchar>(binaryMask_y, binaryMask_x);

                // Compare pixels
                if (pixelMat1 != pixelMat2) {
                    binaryMask.at<uchar>(binaryMask_y, binaryMask_x) = 0;
                }
            }
        }

    } else {  // case: AddMask
        //std::cout << "AddMask mode" << std::endl;
        for (int y = 0; y < mat.rows; ++y) {
            for (int x = 0; x < mat.cols; ++x) {
                // Get corresponding pixel in mat2
                int binaryMask_x = rect.x() + x;
                int binaryMask_y = rect.y() + y;

                uchar pixelMat1 = mat.at<uchar>(y, x);
                uchar pixelMat2 = binaryMask.at<uchar>(binaryMask_y, binaryMask_x);

                // Compare pixels
                if (pixelMat1 != pixelMat2) {
                    binaryMask.at<uchar>(binaryMask_y, binaryMask_x) = 255;
                }
            }
        }
    }
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat4 = binaryMask.clone();

    // 修改 Mat3 粉红色掩膜
    cv::Mat mask(binaryMask.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));  // 创建透明掩膜 (包含 Alpha 通道)
    cv::Scalar fillColor(0, 0, 255, 100);                              // 淡红色，100 是 alpha 值 (0-255)，表示透明度
    for (int y = 0; y < binaryMask.rows; ++y) {
        for (int x = 0; x < binaryMask.cols; ++x) {
            uchar pixel = binaryMask.at<uchar>(y, x);
            if (pixel == 255) {
                mask.at<cv::Vec4b>(y, x) = fillColor;
            }
        }
    }

    // 更新 m_Rect_vec 容器内的掩膜
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat3 = mask;

    // 重新叠加
    cv::Mat Mat0 = m_Rect_vec[m_Rect_vec.size() - 1]->Mat0.clone();
    cv::cvtColor(Mat0, Mat0, cv::COLOR_BGR2BGRA);  // 1 Channel -> 4 Channel (BGRA)
    cv::addWeighted(Mat0, 1.0, mask, 1.0, 0.0, Mat0);
    cv::cvtColor(Mat0, Mat0, cv::COLOR_BGRA2RGBA);
    m_Rect_vec[m_Rect_vec.size() - 1]->Mat2 = Mat0;  // 通道顺序: RGBA

    // @query - 这里我只让 ViewBox2 刷新这张照片, 我不知道有什么好办法? 是不是要设计时让这种会创建多个对象的类在创建时其下用成员函数记录对象内存地址来区分.
    //          这里我只能让这里发送信号到外层的 Gui 类下, 让 Gui 类使用其下的 ViewBox2 指针来指定视窗对象.
    emit RefreshViewBox2();
}

void GraphicsView::RemoveMask(const QPoint& ItemPoint) {
    //QImage FTqimg = m_FT_pair.first;
    //cv::Mat mat;
    //if (FTqimg.format() == QImage::Format_RGBA8888) {
    //    // 使用 QImage 数据构造 cv::Mat，不进行数据拷贝
    //    mat = cv::Mat(FTqimg.height(), FTqimg.width(), CV_8UC4, const_cast<uchar*>(FTqimg.bits()), FTqimg.bytesPerLine());
    //}
    //// 处理 QImage::Format_ARGB32 格式
    //else if (FTqimg.format() == QImage::Format_ARGB32) {
    //    // OpenCV 使用的是 BGRA 排列，所以需要手动将 RGBA 排列转换为 BGRA
    //    mat = cv::Mat(FTqimg.height(), FTqimg.width(), CV_8UC4, const_cast<uchar*>(FTqimg.bits()), FTqimg.bytesPerLine());
    //    cv::cvtColor(mat, mat, cv::COLOR_BGRA2RGBA);  // 进行颜色通道转换
    //} else if (FTqimg.format() == QImage::Format_RGB888) {
    //    // 使用 QImage 数据构造 cv::Mat，不进行数据拷贝
    //    mat = cv::Mat(FTqimg.height(), FTqimg.width(), CV_8UC3, const_cast<uchar*>(FTqimg.bits()), FTqimg.bytesPerLine());

    //    // OpenCV 使用 BGR 顺序，而 QImage 使用 RGB，因此需要转换颜色通道顺序
    //    cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
    //}

    QRgb pixelValue = m_FT_pair.first.pixel(ItemPoint.x(), ItemPoint.y());

    // case: 防止越界
    if (m_FT_pair.second.rect().contains(ItemPoint)) {
        int bytesPerLine = m_FT_pair.second.bytesPerLine();
        int bpp          = m_FT_pair.second.depth() / 8;  // 每像素的字节数 (ie: 24位RGB图像是3字节)

        uchar* data  = m_FT_pair.second.bits();  // 首地址
        //uchar* pixel = data + ItemPos.y() * bytesPerLine + ItemPos.x() * bpp;
        uchar* pixel = data + ItemPoint.y() * bytesPerLine + ItemPoint.x() * bpp;

        pixel[2] = qBlue(pixelValue);   // B
        pixel[1] = qGreen(pixelValue);  // G
        pixel[0] = qRed(pixelValue);    // R
        pixel[3] = 255;                 // A
    }
}

// @brief - 设置视觉窗口背景为棋盘格样式
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

        //setBackgroundBrush(m_tilePixmap);  // 当取消注释时，视图窗口背景棋盘格会跟随视窗一起缩放
    } else {
        //setBackgroundBrush(Qt::transparent);
    }
}

QString GraphicsView::getSuffix(const QString& qstr) {
    return QFileInfo(qstr).fileName();
}

void GraphicsView::clearItem() {
    //this->;
}

void GraphicsView::invertPixel(int x, int y) {
    // @1 - 因为是4通道图, 不能用`QRgb`, 改用`QColor`

    // 先保存当前像素值, 以供后期恢复
    //m_lastPosPixel = m_originImg.pixelColor(x, y);
    m_lastPosPixel = m_FT_pair.second.pixelColor(x, y);

    // QColor invertedPixel = QColor(72, 201, 165, m_lastPosPixel.alpha());
    QColor invertedPixel = QColor(255 - m_lastPosPixel.red(), 255 - m_lastPosPixel.green(), 255 - m_lastPosPixel.blue(), m_lastPosPixel.alpha());
    m_updateImg.setPixelColor(x, y, invertedPixel);
    updatePixmap();
}

void GraphicsView::restorePixel(int x, int y) {
    // @1 - 不加这一步, 到出现 README 中记录的第一个问题.
    m_updateImg = m_FT_pair.second;
    m_updateImg.setPixelColor(x, y, m_lastPosPixel);
    updatePixmap();
}