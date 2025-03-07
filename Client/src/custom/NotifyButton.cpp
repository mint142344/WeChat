#include "custom/NotifyButton.h"

NotifyButton::NotifyButton(QWidget* parent) : QPushButton{parent} {
	setCursor(Qt::PointingHandCursor);
	setFixedHeight(40);
	setFlat(true);
}
