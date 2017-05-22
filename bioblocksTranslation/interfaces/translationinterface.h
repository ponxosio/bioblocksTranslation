#ifndef TRANSLATIONINTERFACE_H
#define TRANSLATIONINTERFACE_H

#include <memory>
#include <string>

//local
#include <protocolGraph/ProtocolGraph.h>

class TranslationInterface
{
public:
    virtual ~TranslationInterface(){}

    virtual std::shared_ptr<ProtocolGraph> translateFile(std::shared_ptr<LogicBlocksManager> logicBlocskManager = NULL) = 0;
};

#endif // TRANSLATIONINTERFACE_H
