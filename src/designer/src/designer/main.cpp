// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "qdesigner.h"
#include <QtCore/qlibraryinfo.h>
#include <QtCore/qdir.h>

#include <stdlib.h>

#ifdef DMalterlibQtFeatures
#include <Mib/Core/Core>
#include <AOCC/AOLocalizationUtil.h>

NTranslate::CLocalizationEnvironment g_LocalizationEnv;
extern char const *gc_ProjectName;
#endif

QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
#ifdef DMalterlibQtFeatures
    g_LocalizationEnv.f_LoadExternal
        (
            NFile::CFile::fs_GetPath(CStr(DMibPFile)) + "../../../../../../../.."
            , gc_ProjectName
            , nullptr
            , false
            , false
        )
    ;
#endif
    // required for QWebEngineView
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QDesigner app(argc, argv);
    switch (app.parseCommandLineArguments()) {
    case QDesigner::ParseArgumentsSuccess:
        break;
    case QDesigner::ParseArgumentsError:
        return 1;
    case QDesigner::ParseArgumentsHelpRequested:
        return 0;
    }
    QGuiApplication::setQuitOnLastWindowClosed(false);

    return QApplication::exec();
}
