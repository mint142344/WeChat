#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class AddFriendDialog;
}

class AddFriendDialog : public QDialog {
	Q_OBJECT
public:
	explicit AddFriendDialog(QWidget* parent = nullptr);

private:
	Ui::AddFriendDialog* ui;
};
