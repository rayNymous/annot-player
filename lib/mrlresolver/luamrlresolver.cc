// luamrlresolver.cc
// 2/2/2012

#include "lib/mrlresolver/luamrlresolver.h"
#include "lib/mrlresolver/mrlresolversettings.h"
#include "htmlutil/htmltags.h"
#ifdef WITH_LIB_LUARESOLVER
# include "lib/luaresolver/luaresolver.h"
#else
# error "require lib luaresolver"
#endif // WITH_LIB_LUARESOLVER
#ifdef WITH_LIB_MRLANALYSIS
# include "lib/mrlanalysis/mrlanalysis.h"
#else
# error "require lib mrlanalysis"
#endif // WITH_LIB_MRLANALYSIS
#ifdef WITH_LIB_DOWNLOAD
# include "lib/download/download.h"
#else
# error "require lib download"
#endif // WITH_LIB_MRLANALYSIS
#include <QtNetwork/QNetworkCookieJar>
#include <QtCore/QCoreApplication>
#include <QtCore/QTextCodec>
#include <QtCore/QtConcurrentRun>

#define DEBUG "luaemrlresolver"
#include "qtx/qxdebug.h"

// TODO: move to project source project instead of hard code here
#ifdef Q_OS_WIN
# define LUA_PATH "lua/luascript" // omit applicationDirPath, which might contains non-ascii chars and cannot handled by liblua
#elif defined(Q_OS_MAC)
# define LUA_PATH QCoreApplication::applicationDirPath() + "/lua"
#elif defined(Q_OS_LINUX)
# define LUA_PATH LUADIR
#endif // Q_OS_

#ifdef LUA_PATH
# define LUAPACKAGE_PATH LUA_PATH "/?.lua"
# define LUASCRIPT_PATH  LUA_PATH "/luascript.lua"
#else
# define LUAPACKAGE_PATH ""
# define LUASCRIPT_PATH  "luascript.lua"
#endif // LUA_PATH

// - Analysis -

