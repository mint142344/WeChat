#pragma once

#include <QListWidget>

class MessageItem;

// 搜索结果列表(好友/群)
class SearchListWidget : public QListWidget {
	Q_OBJECT
public:
	explicit SearchListWidget(QWidget* parent = nullptr);

	void addSearchItem(MessageItem* item);

signals:
	void itemSelected(MessageItem* item);

protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;

private slots:
	void onItemClicked(QListWidgetItem* item);
};