#pragma once

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QKeyEvent>

class LoadingDialog : public QDialog {
	Q_OBJECT
public:
	explicit LoadingDialog(QWidget* parent = nullptr)
		: QDialog(parent), m_label(new QLabel(this)), m_layout(new QVBoxLayout(this)) {
		setFixedSize(200, 100);

		// 标题栏 + 取消最小化、最大化按钮、关闭按钮
		setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
		m_label->setAlignment(Qt::AlignCenter);
		m_label->setText("登录中...");
		m_label->setFont(QFont("Microsoft YaHei", 12));

		m_layout->addWidget(m_label);
		setLayout(m_layout);

		setStyleSheet("LoadingDialog { border-radius: 10px; }");
	}

	void setText(const QString& text) { m_label->setText(text); }

protected:
	// 禁用 Esc 关闭窗口
	void keyPressEvent(QKeyEvent* event) override {
		if (event->key() == Qt::Key_Escape) {
			event->ignore();
		} else {
			QDialog::keyPressEvent(event);
		}
	}

private:
	QLabel* m_label;
	QVBoxLayout* m_layout;
};