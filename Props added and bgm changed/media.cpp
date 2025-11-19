#include "media.h"

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QMediaDevices>
#include <QSoundEffect>
#include <QPropertyAnimation>
#include <QCoreApplication>
#include <QFile>
#include <QUrl>


namespace {


QUrl pickBgmUrl(const QString& alias, const QString& fileName)
{
    
    const QString qrcPath = QString(":/audio/%1").arg(alias);
    if (QFile::exists(qrcPath)) {
        return QUrl(QStringLiteral("qrc:/audio/") + alias);
    }

    
    const QString fsPath =
        QCoreApplication::applicationDirPath() + "/assets/audio/" + fileName;
    if (QFile::exists(fsPath)) {
        return QUrl::fromLocalFile(fsPath);
    }

    return {};
}


QUrl defaultBgmUrl()
{
    const QString qrcPath = QStringLiteral(":/audio/bgm.mp3");
    if (QFile::exists(qrcPath)) {
        return QUrl(QStringLiteral("qrc:/audio/bgm.mp3"));
    }

    const QString fsPath =
        QCoreApplication::applicationDirPath() + "/assets/audio/bgm.mp3";
    if (QFile::exists(fsPath)) {
        return QUrl::fromLocalFile(fsPath);
    }

    return {};
}

} 

Media::Media(QObject* parent)
    : QObject(parent)
{
    
    auto* e = new QSoundEffect(this);
    e->setSource(QUrl(QStringLiteral("qrc:/sfx/accelerate.wav")));
    e->setLoopCount(1);
    e->setVolume(1.0);
    m_driveSfx.push_back(e);

    
    m_nitroSfx = new QSoundEffect(this);
    m_nitroSfx->setSource(QUrl(QStringLiteral("qrc:/sfx/nitro.wav")));
    m_nitroSfx->setLoopCount(1);
    m_nitroSfx->setVolume(1.0);

    
    m_gameOverOut = new QAudioOutput(this);
    m_gameOverOut->setVolume(1.0);
    m_gameOver = new QMediaPlayer(this);
    m_gameOver->setAudioOutput(m_gameOverOut);
    m_gameOver->setSource(QUrl(QStringLiteral("qrc:/sfx/gameOver.mp3")));

    
    const char* coinPath = "qrc:/sfx/coin.mp3";
    for (int i = 0; i < 3; ++i) {
        auto* out = new QAudioOutput(this);
        out->setVolume(1.5);
        auto* p = new QMediaPlayer(this);
        p->setAudioOutput(out);
        p->setSource(QUrl(coinPath));
        m_coinOuts.push_back(out);
        m_coinPlayers.push_back(p);
    }

    
    const char* fuelPath = "qrc:/sfx/fuel.mp3";
    for (int i = 0; i < 3; ++i) {
        auto* out = new QAudioOutput(this);
        out->setVolume(2.5);
        auto* p = new QMediaPlayer(this);
        p->setAudioOutput(out);
        p->setSource(QUrl(fuelPath));
        m_fuelOuts.push_back(out);
        m_fuelPlayers.push_back(p);
    }
}

Media::~Media() = default;

void Media::setupBgm()
{
    
    m_bgmOut = new QAudioOutput(this);
    m_bgmOut->setVolume(0.8);

    m_bgm = new QMediaPlayer(this);
    m_bgm->setAudioOutput(m_bgmOut);
    m_bgm->setLoops(QMediaPlayer::Infinite);

    
    const QUrl src = defaultBgmUrl();
    if (!src.isEmpty()) {
        m_bgm->setSource(src);
    }
}

void Media::setBgmVolume(qreal v)
{
    if (m_bgmOut) {
        m_bgmOut->setVolume(v);
    }
}

void Media::playBgm()
{
    if (m_bgm) {
        m_bgm->play();
    }
}

void Media::stopBgm()
{
    if (m_bgm) {
        m_bgm->stop();
    }
}

void Media::setStageBgm(int levelIndex)
{
    if (!m_bgm) {
        return;
    }

    QUrl src;

    
    
    
    
    
    switch (levelIndex) {
    case 0: 
        src = pickBgmUrl(QStringLiteral("bgm_meadow.mp3"),
                         QStringLiteral("bgm_meadow.mp3"));
        break;
    case 1: 
        src = pickBgmUrl(QStringLiteral("bgm_desert.mp3"),
                         QStringLiteral("bgm_desert.mp3"));
        break;
    case 2: 
        src = pickBgmUrl(QStringLiteral("bgm_tundra.mp3"),
                         QStringLiteral("bgm_tundra.mp3"));
        break;
    case 3: 
        src = pickBgmUrl(QStringLiteral("bgm_lunar.mp3"),
                         QStringLiteral("bgm_lunar.mp3"));
        break;
    case 4: 
        src = pickBgmUrl(QStringLiteral("bgm_martian.mp3"),
                         QStringLiteral("bgm_martian.mp3"));
        break;
    default:
        break;
    }

    
    if (src.isEmpty()) {
        src = defaultBgmUrl();
    }

    if (!src.isEmpty()) {
        m_bgm->setSource(src);
    }

    
    m_bgm->play();
}



void Media::startAccelLoop()
{
    if (m_driveSfx.isEmpty()) {
        return;
    }
    auto* e = m_driveSfx[0];

    
    if (m_accelFade) {
        m_accelFade->stop();
        delete m_accelFade;
        m_accelFade = nullptr;
    }

    e->setLoopCount(QSoundEffect::Infinite);
    e->setVolume(1.0);
    if (!e->isPlaying()) {
        e->play();
    }
}

void Media::stopAccelLoop()
{
    if (m_driveSfx.isEmpty()) {
        return;
    }
    auto* e = m_driveSfx[0];

    if (m_accelFade) {
        m_accelFade->stop();
        delete m_accelFade;
        m_accelFade = nullptr;
    }

    
    m_accelFade = new QPropertyAnimation(e, "volume", this);
    m_accelFade->setStartValue(e->volume());
    m_accelFade->setEndValue(0.0);
    m_accelFade->setDuration(250);

    connect(m_accelFade, &QPropertyAnimation::finished, this, [this, e] {
        e->stop();
        e->setVolume(0.0);
        if (m_accelFade) {
            m_accelFade->deleteLater();
            m_accelFade = nullptr;
        }
    });

    m_accelFade->start();
}



void Media::playNitroOnce()
{
    if (!m_nitroSfx) {
        return;
    }
    m_nitroSfx->stop();
    m_nitroSfx->setLoopCount(1);
    m_nitroSfx->setVolume(1.0);
    m_nitroSfx->play();
}



void Media::coinPickup()
{
    if (m_coinPlayers.isEmpty()) {
        return;
    }
    m_nextCoinPl = (m_nextCoinPl + 1) % m_coinPlayers.size();
    auto* p = m_coinPlayers[m_nextCoinPl];
    p->setPosition(0);
    p->play();
}

void Media::fuelPickup()
{
    if (m_fuelPlayers.isEmpty()) {
        return;
    }
    m_nextFuelPl = (m_nextFuelPl + 1) % m_fuelPlayers.size();
    auto* p = m_fuelPlayers[m_nextFuelPl];
    p->setPosition(0);
    p->play();
}



void Media::playGameOverOnce()
{
    if (!m_gameOver) {
        return;
    }
    m_gameOver->setPosition(0);
    m_gameOver->play();
}
