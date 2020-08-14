
#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <QDir>
#include <QDebug>
#include <QDebug>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QString>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
	qDebug() << "|___________________________________________________________________________________|";
	auto time = QDateTime::currentDateTime();
	time.time().start();
	qDebug() << "#___________start time: " << time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
	qDebug() << "|___________________________________________________________________________________|";
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    Q_INIT_RESOURCE(application);
	QApplication app(argc, argv);
	QTranslator translator;
	bool ok = translator.load(QString(":/res/zh_CN.qm"));
	if (!ok) {
		qDebug() << "#___________load translator file error!";
	}
	app.installTranslator(&translator);


	QFile file(":/res/skin/black.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	app.setStyleSheet(styleSheet);
	file.close();

	auto mainWin = new nametls::view::MainWindow;
	mainWin->show();

    return app.exec();
}
