#ifndef ANNOTATIONLISTMODEL_H
#define ANNOTATIONLISTMODEL_H

// annotationlistmodel.h
// 10/23/2011

#include "lib/annotcloud/annotation.h"
#include <QtGui/QStandardItemModel>

class SignalHub;
class AnnotationListModel : public QStandardItemModel
{
  Q_OBJECT
  //Q_DISABLE_COPY(AnnotationListModel)
  typedef AnnotationListModel Self;
  typedef QStandardItemModel Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  SignalHub *hub_;

public:
  enum HeaderData {
    HD_Text = 0,
    HD_Pos,
    HD_UserAlias,
    HD_CreateTime,
    HD_UpdateTime,
    HD_Id,
    HD_Language,
    HD_Status,
    HD_UserId,
    HD_Flags,
    HD_BlessCount,
    HD_CurseCount,
    HD_BlockCount,
    HD_Count
  };

public:
  explicit AnnotationListModel(SignalHub *hub, QObject *parent = nullptr);

  // - Queries -
public:
  int rowById(qint64 aid) const;

  static qint64 indexId(const QModelIndex &index);
  static qint64 indexUserId(const QModelIndex &index);
  static QString indexText(const QModelIndex &index);
  static QString indexUserAlias(const QModelIndex &index);

  // - Properties -
public:
  bool isEmpty() const { return rowCount() == 0; }

  Qt::ItemFlags flags(const QModelIndex &index) const override;

public slots:
  void addAnnotation(const Annotation &annot);

  void updateAnnotationTextWithId(const QString &text, qint64 id);
  void updateAnnotationUserIdWithId(qint64 userId, qint64 id);

  void reset() { while (rowCount()) removeRow(0); }

protected:
  void resetHeader() { setHeaderData(this); }
  static void setHeaderData(QAbstractItemModel *model);

public:  // TO BE MOVED TO A COMMON PLACE
  static QString languageToString(int lang);
  static QStringList annotationFlagsToStringList(int flags);
  static QString annotationStatusToString(int flags);
};

#endif // ANNOTATIONLISTMODEL_H
