#ifndef BIOBLOCKSTRANSLATOR_H
#define BIOBLOCKSTRANSLATOR_H

#define BIOBLOCKS_VARIABLE_SET_STR "variables_set"

#define BIOBLOCKS_IF_STR "controls_if"
#define BIOBLOCKS_WHILE_STR "controls_whileUntil"
#define BIOBLOCKS_THERMOCYCLING_STR "thermocycling"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

//lib
#include <json.hpp>

//local
#include <protocolGraph/ProtocolGraph.h>
#include <protocolGraph/operables/comparison/protocolboolf.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>

#include <utils/AutoEnumerate.h>
#include <utils/utilsjson.h>

//own project
#include "bioblocksTranslation/blocks/internalPOJO/bioblocksoppojo.h"
#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"
#include "bioblocksTranslation/blocks/internalPOJO/cpublockpojo.h"
#include "bioblocksTranslation/blocks/internalPOJO/ifblockpojo.h"
#include "bioblocksTranslation/blocks/internalPOJO/logicblockpojointerface.h"
#include "bioblocksTranslation/blocks/internalPOJO/whileblockpojo.h"

#include "bioblocksTranslation/blocks/blocksutils.h"
#include "bioblocksTranslation/blocks/containermanager.h"
#include "bioblocksTranslation/blocks/logicblocks.h"
#include "bioblocksTranslation/blocks/mathblocks.h"
#include "bioblocksTranslation/blocks/operationsblocks.h"

#include "bioblocksTranslation/interfaces/translationinterface.h"

#include "bioblocksTranslation/logicblocksmanager.h"

#include "bioblocksTranslation/bioblockstranslator_global.h"

class BIOBLOCKSTRANSLATORSHARED_EXPORT BioBlocksTranslator : public TranslationInterface
{
public:
    BioBlocksTranslator(units::Time timeSlice, const std::string & jsonFilepath);
    virtual ~BioBlocksTranslator();

    virtual std::shared_ptr<ProtocolGraph> translateFile(std::shared_ptr<LogicBlocksManager> logicBlocskManager = NULL) throw(std::invalid_argument);

protected:
    std::string filePath;
    units::Time timeSliceValue;
    std::shared_ptr<ProtocolGraph> ptrGraph;

    std::vector<int> initializationOps;
    std::vector<std::shared_ptr<BlockPOJOInterface>> blocksOps;
    std::vector<std::shared_ptr<BlockPOJOInterface>> linkedlogicOps;
    std::vector<std::shared_ptr<BlockPOJOInterface>> freelogicOps;

    AutoEnumerate logicSerial;
    std::shared_ptr<BlockPOJOInterface> lastBlockProcess;
    std::shared_ptr<MathematicOperable> timeSlice;

    std::vector<std::shared_ptr<MathematicOperable>> endValues;
    std::shared_ptr<MathematicOperable> protocolEndTime;

    void makeProtocolGraph(std::shared_ptr<LogicBlocksManager> logicBlocskManager);
    void setTimeStep();
    void initActualTimeVar();
    void processLoadContainers();

    void resetAttributes(const std::string & protocolName);

    void processLinkedBlocks(nlohmann::json blockObj,
                             const OperationsBlocks & blocksTrans) throw(std::invalid_argument);

    void processBlock(nlohmann::json blockObj,
                      const OperationsBlocks & blocksTrans,
                      std::shared_ptr<MathematicOperable> initTime = NULL,
                      std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL,
                      std::vector<std::shared_ptr<VariableEntry>> endIfVar = std::vector<std::shared_ptr<VariableEntry>>{}) throw(std::invalid_argument);

    void thermocyclingOperation(const nlohmann::json & thermocyclingObj,
                                const OperationsBlocks & blocksTrans,
                                std::shared_ptr<MathematicOperable> initTime = NULL,
                                std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL,
                                std::vector<std::shared_ptr<VariableEntry>> endIfVar = std::vector<std::shared_ptr<VariableEntry>>{}) throw(std::invalid_argument);

    void variablesSet(const nlohmann::json & variableSetObj,
                      const OperationsBlocks & blocksTrans,
                      std::shared_ptr<MathematicOperable> initTime,
                      std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL,
                      std::vector<std::shared_ptr<VariableEntry>> endIfVar = std::vector<std::shared_ptr<VariableEntry>>{}) throw(std::invalid_argument);

    void bioblocksIfOperation(const nlohmann::json & bioblocksIfObj,
                              const OperationsBlocks & blocksTrans,
                              std::shared_ptr<MathematicOperable> initTime = NULL,
                              std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL,
                              std::vector<std::shared_ptr<VariableEntry>> endIfVar = std::vector<std::shared_ptr<VariableEntry>>{}) throw(std::invalid_argument);

    void bioblocksWhileOperation(const nlohmann::json & bioblocksWhileObj,
                                 const OperationsBlocks & blocksTrans,
                                 std::shared_ptr<MathematicOperable> initTime = NULL,
                                 std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL,
                                 std::vector<std::shared_ptr<VariableEntry>> endIfVar = std::vector<std::shared_ptr<VariableEntry>>{}) throw(std::invalid_argument);

    void processBioBlocksOp(const nlohmann::json & bioblocksObj,
                            const OperationsBlocks & blocksTrans,
                            std::shared_ptr<MathematicOperable> initTime = NULL,
                            std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL,
                            std::vector<std::shared_ptr<VariableEntry>> endIfVar = std::vector<std::shared_ptr<VariableEntry>>{});

    void initializeVarToInfinite(const std::string & name);
    void initializeVarToZero(const std::string & name);
    void initializeVarToValue(const std::string & name, std::shared_ptr<MathematicOperable> value);

    std::shared_ptr<MathematicOperable> processIniTime(units::Time initTime) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> processDuration(units::Time duration, const std::vector<int> & ops);
};

#endif // BIOBLOCKSTRANSLATOR_H
