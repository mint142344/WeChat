#pragma once

#include <QString>
#include <QDateTime>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

namespace Tool {
// 格式化时间
inline QString formatTime(const QDateTime& time) {
	QDateTime now = QDateTime::currentDateTime();
	qint64 daysTo = time.daysTo(now);

	// 今天
	if (daysTo == 0) {
		return time.toString("HH:mm");
	}

	// 昨天
	if (daysTo == 1) {
		return "昨天";
	}

	// 本周内 - 显示星期
	if (daysTo < 7) {
		switch (time.date().dayOfWeek()) {
			case 1:
				return "周一";
			case 2:
				return "周二";
			case 3:
				return "周三";
			case 4:
				return "周四";
			case 5:
				return "周五";
			case 6:
				return "周六";
			case 7:
				return "周日";
		}
	}

	// 今年 - 显示月日
	if (time.date().year() == now.date().year()) {
		return time.toString("MM/dd");
	}

	// 往年 - 显示年月日
	return time.toString("yyyy/MM/dd");
}

// 圆形头像
inline QPixmap roundImage(const QPixmap& src) {
	if (src.isNull()) {
		return {};
	}

	QPixmap dest(src.size());
	dest.fill(Qt::transparent);

	QPainter painter(&dest);
	painter.setRenderHint(QPainter::Antialiasing);

	QPainterPath path;
	path.addEllipse(dest.rect());
	painter.setClipPath(path);

	painter.drawPixmap(0, 0, src);

	return dest;
}
} // namespace Tool
