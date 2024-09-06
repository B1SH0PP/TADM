#pragma once
// Qt
#include <QWidget>
#include <QToolBar>
#include <QDrag>
#include <QMimeData>
#include "ui_FineTuning.h"
// own
#include "GraphicsView.h"
//#include "RectItem.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class FineTuningClass;
};

QT_END_NAMESPACE

class GraphicsView;

class FineTuning : public QWidget {
    Q_OBJECT

 public:
    FineTuning(QWidget* parent = nullptr, const std::pair<QImage, QImage>& FTImage_pair = std::pair<QImage, QImage>(QImage(), QImage()));
    ~FineTuning();
    void StartAddMask();
    void StartRemoveMask();

 protected:
    void closeEvent(QCloseEvent* event) override;


 public:
    Ui::FineTuningClass* ui;

    GraphicsView* m_FineTuningBox;

    QImage m_FTImageWithOriMask;
};
