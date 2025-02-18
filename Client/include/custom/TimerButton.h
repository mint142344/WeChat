#pragma once

#include <QPushButton>

// 获取验证码 倒计时按钮
class TimerButton : public QPushButton {
	Q_OBJECT

public:
	explicit TimerButton(QWidget *parent = nullptr);
	~TimerButton();

	void startTimer();
	void stopTimer();

private:
	int m_counter = 180; // 计数器
	QTimer *m_timer;	 // 定时器
};