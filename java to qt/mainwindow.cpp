#include "mainwindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QScreen>
#include <QGuiApplication>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_rng(std::random_device{}()), m_dist(0.0f, 1.0f) {
    setWindowTitle("Driver");
    setFocusPolicy(Qt::StrongFocus); // Ensure the widget can receive key events

    // Set background to black
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    showFullScreen();

    generateInitialTerrain();

    // Create wheels
    Wheel* w1 = new Wheel(100, 300, 20);
    Wheel* w2 = new Wheel(200, 300, 20);
    w1->attach(w2);
    m_wheels.append(w1);
    m_wheels.append(w2);

    // Setup and start the game loop timer
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    m_timer->start(5); // 5ms interval, ~200 FPS
}

MainWindow::~MainWindow() {
    qDeleteAll(m_wheels); // Clean up allocated memory for wheels
}

void MainWindow::generateInitialTerrain() {
    m_lastY = height() / 2;
    for (int i = m_step; i <= width() + m_step; i += m_step) {
        m_slope += (m_dist(m_rng) - static_cast<float>(m_lastY) / height()) * m_difficulty;
        m_slope = std::clamp(m_slope, -1.0f, 1.0f);
        int newY = m_lastY + std::round(m_slope * std::pow(std::abs(m_slope), m_irregularity) * m_step);
        m_lines.append(Line(i - m_step, m_lastY, i, newY));
        m_lastY = newY;
        m_difficulty += m_difficultyIncrement;
    }
    m_lastX = m_step * m_lines.size();
}

void MainWindow::gameLoop() {
    // Calculate average position of wheels for camera focus
    double averageX = 0;
    double averageY = 0;
    if (!m_wheels.isEmpty()) {
        for (const Wheel* wheel : m_wheels) {
            averageX += wheel->x;
            averageY += wheel->y;
        }
        averageX /= m_wheels.size();
        averageY /= m_wheels.size();
    }

    m_cameraX = averageX - 200;
    m_cameraY = -averageY + height() / 2;

    // Generate new terrain as the player moves forward
    if (m_cameraX > m_cameraXFarthest) {
        m_cameraXFarthest += m_step;
        m_slope += (m_dist(m_rng) - static_cast<float>(m_lastY) / height()) * m_difficulty;
        m_slope = std::clamp(m_slope, -1.0f, 1.0f);
        int newY = m_lastY + std::round(m_slope * std::pow(std::abs(m_slope), m_irregularity) * m_step);
        m_lines.append(Line(m_lastX, m_lastY, m_lastX + m_step, newY));
        m_lastY = newY;
        m_lastX += m_step;

        // Remove old terrain to save memory
        if (m_lines.size() > (width() / m_step) * 3) {
            m_lines.removeFirst();
        }
        m_difficulty += m_difficultyIncrement;
    }

    // Update physics for each wheel
    for (Wheel* wheel : m_wheels) {
        wheel->simulate(m_lines, m_accelerating, m_braking);
    }

    update(); // Schedule a repaint
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw terrain lines
    QPen linePen(QColor(0, 150, 50));
    linePen.setWidth(5);
    painter.setPen(linePen);
    for (const Line& line : m_lines) {
        if (auto coordsOpt = line.get(0, 0, width(), height(), -m_cameraX, m_cameraY)) {
            const auto& coords = *coordsOpt;
            painter.drawLine(coords[0], coords[1], coords[2], coords[3]);
        }
    }

    // Draw wheels
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    for (const Wheel* wheel : m_wheels) {
        if (auto coordsOpt = wheel->get(0, 0, width(), height(), -m_cameraX, m_cameraY)) {
            const auto& coords = *coordsOpt;
            int x = coords[0];
            int y = coords[1];
            int r = coords[2];
            painter.drawEllipse(QPoint(x, y), r, r);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_D:       // accelerate (was W)
        m_accelerating = true;
        break;
    case Qt::Key_A:       // brake (was S)
        m_braking = true;
        break;
    case Qt::Key_Escape:
        close(); // Exit the application
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_D:       // accelerate stop
        m_accelerating = false;
        break;
    case Qt::Key_A:       // brake stop
        m_braking = false;
        break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}
