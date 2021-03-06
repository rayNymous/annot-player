#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// mainwindow.h
// 6/30/2011

#ifdef AC_ENABLE_GAME
# include "textthread.h"
# include "processinfo.h"
#endif // AC_ENABLE_GAME
#include "lib/annotcloud/alias.h"
#include "lib/annotcloud/annotation.h"
#include "lib/mrlresolver/mrlinfo.h"
#include "lib/searchengine/searchenginefactory.h"
#include "lib/translator/translatormanager.h"
#include <QtCore/QMutex>
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QFileInfoList>
#include <QtGui/QMainWindow>

QT_BEGIN_NAMESPACE
class QGestureEvent;
class QMimeData;
class QNetworkAccessManager;
class QNetworkCookieJar;
class QPanGesture;
class QPinchGesture;
class QSwipeGesture;
class QTimer;
class QUrl;
QT_END_NAMESPACE

class QxCountdownTimer;
class QxMouseRubberBand;
class QxCircularRubberBand;

// Objects
class Atlas;
class AnnotationDatabase;
class AnnotationDownloader;
class AnnotationFilter;
class AnnotationServerAgent;
class ClientAgent;
class ClipboardMonitor;
class DataManager;
class DataServer;
class EventLogger;
class FadeAnimation;
class Grabber;
class Magnifier;
class GameLibrary;
class MediaLibrary;
class MediaLibraryView;
class MainLibraryView;
class MrlResolver;
class Player;
class RippleImageFilter;
class SignalHub;
class Tray;
class TranslatorManager;
class MeCabParser;
class Updater;

class AcPlayerServer;
class AcDownloader;
class AcBrowser;
class AcTranslator;
class AcUpdater;

class MouseClickEventListener;

// Views
class AnnotationGraphicsView;
class AnnotationAnalyticsView;
class AnnotationListView;
//class CommentView;
class MainPlayerUi;
class MessageHandler;
class MiniPlayerUi;
//class MiniPlayerDock;
//class OsdDock;
class OsdWindow;
class MainConsole;
class MiniConsole;
class EmbeddedPlayerUi;
class EmbeddedCanvas;
class PlayerUi;
class Preferences;
class ProcessFilter;
class SyncView;
class BacklogDialog;
class ConsoleDialog;
class UserAnalyticsView;
class VideoView;

// Dialogs
//class NetworkProxyDialog;
//class SiteAccountView;
class UrlDialog;

// MainWindow
class MainWindow : public QMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)
  typedef MainWindow Self;
  typedef QMainWindow Base;

  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Types -
public:
  enum Color {
    DefaultColor = 0,
    White, Cyan, Blue, Red, Purple, Orange, Black
  };

  // - Constructions -
public:
  explicit MainWindow(bool unique = true, QWidget *parent = nullptr, Qt::WindowFlags f = 0);
  //bool isValid() const;

  void launch(const QStringList &args);

  // - Signals -
signals:
  void windowPicked(WId);
  void userIdChanged(qint64 uid);
  void openProcessRequested();
  void posChanged();
  void openAnnotationUrlRequested(const QString &url);
  void removeAnnotationsFromAnnotationViewRequested();
  void openAnnotationFileRequested(const QString &file);
  void annotationUrlAdded(QString url);
  void windowMaximized();
  void playingFinished();
  void playMediaRequested();
  void trayToolTip(const QString &tip);
  void trayMessage(const QString &title, const QString &message);
  void response(const QString &text);
  void said(const QString &text, const QString &color = QString());
  void showTextRequested(const QString &text);
  void windowClosed();
  void windowTitleSuffixToChange(QString suffix);
  void downloadProgressUpdated();
  void downloadFinished();
  void progressMessageChanged(const QString &text);

  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);
  void messageOnce(const QString &text);
  void errorMessageOnce(const QString &text);
  void warningOnce(const QString &text);
  void notificationOnce(const QString &text);

  void seeked();
  void addAndShowAnnotationRequested(const Annotation &a);
  void addAnnotationsRequested(const AnnotationList &l);
  void showAnnotationRequested(const Annotation &a);
  void showAnnotationOnceRequested(const Annotation &a);
  void setAnnotationsRequested(const AnnotationList &l);
  void appendAnnotationsRequested(const AnnotationList &l);

  void userChanged(User user);

  // - States -
public:
  bool isPlaying() const;
  bool isEditing() const;

  bool isWindowOnTop() const;

  bool hasVisiblePlayer() const;
  PlayerUi *visiblePlayer(); // One of three player Uis, or nullptr.

  bool inputLineHasFocus() const;
  bool prefixLineHasFocus() const;

#ifdef AC_ENABLE_HONYAKU
  bool isTranslateEnabled() const;
#endif // AC_ENABLE_HONYAKU

  bool isAutoPlayNext() const;
  bool repeatCurrent() const;
  bool noRepeat() const;

public slots:
  void setAutoPlayNext();
  void setRepeatCurrent();
  void setNoRepeat();

  void showMainLibrary();

  void maximizedToFullScreen();

#ifdef AC_ENABLE_HONYAKU
  void setTranslateEnabled(bool enabled);
  void translate(const QString &text, bool extra = false);
  void translate(const QString &text, int lang, bool extra = false);
#endif // AC_ENABLE_HONYAKU

  void showTraditionalChinese(const QString &gbk);

  void setWindowOnTop(bool t = true);
  void toggleWindowOnTop()
  { setWindowOnTop(!isWindowOnTop()); }

  //void ensureWindowOnTop();

  void setSubtitleColorToDefault();
  void setSubtitleColorToWhite();
  void setSubtitleColorToCyan();
  void setSubtitleColorToBlue();
  void setSubtitleColorToRed();
  void setSubtitleColorToPurple();
  void setSubtitleColorToOrange();
  void setSubtitleColorToBlack();
  int subtitleColor() const;
  void setSubtitleColor(int colorId);
