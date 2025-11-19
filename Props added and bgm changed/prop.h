#ifndef PROP_H
#define PROP_H

#include <QVector>
#include <QColor>
#include <random>
#include <QPainter>
#include "constants.h"


enum class PropType {
    
    Tree, Rock, Flower, Mushroom, 
    
    Cactus, Tumbleweed,
    
    Igloo, Penguin, Snowman, IceSpike, DeadTree, Signpost,
    
    UFO,
    
    Rover, Alien
};

struct Prop {
    int wx;         
    int wy;         
    PropType type;
    int variant;    
    bool flipped;   
};

class PropSystem {
public:
    PropSystem();

    
    void maybeSpawnProp(int worldX, int groundGy, int levelIndex, std::mt19937& rng);

    
    void draw(QPainter& p, int camX, int camY, int screenW, int screenH);

    
    void prune(int minWorldX);

    
    void clear();

private:
    QVector<Prop> m_props;

    
    void plot(QPainter& p, int gx, int gy, const QColor& c);

    
    
    void drawTree(QPainter& p, int gx, int gy, int wx, int wy, int variant);
    void drawRock(QPainter& p, int gx, int gy, int variant);
    void drawFlower(QPainter& p, int gx, int gy, int variant);
    void drawMushroom(QPainter& p, int gx, int gy, int variant);

    
    void drawCactus(QPainter& p, int gx, int gy, int variant);
    void drawTumbleweed(QPainter& p, int gx, int gy, int variant);

    
    void drawIgloo(QPainter& p, int gx, int gy, int variant);
    void drawPenguin(QPainter& p, int gx, int gy, int variant, bool flipped);
    void drawSnowman(QPainter& p, int gx, int gy, int variant);
    void drawIceSpike(QPainter& p, int gx, int gy, int variant);
    void drawDeadTree(QPainter& p, int gx, int gy, int variant);
    void drawSignpost(QPainter& p, int gx, int gy, int variant);

    
    void drawUFO(QPainter& p, int gx, int gy, int variant);

    
    void drawRover(QPainter& p, int gx, int gy, int variant, bool flipped);
    void drawAlien(QPainter& p, int gx, int gy, int variant);
};

#endif 
