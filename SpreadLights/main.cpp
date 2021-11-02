#include <QApplication>
#include <QQmlApplicationEngine>
#include <QSpinBox>
#include <QComboBox>
#include <QtQml>
#include <QQuickView>
#include <QButtonGroup>
#include <QStringList>
#include "controls.h"




int main(int argc, char *argv[]) {

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

//    qRegisterMetaType<Model *>();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
//    engine.load(url);

    QQmlComponent component(&engine, url);

    QObject *mainPage = component.create();
    QObject *controlsQObj = mainPage->findChild<QObject *>("controls");
    Controls *controls = qobject_cast<Controls *>(controlsQObj);
    qDebug() << "main: " << mainPage << "qobje: " << controlsQObj << " controls: " << controls;
//    controls.populate();

//    QObject* buttonTest = mainPage->findChild<QObject *>("buttonTest");
    QObject* comboBox1 = mainPage->findChild<QObject *>("modelSelect");
    QObject* comboBox2 = mainPage->findChild<QObject *>("childSelect");

    //    QObject *checkBox = mainPage->findChild<QObject *>("checkBox");

//    QObject::connect(buttonTest, SIGNAL(clickedButton(QString)), controls, SLOT(onButtonClicked(QString)));
//    QObject::connect(checkBox, SIGNAL(boxChecked(QObject)), &controls, SLOT(onBoxChecked(QObject)));
    QObject::connect(comboBox1, SIGNAL(activated(int)), controls, SLOT(displayModel(int)));
    QObject::connect(comboBox2, SIGNAL(activated(int)), controls, SLOT(displayModel(int)));
    qDebug() << "parent list " << controls->parentList();

    return app.exec();
}
