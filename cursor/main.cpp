#include <DocumentManager.h>
#include <MessageSender.h>

#include <ILexer.h>
#include <ScintillaEditBase.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QMainWindow>
#include <QPointer>
#include <QString>

#include <algorithm>
#include <limits>

namespace {
QString applicationName() {
    return "Cursor";
}

QString applicationVersion() {
    return "0.1";
}

QString applicationDescription() {
    return "Cursor Text Editor";
}
}

int main(int argc, char** argv) {
    using namespace cursor;
    QApplication application(argc, argv);
    QCoreApplication::setApplicationName(applicationName());
    QCoreApplication::setApplicationVersion(applicationVersion());

    QCommandLineParser parser;
    parser.setApplicationDescription(applicationDescription());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("filePath", "Path to file to open");
    parser.process(application);

    QMainWindow window;
    QPointer<ScintillaEditBase> editor(new ScintillaEditBase);
    window.setCentralWidget(editor);
    window.showMaximized();

    MessageSender messageSender(editor);
    DocumentManager documentManager(messageSender);

    auto positionalArguments = parser.positionalArguments();
    std::for_each(std::begin(positionalArguments),
                  std::end(positionalArguments),
                  [&documentManager](const QString& filePath) {
        documentManager.loadDocument(filePath);
    });

    return application.exec();
}
