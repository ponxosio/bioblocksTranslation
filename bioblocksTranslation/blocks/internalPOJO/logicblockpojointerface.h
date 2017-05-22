#ifndef LOGICBLOCKPOJOINTERFACE_H
#define LOGICBLOCKPOJOINTERFACE_H

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"

#include "bioblocksTranslation/logicblocksmanager.h"

class LogicBlockPOJOInterface : public BlockPOJOInterface
{
public:
    virtual ~LogicBlockPOJOInterface(){}

    inline virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const throw(std::runtime_error) {
        appendOperationsToGraphs(graphPtr, NULL);
    }
    virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr, std::shared_ptr<LogicBlocksManager> logicManager) const throw(std::runtime_error) = 0;
};

#endif // LOGICBLOCKPOJOINTERFACE_H
