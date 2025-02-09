#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include "Common.h"

#include <QMap>
#include <QDialog>
#include <functional>

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
	void http_request_finished(RequestType request_type, const QJsonObject &json);

	// button: 发送验证码
	void on_button_sendCode_clicked();

private:
	void registerAllCallback();
	void setErrorHint(bool isError, const QString &hint);

private:
	Ui::RegisterDialog *ui;
	QMap<RequestType, std::function<void(const QJsonObject &)>> m_request_map;
};

#endif // REGISTERDIALOG_H
