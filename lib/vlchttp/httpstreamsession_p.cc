// httpstreamsession_p.cc
// 3/14/2012
//
// QNetworkAccessManager is not shared, because of 6 requests limits, as follows:
// Note: http://qt-project.org/doc/qt-4.8/QNetworkAccessManager.html
// QNetworkAccessManager queues the requests it receives. The number of requests executed in parallel is dependent on the protocol. Currently, for the HTTP protocol on desktop platforms, 6 requests are executed in parallel for one host/port combination.

#include "lib/vlchttp/httpstreamsession_p.h"
#ifdef WITH_LIB_STREAM
# include "lib/stream/bufferedremotestream.h"
# include "lib/stream/securebufferedfifostream.h"
#else
# error "require lib stream"
#endif // WITH_LIB_STREAM
#ifdef WITH_LIB_MEDIACODEC
# include "lib/mediacodec/flvcodec.h"
# include "lib/mediacodec/flvmerge.h"
#else
# error "require lib mediacodec"
#endif // WITH_LIB_MEDIACODEC
#ifdef WITH_LIB_MRLRESOLVER
# include "lib/mrlresolver/luamrlresolver.h"
#endif // WITH_LIB_LUARESOLVER
#include "qtx/qxnetwork.h"
#include "qtx/qxfs.h"
#include "htmlutil/htmltags.h"
#include "qtx/qxos.h"
#include <QtCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#define DEBUG "httpstreamsession"
#include "qtx/qxdebug.h"

enum { MaxDownloadRetries = 10, MinDownloadRetries = 2 };
enum { MaxIndividualDownloadRetries = 3 };
enum { BufferingInterval = 3000 }; // 3 second

// - Progress -

namespace { namespace detail {

  class ProgressTask : public QxStoppableTask
  {
    enum { SleepInterval = 1000 }; // wake up every 1 second

    HttpStreamSession *session_;
    bool stop_;
  public:
    explicit ProgressTask(HttpStreamSession *session)
      : session_(session), stop_(false) { Q_ASSERT(session_); }

    void stop() override
    { stop_ = true; }

    void run() override
    {
      while (!stop_ && session_->isRunning()) {
        session_->updateProgress();
        qxSleep(SleepInterval);
      }
    }
  };

  QxSleepTimer sleep;

} } // anonymous detail

// - Construction -

HttpStreamSession::~HttpStreamSession()
{
  if (progressTask_)
    progressTask_->stop();
  if (!ins_.isEmpty())
     // revert list so that nam will be deleted later
    for (auto p = ins_.constEnd(); p != ins_.constBegin();
         delete *--p);

  if (fifo_)
    delete fifo_;
  if (merger_)
    delete merger_;

  //foreach (QObject *obj, deleteLater_)
  //  delete obj;

  //if (nam_)
  //  delete nam_;
}

// - Properties -

qint64
HttpStreamSession::availableDuration() const
{ return merger_ ? merger_->timestamp() : 0; }

qint64
HttpStreamSession::receivedSize() const
{
  if (ins_.isEmpty())
    return 0;

  qint64 ret = 0;
  foreach (InputStream *in, ins_) {
    auto p = static_cast<BufferedRemoteStream *>(in);
    ret += p->availableSize();
  }
  return ret;
}

QString
HttpStreamSession::contentType() const
{
  if (ins_.isEmpty())
    return QString();

  auto in = static_cast<RemoteStream *>(ins_.first());
  Q_ASSERT(in);
  return in ? in->contentType() : QString();
}

void
HttpStreamSession::updateSize()
{
  if (!fifo_ || ins_.isEmpty())
    return;
  qint64 size = 0;
  foreach (InputStream *in, ins_) {
    auto p = static_cast<RemoteStream *>(in);
    size += p->size();
  }

  if (size)
    fifo_->setSize(size);
  DOUT("size =" << size);
}

void
HttpStreamSession::updateFileName()
{
  //bool mp4 = contentType().contains("mp4", Qt::CaseInsensitive);
  QString suf = ".flv";
  fileName_ = cacheDirectory() + QDir::separator() + qxEscapeFileName(mediaTitle()) + suf;
}

// - Actions -

