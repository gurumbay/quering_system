#pragma once

#include <QMainWindow>
#include <QTimer>
#include "sim/core/Simulator.h"

// Forward declarations for Qt widgets
class QLabel;
class QSpinBox;
class QTableWidget;
class QDoubleSpinBox;
class QPushButton;
class QWidget;

// Forward declaration for timeline widget
class TimelineWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onStep();
    void onRun();
    void onPause();
    void onRunToEnd();
    void onReset();
    void onPrintCsv();
    void onTick();

private:
    void rebuildSimulator();
    void updateUi();
    void updateConfigFromUI();
    void updateButtonStates();
    void updateTimeline();

    Simulator* simulator_;
    SimulationConfig config_;
    QTimer timer_;
    bool running_;
    QLabel* infoLabel_;
    
    // Configuration widgets
    QSpinBox* numDevicesSpin_;
    QSpinBox* bufferCapacitySpin_;
    QDoubleSpinBox* deviceIntensitySpin_;
    QSpinBox* maxArrivalsSpin_;
    QSpinBox* seedSpin_;
    QTableWidget* sourcesTable_;
    
    // Control buttons
    QPushButton* btnStep_;
    QPushButton* btnRun_;
    QPushButton* btnPause_;
    QPushButton* btnRunToEnd_;
    QPushButton* btnReset_;
    QPushButton* btnPrintCsv_;
    
    // Timeline widget
    TimelineWidget* timelineWidget_;
};
