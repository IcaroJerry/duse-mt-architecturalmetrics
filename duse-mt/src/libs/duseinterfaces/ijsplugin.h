#ifndef IJSPLUGIN_H
#define IJSPLUGIN_H

#include "iplugin.h"

namespace DuSE
{

class DUSEINTERFACESSHARED_EXPORT IJsPlugin : public IPlugin
{
    Q_OBJECT
public:
    IJsPlugin(QObject *parent = 0);
    virtual ~IJsPlugin();
    QString jsProgram() const;
};

}

#endif // IJSPLUGIN_H
