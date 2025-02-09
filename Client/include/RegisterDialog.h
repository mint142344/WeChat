#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <qjsonobject.h>
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
	void on_request_finished(RequestType request_type, const QJsonObject &json);

private:
	void registerCallback(const QJsonObject &json);
	void setErrorHint(bool isError, const QString &hint);

private:
	Ui::RegisterDialog *ui;
	QMap<RequestType, std::function<void(const QJsonObject &)>> m_request_map;
};

#endif // REGISTERDIALOG_H
