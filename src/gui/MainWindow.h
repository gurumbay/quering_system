#pragma once

#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTimer>
#include <QWidget>

#include "EventCalendarWidget.h"
#include "TimelineWidget.h"
#include "sim/core/Simulator.h"

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

  // Event calendar widget
  EventCalendarWidget* eventCalendarWidget_;

  // Helpers
  void renumberSourcesTable();
  void setupResultsGroup();
};