private:
  void uncheckSubtitleColorActions();

public slots:
  void onFocusedWidgetChanged(QWidget *w_old, QWidget *w_new);

  void setEmbeddedWindow(WId winId);

  void about();
  //void quit();
  void help();

  void login(const QString &userName, const QString &encryptedPassword, bool async = true);
  void logout(bool async = true);

  void checkInternetConnection(bool async = true);
  void deleteCaches();

  void newWindow();

  void openSource(const QString &source);
  void openSources(const QStringList &l);
  void open();  ///< By default the same as openFile()
  void openFile();
  void openAnnotationFile();
  void openAnnotationFile(const QString &fileName, bool import = true);
  void importAnnotationFile(const QString &fileName, const QString &url);

  void openUrl();
  void openAnnotationUrl();

  void openAnnotationUrlFromAliases(const AliasList &l, const QHash<QString, QString> &cache, bool async = true);
  void openAnnotationUrlFromAliases(const AliasList &l, bool async = true)
  { openAnnotationUrlFromAliases(l, QHash<QString, QString>(), async); } // bypass clang compile error for qhash default initializer

  void openAnnotationUrl(const QString &url, bool submit = false);
  void openAnnotationUrlAndSubmit(const QString &url) { openAnnotationUrl(url, true); }
  void openUrl(const QString &url);
  //void openUrl(const QString &url, bool save);
  void openProcess();
  void openWindow();
  void openDevice();
  void openDevicePath(const QString &path, bool isAudioCD);
  void openAudioDevice();
  void openVideoDevice();
  void openSubtitle();
  void openDirectory();
  void openMrl(const QString &path, bool checkPath = true);
  void openStreamUrl(const QString &rtsp);
  void openRemoteMedia(const MediaInfo &mi, QNetworkCookieJar *jar = nullptr);
  void openLocalUrl(const QUrl &url);
  void openLocalUrls(const QList<QUrl> &urls);
  void openMimeData(const QMimeData *urls);
  void openSubtitleFile(const QString &path);
  void closeMedia();
  void play();
  void playPause();
  void playPauseMedia();
  void replay(); /// Restart from the beginning.
  void pause();
  void stop();
  void promptStop();
  void nextFrame();
  void snapshot(bool mediaOnly = true);
  void snapshotAll() { snapshot(false); }

  void previous();
  void next();
  void autoPlayNext();

  void seek(qint64 msecs);
  void forward(qint64 msecs = 0);  ///< If <=0, default delta time is used.
  void backward(qint64 msecs = 0); ///< If <=0, default delta time is used.

  void forward5s()      { forward(5 * 1000); }
  void backward5s()     { backward(5 * 1000); }
  void forward10s()     { forward(10 * 1000); }
  void backward10s()    { backward(10 * 1000); }
  void forward25s()     { forward(25 * 1000); }
  void forward30s()     { forward(30 * 1000); }
  void backward30s()    { backward(30 * 1000); }
  void forward60s()     { forward(60 * 1000); }
  void backward60s()    { backward(60 * 1000); }
  void forward85s()     { forward(85 * 1000); }
  void forward90s()     { forward(90 * 1000); }
  void backward90s()    { backward(90 * 1000); }
  void forward1m()      { forward(1 * 60000); }
  void backward1m()     { backward(1 * 60000); }
  void forward5m()      { forward(5 * 60000); }
  void backward5m()     { backward(5 * 60000); }
  void forward10m()     { forward(10 * 60000); }
  void backward10m()    { backward(10 * 60000); }

  void volumeUp(qreal delta = 0);  ///< If <=0, default delta percent is used.
  void volumeDown(qreal delta = 0);///< If <=0, default delta percent is used.

  //void about(); // TODO
  //void help(); // TODO

  void updateWindowTitle();
  void setWindowTitleSuffix(const QString &suffix)
  { windowTitleSuffix_ = suffix; updateWindowTitle(); }

  void updateDownloadProgress(qint64 receivedBytes, qint64 totalBytes);
  void syncInputLineText(const QString &text);
  void syncPrefixLineText(const QString &text);

  void enableWindowTransparency();
  void disableWindowTransparency();

  void showVideoViewIfAvailable();

  void setMultipleWindowsEnabled(bool t);

  void openInWebBrowser();
  void downloadCurrentUrl();
  void downloadAnnotations();
  void copyCurrentUrl();
  void copyCurrentTitle();
  void clickProgressButton();
protected:
  bool hasNext() const;
  QString windowTitleBase() const;
  QString currentUrl() const;
  QString currentTitle() const;
  static QString downloadSpeedToString(int speed);

protected slots:
  void checkReachEnd();

#ifdef AC_ENABLE_HONYAKU
  void showTranslator();
  void translateUsingTranslator(const QString &text);
#endif // // AC_ENABLE_HONYAKU

  void toggleLibrary();
  void toggleMainLibrary();
  void toggleMediaLibrary();

  void update();
protected slots:
  void checkVersion(long version);

public slots:
  void invalidateMediaAndPlay(bool async = true);
protected slots:
  void playMedia();
  void updateWindowSize();

  void updateTraceButtons();

  void setRippleEnabled(bool t);
  void enableRipple() { setRippleEnabled(true); }
  void disableRipple() { setRippleEnabled(false); }

  // - Live -
public slots:
  //void openChannel();
  //void closeChannel();
  //void updateLiveAnnotations(bool async = true);

  // - Cursor -
//protected:
//  bool isCursorVisible() const { return cursorVisible_; }
//protected slots:
//  void setCursorVisible(bool t);
//  void showCursor() { setCursorVisible(true); }
//  void hideCursor() { setCursorVisible(false); }
//  void resetAutoHideCursor();
//  void autoHideCursor();

  // - Video adjustment -