qint64
HttpStreamSession::read(char *data, qint64 maxSize)
{
  if (!fifo_)
    return 0;
  qint64 ret = fifo_->tryRead(data, maxSize);
  if (ret)
    return ret;
  emit buffering();
  if (isRunning()) {
    //detail::sleep.start(BufferingInterval);
    //if (isRunning())
      return fifo_->read(data, maxSize);
  }
  return 0;
}

void
HttpStreamSession::save()
{
  DOUT("enter");
  if (!isFinished()) {
    emit errorMessage(tr("downloading"));
    DOUT("exit: not finished");
    return;
  }
  if (fileName_.isEmpty() || !fifo_ ||
      fifo_->availableSize() < MinimumFileSize) {
    DOUT("exit: empty fileName or fifo");
    return;
  }

  QFileInfo fi(fileName_);
  QDir dir = fi.absoluteDir();
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  //for (int i = 2; QFile::exists(fileName_); i++)
  //  fileName_ = fi.absolutePath() + QDir::separator() + fi.completeBaseName() + " " + QString::number(i) + "." + fi.suffix();
  qxTrashOrRemoveFile(fileName_);

  bool ok = fifo_->writeFile(fileName_);
  if (!ok || !FlvCodec::isFlvFile(fileName_)) {
    //QFile::remove(fileName_);
    qxTrashOrRemoveFile(fileName_);
    emit errorMessage(tr("download failed") + ": " + fileName_);
    DOUT("exit: fifo failed to write to file");
    return;
  }

  ok = FlvCodec::updateFlvFileMeta(fileName_);
  if (!ok)
    DOUT("warning: failed to update FLV meta:" << fileName_);

  emit message(tr("file saved") + ": " + fileName_);
  emit fileSaved(fileName_);
  DOUT("exit");
}

void
HttpStreamSession::finish()
{
  DOUT("enter");
  qint64 size = 0;
  if (fifo_) {
    fifo_->finish();
    size = fifo_->size();
  }

  if (isRunning()) {
    setState(Finished);
    if (isBufferSaved())
      save();
  }

  //if (reply_ && reply_->isRunning())
  //  reply_->abort();

  emit progress(size, size);

  stoppedCond_.wakeAll();
  DOUT("exit");
}

void
HttpStreamSession::stop()
{
  DOUT("enter");
  setState(Stopped);

  if (progressTask_)
    progressTask_->stop();

  foreach (InputStream *in, ins_) {
    auto p = static_cast<RemoteStream *>(in);
    p->stop();
  }

  detail::sleep.stop();

  emit stopped();
  //quit();
  DOUT("exit");
}

void
HttpStreamSession::waitForReady()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (!ready_)
    readyCond_.wait(&m_);
  DOUT("exit");
}

void
HttpStreamSession::waitForStopped()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (isRunning())
    stoppedCond_.wait(&m_);
  DOUT("exit");
}

