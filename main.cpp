#include "mainwindow.h"
#include "LoginDialog.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QFile file(":/style/styleSheet.qss");
	if (file.open(QFile::ReadOnly)) {
		QString styleSheet = QLatin1String(file.readAll());
		a.setStyleSheet(styleSheet);
		file.close();
	}
	a.setWindowIcon(QIcon(":/images/app_icon.png"));

	MainWindow w;
	LoginDialog loginDialog;
	if (loginDialog.exec() == QDialog::Accepted) {
		w.show();
		return a.exec();
	}
	return 0;
}
