#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QWidget>
#include <QSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QTableWidget>
#include <QDoubleSpinBox>
#include "sim/core/Simulator.h"
#include "TimelineWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onAddSource();
    void onRemoveSource();

    void onStep();
    void onRun();
    void onPause();
    void onRunToEnd();
    void onReset();
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
    // Results UI
    QGroupBox* resultsGroup_;
    QLabel* currentTimeValue_;
    QLabel* arrivedValue_;
    QLabel* refusedValue_;
    QLabel* completedValue_;
    QLabel* prefValue_;
    QLabel* avgWaitValue_;
    QLabel* avgServiceValue_;
    QLabel* avgSystemValue_;
    QLabel* lambdaValue_;
    QLabel* muValue_;
    QLabel* rhoValue_;
    
    // Configuration widgets
    QSpinBox* numDevicesSpin_;
    QSpinBox* bufferCapacitySpin_;
    QDoubleSpinBox* deviceIntensitySpin_;
    QSpinBox* maxArrivalsSpin_;
    QSpinBox* seedSpin_;
    QTableWidget* sourcesTable_;
    QPushButton* btnAddSource_;
    QPushButton* btnRemoveSource_;
    
    // Control buttons
    QPushButton* btnStep_;
    QPushButton* btnRun_;
    QPushButton* btnPause_;
    QPushButton* btnRunToEnd_;
    QPushButton* btnReset_;
    
    // Timeline widget
    TimelineWidget* timelineWidget_;

    // Helpers
    void renumberSourcesTable();
    void setupResultsGroup();
};