protected slots:
  void contrastUp(); void contrastDown(); void contrastReset();
  void brightnessUp(); void brightnessDown(); void brightnessReset();
  void hueUp(); void hueDown(); void hueReset();
  void saturationUp(); void saturationDown(); void saturationReset();
  void gammaUp(); void gammaDown(); void gammaReset();

  // - Annotation transformations -
protected slots:
  void resetAnnotationScale();
  void annotationScaleUp();
  void annotationScaleDown();
  void annotationRotateUp();
  void annotationRotateDown();
  void increaseAnnotationOffset();
  void decreaseAnnotationOffset();

  // - Dialogs -
protected:
  UrlDialog *annotationUrlDialog();
  UrlDialog *mediaUrlDialog();
  UrlDialog *annotationDownloaderDialog();
  AnnotationAnalyticsView *annotationAnalyticsView();

protected slots:
  void showUserAnalytics(qint64 userId);

  void showLogin();

  void setMagnifierVisible(bool visible);
  void showMagnifier() { setMagnifierVisible(true); }
  void hideMagnifier() { setMagnifierVisible(true); }
  void toggleMagnifierVisible();

  void showLibrary();
  void showGameWithDigest(const QString &digest);

  //void setNetworkProxyDialogVisible(bool visible);

  void showUser();

  void showAnnotationEditor();

  void showBlacklist();

  void showAnnotationSource();

  void showAnnotationBrowser();

  void showAnnotationAnalytics();

  void openHomePage();

  void showSeek();

  void showAudioDelayDialog();

  void showAnnotationCount();

  //void showCommentView();
  //void hideCommentView();
  //void setCommentViewVisible(bool visible);

  void showWindowPicker();
  void showProcess();

  void showBacklog();

  void showConsole();

  void showDownloader();

  void showMediaInfo();

  //void setSiteAccountViewVisible(bool visible);
protected slots:
  //void invalidateSiteAccounts();
  BacklogDialog *backlogDialog();
  ConsoleDialog *consoleDialog();

  // - User -
public slots:
  void setUserAnonymous(bool t, bool async = true);
  void setUserLanguage(int lang, bool async = true);

  // - Languages -
protected slots:
  void setAppLanguageToEnglish();
  void setAppLanguageToJapanese();
  void setAppLanguageToTraditionalChinese();
  void setAppLanguageToSimplifiedChinese();
  void invalidateAppLanguage();

  void setUserLanguageToEnglish();
  void setUserLanguageToJapanese();
  void setUserLanguageToTraditionalChinese();
  void setUserLanguageToSimplifiedChinese();
  void setUserLanguageToKorean();
  void setUserLanguageToFrench();
  void setUserLanguageToGerman();
  void setUserLanguageToItalian();
  void setUserLanguageToSpanish();
  void setUserLanguageToPortuguese();
  void setUserLanguageToRussian();
  //void setUserLanguageToUnknown();

  void invalidateAnnotationLanguages();

  // - Error handling -
protected slots:
  void handlePlayerError();

  // - Annotations -
public slots:
  void eval(const QString &cmd);
  void chat(const QString &text, bool async = true);
  void respond(const QString &text);
  void say(const QString &text, const QString &color = QString());

  void showMessage(const QString &text);
  void showError(const QString &text);
  void notify(const QString &text);
  void warn(const QString &text);
  void showMessageOnce(const QString &text);
  void showErrorOnce(const QString &text);
  void notifyOnce(const QString &text);
  void warnOnce(const QString &text);

  void submitText(const QString &text, bool async = true);
  void showText(const QString &text, bool isSigned = false);

  //void submitLiveText(const QString &text, bool async = true);

  void showSubtitle(const QString &text, int userLang = 0, const QString &prefix = QString());

  void updateAnnotations(bool async = true);
  void updateOnlineAnnotations(bool async = true);
  void updateOfflineAnnotations(bool async = true);
  void setToken(const QString &filePath = QString(), bool async = true);
  void invalidateToken(const QString &mrl = QString());
  void submitAlias(const Alias &alias, bool async = true, bool lockfree = false);
  void submitAliasText(const QString &text, qint32 type = 0, bool async = true, bool lock = true);

  void signFile(const QString &path, bool async = true);
  void signFileWithUrl(const QString &path, const QString &url, bool async = true);

  void blessTokenWithId(qint64 tid, bool async = true);
  void curseTokenWithId(qint64 tid, bool async = true);

  void blessUserWithId(qint64 tid, bool async = true);
  void curseUserWithId(qint64 tid, bool async = true);
  void blockUserWithId(qint64 tid, bool async = true);

  void blessAliasWithId(qint64 tid, bool async = true);
  void curseAliasWithId(qint64 tid, bool async = true);
  void blockAliasWithId(qint64 tid, bool async = true);

  void blessAnnotationWithId(qint64 tid, bool async = true);
  void curseAnnotationWithId(qint64 tid, bool async = true);
  void blockAnnotationWithId(qint64 tid, bool async = true);

#ifdef AC_ENABLE_HONYAKU
  // - Translation -
