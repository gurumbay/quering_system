#pragma once

#include <map>
#include <set>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QWheelEvent>
#include "sim/core/Simulator.h"

// Canvas that does the actual drawing
class TimelineCanvas : public QWidget {
    Q_OBJECT
public:
    explicit TimelineCanvas(QWidget* parent = nullptr);
    
    void setSimulator(Simulator* sim, const SimulationConfig& config);
    void updateSize();
    void setZoomLevel(double zoom);
    double getZoomLevel() const { return zoomLevel_; }
    Simulator* getSimulator() const { return simulator_; }
    const SimulationConfig& getConfig() const { return config_; }
    
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void zoomChanged(double zoom);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
private:
    void drawTimeline(QPainter& painter);
    double calculateNiceTimeStep(double range);
    QString formatTimeValue(double value);

    Simulator* simulator_ = nullptr;
    SimulationConfig config_;
    double zoomLevel_ = 1.0;
    
public:
    // Drawing parameters
    static constexpr int MARGIN_LEFT = 80;
    static constexpr int MARGIN_RIGHT = 40;
    static constexpr int MARGIN_TOP = 60;
    static constexpr int MARGIN_BOTTOM = 40;
    static constexpr int LINE_HEIGHT = 40;
    static constexpr int PIXELS_PER_TIME_UNIT = 50;
};

// Main timeline widget with controls and scroll area
class TimelineWidget : public QWidget {
    Q_OBJECT
public:
    explicit TimelineWidget(QWidget* parent = nullptr);
    
    void setSimulator(Simulator* sim, const SimulationConfig& config);
    void updateCanvas();
    void scrollToCurrentTime();
    void updateOverlayLabels();
    
private slots:
    void onZoomIn();
    void onZoomOut();
    void onCanvasZoomChanged(double zoom);
    void updateZoomLabel();
    
private:
    TimelineCanvas* canvas_;
    QScrollArea* scrollArea_;
    QWidget* overlayLabels_;
    QPushButton* btnZoomIn_;
    QPushButton* btnZoomOut_;
    QLabel* zoomLabel_;
};
