#pragma once

#include <QGraphicsObject>
#include <QVariantAnimation>
#include <QGraphicsProxyWidget>
#include "commonwidgets/custommenulineedit.h"
#include "commongraphics/clickablegraphicsobject.h"
#include "blockableqlineedit.h"

namespace LoginWindow {


class UsernamePasswordEntry : public ClickableGraphicsObject
{
    Q_OBJECT
public:

    explicit UsernamePasswordEntry(const QString &text, bool password, const QString &customIcon, ScalableGraphicsObject * parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void clearActiveState();

    QString getText() const;
    void setText(const QString &text);
    void setDescription(const QString &text);
    void setPlaceholderText(const QString &placeholderText);

    void setError(bool error);

    void setWidth(int width);
    void setOpacityByFactor(double newOpacityFactor);

    void setClickable(bool clickable) override;

    void setFocus();
    void updateScaling() override;

signals:
    void textChanged(const QString &text);
    void iconClicked();

private slots:
    void onTextChanged(const QString &text);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsProxyWidget *userEntryProxy_;
    CommonWidgets::CustomMenuLineEdit *userEntryLine_;
    const QString userEntryLineAddSS_;

    QString descriptionText_;

    int height_;
    int width_;

    double curDescriptionOpacity_;
    double curLineEditOpacity_;

    static constexpr int DESCRIPTION_TEXT_HEIGHT = 16;

    void updateFontSize();
};

}
