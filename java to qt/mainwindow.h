#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QList>
#include <QTimer>
#include <random>
#include "line.h"
#include "wheel.h"

// Forward declaration of QKeyEvent
class QKeyEvent;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    void generateInitialTerrain();

    QTimer *m_timer;
    QList<Line> m_lines;
    QList<Wheel*> m_wheels;

    int m_lastX = 0;
    int m_lastY = 0;
    float m_slope = 0.0f;
    const int m_step = 20;
    float m_difficulty = 0.01f;
    const float m_difficultyIncrement = 0.001f;
    const float m_irregularity = 0.5f;

    int m_cameraX = 0;
    int m_cameraY = 200;
    int m_cameraXFarthest = 0;

    bool m_accelerating = false;
    bool m_braking = false;

    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_dist;
};

#endif // MAINWINDOW_H
