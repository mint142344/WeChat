#include "AddFriendDialog.h"
#include "./ui_AddFriendDialog.h"

AddFriendDialog::AddFriendDialog(QWidget* parent) : QDialog{parent}, ui{new Ui::AddFriendDialog} {
	ui->setupUi(this);
}
