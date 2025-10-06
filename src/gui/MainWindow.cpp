#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), simulator_(nullptr), running_(false) {
    // Match CLI defaults for consistent results
    config_.num_devices = 3;
    config_.buffer_capacity = 20;
    config_.service_rate = 2.0;
    config_.max_arrivals = 1000;
    config_.seed = 42;
    config_.sources = { {0, 2.0}, {1, 3.0} };

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    auto* btnStep = new QPushButton("Step", central);
    auto* btnRun = new QPushButton("Run", central);
    auto* btnPause = new QPushButton("Pause", central);
    auto* btnRunToEnd = new QPushButton("Run to end", central);
    auto* btnReset = new QPushButton("Reset", central);
    auto* btnPrintCsv = new QPushButton("Print CSV", central);
    infoLabel_ = new QLabel("Metrics will appear here", central);

    layout->addWidget(btnStep);
    layout->addWidget(btnRun);
    layout->addWidget(btnPause);
    layout->addWidget(btnRunToEnd);
    layout->addWidget(infoLabel_);
    layout->addWidget(btnReset);
    layout->addWidget(btnPrintCsv);

    setCentralWidget(central);

    connect(btnStep, &QPushButton::clicked, this, &MainWindow::onStep);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
    connect(btnPause, &QPushButton::clicked, this, &MainWindow::onPause);
    connect(btnRunToEnd, &QPushButton::clicked, this, &MainWindow::onRunToEnd);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(btnPrintCsv, &QPushButton::clicked, this, &MainWindow::onPrintCsv);

    timer_.setInterval(16);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::onTick);

    rebuildSimulator();
    updateUi();
}

void MainWindow::rebuildSimulator() {
    delete simulator_;
    simulator_ = new Simulator(config_);
}

void MainWindow::updateUi() {
    if (!simulator_) return;
    Metrics m = simulator_->get_metrics();
    setWindowTitle(QString("t=%1").arg(simulator_->get_current_time()));
    infoLabel_->setText(
        QString("arrived=%1, refused=%2, completed=%3, p_ref=%4, avgW=%5, avgS=%6, avgT=%7")
            .arg(m.get_arrived())
            .arg(m.get_refused())
            .arg(m.get_completed())
            .arg(QString::number(m.get_refusal_probability(), 'f', 3))
            .arg(QString::number(m.get_avg_waiting_time(), 'f', 4))
            .arg(QString::number(m.get_avg_service_time(), 'f', 4))
            .arg(QString::number(m.get_avg_time_in_system(), 'f', 4))
    );
}

void MainWindow::onStep() {
    if (!simulator_) return;
    simulator_->step();
    updateUi();
}

void MainWindow::onRun() {
    running_ = true;
    timer_.start();
}

void MainWindow::onPause() {
    running_ = false;
    timer_.stop();
}

void MainWindow::onRunToEnd() {
    if (!simulator_) return;
    simulator_->run();
    running_ = false;
    timer_.stop();
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

void MainWindow::onReset() {
    running_ = false;
    timer_.stop();
    rebuildSimulator();
    updateUi();
}

void MainWindow::onPrintCsv() {
    if (!simulator_) return;
    Metrics m = simulator_->get_metrics();
    printf("arrived,%zu\nrefused,%zu\ncompleted,%zu\np_ref,%.6f\navg_time_in_system,%.6f\navg_waiting,%.6f\navg_service,%.6f\n",
        m.get_arrived(), m.get_refused(), m.get_completed(),
        m.get_refusal_probability(), m.get_avg_time_in_system(),
        m.get_avg_waiting_time(), m.get_avg_service_time());
}
