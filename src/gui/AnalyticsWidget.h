#pragma once

#include <QTableWidget>
#include <QWidget>

#include "sim/core/Simulator.h"

class AnalyticsWidget : public QWidget {
  Q_OBJECT
 public:
  explicit AnalyticsWidget(QWidget* parent = nullptr);
  void updateAnalytics(Simulator* sim, const SimulationConfig& config);

 private:
  QTableWidget* sourcesTable_;
  QTableWidget* devicesTable_;
  void setupTables();
  void populateSourcesTable(Simulator* sim, const SimulationConfig& config);
  void populateDevicesTable(Simulator* sim, const SimulationConfig& config);
};

