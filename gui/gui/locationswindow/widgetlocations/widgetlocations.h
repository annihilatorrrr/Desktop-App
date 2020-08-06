#ifndef WIDGETLOCATIONS_H
#define WIDGETLOCATIONS_H

#include <QElapsedTimer>
#include "customscrollbar.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QVBoxLayout>
#include <QAbstractScrollArea>
#include <QEasingCurve>
#include "locationitem.h"
#include "../backend/locationsmodel/basiclocationsmodel.h"
#include "../backend/locationsmodel/favoritelocationsstorage.h"
#include "../backend/types/types.h"
#include "iwidgetlocationsinfo.h"
#include "backgroundpixmapanimation.h"
#include "tooltips/tooltiptypes.h"

class FormConnect;

namespace GuiLocations {

class CursorUpdateHelper;

class WidgetLocations : public QAbstractScrollArea, public IWidgetLocationsInfo
{
    Q_OBJECT

public:
    explicit WidgetLocations(QWidget *parent);
    virtual ~WidgetLocations() override;

    bool cursorInViewport() override;
    bool hasSelection() override;
    void centerCursorOnSelectedItem() override;

    void setModel(BasicLocationsModel *locationsModel);
    void setCurrentSelected(LocationID id);
    void setFirstSelected() override;
    void setExpanded(bool bExpanded);
    void startAnimationWithPixmap(const QPixmap &pixmap);

    void setShowLatencyInMs(bool showLatencyInMs);
    virtual bool isShowLatencyInMs()   override;
    virtual bool isFreeSessionStatus() override;

    virtual int getWidth()          override;
    virtual int getScrollBarWidth() override;

    void setCountAvailableItemSlots(int cnt);
    int getCountAvailableItemSlots();
    virtual QSize sizeHint() const override;

    void onKeyPressEvent(QKeyEvent *event);

    bool eventFilter(QObject *object, QEvent *event) override;

    void handleKeyEvent(QKeyEvent *event) override;

    bool isIdVisible(LocationID id);

    int countVisibleItems() override;

    void setSize(int width, int height);
    void updateScaling();

protected:
    virtual void paintEvent(QPaintEvent *event)            override;
    virtual void scrollContentsBy(int dx, int dy)          override;
    virtual void mouseMoveEvent(QMouseEvent *event)        override;
    virtual void mousePressEvent(QMouseEvent *event)       override;
    virtual void mouseReleaseEvent(QMouseEvent *event)     override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void leaveEvent(QEvent *event)                 override;
    virtual void enterEvent(QEvent *event)                 override;
    virtual void resizeEvent(QResizeEvent *event)          override;

signals:
    void selected(LocationID id);
    void switchFavorite(LocationID id, bool isFavorite);
    void heightChanged(int oldHeight, int newHeight);
    void addStaticIpURLClicked();
    void showTooltip(TooltipInfo info);
    void hideTooltip(TooltipId type);

private slots:
    void onItemsUpdated(QVector<LocationModelItem*> items);
    void onConnectionSpeedChanged(LocationID id, PingTime timeMs);
    void onIsFavoriteChanged(LocationID id, bool isFavorite);
    void onFreeSessionStatusChanged(bool isFreeSessionStatus);
    void onTopScrollBarValueChanged(int value);

    void onLanguageChanged();

private:
    int width_;
    int height_;

    int topInd_;
    int topOffs_;
    int indSelected_;
    int topOffsSelected_;
    int indParentPressed_;
    int indChildPressed_;
    int indChildFavourite_;

    int countOfAvailableItemSlots_;

    QVector<LocationItem *> items_;
    bool bIsFreeSession_;
    LocationItem * bestLocation_;
    QString bestLocationName_;

    int startAnimationTop_;
    int endAnimationTop_;
    double scrollAnimationDuration_;
    QElapsedTimer scrollAnimationElapsedTimer_;
    bool isScrollAnimationNow_;

    double currentScale_;

    QEasingCurve easingCurve_;

    std::unique_ptr<CursorUpdateHelper> cursorUpdateHelper_;

    // variables for tooltip
    QPoint prevCursorPos_;
    bool bMouseInViewport_;

    bool bExpanded_;
    bool bShowLatencyInMs_;

    bool bTapGestureStarted_;

    CustomScrollBar *scrollBarHidden_;
    QScrollBar *scrollBarOnTop_;
    BasicLocationsModel *locationsModel_;

    QList<LocationItem *> currentVisibleItems_;

    BackgroundPixmapAnimation backgroundPixmapAnimation_;

    int getItemHeight() const;
    int getTopOffset() const;
    void calcScrollPosition();
    void setupScrollBar();
    void setupScrollBarMaxValue();
    bool detectSelectedItem(const QPoint &cursorPos);
    bool detectItemClickOnArrow();
    int detectItemClickOnFavoriteLocationIcon();
    bool isExpandAnimationNow();
    void setCursorForSelected();
    void setVisibleExpandedItem(int ind);
    LocationID detectLocationForTopInd(int topInd);
    int detectVisibleIndForCursorPos(const QPoint &pt);

    void handleMouseMoveForTooltip();
    void handleLeaveForTooltip();

    QPoint adjustToolTipPosition(const QPoint &globalPoint, const QSize &toolTipSize, bool isP2PToolTip);
    QPoint getSelectItemConnectionMeterCenter();
    void clearItems();
    double calcScrollingSpeed(double scrollItemsCount);
    bool isGlobalPointInViewport(const QPoint &pt);
    void scrollDownToSelected();
    void scrollUpToSelected();
    void handleTapClick(const QPoint &cursorPos);
    void emitSelectedIfNeed();
    void expandOrCollapseSelectedItem();

    int countVisibleItemsInViewport(LocationItem *locationItem);
    int viewportPosYOfIndex(int index, bool centerY);
    int viewportIndexOfLocationItemSelection(LocationItem *locationItem);
    QRect globalLocationsListViewportRect();

    QRect locationItemRect(LocationItem *locationItem, bool includeCities);
    QRect cityRect(LocationItem *locationItem, int cityIndex);
    bool itemHeaderInViewport(LocationItem *locationItem);
    QList<int> cityIndexesInViewport(LocationItem *locationItem);
    int viewportIndexOfLocationHeader(LocationItem *locationItem);

    void updateSelectionCursorAndToolTipByCursorPos();

};

} // namespace GuiLocations

#endif // WIDGETLOCATIONS_H