#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QPainter>
#include <QFontMetrics>

// Simple Timeline Widget
class TimelineWidget : public QWidget {
    Q_OBJECT
public:
    explicit TimelineWidget(QWidget* parent = nullptr) : QWidget(parent) {}
    
    void setSimulator(Simulator* sim, const SimulationConfig& config) {
        simulator_ = sim;
        config_ = config;
        update();
    }
    
protected:
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Clear background
        painter.fillRect(rect(), Qt::white);
        
        if (!simulator_) {
            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", 14, QFont::Bold));
            painter.drawText(rect(), Qt::AlignCenter, "Click 'Reset' to start simulation");
            return;
        }
        
        // Setup drawing parameters
        int margin = 60;
        int lineHeight = 30;
        int startY = 50;
        double currentTime = simulator_->get_current_time();
        double maxTime = std::max(currentTime + 5.0, 20.0);
        
        int width = this->width() - 2 * margin;
        int height = this->height() - 2 * startY;
        
        // Draw time axis
        painter.setPen(QPen(Qt::black, 2));
        painter.drawLine(margin, startY, margin + width, startY);
        painter.drawLine(margin, startY + height, margin + width, startY + height);
        
        // Draw time markers
        painter.setFont(QFont("Arial", 8));
        for (int i = 0; i <= 10; ++i) {
            double time = (maxTime * i) / 10.0;
            int x = margin + (width * i) / 10;
            painter.drawLine(x, startY - 5, x, startY + 5);
            painter.drawText(x - 10, startY - 10, QString::number(time, 'f', 1));
        }
        
        // Draw component lines
        int yPos = startY + 30;
        int lineIndex = 0;
        
        // Sources
        for (size_t i = 0; i < config_.sources.size(); ++i) {
            painter.setPen(QPen(Qt::black, 2));
            painter.drawLine(margin, yPos, margin + width, yPos);
            
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(10, yPos + 5, QString("И%1").arg(i + 1));
            
            yPos += lineHeight;
            lineIndex++;
        }
        
        // Devices
        for (size_t i = 0; i < config_.num_devices; ++i) {
            painter.setPen(QPen(Qt::black, 2));
            painter.drawLine(margin, yPos, margin + width, yPos);
            
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(10, yPos + 5, QString("П%1").arg(i + 1));
            
            yPos += lineHeight;
            lineIndex++;
        }
        
        // Buffer slots
        for (size_t i = 0; i < config_.buffer_capacity; ++i) {
            painter.setPen(QPen(Qt::black, 2));
            painter.drawLine(margin, yPos, margin + width, yPos);
            
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(10, yPos + 5, QString("Б%1").arg(i + 1));
            
            yPos += lineHeight;
            lineIndex++;
        }
        
        // Refusal line
        painter.setPen(QPen(Qt::red, 2));
        painter.drawLine(margin, yPos, margin + width, yPos);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.setPen(Qt::red);
        painter.drawText(10, yPos + 5, "Отказ");
        
        // Draw current time marker
        if (currentTime > 0) {
            int currentX = margin + (width * currentTime) / maxTime;
            painter.setPen(QPen(Qt::blue, 3, Qt::DashLine));
            painter.drawLine(currentX, startY, currentX, yPos);
        }
        
        // Draw title
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, "Временная диаграмма системы массового обслуживания");
    }
    
