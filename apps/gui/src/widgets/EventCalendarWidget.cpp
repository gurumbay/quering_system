#include "EventCalendarWidget.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidgetItem>

EventCalendarWidget::EventCalendarWidget(QWidget* parent)
    : QWidget(parent), table_(nullptr) {
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Title
  auto* titleLabel = new QLabel("Event Calendar", this);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet(
      "QLabel { font-size: 14px; font-weight: bold; padding: 8px; background: "
      "palette(base); border-bottom: 1px solid palette(mid); }");
  layout->addWidget(titleLabel);

  // Table
  table_ = new QTableWidget(this);
  setupTable();
  layout->addWidget(table_, 1);
}

void EventCalendarWidget::setupTable() {
  table_->setColumnCount(3);
  table_->setHorizontalHeaderLabels({"Entity", "State", "Next Event Time"});
  table_->horizontalHeader()->setStretchLastSection(true);
  table_->verticalHeader()->setVisible(false);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->setSelectionMode(QAbstractItemView::NoSelection);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setAlternatingRowColors(true);
}

void EventCalendarWidget::updateState(Simulator* sim,
                                      const SimulationConfig& config) {
  if (!sim) {
    table_->setRowCount(0);
    return;
  }
  populateTable(sim, config);
}

void EventCalendarWidget::populateTable(Simulator* sim,
                                        const SimulationConfig& config) {
  table_->setRowCount(0);

  auto source_states = sim->get_source_states();
  auto source_next_times = sim->get_source_next_event_times();
  auto device_states = sim->get_device_states();
  auto device_next_times = sim->get_device_next_event_times();

  int row = 0;

  // Sources section header
  table_->insertRow(row);
  auto* sourceHeader = new QTableWidgetItem("Sources");
  sourceHeader->setFlags(Qt::ItemIsEnabled);
  sourceHeader->setData(Qt::FontRole, QFont("", -1, QFont::Bold));
  sourceHeader->setBackground(QBrush(QColor(240, 240, 240)));
  sourceHeader->setForeground(QBrush(Qt::black));
  table_->setItem(row, 0, sourceHeader);
  table_->setSpan(row, 0, 1, 3);
  row++;

  // Sources rows
  for (size_t i = 0; i < config.sources.size(); ++i) {
    table_->insertRow(row);

    // Entity name
    QString entityName = QString("S%1").arg(i + 1);
    auto* nameItem = new QTableWidgetItem(entityName);
    nameItem->setFlags(Qt::ItemIsEnabled);
    table_->setItem(row, 0, nameItem);

    // State
    bool isActive = i < source_states.size() ? source_states[i] : false;
    auto* stateItem =
        new QTableWidgetItem(isActive ? "Active" : "Inactive");
    stateItem->setFlags(Qt::ItemIsEnabled);
    if (isActive) {
      stateItem->setForeground(QBrush(QColor(0, 128, 0)));  // Green
    } else {
      stateItem->setForeground(QBrush(QColor(128, 128, 128)));  // Gray
    }
    table_->setItem(row, 1, stateItem);

    // Next event time
    double nextTime = (i < source_next_times.size()) ? source_next_times[i] : -1.0;
    QString timeStr = (nextTime >= 0.0) ? QString::number(nextTime, 'f', 2) : "—";
    auto* timeItem = new QTableWidgetItem(timeStr);
    timeItem->setFlags(Qt::ItemIsEnabled);
    timeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    table_->setItem(row, 2, timeItem);

    row++;
  }

  // Empty row for spacing
  table_->insertRow(row);
  row++;

  // Devices section header
  table_->insertRow(row);
  auto* deviceHeader = new QTableWidgetItem("Devices");
  deviceHeader->setFlags(Qt::ItemIsEnabled);
  deviceHeader->setData(Qt::FontRole, QFont("", -1, QFont::Bold));
  deviceHeader->setBackground(QBrush(QColor(240, 240, 240)));
  deviceHeader->setForeground(QBrush(Qt::black));
  table_->setItem(row, 0, deviceHeader);
  table_->setSpan(row, 0, 1, 3);
  row++;

  // Devices rows
  for (size_t i = 0; i < config.devices.size(); ++i) {
    table_->insertRow(row);

    // Entity name
    QString entityName = QString("D%1").arg(i + 1);
    auto* nameItem = new QTableWidgetItem(entityName);
    nameItem->setFlags(Qt::ItemIsEnabled);
    table_->setItem(row, 0, nameItem);

    // State
    bool isActive = i < device_states.size() ? device_states[i] : false;
    auto* stateItem =
        new QTableWidgetItem(isActive ? "Active" : "Inactive");
    stateItem->setFlags(Qt::ItemIsEnabled);
    if (isActive) {
      stateItem->setForeground(QBrush(QColor(0, 128, 0)));  // Green
    } else {
      stateItem->setForeground(QBrush(QColor(128, 128, 128)));  // Gray
    }
    table_->setItem(row, 1, stateItem);

    // Next event time
    double nextTime = (i < device_next_times.size()) ? device_next_times[i] : -1.0;
    QString timeStr = (nextTime >= 0.0) ? QString::number(nextTime, 'f', 2) : "—";
    auto* timeItem = new QTableWidgetItem(timeStr);
    timeItem->setFlags(Qt::ItemIsEnabled);
    timeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    table_->setItem(row, 2, timeItem);

    row++;
  }

  // Resize columns to content
  table_->resizeColumnsToContents();
  table_->horizontalHeader()->setStretchLastSection(true);
}

