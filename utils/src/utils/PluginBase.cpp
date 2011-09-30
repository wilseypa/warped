
// See copyright notice in file Copyright in the root directory of this archive.

#include "PluginBase.h"

PluginBase::PluginBase(){}

PluginBase::~PluginBase(){}

const int 
PluginBase::pluginSignature(){
  return 0xdeadbeef;
}

const int 
PluginBase::getPluginSignature(){
  return pluginSignature();
}
