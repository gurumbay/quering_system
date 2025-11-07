#include "MainWindow.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>

#include "AnalyticsWidget.h"
#include "EventCalendarWidget.h"
#include "TimelineWidget.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), simulator_(nullptr), running_(false) {
  // Default configuration
  config_.num_devices = 3;
  config_.buffer_capacity = 3;
  config_.device_intensity = 0.3;
  config_.max_arrivals = 1000;
  config_.seed = 52;
  config_.sources = {{0, 3.0}, {1, 4.0}, {2, 5.0}};

  auto* central = new QWidget(this);

  // Left panel: Configuration
  auto* configGroup = new QGroupBox("Simulation Configuration", this);
  configGroup->setMaximumWidth(300);
  auto* configLayout = new QGridLayout(configGroup);

  // Basic parameters
  configLayout->addWidget(new QLabel("Devices:"), 0, 0);
  numDevicesSpin_ = new QSpinBox(this);
  numDevicesSpin_->setRange(1, 10);
  numDevicesSpin_->setValue(static_cast<int>(config_.num_devices));
  configLayout->addWidget(numDevicesSpin_, 0, 1);

  configLayout->addWidget(new QLabel("Device intensity (μ):"), 1, 0);
  deviceIntensitySpin_ = new QDoubleSpinBox(this);
  deviceIntensitySpin_->setRange(0.01, 10.0);
  deviceIntensitySpin_->setDecimals(2);
  deviceIntensitySpin_->setSingleStep(0.1);
  deviceIntensitySpin_->setValue(config_.device_intensity);
  configLayout->addWidget(deviceIntensitySpin_, 1, 1);

  configLayout->addWidget(new QLabel("Buffer:"), 2, 0);
  bufferCapacitySpin_ = new QSpinBox(this);
  bufferCapacitySpin_->setRange(1, 100);
  bufferCapacitySpin_->setValue(static_cast<int>(config_.buffer_capacity));
  configLayout->addWidget(bufferCapacitySpin_, 2, 1);

  configLayout->addWidget(new QLabel("Max arrivals:"), 3, 0);
  maxArrivalsSpin_ = new QSpinBox(this);
  maxArrivalsSpin_->setRange(1, 10000);
  maxArrivalsSpin_->setValue(static_cast<int>(config_.max_arrivals));
  configLayout->addWidget(maxArrivalsSpin_, 3, 1);

  configLayout->addWidget(new QLabel("Seed:"), 4, 0);
  seedSpin_ = new QSpinBox(this);
  seedSpin_->setRange(1, 999999);
  seedSpin_->setValue(static_cast<int>(config_.seed));
  configLayout->addWidget(seedSpin_, 4, 1);

  // Sources block
  auto* sourcesLayout = new QVBoxLayout();
  sourcesLayout->addWidget(new QLabel("Sources:"));

  sourcesTable_ =
      new QTableWidget(static_cast<int>(config_.sources.size()), 2, this);
  sourcesTable_->setHorizontalHeaderLabels({"ID", "Interval"});
  sourcesTable_->horizontalHeader()->setStretchLastSection(true);
  sourcesTable_->verticalHeader()->setVisible(false);
  sourcesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  sourcesTable_->setSelectionMode(QAbstractItemView::SingleSelection);
  sourcesTable_->setEditTriggers(QAbstractItemView::DoubleClicked |
                                 QAbstractItemView::EditKeyPressed);
  sourcesTable_->setMaximumHeight(140);
  sourcesLayout->addWidget(sourcesTable_);

  // Fill sources table
  for (int i = 0; i < config_.sources.size(); ++i) {
    sourcesTable_->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
    sourcesTable_->item(i, 0)->setFlags(Qt::ItemIsSelectable |
                                        Qt::ItemIsEnabled);
    sourcesTable_->setItem(i, 1,
                           new QTableWidgetItem(QString::number(
                               config_.sources[i].arrival_interval)));
  }

  // Sources controls
  btnAddSource_ = new QPushButton("Add source", this);
  btnRemoveSource_ = new QPushButton("Remove source", this);
  auto* sourcesBtns = new QHBoxLayout();
  sourcesBtns->addWidget(btnAddSource_);
  sourcesBtns->addWidget(btnRemoveSource_);
  sourcesLayout->addLayout(sourcesBtns);
  configLayout->addLayout(sourcesLayout, 5, 0, 1, 2);
  configLayout->setRowStretch(configLayout->rowCount(), 1);

  // Left panel: Control and metrics
  auto* controlGroup = new QGroupBox("Simulation Control", this);
  controlGroup->setMaximumWidth(300);
  auto* controlLayout = new QVBoxLayout(controlGroup);

  btnStep_ = new QPushButton("Step", this);
  btnRun_ = new QPushButton("Run", this);
  btnPause_ = new QPushButton("Pause", this);
  btnRunToEnd_ = new QPushButton("Run to end", this);
  btnReset_ = new QPushButton("Reset", this);

  controlLayout->addWidget(btnStep_);
  controlLayout->addWidget(btnRun_);
  controlLayout->addWidget(btnPause_);
  controlLayout->addWidget(btnRunToEnd_);
  controlLayout->addWidget(btnReset_);

  // Timeline widget
  timelineWidget_ = new TimelineWidget(this);

  // Event calendar widget
  eventCalendarWidget_ = new EventCalendarWidget(this);

  // Analytics widget
  analyticsWidget_ = new AnalyticsWidget(this);

  // Create tab widget for timeline and analytics
  tabWidget_ = new QTabWidget(this);
  
  // Tab 1: Timeline and Event Calendar
  auto* timelineTabWidget = new QWidget();
  auto* timelineTabLayout = new QVBoxLayout(timelineTabWidget);
  timelineTabLayout->setContentsMargins(0, 0, 0, 0);
  timelineTabLayout->setSpacing(8);
  timelineTabLayout->addWidget(timelineWidget_, 1);
  timelineTabLayout->addWidget(eventCalendarWidget_, 0);
  tabWidget_->addTab(timelineTabWidget, "Timeline");

  // Tab 2: Analytics
  tabWidget_->addTab(analyticsWidget_, "Analytics");

  // Create a simple horizontal layout
  auto* mainLayout = new QHBoxLayout(central);

  // Left side: Configuration and controls
  auto* leftWidget = new QWidget();
  auto* leftLayout = new QVBoxLayout(leftWidget);
  leftLayout->addWidget(configGroup);
  leftLayout->addWidget(controlGroup);
  // Results group
  setupResultsGroup();
  leftLayout->addWidget(resultsGroup_);
  leftWidget->setMaximumWidth(320);

  // Right side: Tab widget
  mainLayout->addWidget(leftWidget, 0);   // Fixed width left panel
  mainLayout->addWidget(tabWidget_, 1);   // Expandable right panel with tabs

  setCentralWidget(central);

  // Set window size and properties
  resize(1200, 800);
  setMinimumSize(800, 600);
  setWindowTitle("Queuing System Simulator");

  connect(btnStep_, &QPushButton::clicked, this, &MainWindow::onStep);
  connect(btnRun_, &QPushButton::clicked, this, &MainWindow::onRun);
  connect(btnPause_, &QPushButton::clicked, this, &MainWindow::onPause);
  connect(btnRunToEnd_, &QPushButton::clicked, this, &MainWindow::onRunToEnd);
  connect(btnReset_, &QPushButton::clicked, this, &MainWindow::onReset);
  connect(btnAddSource_, &QPushButton::clicked, this, &MainWindow::onAddSource);
  connect(btnRemoveSource_, &QPushButton::clicked, this,
          &MainWindow::onRemoveSource);

  timer_.setInterval(16);
  connect(&timer_, &QTimer::timeout, this, &MainWindow::onTick);

  rebuildSimulator();
  updateUi();
}

