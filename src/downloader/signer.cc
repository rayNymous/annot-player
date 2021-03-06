// signer.cc
// 3/30/2012

#include "signer.h"
#include "src/common/acsettings.h"
#include "lib/annotcloud/alias.h"
#include "lib/annotcloud/token.h"
#include "lib/annotcloud/traits.h"
#include "lib/annotcloud/user.h"
#include "lib/annotservice/annotserveragent.h"
#include <QtCore>

using namespace AnnotCloud;

#define DEBUG "signer"
#include "qtx/qxdebug.h"

// - Construction -

Signer::Signer(QObject *parent)
  : Base(parent), disposed_(false)
{ server_ = new AnnotationServerAgent(this); }

// - Sign -

void
Signer::signFileWithUrl(const QString &fileName, const QString &url, bool async)
{
  DOUT("enter: async =" << async << ", fileName =" << fileName << ", url =" << url);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (url.isEmpty() || url.size() > Traits::MAX_ALIAS_LENGTH) {
    emit warning(tr("URL is too long") + ": " + url);
    return;
  }
  if (async) {
    emit message(tr("signing media ...") + " " + fileName);
    QtConcurrent::run(this, &Self::signFileWithUrl, fileName, url, false);
    DOUT("exit: returned from async branch");
    return;
  }
  AcSettings::globalSettings()->sync();
  if (!server_->isConnected() || !server_->isAuthorized() ||
      server_->user().name() != AcSettings::globalSettings()->userName()) {
    login(false); // async = false
    if (!server_->isConnected() || !server_->isAuthorized()) {
      emit warning(tr("user didn't login, skip signing file") + ": " + url);
      return;
    }
  }

  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }

  QString digest = digestForFile(fileName);
  if (digest.isEmpty()) {
    emit warning(tr("failed to analyze media") + ": " + fileName);
    DOUT("exit: failed to compute file digest");
    return;
  }

  qint64 now = QDateTime::currentMSecsSinceEpoch() / 1000;

  Token token; {
    token.setUserId(server_->user().id());
    token.setCreateTime(now);
    token.setDigest(digest);
    token.setType(Token::TT_Video);
  }

  Alias srcAlias;
  QString name = QFileInfo(fileName).fileName();
  if (!name.isEmpty()) {
    if (name.size() > Traits::MAX_ALIAS_LENGTH) {
      name = name.left(Traits::MAX_ALIAS_LENGTH);
      emit warning(tr("truncate long file name") + ": " + name);
    }
    srcAlias.setUserId(server_->user().id());
    srcAlias.setType(Alias::AT_File);
    srcAlias.setLanguage(server_->user().language());
    srcAlias.setText(name);
    srcAlias.setUpdateTime(now);
  }

  Alias urlAlias; {
    urlAlias.setType(Alias::AT_Url);
    urlAlias.setUserId(server_->user().id());
    urlAlias.setLanguage(Alias::guessUrlLanguage(url, server_->user().language()));
    urlAlias.setText(url);
    urlAlias.setUpdateTime(now);
  }

  DOUT("mutex locking"); m_.lock(); DOUT("mutex locked");

  emit message(tr("saving aliases ..."));
  qint64 tid = server_->submitToken(token);
  if (tid) {
    urlAlias.setTokenId(tid);
    if (srcAlias.hasText())
      srcAlias.setTokenId(tid);
    server_->submitAlias(urlAlias);
    server_->submitAlias(srcAlias);
  }

  DOUT("mutex unlocking"); m_.unlock(); DOUT("mutex unlocked");

  if (tid)
    emit message(tr("media signed") + ": " + fileName);
  else
    emit errorMessage(tr("failed to sign media") + ": " + fileName);

  DOUT("exit");
}

QString
Signer::digestForFile(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  QString digest = Token::digestFromFile(fileName);
  if (digest.isEmpty()) {
    QString localPath = QUrl(fileName).toLocalFile();
    QString decodedLocalPath = QUrl::fromPercentEncoding(localPath.toAscii());
    digest = Token::digestFromFile(decodedLocalPath);
  }
  DOUT("exit: ret =" << digest);
  return digest;
}

// - Login -

void
Signer::login(bool async)
{
  DOUT("enter: async =" << async);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (async) {
    QtConcurrent::run(this, &Self::login, false);
    DOUT("exit: returned from async branch");
    return;
  }

  AcSettings *settings = AcSettings::globalSettings();
  settings->sync();
  QString userName = settings->userName(),
          password = settings->password();
  if (userName.isEmpty() || password.isEmpty()) {
    userName = AnnotCloud::User::guest().name();
    password = AnnotCloud::User::guest().password();
    //emit warning(tr("login aborted, missing username or password"));
    //return;
  }

  login(userName, password);
  DOUT("exit");
}

void
Signer::login(const QString &userName, const QString &encryptedPassword)
{
  DOUT("enter: userName =" << userName);
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }

  QMutexLocker lock(&m_);

  emit message(tr("logging in as %1 ...").arg(userName));

  server_->updateConnected();
  if (disposed_) {
    DOUT("exit: returned from disposed branch");
    return;
  }
  if (!server_->isConnected()) {
    emit tr("failed to connect to server");
    DOUT("exit: not connected");
    return;
  }

  server_->login(userName, encryptedPassword);
  if (server_->isAuthorized())
    emit message(tr("login succeeded as %1").arg(userName));
  else
    emit warning(tr("failed to login as %1").arg(userName));
  DOUT("exit");
}

// EOF
