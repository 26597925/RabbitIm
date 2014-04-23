#include "FrmPlayer.h"
#include <QPainter>
#include <QImage>
#include <QThread>
#include "DataVideoBuffer.h"

QThread g_CFrmPlayer_Thread;//转换格式的线程
CFrmPlayer::CFrmPlayer(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
    m_FrameProcess.moveToThread(&g_CFrmPlayer_Thread);
    bool check = true;
    check = connect(this, SIGNAL(sigConverteToRGB32Frame(const QVideoFrame&, const QRect&)),
                    &m_FrameProcess, SLOT(slotFrameConvertedToRGB32(const QVideoFrame&, const QRect&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigConverteToRGB32Frame(const QXmppVideoFrame&, const QRect&)),
                    &m_FrameProcess, SLOT(slotFrameConvertedToRGB32(const QXmppVideoFrame&, const QRect&)));
    Q_ASSERT(check);
    check = connect(&m_FrameProcess, SIGNAL(sigConvertedToRGB32Frame(const QVideoFrame&)),
                        SLOT(slotPaint(const QVideoFrame&)));
    Q_ASSERT(check);
    g_CFrmPlayer_Thread.start();
}

CFrmPlayer::~CFrmPlayer()
{
    g_CFrmPlayer_Thread.quit();
    g_CFrmPlayer_Thread.wait();
    qDebug("CFrmPlayer::~CFrmPlayer");
}

void CFrmPlayer::paintEvent(QPaintEvent *)
{
    if(!m_VideoFrame.isValid())
        return;
    if(!m_VideoFrame.map(QAbstractVideoBuffer::ReadOnly))
        return;
    QPainter painter(this);
    QImage image(m_VideoFrame.bits(),
                 m_VideoFrame.width(),
                 m_VideoFrame.height(),
                 m_VideoFrame.bytesPerLine(),
                 QVideoFrame::imageFormatFromPixelFormat(m_VideoFrame.pixelFormat()));
    painter.drawImage(this->rect(), image);
    m_VideoFrame.unmap();
}

void CFrmPlayer::slotPaint(const QVideoFrame &frame)
{
    m_VideoFrame = frame;
    update();
}

//从摄像头捕获的帧
void CFrmPlayer::slotPresent(const QVideoFrame &frame)
{
#ifdef ANDROID
    QRect rect = this->rect();
    emit sigConverteToRGB32Frame(frame, rect);
#else
    slotPaint(frame);
#endif
}

//从网络上接收的帧
void CFrmPlayer::slotPresent(const QXmppVideoFrame &frame)
{
    QRect rect = this->rect();
    emit sigConverteToRGB32Frame(frame, rect);
}
