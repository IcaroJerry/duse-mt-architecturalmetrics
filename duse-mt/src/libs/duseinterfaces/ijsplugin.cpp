#include "ijsplugin.h"

namespace DuSE
{

IJsPlugin::IJsPlugin(QObject *parent) :
    IPlugin(parent)
{

}

IJsPlugin::~IJsPlugin()
{

}

QString IJsPlugin::jsProgram() const
{
    return "ok";
}

}
