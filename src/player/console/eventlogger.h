#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

// eventlogger.h
// 8/7/2011

#include <QtCore/QList>
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QTimer)

class MainWindow;
class Player;
class SignalHub;

class EventLogger : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(EventLogger)
  typedef EventLogger Self;
  typedef QObject Base;

  MainWindow *logger_;
  Player *player_;
  SignalHub *hub_;
  QTimer *logUntilPlayingTimer_;
  int logCount_;

public:
  EventLogger(MainWindow *logger, Player *player, SignalHub *hub, QObject *parent = nullptr);

public slots:
  void logSubtitleChanged();
  void logAudioTrackChanged();
  void logVolumeChanged();
  void logMediaChanged();
  void logMediaClosed();
  void logTitleChanged();
  void logOpening();
  void logBuffering();
  void logPlaying();
  void logPaused();
  void logStopped();
  void logPlayerError();
  void logTrackNumberChanged(int track);
  void logAudioChannelChanged(int ch);
  void logPlayRateChanged(qreal rate);

  void logCanvasEnabled(bool t);

  void logTextEncodingChanged(const QString &enc);

  void logLoginRequested(const QString &userName);
  void logLoginSucceeded(const QString &userName);
  void logLoginFailed(const QString &userName);
  void logLogoutRequested();
  void logLogoutFinished();

  void logSeeked(qint64 msecs);
  void logAudioDelayChanged(qint64 msecs);

  void logCacheCleared();

  void logTrackedWindowDestroyed();

  void logServerAgentConnectionError();
  void logServerAgentServerError();
  void logServerAgentError404();
  void logServerAgentUnknownError();

  void logClientAgentAuthorized();
  void logClientAgentDeauthorized();
  void logClientAgentAuthorizationError();

  void logAspectRatioChanged(const QString &ratio);
  void logContrastChanged(qreal value);
  void logBrightnessChanged(qreal value);
  void logHueChanged(int value);
  void logSaturationChanged(qreal value);
  void logGammaChanged(qreal value);

  void logSubtitleOnTopChanged(bool value);
  void logAnnotationScaleChanged(qreal value);
  void logAnnotationFullscreenScaleChanged(qreal value);
  void logAnnotationOpacityFactorChanged(int value);
  void logAnnotationBackgroundOpacityFactorChanged(int value);
  void logAnnotationSpeedupChanged(qreal value);
  void logAnnotationRotationChanged(qreal value);
  void logAnnotationOffsetChanged(int value);
  void logAnnotationCountLimitedChanged(bool value);
  void logAnnotationSkipped();

  void logPauseHoveredAnnotations(bool t);
  void logResumeHoveredAnnotations(bool t);
  void logRemoveHoveredAnnotations(bool t);
  void logExpelNearbyAnnotations(bool t);
  void logAttractNearbyAnnotations(bool t);

  void logPreferLocalDatabaseChanged(bool t);

  void logSelectedUserIds(const QList<qint64> &uids);

  void logInternetConnectionChanged(bool t);
  void logPreferFloatAnnotationChanged(bool t);

  void logFileSaved(const QString &fileName);

protected slots:
  void logAeroEnabledChanged(bool t);
  void logMenuEnabledChanged(bool t);

protected slots:
  void logUntilPlaying();
  void startLogUntilPlaying();
  void stopLogUntilPlaying();
};

#endif // EVENTLOGGER_H