bool
LuaMrlResolver::matchMedia(const QString &href) const
{
  DOUT("enter");
  int site;
  bool ret = (site = MrlAnalysis::matchSite(href)) &&
              site < MrlAnalysis::ChineseVideoSite;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
LuaMrlResolver::matchSubtitle(const QString &href) const
{
  DOUT("enter");
  int site;
  bool ret = (site = MrlAnalysis::matchSite(href)) &&
              site < MrlAnalysis::AnnotationSite;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
LuaMrlResolver::resolveMedia(const QString &href, bool async)
{
  if (async) {
    if (!checkSiteAccount(href))
      return false;
    QtConcurrent::run(this, &Self::resolveMedia, href, false);
    return true;
  }
  if (!checkSiteAccount(href))
    return false;
  DOUT("enter: href =" << href);
  QString url = cleanUrl(href);
  DOUT("url =" << url);

  MrlResolverSettings *settings = MrlResolverSettings::globalSettings();

  //LuaResolver *lua = makeResolver();
  LuaResolver lua(LUASCRIPT_PATH, LUAPACKAGE_PATH);
  //lua.setNetworkAccessManager(networkAccessManager());
  //if (!lua.cookieJar())
  //  lua.setCookieJar(new QNetworkCookieJar);
  if (settings->hasNicovideoAccount())
    lua.setNicovideoAccount(settings->nicovideoAccount().username,
                            settings->nicovideoAccount().password);
  if (settings->hasBilibiliAccount())
    lua.setBilibiliAccount(settings->bilibiliAccount().username,
                           settings->bilibiliAccount().password);
  int siteid;
  QString refurl, title, suburl;
  QStringList mrls;
  QList<qint64> durations, sizes;
  bool ok = lua.resolve(url, &siteid, &refurl, &title, &suburl, &mrls, &durations, &sizes);
  if (!ok) {
    emit errorMessage(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: LuaResolver returned false");
    return false;
  }

  if (mrls.isEmpty()) {
    switch (siteid) {
    case LuaResolver::Nicovideo:
      emit errorMessage(tr("failed to resolve URL using nicovideo account") + ": " + url);
      break;
    case LuaResolver::Bilibili:
      emit errorMessage(tr("failed to resolve URL using bilibili account") + ": " + url);
      break;
    default:
      emit errorMessage(tr("failed to resolve URL") + ": " + url);
    }
    DOUT("exit: mrls is empty");
    return false;
  }

  MediaInfo mi;
  mi.suburl = formatUrl(suburl);
  mi.refurl = formatUrl(refurl);
  mi.title = formatTitle(title);
  //switch (siteid) {
  //case LuaResolver::Acfun: encoding = "GBK"; break;
  ////case LuaResolver::Bilibili: encoding = "UTF-8"; break;
  //default: encoding = 0;
  //}

  switch (mrls.size()) { // Specialization for better performance
  case 0: break;
  case 1:
    {
      MrlInfo m(formatUrl(mrls.first()));
      if (!durations.isEmpty())
        m.duration = durations.front();
      if (!sizes.isEmpty())
        m.size = sizes.first();
      mi.mrls.append(m);
    } break;

  default:
    for (int i = 0; i < mrls.size(); i++) {
      MrlInfo m(formatUrl(mrls[i]));
      if (durations.size() > i)
        m.duration = durations[i];
      if (sizes.size() > i)
        m.size = sizes[i];
      mi.mrls.append(m);
    }
  }

  //QNetworkCookieJar *jar = lua.cookieJar();
  //Q_ASSERT(jar);
  //if (jar)
  //  jar->setParent(nullptr);
  QNetworkCookieJar *jar = lua.cookieJar();
  if (jar)
    jar->setParent(nullptr);

  emit mediaResolved(mi, jar);
  if (isSynchronized()) {
    setResolvedCookieJar(jar);
    setResolvedMediaInfo(mi);
  }
  DOUT("exit: title =" << mi.title);
  return true;
}

bool
LuaMrlResolver::resolveSubtitle(const QString &href, bool async)
{
  if (async) {
    if (!checkSiteAccount(href))
      return false;
    QtConcurrent::run(this, &Self::resolveSubtitle, href, false);
    return true;
  }
  if (!checkSiteAccount(href))
    return false;
  DOUT("enter: href =" << href);
  QString url = cleanUrl(href);
  DOUT("url =" << url);

  MrlResolverSettings *settings = MrlResolverSettings::globalSettings();

  //LuaResolver *lua = makeResolver();
  LuaResolver lua(LUASCRIPT_PATH, LUAPACKAGE_PATH);
  //lua.setNetworkAccessManager(networkAccessManager());
  //lua.setCookieJar(new QNetworkCookieJar); // potential memory leak on error
  if (settings->hasNicovideoAccount())
    lua.setNicovideoAccount(settings->nicovideoAccount().username,
                            settings->nicovideoAccount().password);
  if (settings->hasBilibiliAccount())
    lua.setBilibiliAccount(settings->bilibiliAccount().username,
                           settings->bilibiliAccount().password);

  // LuaResolver::resolve(const QString &href, QString *refurl, QString *title, QString *suburl, QStringList *mrls) const
  int siteid;
  QString suburl, title, refurl;
  bool ok = lua.resolve(url, &siteid, &refurl, &title, &suburl);

  if (!ok) {
    emit errorMessage(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: LuaResolver returned false");
    return false;
  }

  if (suburl.isEmpty()) {
    switch (siteid) {
    case LuaResolver::Nicovideo:
      emit errorMessage(tr("failed to resolve URL using nicovideo account") + ": " + url);
      break;
    case LuaResolver::Bilibili:
      emit errorMessage(tr("failed to resolve URL using bilibili account") + ": " + url);
      break;
    default:
      emit errorMessage(tr("failed to resolve URL") + ": " + url);
    }
    DOUT("exit: suburl is empty");
    return false;
  }

  suburl = formatUrl(suburl);
  refurl = formatUrl(refurl);
  title = formatTitle(title);
  if (!suburl.isEmpty())
    emit subtitleResolved(suburl, refurl, title);
  if (isSynchronized())
    setResolvedSubtitleUrl(suburl);
  DOUT("exit: suburl =" << suburl);
  return true;
}

// - Helper -

//LuaResolver*
//LuaMrlResolver::makeResolver(QObject *parent)
//{
//  LuaResolver *ret = new LuaResolver(LUASCRIPT_PATH, LUAPACKAGE_PATH, 0, parent);
//  if (hasNicovideoAccount())
//    ret->setNicovideoAccount(nicovideoUsername_, nicovideoPassword_);
//  if (hasBilibiliAccount())
//    ret->setBilibiliAccount(bilibiliUsername_, bilibiliPassword_);
//  return ret;
//}

QString
LuaMrlResolver::decodeText(const QString &text, const char *encoding)
{
  if (text.isEmpty())
    return QString();
  if (encoding) {
    QTextCodec *tc = QTextCodec::codecForName(encoding);
    if (tc) {
      QTextDecoder *dc = tc->makeDecoder();
      if (dc)
        return dc->toUnicode(text.toLocal8Bit());
    }
  }
  return text;
}

QString
LuaMrlResolver::formatTitle(const QString &title)
{
  return title.isEmpty() ? title :
         title == "ニコニコ動画:Zero" ? QString() :
    ::htmlUnescape(title)
    .remove(QRegExp("..ニコニコ動画\\(原宿\\)$"))
    .remove(QRegExp("..ニコニコ動画:Zero$"))
    .remove(QRegExp(" - 嗶哩嗶哩 - .*"))
    .remove(QRegExp(" - AcFun.*"))
     // Youku
#ifdef _MSC_VER
    .remove(QRegExp(" - \xe7\x94\xb5\xe8\xa7\x86\xe5\x89\xa7 - .*"))
    .remove(QRegExp(" - \xe8\xa7\x86\xe9\xa2\x91 - .*"))
    .remove(QRegExp(" - \xe8\xa7\x86\xe9\xa2\x91$"))
    .remove(QRegExp(" - ..\xe8\xa7\x86\xe9\xa2\x91 - .*"))
    .remove(QRegExp(" - \xe5\x8a\xa8\xe6\xbc\xab$"))
    .remove(QRegExp("—\xe5\x9c\xa8\xe7\xba\xbf\xe6\x92\xad\xe6\x94\xbe—.*"))
    .remove(QRegExp("_\xe5\x9c\xa8\xe7\xba\xbf.*"))
#else
    .remove(QRegExp(" - 电视剧 - .*"))
    .remove(QRegExp(" - 视频 - .*"))
    .remove(QRegExp(" - 视频$"))
    .remove(QRegExp(" - 优酷视频 - .*")) // Youku
    .remove(QRegExp(" - 动漫$")) // Youku
    .remove(QRegExp("—在线播放—.*")) // Youku
    .remove(QRegExp("_在线.*")) // Tudou
#endif // _MSC_VER
    .remove(QRegExp(" - 20[01]\\d$")) // Youku
    .remove(QRegExp(" - 19[89]\\d$")) // Youku
     .simplified();
}

QString
LuaMrlResolver::formatUrl(const QString &href)
{ return MrlAnalysis::normalizeUrl(href); }

QString
LuaMrlResolver::cleanUrl(const QString &url)
{
  QString ret = url.trimmed();
  ret.replace("http://acfun.tv/" , "http://www.acfun.tv", Qt::CaseInsensitive)
     .replace("http://bilibili.com/" , "http://www.bilibili.com", Qt::CaseInsensitive);
  if (ret.startsWith("http://www.nicovideo.jp/watch/", Qt::CaseInsensitive)) {
    ret.remove(QRegExp("\\?.*"));
    if (ret.startsWith("http://www.nicovideo.jp/watch/so", Qt::CaseInsensitive)) {
      DOUT("try redirect: url =" << url);
      QUrl r = ::dlredirect(url);
      if (!r.isEmpty())
        ret = r.toString();
    }
  }
  return formatUrl(ret);
}

bool
LuaMrlResolver::checkSiteAccount(const QString &href)
{
  DOUT("enter");
  if (href.contains("nicovideo.jp/", Qt::CaseInsensitive)
      && !MrlResolverSettings::globalSettings()->hasNicovideoAccount()) {
    emit errorMessage(tr("nicovideo.jp account is required to resolve URL") + ": " + href);
    DOUT("exit: ret = false, nico account required");
    return false;
  }
  DOUT("exit: ret = true");
  return true;
}

// EOF
