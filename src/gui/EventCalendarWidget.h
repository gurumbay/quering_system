#pragma once

#include <QTableWidget>
#include <QWidget>

#include "sim/core/Simulator.h"

class EventCalendarWidget : public QWidget {
  Q_OBJECT
 public:
  explicit EventCalendarWidget(QWidget* parent = nullptr);
  void updateState(Simulator* sim, const SimulationConfig& config);

 private:
  QTableWidget* table_;
  void setupTable();
  void populateTable(Simulator* sim, const SimulationConfig& config);
};

