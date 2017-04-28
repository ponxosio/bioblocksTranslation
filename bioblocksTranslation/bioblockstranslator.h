#ifndef BIOBLOCKSTRANSLATOR_H
#define BIOBLOCKSTRANSLATOR_H

#define BIOBLOCKS_VARIABLE_SET_STR "variables_set"

#define BIOBLOCKS_IF_STR "bioblocks_if"
#define BIOBLOCKS_WHILE_STR "bioblocks_while"

#include <fstream>
#include <stdexcept>
#include <string>

//lib
#include <json.hpp>

//local
#include <protocolGraph/ProtocolGraph.h>

//own project
#include "bioblocksTranslation/blocks/internalPOJO/ifblockpojo.h"
#include "bioblocksTranslation/blocks/internalPOJO/whileblockpojo.h"

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

    int variablesSet(const nlohmann::json & variableSetObj) throw(std::invalid_argument);
    IfBlockPOJO bioblocksIfOperation(const nlohmann::json & bioblocksIfObj) throw(std::invalid_argument);
    WhileBlockPOJO bioblocksWhileOperation(const nlohmann::json & bioblocksWhileObj) throw(std::invalid_argument);
};

#endif // BIOBLOCKSTRANSLATOR_H
