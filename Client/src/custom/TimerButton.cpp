#include "custom/TimerButton.h"
#include <qtimer.h>

#include <QTimer>

TimerButton::TimerButton(QWidget *parent) : m_timer{new QTimer(this)} {
	connect(m_timer, &QTimer::timeout, [this]() {
		if (m_counter <= 0) {
			stopTimer();
			return;
		}

		setText(QString::number(m_counter--) + "s");
	});
}

TimerButton::~TimerButton() { m_timer->deleteLater(); }

void TimerButton::startTimer() {
	setEnabled(false);
	m_timer->start(1000);
}

void TimerButton::stopTimer() {
	m_timer->stop();
	setEnabled(true);
	setText("获取验证码");
	m_counter = 180;
}