void
HttpStreamSession::run()
{
  DOUT("enter: urls.size =" << urls_.size() << ", urls =" << urls_);
  Q_ASSERT(!urls_.isEmpty());
  if (urls_.isEmpty()) {
    DOUT("exit: empty urls");
    return;
  }
  setState(Running);
  ready_ = false;
  //progress_.clear();
  updateFileName();

  if (merger_) {
    DOUT("delete last merger");
    delete merger_;
    merger_ = 0;
  }
  if (fifo_) {
    DOUT("delete last fifo");
    delete fifo_;
    fifo_ = 0;
  }
  if (!ins_.isEmpty()) {
    DOUT("delete last ins");
    // revert list so that nam will be deleted later
    for (auto p = ins_.constEnd(); p != ins_.constBegin();
         delete *--p);
    ins_.clear();
  }

  if (mediaTitle().isEmpty())
    setMediaTitle(QDateTime::currentDateTime().toString(Qt::ISODate));

  //if (nam_)
  //  nam_->deleteLater();

  //nam_ = new QNetworkAccessManager;

  //if (cookieJar()) {
  //  nam_->setCookieJar(cookieJar());
  //  cookieJar()->setParent(nullptr);
  //}

  bool ok = false;
  int count = 0;
  QNetworkAccessManager *nam = nullptr;
  int namCount = 0;
  for (int j = 0; j < MaxDownloadRetries && !ok && !isStopped(); j++) {
    DOUT("total retry =" << j);
    //foreach (const QUrl &url, urls_) {
#ifdef WITH_LIB_MRLRESOLVER
    if (j && !originalUrl().isEmpty()) {
      DOUT("retry resolving mrl, orignal url =" << originalUrl());
      count--;
      emit warning(
        QString("%1 (%2/%3): ")
        .arg(tr("failed to fetch part %1, retry").arg(QString::number(count+1)))
        .arg(QString::number(j+1))
        .arg(QString::number(MaxDownloadRetries))
        + originalUrl()
      );

      if (j >= MinDownloadRetries) {
        enum { WaitInterval = 5 }; // wait 5 seconds
        int secs = WaitInterval + j - MinDownloadRetries;
        emit warning(tr("wait %1 seconds and try again").arg(QString::number(secs)) + " ...");
        DOUT(QString("wait for %1 seconds").arg(QString::number(secs)));
        detail::sleep.start(secs);
      }
      if (isStopped()) {
        DOUT("stopped, break");
        break;
      }

      DOUT("with mrlresolver");
      LuaMrlResolver resolver_impl;
      MrlResolver &resolver = resolver_impl;
      resolver.setSynchronized(true);
      //connect(resolver, SIGNAL(mediaResolved(MediaInfo,QNetworkCookieJar*)), SLOT(updateUrls(MediaInfo,QNetworkCookieJar*)));
      resolver.resolveMedia(originalUrl());
      const MediaInfo &mi = resolver.resolvedMediaInfo();
      QNetworkCookieJar *jar = resolver.resolvedCookieJar();
      if (jar) {
        setCookieJar(jar);
        //nam_->setCookieJar(jar);
        jar->setParent(nullptr); // memory leak
      }
      nam = nullptr; // reset nam

      urls_.clear();
      bool updateDuration = !duration_;
      foreach (const MrlInfo &i, mi.mrls) {
        urls_.append(i.url);
        if (updateDuration)
          duration_ += i.duration;
      }
    }
#endif // WITH_LIB_MRLRESOLVER
    ok = true;
    for (; count < urls_.size() && ok && !isStopped(); count++) {
      const QUrl url = urls_[count];
      //if (isStopped()) {
      //  foreach (InputStream *is, QxrevertList(ins_)) { // revert list so that nam will be deleted later
      //    auto p = static_cast<RemoteStream *>(is);
      //    p->stop();
      //    delete p;
      //  }
      //  ins_.clear();
      //  DOUT("WARNING: failed to download url:" << url.toString());
      //  break;
      //}
      emit message(
        (tr("preparing") + HTML_SS_OPEN(color:orange) " %1/%2: " HTML_SS_CLOSE())
          .arg(QString::number(count+1))
          .arg(QString::number(urls_.size()))
        + url.toString()
      );
      DOUT("part =" << count);

      ok = false;
      for (int i = 0; i < MaxIndividualDownloadRetries && !ok && !isStopped(); i++) {
        DOUT("individual retry =" << i);

        RemoteStream *in = new BufferedRemoteStream(nullptr);
        if (!nam || count % QxNetwork::MaxConcurrentNetworkRequestCount == namCount) {
          if (!nam)
            namCount = count % QxNetwork::MaxConcurrentNetworkRequestCount;
          nam = new QNetworkAccessManager(in);
          if (cookieJar()) {
            nam->setCookieJar(cookieJar());
            cookieJar()->setParent(nullptr);
          }
        }
        Q_ASSERT(nam);
        in->setNetworkAccessManager(nam);
        in->setUrl(url);
        switch (count) {
        case 0: in->request().setPriority(QNetworkRequest::HighPriority); break;
        case 1: in->request().setPriority(QNetworkRequest::NormalPriority); break;
        default: ;
        }
        in->run();
        ins_.append(in);
        DOUT("wait for ready, i =" << i << ", count =" << count << ", j =" << j);
        if (isStopped())
          break;
        in->waitForReady();
        DOUT("ready, i =" << i << ", count =" << count << ", j =" << j);
        QString type = in->contentType();
        ok = isMultiMediaMimeType(type);
        if (!ok) {
          DOUT("invalid contentType =" << type << ", url =" << url.toString());
          ins_.removeLast();
          in->stop();
          //in->deleteLater();
          DOUT("deleting remote stream");
          if (nam->parent() == in)
            nam = nullptr;
          delete in;
          DOUT("remote stream deleted");
          emit warning(
            QString("%1 (%2/%3): ")
            .arg(tr("failed to fetch HTTP header, retry"))
            .arg(QString::number(i+1))
            .arg(QString::number(MaxIndividualDownloadRetries))
            + url.toString()
          );
        }
      }
    }
  }
  DOUT("stream count =" << ins_.size());

  //if (reply_->error() != QNetworkReply::NoError) {
  //  setState(Error);
  //  ready_ = true;
  //  readyCond_.wakeAll();
  //  stoppedCond_.wakeAll();
  //  emit errorMessage(tr("network error to access URL") + ": " + url_.toString());
  //  DOUT("exit: network error");
  //  return;
  //}

  if (!ok || isStopped()) {
    // revert list so that nam will be deleted later
    for (auto p = ins_.constEnd(); p != ins_.constBegin();) {
      auto in = static_cast<RemoteStream *>(*--p);
      in->stop();
      delete in;
    }
    ins_.clear();
    DOUT("WARNING: failed to download url:");
  }

 if (ins_.isEmpty()) {
    DOUT("access forbidden");
    setState(Error);
    emit errorMessage(tr("access forbidden") + ": " + urls_.first().toString());

    //if (nam_) {
    //  nam_->deleteLater();
    //  nam_ = 0;
    //}

    ready_ = true;
    readyCond_.wakeAll();
    stoppedCond_.wakeAll();
    DOUT("exit: error caused by invalid content type");
    return;
  }

  DOUT("create fifo");
  if (fifo_)
    delete fifo_;
  fifo_ = new SecureBufferedFifoStream;
  updateSize();

  updateProgress();

 DOUT("create merger");
  if (merger_)
    delete merger_;

  merger_ = new FlvMerge; {
    merger_->setInputStreams(ins_);
    merger_->setOutputStream(fifo_);
    merger_->setDuration(duration_);

    connect(merger_, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString)));
    connect(merger_, SIGNAL(errorMessage(QString)), SLOT(stop()));
    //connect(merger_, SIGNAL(timestampChanged(qint64)), SLOT(updateProgress()));
  }

  //QTimer progressTimer;
  //progressTimer.setInterval(0); // report every second
  //connect(&progressTimer, SIGNAL(timeout()), SLOT(updateProgress()), Qt::QueuedConnection);

  //progressTimer.start();
  //merger_->run();
  //progressTimer.stop();

  //ins_.reset();

  emit message(tr("buffering ..."));

 DOUT("parse stream");
  ok = merger_->parse();
  if (ok) {
    duration_ = merger_->duration();
    DOUT("duration =" << duration_);
  } else {
    setState(Error);
    emit errorMessage(tr("failed to parse FLV streams"));
  }

  ready_ = true;
  readyCond_.wakeAll();

  if (ok) {
    progressTask_ = new detail::ProgressTask(this);
    QThreadPool::globalInstance()->start(progressTask_);
  }

  if (ok) {
    ins_.reset();
    ok = merger_->merge();
    if (ok)
      merger_->finish();
    else {
      setState(Error);
      emit errorMessage(tr("failed to merge FLV streams"));
    }
  }

  if (progressTask_)
    progressTask_->stop();

  if (isRunning()) {
    finish();

    DOUT("exec after finished");
  } else
    emit tr("failed to process remote media");

  DOUT("exit");
}

// EOF

/*
// - Progress -

void
HttpStreamSession::updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id)
{
  progress_[id] = Progress(receivedBytes, totalBytes);

  receivedBytes = 0;
  totalBytes = 0;
  foreach (const Progress &p, progress_) {
    receivedBytes += p.receivedBytes;
    totalBytes += p.totalBytes;
  }
  emit progress(receivedBytes, totalBytes);
}
*/
