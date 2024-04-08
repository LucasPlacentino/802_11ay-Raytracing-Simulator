#include "heatmapgradientlegend.h"
#include <QPainter>

int hsl_lightness = 92;

HeatmapGradientLegend::HeatmapGradientLegend(QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
    // TODO: ? gradient legend inside a bounding rectangle ?
    setRect(30,70,25,100);
    gradient_.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient_.setColorAt(0.0, QColor::fromHsl(240,255,hsl_lightness)); // dark blue
    gradient_.setColorAt(0.25, QColor::fromHsl(240*0.75,255,hsl_lightness)); // around cyan
    gradient_.setColorAt(0.5, QColor::fromHsl(120,255,hsl_lightness)); // liek green
    gradient_.setColorAt(0.75, QColor::fromHsl(240*0.25,255,hsl_lightness)); // around yellow (or orange)
    gradient_.setColorAt(1.0, QColor::fromHsl(0,255,hsl_lightness)); // red
}

void HeatmapGradientLegend::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    int left = rect().left() + rect().width() + 10;
    painter->setPen(QPen());
    painter->setBrush(gradient_);
    painter->drawRect(rect());
    painter->drawText(left, rect().top(), "-40dBm");
    painter->drawText(left, rect().top() + rect().height() / 2, "-65dBm");
    painter->drawText(left, rect().top() + rect().height(), "-90dBm");
}