void MainWindow::updateConfigFromUI() {
  config_.num_devices = numDevicesSpin_->value();
  config_.buffer_capacity = bufferCapacitySpin_->value();
  config_.device_intensity = deviceIntensitySpin_->value();
  config_.max_arrivals = maxArrivalsSpin_->value();
  config_.seed = seedSpin_->value();

  // Update sources from table
  config_.sources.clear();
  for (int i = 0; i < sourcesTable_->rowCount(); ++i) {
    auto* idItem = sourcesTable_->item(i, 0);
    auto* intervalItem = sourcesTable_->item(i, 1);
    if (idItem && intervalItem) {
      bool ok;
      double interval = intervalItem->text().toDouble(&ok);
      if (ok && interval > 0) {
        config_.sources.push_back({static_cast<size_t>(i), interval});
      }
    }
  }
}

void MainWindow::rebuildSimulator() {
  updateConfigFromUI();
  delete simulator_;
  simulator_ = new Simulator(config_);
  updateButtonStates();
  updateTimeline();
}

void MainWindow::onAddSource() {
  int row = sourcesTable_->rowCount();
  sourcesTable_->insertRow(row);
  sourcesTable_->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
  sourcesTable_->item(row, 0)->setFlags(Qt::ItemIsSelectable |
                                        Qt::ItemIsEnabled);
  double defaultInterval = 3.0;
  if (row > 0) {
    bool ok = false;
    double prev = sourcesTable_->item(row - 1, 1)
                      ? sourcesTable_->item(row - 1, 1)->text().toDouble(&ok)
                      : 0.0;
    if (ok && prev > 0.0) defaultInterval = prev;
  }
  sourcesTable_->setItem(
      row, 1, new QTableWidgetItem(QString::number(defaultInterval)));
  renumberSourcesTable();
}

