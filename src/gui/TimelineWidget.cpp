#include "TimelineWidget.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWheelEvent>
#include <QScrollBar>
#include <cmath>
#include <algorithm>

// TimelineCanvas implementation
TimelineCanvas::TimelineCanvas(QWidget* parent) 
    : QWidget(parent), simulator_(nullptr), zoomLevel_(1.0) {
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TimelineCanvas::setSimulator(Simulator* sim, const SimulationConfig& config) {
    simulator_ = sim;
    config_ = config;
    updateSize();
}

void TimelineCanvas::updateSize() {
    // Calculate and set the actual size needed for the content
    if (simulator_) {
        QSize needed = sizeHint();
        setMinimumSize(needed);
        resize(needed);
    }
    update();
}

void TimelineCanvas::setZoomLevel(double zoom) {
    zoomLevel_ = std::max(0.1, std::min(zoom, 10.0)); // Limit zoom range 0.1x to 10x
    updateSize();
    emit zoomChanged(zoomLevel_); // Notify widget that zoom changed
}

void TimelineCanvas::wheelEvent(QWheelEvent* event) {
    // Ctrl + Wheel = Zoom (like GTKwave)
    if (event->modifiers() & Qt::ControlModifier) {
        double delta = event->angleDelta().y() / 120.0; // Standard wheel step
        double zoomFactor = std::pow(1.15, delta); // 15% per step
        setZoomLevel(zoomLevel_ * zoomFactor);
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

QSize TimelineCanvas::sizeHint() const {
    if (!simulator_) {
        return QSize(1200, 800);
    }
    
    // Calculate required height based on number of components
    int numLines = static_cast<int>(config_.sources.size() + 
                                    config_.num_devices + 
                                    config_.buffer_capacity + 
                                    1); // +1 for refusal line
    
    int height = MARGIN_TOP + numLines * LINE_HEIGHT + MARGIN_BOTTOM;
    
    // Calculate required width based on time range and zoom
    double currentTime = simulator_->get_current_time();
    Metrics m = simulator_->get_metrics();
    const auto& events = m.get_timeline_events();
    
    double maxEventTime = currentTime;
    for (const auto& event : events) {
        if (event.time > maxEventTime) {
            maxEventTime = event.time;
        }
    }
    double maxTime = std::max({currentTime + 5.0, maxEventTime + 2.0, 20.0});
    
    int width = MARGIN_LEFT + MARGIN_RIGHT + 
                static_cast<int>(maxTime * PIXELS_PER_TIME_UNIT * zoomLevel_);
    
    // At zoom 1.0, try to fit nicely; when zoomed, grow proportionally
    // Minimum width ensures timeline is always usable
    int minWidth = 800;
    int preferredWidth = std::max(minWidth, width);
    
    // Don't cap height - let it grow to show all components
    // Minimum height ensures basic usability
    int minHeight = 400;
    int preferredHeight = std::max(minHeight, height);
    
    return QSize(preferredWidth, preferredHeight);
}

QSize TimelineCanvas::minimumSizeHint() const {
    if (!simulator_) {
        return QSize(400, 300);
    }
    
    // Calculate minimum height based on components
    int numLines = static_cast<int>(config_.sources.size() + 
                                    config_.num_devices + 
                                    config_.buffer_capacity + 
                                    1); // +1 for refusal line
    
    int minHeight = MARGIN_TOP + numLines * LINE_HEIGHT + MARGIN_BOTTOM;
    
    return QSize(400, std::max(300, minHeight));
}

double TimelineCanvas::calculateNiceTimeStep(double range) {
    // Find appropriate step following the rule: 10^n, 2×10^n, or 5×10^n
    // Target: 8-12 divisions on screen
    
    double rawStep = range / 10.0;
    
    // Find the order of magnitude
    int exponent = static_cast<int>(std::floor(std::log10(rawStep)));
    double magnitude = std::pow(10.0, exponent);
    
    // Normalize to [1, 10) range
    double normalized = rawStep / magnitude;
    
    // Choose nearest nice value: 1, 2, 5, or 10
    double niceNormalized;
    if (normalized <= 1.5) {
        niceNormalized = 1.0;
    } else if (normalized <= 3.5) {
        niceNormalized = 2.0;
    } else if (normalized <= 7.5) {
        niceNormalized = 5.0;
    } else {
        niceNormalized = 10.0;
    }
    
    return niceNormalized * magnitude;
}

QString TimelineCanvas::formatTimeValue(double value) {
    // Smart formatting based on magnitude
    if (value == 0.0) {
        return "0";
    }
    
    double absValue = std::abs(value);
    
    if (absValue >= 1000.0) {
        return QString::number(value, 'f', 0);
    } else if (absValue >= 100.0) {
        return QString::number(value, 'f', 1);
    } else if (absValue >= 10.0) {
        return QString::number(value, 'f', 1);
    } else if (absValue >= 1.0) {
        return QString::number(value, 'f', 2);
    } else if (absValue >= 0.1) {
        return QString::number(value, 'f', 2);
    } else if (absValue >= 0.01) {
        return QString::number(value, 'f', 3);
    } else {
        return QString::number(value, 'e', 2);
    }
}

void TimelineCanvas::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Clear background
    painter.fillRect(rect(), Qt::white);
    
    if (!simulator_) {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "Нажмите 'Reset' для начала симуляции");
        return;
    }
    
    // Draw timeline
    drawTimeline(painter);
}

void TimelineCanvas::drawTimeline(QPainter& painter) {
    double currentTime = simulator_->get_current_time();
    
    // Calculate max time from events
    Metrics m = simulator_->get_metrics();
    const auto& events = m.get_timeline_events();
    double maxEventTime = currentTime;
    for (const auto& event : events) {
        if (event.time > maxEventTime) {
            maxEventTime = event.time;
        }
    }
    double maxTime = std::max({currentTime + 5.0, maxEventTime + 2.0, 20.0});
    
    int drawWidth = width() - MARGIN_LEFT - MARGIN_RIGHT;
    
    // Draw time axis with proper scale
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(MARGIN_LEFT, MARGIN_TOP, MARGIN_LEFT + drawWidth, MARGIN_TOP);
    
    // Calculate nice time step based on zoom level
    // At higher zoom, we want more detailed time markers
    double visibleTimeRange = maxTime / zoomLevel_;  // Approximate visible time range
    double timeStep = calculateNiceTimeStep(visibleTimeRange);
    
    // Draw time markers with proper divisions
    painter.setFont(QFont("Arial", 9));
    double time = 0.0;
    while (time <= maxTime + timeStep * 0.1) {
        int x = MARGIN_LEFT + static_cast<int>((drawWidth * time) / maxTime);
        
        // Skip markers that are too close together (less than 40 pixels apart)
        if (x > MARGIN_LEFT) {
            int prevX = MARGIN_LEFT + static_cast<int>((drawWidth * (time - timeStep)) / maxTime);
            if (x - prevX < 40) {
                time += timeStep;
                continue;
            }
        }
        
        painter.setPen(QPen(Qt::black, 2));
        painter.drawLine(x, MARGIN_TOP - 8, x, MARGIN_TOP + 8);
        
        // Draw grid line
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
        painter.drawLine(x, MARGIN_TOP, x, height() - MARGIN_BOTTOM);
        
        // Draw label
        painter.setPen(Qt::black);
        QString label = formatTimeValue(time);
        QRect textRect(x - 40, MARGIN_TOP - 30, 80, 20);
        painter.drawText(textRect, Qt::AlignCenter, label);
        
        time += timeStep;
    }
    
    // Draw component lines with labels
    int yPos = MARGIN_TOP + 40;
    
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    
    // Draw source lines and labels
    for (size_t i = 0; i < config_.sources.size(); ++i) {
        painter.setPen(QPen(Qt::black, 2));
        painter.drawLine(MARGIN_LEFT, yPos, MARGIN_LEFT + drawWidth, yPos);
        
        // Draw source label (always visible, positioned relative to viewport)
        painter.setPen(Qt::black);
        QString label = QString("И%1").arg(i + 1);
        
        // Get current scroll position to keep labels visible
        QScrollArea* scrollArea = qobject_cast<QScrollArea*>(parent()->parent());
        int scrollX = scrollArea ? scrollArea->horizontalScrollBar()->value() : 0;
        
        // Position label so it's always visible (5px from left edge of viewport)
        int labelX = std::max(5, 5 - scrollX);
        QRect labelRect(labelX, yPos - 8, MARGIN_LEFT - 10, 16);
        painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
        
        yPos += LINE_HEIGHT;
    }
    
    // Draw device lines and labels
    for (size_t i = 0; i < config_.num_devices; ++i) {
        painter.setPen(QPen(Qt::black, 2));
        painter.drawLine(MARGIN_LEFT, yPos, MARGIN_LEFT + drawWidth, yPos);
        
        // Draw device label
        painter.setPen(Qt::black);
        QString label = QString("П%1").arg(i + 1);
        
        // Get current scroll position to keep labels visible
        QScrollArea* scrollArea = qobject_cast<QScrollArea*>(parent()->parent());
        int scrollX = scrollArea ? scrollArea->horizontalScrollBar()->value() : 0;
        
        // Position label so it's always visible (5px from left edge of viewport)
        int labelX = std::max(5, 5 - scrollX);
        QRect labelRect(labelX, yPos - 8, MARGIN_LEFT - 10, 16);
        painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
        
        yPos += LINE_HEIGHT;
    }
    
    // Draw buffer lines and labels
    for (size_t i = 0; i < config_.buffer_capacity; ++i) {
        painter.setPen(QPen(Qt::black, 2));
        painter.drawLine(MARGIN_LEFT, yPos, MARGIN_LEFT + drawWidth, yPos);
        
        // Draw buffer label
        painter.setPen(Qt::black);
        QString label = QString("Б%1").arg(i + 1);
        
        // Get current scroll position to keep labels visible
        QScrollArea* scrollArea = qobject_cast<QScrollArea*>(parent()->parent());
        int scrollX = scrollArea ? scrollArea->horizontalScrollBar()->value() : 0;
        
        // Position label so it's always visible (5px from left edge of viewport)
        int labelX = std::max(5, 5 - scrollX);
        QRect labelRect(labelX, yPos - 8, MARGIN_LEFT - 10, 16);
        painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
        
        yPos += LINE_HEIGHT;
    }
    
    // Draw refusal line and label
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(MARGIN_LEFT, yPos, MARGIN_LEFT + drawWidth, yPos);
    
    // Draw refusal label
    painter.setPen(Qt::red);
    QString label = "Отказ";
    
    // Get current scroll position to keep labels visible
    QScrollArea* scrollArea = qobject_cast<QScrollArea*>(parent()->parent());
    int scrollX = scrollArea ? scrollArea->horizontalScrollBar()->value() : 0;
    
    // Position label so it's always visible (5px from left edge of viewport)
    int labelX = std::max(5, 5 - scrollX);
    QRect labelRect(labelX, yPos - 8, MARGIN_LEFT - 10, 16);
    painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
    
    int refusalY = yPos;
    
    // Process events to build intervals
    std::map<size_t, std::vector<std::pair<double, double>>> deviceServices;
    std::map<size_t, std::vector<std::pair<size_t, std::pair<double, double>>>> bufferOccupancy;
    
    for (const auto& event : events) {
        if (event.type == "service_start") {
            deviceServices[event.device_id].push_back({event.time, -1.0});
        } else if (event.type == "service_end") {
            auto& services = deviceServices[event.device_id];
            for (auto it = services.rbegin(); it != services.rend(); ++it) {
                if (it->second < 0) {
                    it->second = event.time;
                    break;
                }
            }
        } else if (event.type == "buffer_place") {
            bufferOccupancy[event.buffer_slot].push_back({event.request_id, {event.time, -1.0}});
        } else if (event.type == "buffer_take") {
            auto& slot_intervals = bufferOccupancy[event.buffer_slot];
            for (auto& [req_id, interval] : slot_intervals) {
                if (req_id == event.request_id && interval.second < 0) {
                    interval.second = event.time;
                    break;
                }
            }
        } else if (event.type == "buffer_displaced") {
            for (auto& [slot, intervals] : bufferOccupancy) {
                for (auto& [req_id, interval] : intervals) {
                    if (req_id == event.request_id && interval.second < 0) {
                        interval.second = event.time;
                        break;
                    }
                }
            }
        }
    }
    
    int bufferStartY = MARGIN_TOP + 40 + 
                       (static_cast<int>(config_.sources.size()) + 
                        static_cast<int>(config_.num_devices)) * LINE_HEIGHT;
    
    // Draw events
    for (const auto& event : events) {
        if (event.time > maxTime) continue;
        
        int eventX = MARGIN_LEFT + static_cast<int>((drawWidth * event.time) / maxTime);
        
        if (event.type == "arrival") {
            int eventY = MARGIN_TOP + 40 + static_cast<int>(event.source_id) * LINE_HEIGHT;
            painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(eventX, eventY - 8, eventX, eventY + 8);
            painter.setBrush(Qt::red);
            painter.drawEllipse(eventX - 4, eventY - 4, 8, 8);
        } else if (event.type == "refusal") {
            painter.setPen(QPen(Qt::red, 3));
            painter.drawLine(eventX - 6, refusalY - 6, eventX + 6, refusalY + 6);
            painter.drawLine(eventX - 6, refusalY + 6, eventX + 6, refusalY - 6);
        }
    }
    
    // Draw service intervals
    std::set<size_t> displaced_requests;
    for (const auto& event : events) {
        if (event.type == "buffer_displaced") {
            displaced_requests.insert(event.request_id);
        }
    }
    
    for (const auto& [device_id, services] : deviceServices) {
        int deviceY = MARGIN_TOP + 40 + 
                      static_cast<int>(config_.sources.size()) * LINE_HEIGHT + 
                      static_cast<int>(device_id) * LINE_HEIGHT;
        
        for (const auto& [start, end] : services) {
            if (start > maxTime) continue;
            
            int x1 = MARGIN_LEFT + static_cast<int>((drawWidth * start) / maxTime);
            int x2;
            
            if (end < 0) {
                x2 = MARGIN_LEFT + static_cast<int>((drawWidth * currentTime) / maxTime);
                painter.setPen(QPen(Qt::darkGreen, 1, Qt::DashLine));
                painter.setBrush(QBrush(Qt::green, Qt::Dense4Pattern));
            } else {
                x2 = MARGIN_LEFT + static_cast<int>((drawWidth * std::min(end, maxTime)) / maxTime);
                painter.setPen(QPen(Qt::darkGreen, 1));
                painter.setBrush(QBrush(Qt::green));
            }
            
            painter.drawRect(x1, deviceY - 12, x2 - x1, 24);
        }
    }
    
    // Draw buffer occupancy
    for (const auto& [slot, intervals] : bufferOccupancy) {
        int bufferY = bufferStartY + static_cast<int>(slot) * LINE_HEIGHT;
        
        for (const auto& [req_id, interval] : intervals) {
            double start = interval.first;
            double end = interval.second;
            
            if (start > maxTime) continue;
            
            int x1 = MARGIN_LEFT + static_cast<int>((drawWidth * start) / maxTime);
            int x2;
            
            bool was_displaced = displaced_requests.count(req_id) > 0;
            
            if (end < 0) {
                x2 = MARGIN_LEFT + static_cast<int>((drawWidth * currentTime) / maxTime);
                painter.setPen(QPen(Qt::black, 1, Qt::DashLine));
                painter.setBrush(QBrush(QColor(255, 165, 0), Qt::Dense4Pattern));
            } else {
                x2 = MARGIN_LEFT + static_cast<int>((drawWidth * std::min(end, maxTime)) / maxTime);
                painter.setPen(QPen(Qt::black, 1));
                
                if (was_displaced) {
                    painter.setBrush(QBrush(QColor(255, 165, 0, 80)));
                } else {
                    painter.setBrush(QBrush(QColor(255, 165, 0)));
                }
            }
            
            painter.drawRect(x1, bufferY - 12, x2 - x1, 24);
        }
    }
    
    // Draw current time marker
    if (currentTime > 0 && currentTime <= maxTime) {
        int currentX = MARGIN_LEFT + static_cast<int>((drawWidth * currentTime) / maxTime);
        painter.setPen(QPen(Qt::blue, 3, Qt::DashLine));
        painter.drawLine(currentX, MARGIN_TOP, currentX, height() - MARGIN_BOTTOM);
    }
}

// TimelineWidget implementation
TimelineWidget::TimelineWidget(QWidget* parent) : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Title (fixed at top)
    auto* titleLabel = new QLabel("Queuing System Timeline", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: bold; padding: 8px; background: palette(base); }");
    mainLayout->addWidget(titleLabel);
    
    // Toolbar with controls
    auto* toolbar = new QWidget(this);
    auto* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(5, 5, 5, 5);
    
    // Zoom controls
    toolbarLayout->addWidget(new QLabel("Zoom:", this));
    
    btnZoomOut_ = new QPushButton("−", this);
    btnZoomOut_->setFixedSize(30, 30);
    btnZoomOut_->setToolTip("Zoom out (Ctrl+Wheel down)");
    toolbarLayout->addWidget(btnZoomOut_);
    
    btnZoomIn_ = new QPushButton("+", this);
    btnZoomIn_->setFixedSize(30, 30);
    btnZoomIn_->setToolTip("Zoom in (Ctrl+Wheel up)");
    toolbarLayout->addWidget(btnZoomIn_);
    
    zoomLabel_ = new QLabel("100%", this);
    zoomLabel_->setFixedWidth(60);
    zoomLabel_->setAlignment(Qt::AlignCenter);
    toolbarLayout->addWidget(zoomLabel_);
    
    
    toolbarLayout->addStretch();
    
    // Scroll area with canvas
    canvas_ = new TimelineCanvas(this);
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidget(canvas_);
    scrollArea_->setWidgetResizable(false);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(scrollArea_, 1);
    
    // Legend (fixed at bottom)
    auto* legendWidget = new QWidget(this);
    auto* legendLayout = new QHBoxLayout(legendWidget);
    legendLayout->setContentsMargins(10, 5, 10, 5);
    legendLayout->setSpacing(20);
    
    auto addLegendItem = [&](const QString& text, const QColor& color, const QString& style) {
        auto* item = new QWidget(this);
        auto* itemLayout = new QHBoxLayout(item);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(5);
        
        auto* icon = new QLabel(this);
        icon->setFixedSize(20, 16);
        if (style == "arrival") {
            icon->setStyleSheet(QString("QLabel { background: %1; border: 1px solid %1; border-radius: 8px; }").arg(color.name()));
        } else if (style == "service") {
            icon->setStyleSheet(QString("QLabel { background: %1; border: 1px solid darkgreen; }").arg(color.name()));
        } else if (style == "buffer") {
            icon->setStyleSheet(QString("QLabel { background: %1; border: 1px solid black; }").arg(color.name()));
        } else if (style == "refusal") {
            icon->setStyleSheet(QString("QLabel { color: %1; font-size: 14px; font-weight: bold; }").arg(color.name()));
            icon->setText("✕");
            icon->setAlignment(Qt::AlignCenter);
        } else if (style == "time") {
            icon->setStyleSheet(QString("QLabel { border-bottom: 3px dashed %1; }").arg(color.name()));
        }
        
        auto* label = new QLabel(text, this);
        label->setStyleSheet("QLabel { font-size: 9pt; }");
        
        itemLayout->addWidget(icon);
        itemLayout->addWidget(label);
        
        legendLayout->addWidget(item);
    };
    
    addLegendItem("Arrival", Qt::red, "arrival");
    addLegendItem("Service", Qt::green, "service");
    addLegendItem("Buffer", QColor(255, 165, 0), "buffer");
    addLegendItem("Refusal", Qt::red, "refusal");
    addLegendItem("Current Time", Qt::blue, "time");
    legendLayout->addStretch();
    
    legendWidget->setStyleSheet("QWidget { background: palette(base); border-top: 1px solid palette(mid); }");
    mainLayout->addWidget(legendWidget);
    
    // Connect signals
    connect(btnZoomIn_, &QPushButton::clicked, this, &TimelineWidget::onZoomIn);
    connect(btnZoomOut_, &QPushButton::clicked, this, &TimelineWidget::onZoomOut);
    connect(canvas_, &TimelineCanvas::zoomChanged, this, &TimelineWidget::onCanvasZoomChanged);
    
    // Connect scroll signals to update labels position
    connect(scrollArea_->horizontalScrollBar(), &QScrollBar::valueChanged, 
            [this]() { canvas_->update(); });
    connect(scrollArea_->verticalScrollBar(), &QScrollBar::valueChanged, 
            [this]() { canvas_->update(); });
}

