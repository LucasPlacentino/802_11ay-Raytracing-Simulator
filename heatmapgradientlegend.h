#ifndef HEATMAPGRADIENTLEGEND_H
#define HEATMAPGRADIENTLEGEND_H

#include <QGraphicsRectItem>
#include <QLinearGradient>

class HeatmapGradientLegend : public QGraphicsRectItem
{
public:
    HeatmapGradientLegend(QGraphicsItem *parent=nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QLinearGradient gradient_;
};

#endif // HEATMAPGRADIENTLEGEND_H
