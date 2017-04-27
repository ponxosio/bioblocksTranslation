#ifndef BIOBLOCKSTRANSLATOR_H
#define BIOBLOCKSTRANSLATOR_H

#include <fstream>
#include <stdexcept>
#include <string>

//lib
#include <json.hpp>

//local
#include <protocolGraph/ProtocolGraph.h>

//own project
#include "bioblocksTranslation/blocks/blocksutils.h"
#include "bioblocksTranslation/blocks/containermanager.h"
#include "bioblocksTranslation/blocks/logicblocks.h"
#include "bioblocksTranslation/blocks/mathblocks.h"
#include "bioblocksTranslation/blocks/operationsblocks.h"

#include "bioblocksTranslation/interfaces/translationinterface.h"

class BioBlocksTranslator : public TranslationInterface
{
public:
    BioBlocksTranslator();
    virtual ~BioBlocksTranslator();

    virtual std::shared_ptr<ProtocolGraph> translateFile(const std::string & path) throw(std::invalid_argument);

protected:
    void processLinkedBlocksList(const nlohmann::json & blockObj, std::shared_ptr<OperationsBlocks> blocksTrans);
    void processLinkedBlocks(const nlohmann::json & blockObj, std::shared_ptr<OperationsBlocks> blocksTrans);
};

#endif // BIOBLOCKSTRANSLATOR_H
