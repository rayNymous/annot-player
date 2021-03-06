#ifndef ACDOWNLOADER_H
#define ACDOWNLOADER_H

// acdownloader.h
// 4/9/2012

#include "src/common/acipc.h"

class AcDownloaderServer : public AcIpcController
{
  Q_OBJECT
  Q_DISABLE_COPY(AcDownloaderServer)
  typedef AcDownloaderServer Self;
  typedef AcIpcController Base;

  static Self *global_;

  // - Construction -
public:
  explicit AcDownloaderServer(QObject *parent = nullptr);
  ~AcDownloaderServer() { stop(); }

public slots:
  void start();
  void stop();
};

class AcDownloader : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AcDownloader)
  typedef AcDownloader Self;
  typedef QObject Base;
  typedef AcIpcController Delegate;

  Delegate *delegate_;

public:
  explicit AcDownloader(QObject *parent = nullptr);

signals:
  void arguments(const QStringList &args);
  void showRequested();

public:
  bool isRunning() const;

public slots:
  void show();
  void open();
  void openArguments(const QStringList &args);
  void openUrls(const QStringList &urls) { openArguments(urls); }
  void openUrl(const QString &url) { openUrls(QStringList(url)); }
};

#endif // ACDOWNLOADER_H
