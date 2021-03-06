import qbs.FileInfo

QtApplication {
    Depends { name: "Qt.widgets" }

    // The following define makes your compiler emit warnings if you use
    // any Qt feature that has been marked deprecated (the exact warnings
    // depend on your compiler). Please consult the documentation of the
    // deprecated API in order to know how to port your code away from it.
    // You can also make your code fail to compile if it uses deprecated APIs.
    // In order to do so, uncomment the second entry in the list.
    // You can also select to disable deprecated APIs only up to a certain version of Qt.
    cpp.defines: [
        "QT_DEPRECATED_WARNINGS",
        /* "QT_DISABLE_DEPRECATED_BEFORE=0x060000" */ // disables all the APIs deprecated before Qt 6.0.0
    ]
    cpp.cxxLanguageVersion: "c++17"
    cpp.includePaths: base.concat([sourceDirectory, "../qbs/src/lib/corelib/"]);
    cpp.dynamicLibraries: base.concat(["qbscore"])
    cpp.libraryPaths: base.concat(["/Applications/Qt/Qt Creator.app/Contents/Frameworks/"])

    files: [
        "main.cpp",
        "mainwindow.cpp",
        "mainwindow.h",
        "mainwindow.ui",
        "pathchooser.cpp",
        "pathchooser.h",
        "projectmodel.cpp",
        "projectmodel.h",
        "projectmodel_p.h",
        "profilesmodel.cpp",
        "profilesmodel.h",
        "productlistmodel.cpp",
        "productlistmodel.h",
        "qbssession.cpp",
        "qbssession.h",
        "qbssession_p.h",
        "treemodelitem.h",
        "treemodelitem.cpp",
    ]

    install: true
    installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}
