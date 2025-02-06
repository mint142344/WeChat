#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog {
	Q_OBJECT

public:
	explicit RegisterDialog(QWidget *parent = nullptr);
	~RegisterDialog();

private slots:
	void on_lineEdit_email_editingFinished();

private:
	void setErrorHint(bool isError, const QString &hint);

private:
	Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