void MainWindow::onRemoveSource() {
  int row = sourcesTable_->currentRow();
  if (row < 0) row = sourcesTable_->rowCount() - 1;
  if (row >= 0 && sourcesTable_->rowCount() > 1) {
    sourcesTable_->removeRow(row);
    renumberSourcesTable();
  }
}

void MainWindow::renumberSourcesTable() {
  for (int i = 0; i < sourcesTable_->rowCount(); ++i) {
    if (!sourcesTable_->item(i, 0)) {
      sourcesTable_->setItem(i, 0, new QTableWidgetItem());
    }
    sourcesTable_->item(i, 0)->setText(QString::number(i + 1));
    sourcesTable_->item(i, 0)->setFlags(Qt::ItemIsSelectable |
                                        Qt::ItemIsEnabled);
  }
}

void MainWindow::setupResultsGroup() {
  resultsGroup_ = new QGroupBox("Results", this);
  auto* resultsLayout = new QGridLayout(resultsGroup_);
  auto makeValueLabel = [this]() {
    auto* l = new QLabel("—", this);
    l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    return l;
  };
  int r = 0;
  resultsLayout->addWidget(new QLabel("Current Time:"), r, 0);
  currentTimeValue_ = makeValueLabel();
  resultsLayout->addWidget(currentTimeValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("Arrived:"), r, 0);
  arrivedValue_ = makeValueLabel();
  resultsLayout->addWidget(arrivedValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("Refused:"), r, 0);
  refusedValue_ = makeValueLabel();
  resultsLayout->addWidget(refusedValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("Completed:"), r, 0);
  completedValue_ = makeValueLabel();
  resultsLayout->addWidget(completedValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("P_ref:"), r, 0);
  prefValue_ = makeValueLabel();
  resultsLayout->addWidget(prefValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("Avg Wait Time:"), r, 0);
  avgWaitValue_ = makeValueLabel();
  resultsLayout->addWidget(avgWaitValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("Avg Service Time:"), r, 0);
  avgServiceValue_ = makeValueLabel();
  resultsLayout->addWidget(avgServiceValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("Avg System Time:"), r, 0);
  avgSystemValue_ = makeValueLabel();
  resultsLayout->addWidget(avgSystemValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("λ (total):"), r, 0);
  lambdaValue_ = makeValueLabel();
  resultsLayout->addWidget(lambdaValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("μ (total):"), r, 0);
  muValue_ = makeValueLabel();
  resultsLayout->addWidget(muValue_, r++, 1);
  resultsLayout->addWidget(new QLabel("ρ (load):"), r, 0);
  rhoValue_ = makeValueLabel();
  resultsLayout->addWidget(rhoValue_, r++, 1);
}
void MainWindow::updateUi() {
  if (!simulator_) {
    setWindowTitle("Queuing System Simulator - Not Started");
    currentTimeValue_->setText("—");
    arrivedValue_->setText("0");
    refusedValue_->setText("0");
    completedValue_->setText("0");
    prefValue_->setText("0.0000");
    avgWaitValue_->setText("0.0000");
    avgServiceValue_->setText("0.0000");
    avgSystemValue_->setText("0.0000");
    lambdaValue_->setText("0.0000");
    muValue_->setText("0.0000");
    rhoValue_->setText("0.0000");
    updateButtonStates();
    updateTimeline();
    if (eventCalendarWidget_) {
      eventCalendarWidget_->updateState(nullptr, config_);
    }
    if (analyticsWidget_) {
      analyticsWidget_->updateAnalytics(nullptr, config_);
    }
    return;
  }

  Metrics m = simulator_->get_metrics();
  setWindowTitle(QString("Queuing System Simulator - Time: %1")
                     .arg(simulator_->get_current_time(), 0, 'f', 2));

  // Calculate correct lambda (sum of all source intensities)
  double total_lambda = 0.0;
  for (const auto& src : config_.sources) {
    total_lambda += 1.0 / src.arrival_interval;
  }
  double total_mu = config_.device_intensity * config_.num_devices;
  double rho = total_lambda / total_mu;

  currentTimeValue_->setText(
      QString::number(simulator_->get_current_time(), 'f', 2));
  arrivedValue_->setText(QString::number(m.get_arrived()));
  refusedValue_->setText(QString::number(m.get_refused()));
  completedValue_->setText(QString::number(m.get_completed()));
  prefValue_->setText(QString::number(m.get_refusal_probability(), 'f', 4));
  avgWaitValue_->setText(QString::number(m.get_avg_waiting_time(), 'f', 3));
  avgServiceValue_->setText(QString::number(m.get_avg_service_time(), 'f', 3));
  avgSystemValue_->setText(QString::number(m.get_avg_time_in_system(), 'f', 3));
  lambdaValue_->setText(QString::number(total_lambda, 'f', 3));
  muValue_->setText(QString::number(total_mu, 'f', 3));
  rhoValue_->setText(QString::number(rho, 'f', 3));
  updateButtonStates();
  updateTimeline();
  if (eventCalendarWidget_) {
    eventCalendarWidget_->updateState(simulator_, config_);
  }
  if (analyticsWidget_) {
    analyticsWidget_->updateAnalytics(simulator_, config_);
  }
}

