#include "raysegment.h"

RaySegment::RaySegment(qreal start_x, qreal start_y, qreal end_x, qreal end_y) {
    // RaySegment object constructor
    this->setLine(start_x, start_y, end_x, end_y);
    this->graphics->setLine(start_x*10, start_y*10, end_x*10, end_y*10);
    this->distance = this->length();
}