protected slots:
  void showTranslation(const QString &text, int service);
  void showAdditionalTranslation(const QString &text, int service);

  void showRomajiTranslation(const QString &text) { showTranslation(text, TranslatorManager::Romaji); }
  void showMicrosoftTranslation(const QString &text) { showTranslation(text, TranslatorManager::Microsoft); }
  void showGoogleTranslation(const QString &text) { showTranslation(text, TranslatorManager::Google); }
  void showYahooTranslation(const QString &text) { showTranslation(text, TranslatorManager::Yahoo); }
  void showFresheyeTranslation(const QString &text, bool extra = false);
  //void showOcnTranslation(const QString &text, bool extra = false);
  void showExciteTranslation(const QString &text) { showTranslation(text, TranslatorManager::Excite); }
  void showSdlTranslation(const QString &text) { showTranslation(text, TranslatorManager::Sdl); }
  void showSystranTranslation(const QString &text) { showTranslation(text, TranslatorManager::Systran); }
  void showNiftyTranslation(const QString &text) { showTranslation(text, TranslatorManager::Nifty); }
  void showInfoseekTranslation(const QString &text) { showTranslation(text, TranslatorManager::Infoseek); }

  void showRomajiAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Romaji); }
  void showMicrosoftAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Microsoft); }
  void showGoogleAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Google); }
  void showYahooAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Yahoo); }
  void showFresheyeAdditionalTranslation(const QString &text) { showFresheyeTranslation(text, true); }
  //void showOcnAdditionalTranslation(const QString &text) { showOcnTranslation(text, true); }
  void showExciteAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Excite); }
  void showSdlAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Sdl); }
  void showSystranAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Systran); }
  void showNiftyAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Nifty); }
  void showInfoseekAdditionalTranslation(const QString &text) { showAdditionalTranslation(text, TranslatorManager::Infoseek); }
#endif // AC_ENABLE_HONYAKU

  // - Remote annotations -
public slots:
  void importUrl(const QString &url);
  void importAnnotationsFromUrl(const QString &suburl, const QString &refurl);
  bool isAnnotationUrlRegistered(const QString &suburl) const;
protected slots:
  void addRemoteAnnotations(const AnnotationList &l, const QString &url, const QString &originalUrl);

  bool registerAnnotationUrl(const QString &suburl);
  void clearAnnotationUrls();

  bool registerAnnotationFile(const QString &fileName);
  void clearAnnotationFiles();

  // - Recent -
protected slots:
  void addRecent(const QString &path);
  void openRecent(int i);
  void clearRecent();
  void updateRecent();
  void updateRecentMenu();

  // - Playlist -
protected:
  int currentPlaylistIndex() const;
protected slots:
  void setRecentOpenedFile(const QString &path);
  void openPlaylistItem(int i);
  void openNextPlaylistItem();
  void openPreviousPlaylistItem();
  void clearPlaylist();
  void updatePlaylist();
  void updatePlaylistMenu();

  // - Events -
public:
  void move(const QPoint &pos) { Base::move(pos); emit posChanged(); }
  void move(int x, int y) { Base::move(x, y); emit posChanged(); }

public slots:
  void setVisible(bool visible) override;
  void dispose();
public:
  bool event(QEvent *event) override;
  void gestureEvent(QGestureEvent *e);
  void panGesture(QPanGesture *g);
  void pinchGesture(QPinchGesture *g);
  void swipeGesture(QSwipeGesture *g);
protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

  void moveEvent(QMoveEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void changeEvent(QEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

  void contextMenuEvent(QContextMenuEvent *event) override;

  void closeEvent(QCloseEvent *event) override;

  void focusInEvent(QFocusEvent *e) override;
  void focusOutEvent(QFocusEvent *e) override;

protected slots:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

  void updateAnnotationHoverGesture();

#ifdef AC_ENABLE_HONYAKU
  void updateTranslatorMenu();
#endif // AC_ENABLE_HONYAKU
  void updateContextMenu();
  void updateGameMenu();
  void updateAspectRatioMenu();
  void updateAudioChannelMenu();
  void updateSettingsMenu();
  void updateMenuTheme();
  void updateAnnotationSettingsMenu();
  void updateAnnotationSubtitleMenu();
  void updateUserMenu();
  void updateTrackMenu();

  void rememberPlayPos();
  void resumePlayPos();

  void rememberSubtitle();
  void resumeSubtitle();

  void rememberAudioTrack();
  void resumeAudioTrack();

  void rememberAudioChannel();
  void resumeAudioChannel();

  void rememberAspectRatio();
  void resumeAspectRatio();

  void resumeAll();

  void loadSubtitles();
  void loadSubtitlesLater();

protected:
  static bool canResume(const QString &url);

protected:
  bool isGlobalPosNearEmbeddedPlayer(const QPoint &pos) const;
  bool isGlobalPosOverEmbeddedPositionSlider(const QPoint &pos) const;
  bool isGlobalPosOverMainConsole(const QPoint &pos) const;

public slots:
  void updatePlayMode();
  void updatePlayerMode();
  void updateTokenMode();
  void updateWindowMode();
  void updateOsdWindowOnTop();

  void invalidatePlayerMode();

  void resumeAnnotations();

  // - Themes -
public slots:
  void setThemeToDefault();
  void setThemeToRandom();
  void setThemeToDark();
  void setThemeToBlack();
  void setThemeToBlue();
  void setThemeToBrown();
  void setThemeToCyan();
  void setThemeToGray();
  void setThemeToGreen();
  void setThemeToPink();
  void setThemeToPurple();
  void setThemeToRed();
  void setThemeToWhite();
  void setThemeToYellow();

  // - Annotation effect -
protected slots:
  void updateAnnotationEffectMenu();
  void setAnnotationEffectToDefault();
  void setAnnotationEffectToTransparent();
  void setAnnotationEffectToShadow();
  void setAnnotationEffectToBlur();

  // - Navigation -
protected:
  bool isNavigable() const;
protected slots:
  void setPreviousEnabled(bool t);
  void setNextEnabled(bool t);
  void setNavigationEnabled(bool t);
  void enableNavigation() { setNavigationEnabled(true); }
  void disableNavigation() { setNavigationEnabled(false); }

  void openPreviousMrl();
  void openNextMrl();

  // - Browse -
public slots:
  void setBrowsedFile(const QString &fileName);
  void setBrowsedUrl(const QString &mrl);
  void openBrowsedDirectory();
  void openBrowsedFile(int id);
  void moreBrowsedFiles();
  void lessBrowsedFiles();
  void openPreviousFile();
  void openNextFile();

  void showMinimizedAndPause();
  void actualSize();
protected:
  void updateBrowseMenu(const QString &fileName);
  void updateBrowseMenu();
  int currentBrowsedFileId() const;

  QString browsedDirectory() const;

  // - Proxy browser -
protected slots:
  void openProxyBrowser();

  // - Clipboard -
protected slots:
  void enterAnnotationUrl(const QString &url);
  void enterMediaUrl(const QString &url);
  void enterDownloadUrls(const QStringList &urls);

  // - Close menu -
protected slots:
  void nothingAfterFinished();
  void sleepAfterFinished();
  void shutdownAfterFinished();
  void promptSleep();
  void promptShutdown();

  // - Signal mode -
#ifdef WITH_WIN_TEXTHOOK
signals:
  void attached(ProcessInfo pi);
  void detached(ProcessInfo pi);
  void subscribeThreadsRequested(TextThreadList,ProcessInfo);
  void hideSyncViewRequested();
public slots:
  void openProcessPath(const QString &path);
  void subscribeThreads(const TextThreadList &threads, const ProcessInfo &pi);
  //void updateThread(const TextThread &thread);
  void updateSubscription(const TextThreadList &threads, bool async = true);
  void openProcessWindow(WId hwnd);
  void openProcessId(ulong pid);

protected slots:
  void synchronizeProcess(const ProcessInfo &pi, bool async = true);

//protected slots:
//  void rememberGameEncoding(const ProcessInfo &pi);
//  void resumeGameEncoding(const ProcessInfo &pi);
#endif // WITH_WIN_TEXTHOOK

public slots:
  void setWindowTrackingEnabled(bool t);

#ifdef AC_ENABLE_GAME
protected slots:
  void translateGameText(const QString &text, int role);
  void showGameText(const QString &text, int role);
  void showSyncGame(bool showProcess = true);

  void showGamePreferences();

  void openCurrentGame();

protected:
  void updateGameTextMenu();
  bool isGameTextVisible() const;
  bool isAppLocaleEnabled() const;

private:
  SyncView *syncView_;
  MessageHandler *messageHandler_;
  ProcessFilter *processFilter_;

  QMenu *gameTextMenu_;
  QAction *showSyncGameAct_,
          *showGamePreferencesAct_,
          //*toggleAutoOpenCurrentGameAct_,
          *openCurrentGameAct_;
  QAction *toggleGameTextVisibleAct_,
          *toggleGameTextColorfulAct_,
          *toggleGameTextResizableAct_,
          *toggleAppLocaleEnabledAct_;
#endif // AC_ENABLE_GAME
private:
  QAction *toggleTranslateSelectionAct_;

#ifdef WITH_WIN_ATLAS
  // - ATLAS -
protected slots:
  void translateWithAtlas(const QString &text, int role);
protected:
  void showAtlasTranslation(const QString &text);
  void showAtlasAdditionalTranslation(const QString &text);
  bool isAtlasEnabled() const;
private:
  void createAtlas();
  QAction *toggleAtlasEnabledAct_;
  Atlas *atlas_;
#endif // WITH_WIN_ATLAS

  // - Helpers -
protected:
  static bool isMimeDataSupported(const QMimeData *urls);
  static bool isDevicePath(const QString &path);
  static QString removeUrlPrefix(const QString &url);

  static QString digestFromFile(const QString &path);

  static QString languageToString(int lang);
  static int fileType(const QString &fileName);

  static QString shortenText(const QString &text);

  static bool isRemoteMrl(const QString &mrl);

  bool isPosOutOfScreen(const QPoint &globalPos) const;
  bool isRectOutOfScreen(const QRect &globalRect) const;

  bool isDigestReady(const QString &path) const;
  bool isAliasReady(const QString &path) const;

  // - Search -
public slots:
  void searchWithEngine(int engine, const QString &key);
protected slots:
  void searchCurrentTitleWithGoogle() { searchWithEngine(SearchEngineFactory::Google, currentTitle()); }
  void searchCurrentTitleWithGoogleImages() { searchWithEngine(SearchEngineFactory::GoogleImages, currentTitle()); }
  void searchCurrentTitleWithBing()   { searchWithEngine(SearchEngineFactory::Bing, currentTitle()); }
  void searchCurrentTitleWithNicovideo() { searchWithEngine(SearchEngineFactory::Nicovideo, currentTitle()); }
  void searchCurrentTitleWithBilibili() { searchWithEngine(SearchEngineFactory::Bilibili, currentTitle()); }
  void searchCurrentTitleWithAcfun()  { searchWithEngine(SearchEngineFactory::Acfun, currentTitle()); }
  void searchCurrentTitleWithYoutube() { searchWithEngine(SearchEngineFactory::Youtube, currentTitle()); }
  void searchCurrentTitleWithYouku()  { searchWithEngine(SearchEngineFactory::Youku, currentTitle()); }
  void searchCurrentTitleWithWikiEn() { searchWithEngine(SearchEngineFactory::WikiEn, currentTitle()); }
  void searchCurrentTitleWithWikiJa() { searchWithEngine(SearchEngineFactory::WikiJa, currentTitle()); }
  void searchCurrentTitleWithWikiZh() { searchWithEngine(SearchEngineFactory::WikiZh, currentTitle()); }

  // - Dialogs -
protected slots:
  void showPreferences();

  // - External Annotations -
protected slots:
  //void loadExternalAnnotations();

  void removeAliasAnnotationFile(const Alias &a);
  void removeAnnotationFile(const QString &url);
  void repairExternalAnnotationFileNames();
protected:
  bool hasExternalAnnotationFiles() const;
  QStringList externalAnnotationFiles() const;
  QStringList externalAnnotationUrls() const;

#ifdef AC_ENABLE_HONYAKU
  // - Translation -
protected slots:
  void toggleRomajiTranslator(bool t);
  void toggleMicrosoftTranslator(bool t);
  void toggleGoogleTranslator(bool t);
  void toggleYahooTranslator(bool t);
  void toggleFresheyeTranslator(bool t);
  //void toggleOcnTranslator(bool t);
  void toggleExciteTranslator(bool t);
  void toggleSdlTranslator(bool t);
  void toggleSystranTranslator(bool t);
  void toggleNiftyTranslator(bool t);
  void toggleInfoseekTranslator(bool t);
#endif // AC_ENABLE_HONYAKU

  // - Rubber band -
protected slots:
  void pauseAnnotationsAt(const QRect &rect);
  void resumeAnnotationsAt(const QRect &rect);
  void removeAnnotationsAt(const QRect &rect);

protected slots:
  void saveSettings();
  void saveSettingsLater();

protected slots:
  void setDefaultAspectRatio();
  void setWideScreenAspectRatio(); // 16:9
  void setStandardAspectRatio(); // 4:3
  void updateAnnotationMetaVisible();

protected slots:
  void setSubmit(bool t) { submit_ = t; }
  void setSaveMedia(bool t);
  void saveMedia();

protected slots:
  void setLeftChannel();
  void setRightChannel();
  void setStereoChannel();
  void setReverseStereoChannel();
  void setDolbysChannel();

  // - Members for initialization. -
private:
  void setupWindowStyle();
  void resetPlayer();
  void createComponents(bool unique);
  void createActions();
  void createMenus();
  void createSearchMenu();
  void createSearchEngines();
  void createDockWindows();
  void createConnections();
  void installEventFilters();
  void installLogger();
  void installPlayerLogger();

  // - Attributes -
private:
  bool disposed_;
  bool submit_;
  int liveInterval_; // TO BE REMOVED
  mutable QMutex inetMutex_;    // mutext for remote communication
  mutable QMutex annotMutex_;
  Preferences *preferences_;
  Tray *tray_;
  SignalHub *hub_;

  Magnifier *magnifier_;

  MediaLibrary *mediaLibrary_;
  GameLibrary *gameLibrary_;

  MouseClickEventListener *clickListener_;

  QList<SearchEngine *> searchEngines_;

  QxCountdownTimer *loadSubtitlesTimer_;
  QxCountdownTimer *resumePlayTimer_,
                        *resumeSubtitleTimer_,
                        *resumeAudioTrackTimer_,
                        *resumeAudioChannelTimer_,
                        *resumeAspectRatioTimer_;

  //QTimer *liveTimer_;
  QTimer *windowStaysOnTopTimer_;
  QTimer *navigationTimer_;
  QTimer *saveSettingsTimer_;
  //QTimer *autoHideCursorTimer_;
  QTimer *holdTimer_;
  QStringList annotationUrls_;
  QStringList annotationFiles_;

  //QTimer *windowStaysOnTopTimer_;

  QStringList recentFiles_;
  QHash<QString,QString> recentUrlTitles_;
  //QHash<QString,QString> recentGameEncodings_;
  QString browsedUrl_;
  QFileInfoList browsedFiles_;

  QString windowTitleSuffix_;

#ifdef AC_ENABLE_HONYAKU
  TranslatorManager *translator_, *extraTranslator_;
  AcTranslator *translatorDelegate_;
#endif // AC_ENABLE_HONYAKU

  AcPlayerServer *appServer_;
  AcBrowser *browserDelegate_;
  AcDownloader *downloaderDelegate_;
  AcUpdater *updaterDelegate_;
  AnnotationDownloader *annotationDownloader_;

  AnnotationServerAgent *server_;
  Updater *updater_;

  Player *player_;

#ifdef AC_ENABLE_MECAB
  MeCabParser *mecab_;
#endif // AC_ENABLE_MECAB

  AnnotationDatabase *cache_,
                     *queue_;
  DataManager *dataManager_;
  DataServer *dataServer_;

  ClipboardMonitor *clipboardMonitor_;
  MrlResolver *mrlResolver_;

  EventLogger *logger_;
  MainConsole *mainConsole_;
  MiniConsole *miniConsole_;

  //OsdDock *osdDock_;
  OsdWindow *osdWindow_;
  VideoView *videoView_;

  MediaLibraryView *libraryWindow_;
  MainLibraryView *libraryView_;

  MainPlayerUi *mainPlayer_;
  MiniPlayerUi *miniPlayer_;
  EmbeddedPlayerUi *embeddedPlayer_;
  EmbeddedCanvas *embeddedCanvas_;

  AnnotationGraphicsView *annotationView_;
  AnnotationAnalyticsView *annotationAnalyticsView_;
  UserAnalyticsView *userAnalyticsView_;
  AnnotationFilter *annotationFilter_;

  QxMouseRubberBand *pauseRubberBand_,
                    *resumeRubberBand_,
                    *removeRubberBand_;
  QxCircularRubberBand *attractRubberBand_,
                       *expelRubberBand_;

  //NetworkProxyDialog *networkProxyDialog_;
  //SiteAccountView *siteAccountView_;
  ConsoleDialog *consoleDialog_;
  AnnotationListView *annotationBrowser_;

  QPoint dragPos_,
         pressedPos_,
         movedPos_;
  Qt::MouseButtons pressedButtons_;
  qint64 windowModeUpdateTime_;

  qint32 tokenType_; // current token type tt

  Grabber *grabber_;

  QString recentOpenedFile_;
  QString recentPath_;
  QString recentDigest_;
  QString recentUrl_;

  bool recentUrlLocked_;

  //bool cursorVisible_;

  bool cancelContextMenu_;

  bool rippleEnabled_;
  RippleImageFilter *rippleFilter_;
  QTimer *rippleTimer_;

  QStringList playlist_;

  QHash<qint64, qint64> playPosHistory_;
  QHash<qint64, int> subtitleHistory_;
  QHash<qint64, int> audioTrackHistory_;
  QHash<qint64, int> audioChannelHistory_;
  QHash<qint64, QString> aspectRatioHistory_;

  int preferredSubtitleTrack_,
      preferredAudioTrack_,
      preferredAudioChannel_;

  FadeAnimation *fadeAni_;

  QWidgetList windows_;

  // - Menus and actions -
  QMenu *contextMenu_,
        *currentMenu_,
        *searchMenu_,
        *advancedMenu_,
        *helpContextMenu_,
        *recentMenu_,
        *userMenu_,
        *openMenu_,
        *importAnnotationMenu_,
        *openButtonMenu_,
        *playMenu_,
        *subtitleMenu_,
        *audioTrackMenu_,
        *audioChannelMenu_,
        *browseMenu_,
        *trackMenu_,
        *sectionMenu_,
        *backwardMenu_,
        *forwardMenu_,
        *videoMenu_,
        *audioMenu_,
        *appLanguageMenu_,
        *annotationEffectMenu_,
        *userLanguageMenu_,
        *settingsMenu_,
        *utilityMenu_,
        *closeMenu_,
        *annotationLanguageMenu_,
        *themeMenu_,
        *playlistMenu_,
        *subtitleColorMenu_;
  QList<QAction*> contextMenuActions_;

#ifdef AC_ENABLE_HONYAKU
  QMenu *translatorMenu_;
  QAction *toggleExciteTranslatorAct_,
          *toggleGoogleTranslatorAct_,
          *toggleMicrosoftTranslatorAct_,
          *toggleYahooTranslatorAct_,
          *toggleFresheyeTranslatorAct_,
          //*toggleOcnTranslatorAct_,
          *toggleRomajiTranslatorAct_,
          *toggleSdlTranslatorAct_,
          *toggleSystranTranslatorAct_,
          *toggleNiftyTranslatorAct_,
          *toggleInfoseekTranslatorAct_;
#endif // AC_ENABLE_HONYAKU

  QAction *showLibraryAct_;

  QMenu *annotationMenu_;
  QMenu *annotationSettingsMenu_;
  QAction *resumeAnnotationAct_,
          *increaseAnnotationScaleAct_,
          *decreaseAnnotationScaleAct_,
          *resetAnnotationScaleAct_,
          *increaseAnnotationOffsetAct_,
          *decreaseAnnotationOffsetAct_,
          *resetAnnotationOffsetAct_,
          *increaseAnnotationRotationAct_,
          *decreaseAnnotationRotationAct_,
          *resetAnnotationRotationAct_;

  QAction *contrastUpAct_, *contrastDownAct_, *contrastResetAct_,
          *hueUpAct_, *hueDownAct_, *hueResetAct_,
          *gammaUpAct_, *gammaDownAct_, *gammaResetAct_,
          *saturationUpAct_, *saturationDownAct_, *saturationResetAct_,
          *brightnessUpAct_, *brightnessDownAct_, *brightnessResetAct_;

  QAction *previousSectionAct_,
          *nextSectionAct_;
  QAction *previousFileAct_,
          *nextFileAct_,
          *moreFilesAct_,
          *lessFilesAct_,
          *openBrowsedDirectoryAct_;

  QAction *checkInternetConnectionAct_;
  QAction *deleteCachesAct_;

  QAction *setSubtitleColorToDefaultAct_,
          *setSubtitleColorToWhiteAct_,
          *setSubtitleColorToCyanAct_,
          *setSubtitleColorToBlueAct_,
          *setSubtitleColorToRedAct_,
          *setSubtitleColorToPurpleAct_,
          *setSubtitleColorToOrangeAct_,
          *setSubtitleColorToBlackAct_;

  QMenu *aspectRatioMenu_;
  QAction *setDefaultAspectRatioAct_,
          *setStandardAspectRatioAct_,
          *setWideScreenAspectRatioAct_;

  QAction *setAnnotationEffectToDefaultAct_,
          *setAnnotationEffectToTransparentAct_,
          *setAnnotationEffectToShadowAct_,
          *setAnnotationEffectToBlurAct_;

  QAction *newWindowAct_;

  QAction *openAct_,
          *openFileAct_,
          *openUrlAct_,
          *openAnnotationUrlAct_,
          *downloadAnnotationsAct_,
          *openAnnotationFileAct_,
          *openDeviceAct_,
          *openVideoDeviceAct_,
          *openAudioDeviceAct_,
          *openSubtitleAct_,
          *playAct_,
          *pauseAct_,
          *stopAct_,
          *promptStopAct_,
          *replayAct_,
          *nextFrameAct_,
          *previousAct_,
          *nextAct_,
          *snapshotAct_,
          *snapshotAllAct_,
          *actualSizeAct_,
          *toggleMenuBarVisibleAct_,
          *toggleAnnotationVisibleAct_,
          *toggleAnnotationAvatarVisibleAct_,
          *toggleAnnotationMetaVisibleAct_,
          //*toggleAnnotationCountDialogVisibleAct_,
          *toggleSubtitleVisibleAct_,
          *toggleSubtitleAnnotationVisibleAct_,
          *toggleNonSubtitleAnnotationVisibleAct_,
          *togglePreferFloatAnnotationAct_,
          *toggleSaveAnnotationFileAct_,
          *toggleAnnotationBandwidthLimitedAct_,
          *toggleClipboardMonitorEnabledAct_,
          *toggleFullScreenModeAct_,
          *toggleEmbeddedModeAct_,
          *toggleMiniModeAct_,
          *toggleLiveModeAct_,
          *toggleSyncModeAct_,
          *toggleTranslateAct_,
          *toggleSubtitleOnTopAct_,
          *toggleEmbeddedPlayerOnTopAct_,
          *toggleAutoPlayNextAct_,
          *toggleRepeatCurrentAct_,
          *toggleAnnotationTraditionalChineseAct_,
          *toggleMagnifierVisibleAct_,
          //*toggleSiteAccountViewVisibleAct_,
          *toggleAnnotationFilterEnabledAct_,
          *toggleNoRepeatAct_;

  QAction *nothingAfterFinishedAct_,
          *sleepAfterFinishedAct_,
          *shutdownAfterFinishedAct_;

  QAction *toggleWindowOnTopAct_;
  QAction *togglePlayerLabelEnabled_;
  QAction *downloadCurrentUrlAct_,
          *copyCurrentUrlAct_,
          *copyCurrentTitleAct_,
          *openInWebBrowserAct_,
          *openHomePageAct_;

  QAction *updateAnnotationsAct_;

  QAction *loginAct_,
          *logoutAct_,
          *connectAct_,
          *disconnectAct_;

  QAction *clearCacheAct_;

  QMenu   *gameMenu_;
  QAction *showAnnotationBrowserAct_,
          *showAnnotationEditorAct_,
          *showAnnotationSourceAct_,
          *showAnnotationAnalyticsAct_,
          *showLoginAct_,
          *showWindowPickerAct_,
          *showProcessAct_,
          *showSeekAct_,
          *showBlacklistAct_,
          *showMediaInfoAct_,
          *showUserAct_,
          *showBacklogAct_,
          *showDownloaderAct_,
          *showConsoleAct_;
          //*toggleCommentViewVisibleAct_,
  QAction *toggleWindowTrackingAct_;
  WId lastTrackedWindow_;

  QAction *toggleUserAnonymousAct_;
  QAction *openProxyBrowserAct_;
#ifdef AC_ENABLE_HONYAKU
  QAction *showTranslatorAct_;
#endif // AC_ENABLE_HONYAKU

  QAction *forward5sAct_,
          *backward5sAct_,
          *forward10sAct_,
          *backward10sAct_,
          *forward25sAct_,
          *backward30sAct_,
          *forward60sAct_,
          *backward60sAct_,
          *forward85sAct_,
          *backward90sAct_,
          *forward1mAct_,
          *backward1mAct_,
          *forward5mAct_,
          *backward5mAct_,
          *forward10mAct_,
          *backward10mAct_;

  QAction *clearRecentAct_,
          *validateRecentAct_;
  QAction *clearPlaylistAct_,
          *nextPlaylistItemAct_,
          *previousPlaylistItemAct_;

  QAction *setAppLanguageToEnglishAct_,
          *setAppLanguageToJapaneseAct_,
          *setAppLanguageToTraditionalChineseAct_,
          *setAppLanguageToSimplifiedChineseAct_;

  QAction *setUserLanguageToEnglishAct_,
          *setUserLanguageToJapaneseAct_,
          *setUserLanguageToTraditionalChineseAct_,
          *setUserLanguageToSimplifiedChineseAct_,
          *setUserLanguageToKoreanAct_,
          *setUserLanguageToFrenchAct_,
          *setUserLanguageToGermanAct_,
          *setUserLanguageToItalianAct_,
          *setUserLanguageToSpanishAct_,
          *setUserLanguageToPortugueseAct_,
          *setUserLanguageToRussianAct_;
          //*setUserLanguageToUnknownAct_;

  QAction *toggleAnnotationLanguageToEnglishAct_,
          *toggleAnnotationLanguageToJapaneseAct_,
          *toggleAnnotationLanguageToChineseAct_,
          *toggleAnnotationLanguageToKoreanAct_,
          *toggleAnnotationLanguageToFrenchAct_,
          *toggleAnnotationLanguageToGermanAct_,
          *toggleAnnotationLanguageToItalianAct_,
          *toggleAnnotationLanguageToSpanishAct_,
          *toggleAnnotationLanguageToPortugueseAct_,
          *toggleAnnotationLanguageToRussianAct_,
          //*toggleAnnotationLanguageToUnknownAct_,
          *toggleAnnotationLanguageToAnyAct_;

  QAction *toggleAeroEnabledAct_,
          *toggleMenuThemeEnabledAct_;

  QAction *toggleMultipleWindowsEnabledAct_;
  QAction *toggleSubmitAct_;
  QAction *toggleSaveMediaAct_,
          *saveMediaAct_,
          *togglePreferLocalDatabaseAct_;

  QAction *setThemeToDefaultAct_,
          *setThemeToRandomAct_,
          *setThemeToDarkAct_,
          *setThemeToBlackAct_,
          *setThemeToBlueAct_,
          *setThemeToBrownAct_,
          *setThemeToCyanAct_,
          *setThemeToGrayAct_,
          *setThemeToGreenAct_,
          *setThemeToPinkAct_,
          *setThemeToPurpleAct_,
          *setThemeToRedAct_,
          *setThemeToWhiteAct_,
          *setThemeToYellowAct_;


  QAction *showMaximizedAct_,
          *showMinimizedAct_,
          *showNormalAct_,
          *helpAct_,
          *updateAct_,
          *aboutAct_,
          *preferencesAct_,
          *quitAct_;

  QAction *setLeftChannelAct_,
          *setRightChannelAct_,
          *setStereoChannelAct_,
          *setReverseStereoChannelAct_,
          *setDolbysChannelAct_;

  QAction *setAudioDelayAct_;
};

#endif // MAINWINDOW_H
