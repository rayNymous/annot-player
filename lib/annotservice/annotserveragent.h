#ifndef ANNOTSERVERAGENT_H
#define ANNOTSERVERAGENT_H

// annotserveragent.h
// 9/10/2011
// Connect server side ServerAgentService.
// See: http://blog.csdn.net/tingsking18/article/details/5456831
//
// Note: since soap timeout is not slow, synchronized approach is used.
// TODO: Deal with offline case here??

#include "lib/annotcloud/alias.h"
#include "lib/annotcloud/annotation.h"
#include "lib/annotcloud/gamehook.h"
#include "lib/annotcloud/gamethread.h"
#include "lib/annotcloud/token.h"
#include "lib/annotcloud/user.h"
#include <QtCore/QMutex>

class AnnotationServerProxy;
class CastManager;

class AnnotationServerAgent: public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationServerAgent)
  typedef AnnotationServerAgent Self;
  typedef QObject Base;

  typedef AnnotCloud::User User;
  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;
  typedef AnnotCloud::GameHook GameHook;
  typedef AnnotCloud::GameHookList GameHookList;
  typedef AnnotCloud::GameThread GameThread;
  typedef AnnotCloud::GameThreadList GameThreadList;

public:
  explicit AnnotationServerAgent(QObject *parent = nullptr);

  // - Login -
public:
  ///  If is logged in as an authorized user
  bool isAuthorized() const { return authorized_; }
  bool isConnected() const { return connected_; }

  const User &user() const; ///< Logged in user
  User &user(); ///< Used for hacking offline mode
  void setUser(const User &user); ///< \internal

signals:
  void userChanged();

  void loginRequested(const QString &userName);
  void loginSucceeded(const QString &userName);
  void loginFailed(const QString &userName);

  void logoutRequested();
  void logoutFinished();

  // Error signals:
  void connected(); ///< succeeded
  void connectedChanged(bool t);
  void connectionError();
  void error404();
  void serverError();
  void unknownError();

  // - Connections -
public slots:
  bool login(const QString &userName, const QString &passwordDigest);
  bool login(); // Login using last userName and password
  void logout();

  void setConnected(bool yes = true); ///< forcely set connection flag
  void setDisconnected(bool yes = true) { return setConnected(!yes); }
  void setAuthorized(bool yes = true); ///< forcely set authorization flag
  void updateConnected();
  void updateAuthorized();

  void disconnect(); ///< set connected to false

  void reportSoapError(int err); ///< \internal

  bool isSoftwareUpdated();
  static QString version();

  // - User -
public:
  bool setUserAnonymous(bool t = true);
  bool setUserLanguage(qint32 language);

  // - Submissions -
public:
  qint64 submitToken(const Token &token);
  bool submitTokens(const TokenList &tokens);
  qint64 submitAlias(const Alias &alias);
  bool submitAliases(const AliasList &tokens);
  qint64 submitTokenAndAlias(const Token &token, const Alias &alias); ///< return token id only

  qint64 submitAnnotation(const Annotation &annot);
  bool submitAnnotations(const AnnotationList &annots);

  qint64 submitGameHook(const GameHook &hooks);
  bool submitGameHooks(const GameHookList &hooks);

  qint64 submitGameThread(const GameThread &thread);
  bool submitGameThreads(const GameThreadList &threads);

  // - Update -
public:
  bool updateAnnotationTextWithId(const QString &text, qint64 id);
  bool updateAnnotationUserIdWithId(qint64 userId, qint64 id);
  bool updateGameHookTextWithId(const QString &text, qint64 id);
  bool updateGameThread(const GameThread &thread);

  // - Queries -
public:
  qint64 selectTokenId(const Token &token);
  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 section);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid);
  AliasList selectAliasesWithTokenId(qint64 tid);

 ///  Clustered at server based on token's aliases
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid);
  AliasList selectRelatedAliasesWithTokenId(qint64 tid);

  GameHook selectGameHookWithTokenId(qint64 tid);
  GameHook selectGameHookWithTokenDigest(const QString &digest);

  GameThread selectGameThreadWithTokenId(qint64 tid);
  GameThread selectGameThreadWithTokenDigest(const QString &digest);

  // - Deletion -
public:
  bool deleteAnnotationWithId(qint64 id);
  bool deleteAliasWithId(qint64 id);

  // - Cast -
public:
  bool blessAnnotationWithId(qint64 id);
  bool curseAnnotationWithId(qint64 id);
  bool blockAnnotationWithId(qint64 id);

  bool blessAliasWithId(qint64 id);
  bool curseAliasWithId(qint64 id);
  bool blockAliasWithId(qint64 id);

  bool blessTokenWithId(qint64 id);
  bool curseTokenWithId(qint64 id);

  bool blessUserWithId(qint64 id);
  bool blockUserWithId(qint64 id);
  bool curseUserWithId(qint64 id);

  bool isAnnotationBlessedWithId(qint64 id) const;
  bool isAnnotationCursedWithId(qint64 id) const;
  bool isAnnotationBlockedWithId(qint64 id) const;

  bool isAliasBlessedWithId(qint64 id) const;
  bool isAliasCursedWithId(qint64 id) const;
  bool isAliasBlockedWithId(qint64 id) const;

  bool isTokenBlessedWithId(qint64 id) const;
  bool isTokenCursedWithId(qint64 id) const;

  bool isUserBlockedWithId(qint64 id) const;

  // - Live -
public:
  QString chat(const QString &message);

  //qint64 submitLiveAnnotation(const Annotation &annot);
  //qint64 submitLiveAnnotationText(const QString &text);

  //AnnotationList selectLiveAnnotations();

  //qint32 selectLiveTokenInterval();

  // - Implementations -
private:
  QMutex mutex_;
  AnnotationServerProxy *proxy_;
  CastManager *cast_;
  User user_;
  bool authorized_;
  bool connected_;
};

#endif // ANNOTSERVERAGENT_H
