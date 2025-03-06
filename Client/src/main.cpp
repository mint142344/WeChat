#include "mainwindow.h"
#include "LoginDialog.h"

#include <QApplication>

constexpr bool DEBUG = true;

void setAppStyleSheet(const char* path) {}

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	qApp->setWindowIcon(QIcon(":/images/app.ico"));

	MainWindow w;
	if constexpr (DEBUG) {
		w.show();
	} else {
		LoginDialog loginDialog;
		if (loginDialog.exec() == QDialog::Accepted)
			w.show();
		else
			return 0;
	}

	return a.exec();
}
