#pragma once

#include <QString>

class Actions {
public:
    static void paste();
private:
    static QString handleDuplicate(QString newAddr);
    static bool copyDirectory(QString oldPath, QString newPath);
};
