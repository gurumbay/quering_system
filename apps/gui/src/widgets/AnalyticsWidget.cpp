#include "AnalyticsWidget.h"

#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>

AnalyticsWidget::AnalyticsWidget(QWidget* parent)
    : QWidget(parent), sourcesTable_(nullptr), devicesTable_(nullptr) {
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(16);

  // Title
  auto* titleLabel = new QLabel("Analytics", this);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet(
      "QLabel { font-size: 16px; font-weight: bold; padding: 8px; }");
  layout->addWidget(titleLabel);

  // Table 1: Source Characteristics
  auto* sourcesLabel = new QLabel("Таблица 1: Характеристики источников ВС", this);
  sourcesLabel->setStyleSheet("QLabel { font-weight: bold; }");
  layout->addWidget(sourcesLabel);

  sourcesTable_ = new QTableWidget(this);
  sourcesTable_->setColumnCount(8);
  sourcesTable_->setHorizontalHeaderLabels(
      {"№ источника", "количество заявок", "p_отк", "T_преб", "T_БП",
       "T_обсл", "Д_БП", "Д_обсл"});
  sourcesTable_->horizontalHeader()->setStretchLastSection(true);
  sourcesTable_->verticalHeader()->setVisible(false);
  sourcesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  sourcesTable_->setSelectionMode(QAbstractItemView::NoSelection);
  sourcesTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  sourcesTable_->setAlternatingRowColors(true);
  sourcesTable_->setMaximumHeight(300);
  layout->addWidget(sourcesTable_);

  // Table 2: Device Characteristics
  auto* devicesLabel =
      new QLabel("Таблица 2: Характеристики приборов ВС", this);
  devicesLabel->setStyleSheet("QLabel { font-weight: bold; }");
  layout->addWidget(devicesLabel);

  devicesTable_ = new QTableWidget(this);
  devicesTable_->setColumnCount(2);
  devicesTable_->setHorizontalHeaderLabels({"№ прибора", "Коэффициент использования"});
  devicesTable_->horizontalHeader()->setStretchLastSection(true);
  devicesTable_->verticalHeader()->setVisible(false);
  devicesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  devicesTable_->setSelectionMode(QAbstractItemView::NoSelection);
  devicesTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  devicesTable_->setAlternatingRowColors(true);
  devicesTable_->setMaximumHeight(200);
  layout->addWidget(devicesTable_);

  layout->addStretch();
}

void AnalyticsWidget::updateAnalytics(Simulator* sim,
                                      const SimulationConfig& config) {
  if (!sim) {
    sourcesTable_->setRowCount(0);
    devicesTable_->setRowCount(0);
    return;
  }
  populateSourcesTable(sim, config);
  populateDevicesTable(sim, config);
}

void AnalyticsWidget::populateSourcesTable(Simulator* sim,
                                           const SimulationConfig& config) {
  Metrics m = sim->get_metrics();
  sourcesTable_->setRowCount(0);

  for (size_t i = 0; i < config.sources.size(); ++i) {
    int row = sourcesTable_->rowCount();
    sourcesTable_->insertRow(row);

    // Source number (И1, И2, ...)
    auto* sourceItem = new QTableWidgetItem(QString("И%1").arg(i + 1));
    sourceItem->setFlags(Qt::ItemIsEnabled);
    sourcesTable_->setItem(row, 0, sourceItem);

    // Number of requests
    size_t arrivals = m.get_source_arrivals(i);
    auto* arrivalsItem = new QTableWidgetItem(QString::number(arrivals));
    arrivalsItem->setFlags(Qt::ItemIsEnabled);
    arrivalsItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sourcesTable_->setItem(row, 1, arrivalsItem);

    // Refusal probability p_отк
    double p_ref = m.get_source_refusal_probability(i);
    auto* pRefItem =
        new QTableWidgetItem(QString::number(p_ref, 'f', 4));
    pRefItem->setFlags(Qt::ItemIsEnabled);
    pRefItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sourcesTable_->setItem(row, 2, pRefItem);

    // T_преб (avg time in system)
    double avgTimeInSystem = m.get_source_avg_time_in_system(i);
    auto* timeInSystemItem =
        new QTableWidgetItem(QString::number(avgTimeInSystem, 'f', 3));
    timeInSystemItem->setFlags(Qt::ItemIsEnabled);
    timeInSystemItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sourcesTable_->setItem(row, 3, timeInSystemItem);

    // T_БП (avg waiting time)
    double avgWaitingTime = m.get_source_avg_waiting_time(i);
    auto* waitingTimeItem =
        new QTableWidgetItem(QString::number(avgWaitingTime, 'f', 3));
    waitingTimeItem->setFlags(Qt::ItemIsEnabled);
    waitingTimeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sourcesTable_->setItem(row, 4, waitingTimeItem);

    // T_обсл (avg service time)
    double avgServiceTime = m.get_source_avg_service_time(i);
    auto* serviceTimeItem =
        new QTableWidgetItem(QString::number(avgServiceTime, 'f', 3));
    serviceTimeItem->setFlags(Qt::ItemIsEnabled);
    serviceTimeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sourcesTable_->setItem(row, 5, serviceTimeItem);

    // Д_БП (variance of waiting time)
    double varWaitingTime = m.get_source_variance_waiting_time(i);
    auto* varWaitingItem =
        new QTableWidgetItem(QString::number(varWaitingTime, 'f', 3));
    varWaitingItem->setFlags(Qt::ItemIsEnabled);
    varWaitingItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sourcesTable_->setItem(row, 6, varWaitingItem);

    // Д_обсл (variance of service time)
    double varServiceTime = m.get_source_variance_service_time(i);
    auto* varServiceItem =
        new QTableWidgetItem(QString::number(varServiceTime, 'f', 3));
    varServiceItem->setFlags(Qt::ItemIsEnabled);
    varServiceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sourcesTable_->setItem(row, 7, varServiceItem);
  }

  sourcesTable_->resizeColumnsToContents();
  sourcesTable_->horizontalHeader()->setStretchLastSection(true);
}

void AnalyticsWidget::populateDevicesTable(Simulator* sim,
                                           const SimulationConfig& config) {
  Metrics m = sim->get_metrics();
  double currentTime = sim->get_current_time();
  devicesTable_->setRowCount(0);

  for (size_t i = 0; i < config.devices.size(); ++i) {
    int row = devicesTable_->rowCount();
    devicesTable_->insertRow(row);

    // Device number (П1, П2, ...)
    auto* deviceItem = new QTableWidgetItem(QString("П%1").arg(i + 1));
    deviceItem->setFlags(Qt::ItemIsEnabled);
    devicesTable_->setItem(row, 0, deviceItem);

    // Utilization coefficient
    double utilization = m.get_device_utilization(i, currentTime);
    auto* utilItem = new QTableWidgetItem(QString::number(utilization, 'f', 4));
    utilItem->setFlags(Qt::ItemIsEnabled);
    utilItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    devicesTable_->setItem(row, 1, utilItem);
  }

  devicesTable_->resizeColumnsToContents();
  devicesTable_->horizontalHeader()->setStretchLastSection(true);
}

