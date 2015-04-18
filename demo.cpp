#include <QApplication>
#include <QUrl>
#include <QDebug>
#include <QProgressDialog>

#include "DynamicWebWindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(true);
	app.setApplicationName(QGuiApplication::tr("DynamicWeb Demo"));
	app.setOrganizationName("Jan Dalheimer");

	qDebug() << "Available backends:" << DynamicWebWindow::availableBackends().join(", ").toLocal8Bit().constData();

	QProgressDialog dlg;
	dlg.show();

	DynamicWebWindow window;
	QObject::connect(&window, &DynamicWebWindow::loadProgress, &dlg, &QProgressDialog::setValue);
	QObject::connect(&window, &DynamicWebWindow::titleChanged, &dlg, &QProgressDialog::setLabelText);
	QObject::connect(&window, &DynamicWebWindow::urlChanged, [&dlg](const QUrl &url) { dlg.setWindowTitle(url.toString()); });
	window.setUrl(QUrl("http://qt.io"));
	window.show();

	return app.exec();
}
