#ifndef EMAILITEM_H
#define EMAILITEM_H

#include "commongraphics/baseitem.h"
#include "commongraphics/textbutton.h"

namespace PreferencesWindow {

class EmailItem : public CommonGraphics::BaseItem
{
    Q_OBJECT
public:
    explicit EmailItem(ScalableGraphicsObject *parent);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void setEmail(const QString &email);
    void setNeedConfirmEmail(bool b);
    void setConfirmEmailResult(bool bSuccess);

    void updateScaling() override;

signals:
    void sendEmailClick();
    void emptyEmailButtonClick();

private slots:
    void onSendEmailClick();
    void onVisibleChanged();

private:
    static constexpr int WARNING_OFFSET_Y = 40;

    QString email_;
    bool isNeedConfirmEmail_;
    int msgHeight_;

    enum EMAIL_SEND_STATE { EMAIL_NOT_INITIALIZED, EMAIL_NO_SEND, EMAIL_SENDING, EMAIL_SENT, EMAIL_FAILED_SEND };
    EMAIL_SEND_STATE emailSendState_;

    CommonGraphics::TextButton *sendButton_;
    CommonGraphics::TextButton *emptyEmailButton_;

    void updatePositions();
    void setEmailSendState(EMAIL_SEND_STATE state);

    const char *ADD_TEXT = "Get 10GB/Month of data and gain the ability to reset your password.";
    const char *CONFIRM_TEXT = "Please confirm your email";
    const char *EMAIL_TEXT = "Email";
};

} // namespace PreferencesWindow

#endif // EMAILITEM_H