void MainWindow::updateButtonStates() {
  if (!simulator_) {
    // No simulator - only Reset should be enabled
    btnStep_->setEnabled(false);
    btnRun_->setEnabled(false);
    btnPause_->setEnabled(false);
    btnRunToEnd_->setEnabled(false);
    btnReset_->setEnabled(true);

    // Configuration widgets enabled when no simulator
    numDevicesSpin_->setEnabled(true);
    bufferCapacitySpin_->setEnabled(true);
    deviceIntensitySpin_->setEnabled(true);
    maxArrivalsSpin_->setEnabled(true);
    seedSpin_->setEnabled(true);
    sourcesTable_->setEnabled(true);
    btnAddSource_->setEnabled(true);
    btnRemoveSource_->setEnabled(sourcesTable_ &&
                                 sourcesTable_->rowCount() > 1);
    return;
  }

  bool isFinished = simulator_->is_finished();
  bool hasData = simulator_->get_metrics().get_arrived() > 0;

  // Step: enabled when not running and not finished
  btnStep_->setEnabled(!running_ && !isFinished);

  // Run: enabled when not running and not finished
  btnRun_->setEnabled(!running_ && !isFinished);

  // Pause: enabled when running
  btnPause_->setEnabled(running_);

  // Run to end: enabled when not running and not finished
  btnRunToEnd_->setEnabled(!running_ && !isFinished);

  // Reset: always enabled
  btnReset_->setEnabled(true);

  // Configuration widgets: disabled when running or when simulation has started
  bool configEnabled = !running_ && !hasData;
  numDevicesSpin_->setEnabled(configEnabled);
  bufferCapacitySpin_->setEnabled(configEnabled);
  deviceIntensitySpin_->setEnabled(configEnabled);
  maxArrivalsSpin_->setEnabled(configEnabled);
  seedSpin_->setEnabled(configEnabled);
  sourcesTable_->setEnabled(configEnabled);
  btnAddSource_->setEnabled(configEnabled);
  btnRemoveSource_->setEnabled(configEnabled && sourcesTable_->rowCount() > 1);
}

void MainWindow::onStep() {
  if (!simulator_) return;
  simulator_->step();
  updateUi();
}

void MainWindow::onRun() {
  running_ = true;
  timer_.start();
  updateButtonStates();
}

void MainWindow::onPause() {
  running_ = false;
  timer_.stop();
  updateButtonStates();
}

void MainWindow::onRunToEnd() {
  if (!simulator_) return;
  simulator_->run();
  running_ = false;
  timer_.stop();
  updateUi();
}

void MainWindow::onReset() {
  running_ = false;
  timer_.stop();
  rebuildSimulator();
  updateUi();
}

void MainWindow::onTick() {
  if (!simulator_) return;
  if (simulator_->is_finished()) {
    running_ = false;
    timer_.stop();
    updateUi();
    return;
  }
  simulator_->step();
  updateUi();
}

void MainWindow::updateTimeline() {
  timelineWidget_->setSimulator(simulator_, config_);
  timelineWidget_->updateCanvas();

  // Auto-scroll to current time when simulation is running or stepping
  if (simulator_ && simulator_->get_current_time() > 0) {
    timelineWidget_->scrollToCurrentTime();
  }
}