void TimelineWidget::setSimulator(Simulator* sim, const SimulationConfig& config) {
    canvas_->setSimulator(sim, config);
}

void TimelineWidget::updateCanvas() {
    canvas_->updateSize();
}

void TimelineWidget::scrollToCurrentTime() {
    Simulator* sim = canvas_->getSimulator();
    if (!canvas_ || !sim) return;
    
    double currentTime = sim->get_current_time();
    Metrics m = sim->get_metrics();
    const auto& events = m.get_timeline_events();
    
    double maxEventTime = currentTime;
    for (const auto& event : events) {
        if (event.time > maxEventTime) {
            maxEventTime = event.time;
        }
    }
    double maxTime = std::max({currentTime + 5.0, maxEventTime + 2.0, 20.0});
    
    // Calculate position of current time in canvas
    int canvasWidth = canvas_->width();
    int drawWidth = canvasWidth - TimelineCanvas::MARGIN_LEFT - TimelineCanvas::MARGIN_RIGHT;
    int currentX = TimelineCanvas::MARGIN_LEFT + static_cast<int>((drawWidth * currentTime) / maxTime);
    
    // Get scroll area viewport width
    int viewportWidth = scrollArea_->viewport()->width();
    
    // Center the current time in viewport (or show it on right side if near end)
    int targetScrollX = currentX - viewportWidth / 2;
    
    // Ensure we don't scroll beyond bounds
    targetScrollX = std::max(0, std::min(targetScrollX, scrollArea_->horizontalScrollBar()->maximum()));
    
    // Smooth scroll to target position
    scrollArea_->horizontalScrollBar()->setValue(targetScrollX);
}

void TimelineWidget::onZoomIn() {
    double currentZoom = canvas_->getZoomLevel();
    canvas_->setZoomLevel(currentZoom * 1.25);
    updateZoomLabel();
}

void TimelineWidget::onZoomOut() {
    double currentZoom = canvas_->getZoomLevel();
    canvas_->setZoomLevel(currentZoom / 1.25);
    updateZoomLabel();
}



void TimelineWidget::updateZoomLabel() {
    double zoom = canvas_->getZoomLevel();
    zoomLabel_->setText(QString("%1%").arg(static_cast<int>(zoom * 100)));
}


void TimelineWidget::onCanvasZoomChanged(double zoom) {
    // Update UI when canvas zoom changes
    updateZoomLabel();
}


