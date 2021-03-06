#ifndef BRIGHTNESSIMAGEFILTER_H
#define BRIGHTNESSIMAGEFILTER_H

// brightnessimagefilter.h
// 5/17/2012
// See: http://stackoverflow.com/questions/4981474/brightness-contrast-gamma-adjustments-with-c-qt

#include "lib/imagefilter/pixelimagefilter.h"
#include <QtCore/QObject>

class BrightnessImageFilter : public QObject, public PixelImageFilter
{
  Q_OBJECT
  Q_PROPERTY(int value READ value WRITE setValue)
  Q_DISABLE_COPY(BrightnessImageFilter)
  typedef BrightnessImageFilter Self;
  typedef QObject Base;

  int brightness_;

public:
  enum { DefaultValue = 0 };

  explicit BrightnessImageFilter(QObject *parent = nullptr)
    : Base(parent), brightness_(DefaultValue) { }
  explicit BrightnessImageFilter(int brightness, QObject *parent = nullptr)
    : Base(parent), brightness_(brightness) { }

  int value() const { return brightness_; }
public slots:
  void setValue(int brightness) { brightness_ = brightness; }
  void clear() { setValue(DefaultValue); }

public:
  static quint8 filterColor(quint8 value, int brightness)
  { return qBound<int>(0, value + brightness * 255 / 100, 255); }

  static bool needsDisplay(int brightness)
  { return brightness != DefaultValue; }

  quint8 filterColor(quint8 value) const override
  { return filterColor(value, brightness_); }

  bool needsDisplay() const override
  { return needsDisplay(brightness_); }
};

#endif // BRIGHTNESSIMAGEFILTER_H
