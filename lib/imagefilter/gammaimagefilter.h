#ifndef GAMMAIMAGEFILTER_H
#define GAMMAIMAGEFILTER_H

// gammaimagefilter.h
// 5/17/2012
// See: http://stackoverflow.com/questions/4981474/brightness-contrast-gamma-adjustments-with-c-qt

#include "lib/imagefilter/pixelimagefilter.h"
#include <QtCore/QObject>
#include <QtCore/qmath.h>

class GammaImageFilter : public QObject, public PixelImageFilter
{
  Q_OBJECT
  Q_PROPERTY(int value READ value WRITE setValue)
  Q_DISABLE_COPY(GammaImageFilter)
  typedef GammaImageFilter Self;
  typedef QObject Base;

  int gamma_;

public:
  enum { DefaultValue = 100 };

  explicit GammaImageFilter(QObject *parent = nullptr)
    : Base(parent), gamma_(DefaultValue) { }
  explicit GammaImageFilter(int gamma, QObject *parent = nullptr)
    : Base(parent), gamma_(gamma) { }

  int value() const { return gamma_; }
public slots:
  void setValue(int gamma) { gamma_ = gamma; }
  void clear() { setValue(DefaultValue); }

public:
  static bool needsDisplay(int gamma)
  { return gamma != DefaultValue; }

  static quint8 filterColor(quint8 value, int gamma)
  {
    switch (value) {
    case 0:   return 0;
    case 255: return 255;
    default:  return gamma ? qBound<int>(0, qPow(value/255.0,100.0/gamma)*255, 255) : 0;
    }
  }

  bool needsDisplay() const override
  { return needsDisplay(gamma_); }

  quint8 filterColor(quint8 value) const override
  { return filterColor(value, gamma_); }
};

#endif // GAMMAIMAGEFILTER_H
