#include "checkboxbutton.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QCursor>
#include "commongraphics/commongraphics.h"
#include "graphicresources/imageresourcessvg.h"
#include "dpiscalemanager.h"

namespace PreferencesWindow {

CheckBoxButton::CheckBoxButton(ScalableGraphicsObject *parent) : ClickableGraphicsObject(parent),
    animationProgress_(0.0), isChecked_(false), enabled_(true)
{
    setAcceptHoverEvents(true);

    connect(&opacityAnimation_, &QVariantAnimation::valueChanged, this, &CheckBoxButton::onOpacityChanged);
    opacityAnimation_.setStartValue(0.0);
    opacityAnimation_.setEndValue(1.0);
    opacityAnimation_.setDuration(150);

    setClickable(true);
}

QRectF CheckBoxButton::boundingRect() const
{
    return QRectF(0, 0, 40*G_SCALE, 22*G_SCALE);
}

void CheckBoxButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setOpacity(OPACITY_FULL);
    if (!enabled_)
    {
        painter->setOpacity(OPACITY_HALF);
    }

    painter->setRenderHint(QPainter::Antialiasing);
    // At 100%, this would be R85 G255 B138
    QColor bgColor = QColor(255-170*animationProgress_, 255, 255-117*animationProgress_);
    painter->setBrush(bgColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(boundingRect().toRect(), TOGGLE_RADIUS*G_SCALE, TOGGLE_RADIUS*G_SCALE);

    int w1 = BUTTON_OFFSET*G_SCALE;
    int w2 = static_cast<int>(boundingRect().width() - BUTTON_WIDTH*G_SCALE - BUTTON_OFFSET*G_SCALE);
    int curW = static_cast<int>((w2 - w1)*animationProgress_ + w1);

    QColor fgColor = QColor(24, 34, 47);
    painter->setBrush(fgColor);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QRectF(curW, boundingRect().height()/2 - BUTTON_HEIGHT/2*G_SCALE, BUTTON_WIDTH*G_SCALE, BUTTON_HEIGHT*G_SCALE));
}

void CheckBoxButton::setState(bool isChecked)
{
    if (isChecked != isChecked_)
    {
        isChecked_ = isChecked;
        if (isChecked)
        {
            animationProgress_= 1.0;
        }
        else
        {
            animationProgress_ = 0.0;
        }
        update();
    }
}

bool CheckBoxButton::isChecked() const
{
    return isChecked_;
}

void CheckBoxButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (pressed_)
        {
            pressed_ = false;
            if (contains(event->pos()))
            {
                isChecked_ = !isChecked_;
                emit stateChanged(isChecked_);

                if (isChecked_)
                {
                    opacityAnimation_.setDirection(QVariantAnimation::Forward);
                    if (opacityAnimation_.state() != QVariantAnimation::Running)
                    {
                        opacityAnimation_.start();
                    }
                }
                else
                {
                    opacityAnimation_.setDirection(QVariantAnimation::Backward);
                    if (opacityAnimation_.state() != QVariantAnimation::Running)
                    {
                        opacityAnimation_.start();
                    }
                }
            }
        }
    }
}

void CheckBoxButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::PointingHandCursor);
    emit hoverEnter();
}

void CheckBoxButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
    emit hoverLeave();
}

void CheckBoxButton::onOpacityChanged(const QVariant &value)
{
    animationProgress_ = value.toDouble();
    update();
}

void CheckBoxButton::setEnabled(bool enabled)
{
    ClickableGraphicsObject::setEnabled(enabled);
    enabled_ = enabled;
    update();
}

} // namespace PreferencesWindow
