#ifndef PREFERENCES_P_H
#define PREFERENCES_P_H

// preferences_p.h
// 5/25/2012

#include "src/common/acpreferencestab.h"

QT_FORWARD_DECLARE_CLASS(QTextEdit)

class GesturePreferencesTab : public AcNullPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(GesturePreferencesTab)
  typedef GesturePreferencesTab Self;
  typedef AcNullPreferencesTab Base;

  QTextEdit *textEdit_;

public:
  explicit GesturePreferencesTab(QWidget *parent = nullptr);
private:
  void createLayout();
};

#endif // PREFERENCES_P_H