private:
    Simulator* simulator_ = nullptr;
    SimulationConfig config_;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), simulator_(nullptr), running_(false) {
    // Default configuration
    config_.num_devices = 3;
    config_.buffer_capacity = 3;
    config_.device_intensity = 0.8;
    config_.max_arrivals = 1000;
    config_.seed = 52;
    config_.sources = { {0, 1.0}, {1, 1.0}, {2, 1.0} };

    auto* central = new QWidget(this);

    // Left panel: Configuration
    auto* configGroup = new QGroupBox("Simulation Configuration", this);
    configGroup->setMaximumWidth(300);
    auto* configLayout = new QGridLayout(configGroup);

    // Basic parameters
    configLayout->addWidget(new QLabel("Devices:"), 0, 0);
    numDevicesSpin_ = new QSpinBox(this);
    numDevicesSpin_->setRange(1, 10);
    numDevicesSpin_->setValue(config_.num_devices);
    configLayout->addWidget(numDevicesSpin_, 0, 1);

    configLayout->addWidget(new QLabel("Buffer:"), 1, 0);
    bufferCapacitySpin_ = new QSpinBox(this);
    bufferCapacitySpin_->setRange(1, 100);
    bufferCapacitySpin_->setValue(config_.buffer_capacity);
    configLayout->addWidget(bufferCapacitySpin_, 1, 1);

    configLayout->addWidget(new QLabel("Device μ:"), 2, 0);
    deviceIntensitySpin_ = new QDoubleSpinBox(this);
    deviceIntensitySpin_->setRange(0.01, 10.0);
    deviceIntensitySpin_->setDecimals(2);
    deviceIntensitySpin_->setSingleStep(0.1);
    deviceIntensitySpin_->setValue(config_.device_intensity);
    configLayout->addWidget(deviceIntensitySpin_, 2, 1);

    configLayout->addWidget(new QLabel("Max arrivals:"), 3, 0);
    maxArrivalsSpin_ = new QSpinBox(this);
    maxArrivalsSpin_->setRange(1, 100000);
    maxArrivalsSpin_->setValue(config_.max_arrivals);
    configLayout->addWidget(maxArrivalsSpin_, 3, 1);

    configLayout->addWidget(new QLabel("Seed:"), 4, 0);
    seedSpin_ = new QSpinBox(this);
    seedSpin_->setRange(1, 999999);
    seedSpin_->setValue(config_.seed);
    configLayout->addWidget(seedSpin_, 4, 1);

    // Sources table
    configLayout->addWidget(new QLabel("Sources:"), 5, 0, 1, 2);
    sourcesTable_ = new QTableWidget(3, 2, this);
    sourcesTable_->setHorizontalHeaderLabels({"ID", "Interval"});
    sourcesTable_->horizontalHeader()->setStretchLastSection(true);
    sourcesTable_->setMaximumHeight(120);
    
    // Fill sources table
    for (int i = 0; i < 3; ++i) {
        sourcesTable_->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        sourcesTable_->setItem(i, 1, new QTableWidgetItem(QString::number(config_.sources[i].arrival_interval)));
    }
    configLayout->addWidget(sourcesTable_, 6, 0, 1, 2);

    // Right panel: Control and metrics
    auto* controlGroup = new QGroupBox("Simulation Control", this);
    controlGroup->setMaximumWidth(300);
    auto* controlLayout = new QVBoxLayout(controlGroup);

    btnStep_ = new QPushButton("Step", this);
    btnRun_ = new QPushButton("Run", this);
    btnPause_ = new QPushButton("Pause", this);
    btnRunToEnd_ = new QPushButton("Run to end", this);
    btnReset_ = new QPushButton("Reset", this);
    btnPrintCsv_ = new QPushButton("Print CSV", this);
    infoLabel_ = new QLabel("Metrics will appear here", this);
    infoLabel_->setMaximumWidth(300);
    infoLabel_->setWordWrap(true);
    infoLabel_->setAlignment(Qt::AlignTop);

    controlLayout->addWidget(btnStep_);
    controlLayout->addWidget(btnRun_);
    controlLayout->addWidget(btnPause_);
    controlLayout->addWidget(btnRunToEnd_);
    controlLayout->addWidget(btnReset_);
    controlLayout->addWidget(btnPrintCsv_);
    controlLayout->addWidget(infoLabel_);

    // Timeline widget
    timelineWidget_ = new TimelineWidget(this);
    timelineWidget_->setMinimumSize(500, 400);
    timelineWidget_->setMaximumSize(800, 600);
    
    // Create a simple horizontal layout
    auto* mainLayout = new QHBoxLayout(central);
    
    // Left side: Configuration and controls
    auto* leftWidget = new QWidget();
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(configGroup);
    leftLayout->addWidget(controlGroup);
    leftWidget->setMaximumWidth(320);
    
    // Right side: Timeline and metrics
    auto* rightWidget = new QWidget();
    auto* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->addWidget(timelineWidget_);
    rightLayout->addWidget(infoLabel_);
    
    mainLayout->addWidget(leftWidget, 0);   // Fixed width left panel
    mainLayout->addWidget(rightWidget, 1);  // Expandable right panel
    
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
    connect(btnPrintCsv_, &QPushButton::clicked, this, &MainWindow::onPrintCsv);

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

void MainWindow::updateUi() {
    if (!simulator_) {
        setWindowTitle("Queuing System Simulator - Not Started");
        infoLabel_->setText("Configure parameters and click 'Reset' to start simulation");
        updateButtonStates();
        updateTimeline();
        return;
    }
    
    Metrics m = simulator_->get_metrics();
    setWindowTitle(QString("Queuing System Simulator - Time: %1").arg(simulator_->get_current_time(), 0, 'f', 2));
    
    QString infoText = QString(
        "<h3>Simulation Results</h3>"
        "<table border='1' cellpadding='5'>"
        "<tr><td><b>Current Time:</b></td><td>%1</td></tr>"
        "<tr><td><b>Arrived:</b></td><td>%2</td></tr>"
        "<tr><td><b>Refused:</b></td><td>%3</td></tr>"
        "<tr><td><b>Completed:</b></td><td>%4</td></tr>"
        "<tr><td><b>P_ref:</b></td><td>%5</td></tr>"
        "<tr><td><b>Avg Waiting Time:</b></td><td>%6</td></tr>"
        "<tr><td><b>Avg Service Time:</b></td><td>%7</td></tr>"
        "<tr><td><b>Avg Time in System:</b></td><td>%8</td></tr>"
        "</table>"
        "<h4>System Load (ρ):</h4>"
        "<p>Total λ: %9<br>Total μ: %10<br>ρ = %11</p>"
    )
    .arg(simulator_->get_current_time(), 0, 'f', 2)
    .arg(m.get_arrived())
    .arg(m.get_refused())
    .arg(m.get_completed())
    .arg(QString::number(m.get_refusal_probability(), 'f', 3))
    .arg(QString::number(m.get_avg_waiting_time(), 'f', 4))
    .arg(QString::number(m.get_avg_service_time(), 'f', 4))
    .arg(QString::number(m.get_avg_time_in_system(), 'f', 4))
    .arg(QString::number(1.0/config_.sources[0].arrival_interval * config_.sources.size(), 'f', 2))
    .arg(QString::number(config_.device_intensity * config_.num_devices, 'f', 2))
    .arg(QString::number((1.0/config_.sources[0].arrival_interval * config_.sources.size()) / (config_.device_intensity * config_.num_devices), 'f', 2));
    
    infoLabel_->setText(infoText);
    updateButtonStates();
    updateTimeline();
}

void MainWindow::updateButtonStates() {
    if (!simulator_) {
        // No simulator - only Reset should be enabled
        btnStep_->setEnabled(false);
        btnRun_->setEnabled(false);
        btnPause_->setEnabled(false);
        btnRunToEnd_->setEnabled(false);
        btnReset_->setEnabled(true);
        btnPrintCsv_->setEnabled(false);
        
        // Configuration widgets enabled when no simulator
        numDevicesSpin_->setEnabled(true);
        bufferCapacitySpin_->setEnabled(true);
        deviceIntensitySpin_->setEnabled(true);
        maxArrivalsSpin_->setEnabled(true);
        seedSpin_->setEnabled(true);
        sourcesTable_->setEnabled(true);
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
    
    // Print CSV: enabled when we have data
    btnPrintCsv_->setEnabled(hasData);
    
    // Configuration widgets: disabled when running or when simulation has started
    bool configEnabled = !running_ && !hasData;
    numDevicesSpin_->setEnabled(configEnabled);
    bufferCapacitySpin_->setEnabled(configEnabled);
    deviceIntensitySpin_->setEnabled(configEnabled);
    maxArrivalsSpin_->setEnabled(configEnabled);
    seedSpin_->setEnabled(configEnabled);
    sourcesTable_->setEnabled(configEnabled);
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

#include "MainWindow.moc"
