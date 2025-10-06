#pragma once

#include <QMainWindow>
#include <QTimer>
#include "sim/core/Simulator.h"

class QLabel; // forward declaration to avoid include in header

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

    Simulator* simulator_;
    SimulationConfig config_;
    QTimer timer_;
    bool running_;
    QLabel* infoLabel_;
};
