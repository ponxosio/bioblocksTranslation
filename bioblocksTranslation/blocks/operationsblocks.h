#ifndef OPERATIONSBLOCKS_H
#define OPERATIONSBLOCKS_H

#define BIOBLOCKS_PIPETTE_STR "pipette"
#define BIOBLOCKS_CONTINUOUS_FLOW_STR "continuous_flow"
#define BIOBLOCKS_ELECTROPHORESIS_STR "electrophoresis"
#define BIOBLOCKS_INCUBATE_STR "incubate"
#define BIOBLOCKS_CENTRIFUGATION_STR "centrifugation"
#define BIOBLOCKS_THERMOCYCLING_STR "thermocycling"
#define BIOBLOCKS_MEASUREMENT_STR "measurement"
#define BIOBLOCKS_SANGERSEQUENCING_STR "sangerSequencing"
#define BIOBLOCKS_OLIGOSYNTHESIZE_STR "oligosynthesize"
#define BIOBLOCKS_COLONYPICKING_STR "colonyPicking"
#define BIOBLOCKS_CELLSPREADING_STR "cellSpreading"
#define BIOBLOCKS_FLASHFREEZE_STR "flashFreeze"
#define BIOBLOCKS_MIX_STR "mix"
#define BIOBLOCKS_TURBIDOSTAT_STR "turbidostat"
#define BIOBLOCKS_FLOWCITOMETRY_STR "flowCitometry"

#include <memory>
#include <stdexcept>
#include <unordered_map>

//lib
#include <json.hpp>

//local
#include <protocolGraph/ProtocolGraph.h>

#include "bioblocksTranslation/blocks/internalPOJO/bioblocksoppojo.h"

#include "bioblocksTranslation/blocks/blocksutils.h"
#include "bioblocksTranslation/blocks/containermanager.h"
#include "bioblocksTranslation/blocks/logicblocks.h"
#include "bioblocksTranslation/blocks/mathblocks.h"

class OperationsBlocks
{
public:
    OperationsBlocks(std::shared_ptr<ProtocolGraph> graphPtr,
                     std::shared_ptr<ContainerManager> vcManager,
                     std::shared_ptr<MathBlocks> mathBlocks,
                     std::shared_ptr<LogicBlocks> logicBlocks);
    virtual ~OperationsBlocks();

    std::vector<int> processBlock(const nlohmann::json & blockObj) throw(std::invalid_argument);

protected:
    std::shared_ptr<ProtocolGraph> graphPtr;
    std::shared_ptr<ContainerManager> vcManager;
    std::shared_ptr<MathBlocks> mathBlocks;
    std::shared_ptr<LogicBlocks> logicBlocks;

    std::vector<int> pipetteOperation(const nlohmann::json & pipetteObj) throw(std::invalid_argument);
    std::vector<int> continuousflowOperation(const nlohmann::json & continuousflowObj) throw(std::invalid_argument);
    int electrophoresisOperation(const nlohmann::json & electrophoresisObj) throw(std::invalid_argument);
    int incubateOperation(const nlohmann::json & incubateObj) throw(std::invalid_argument);
    int centrifugationOperation(const nlohmann::json & centrifugationObj) throw(std::invalid_argument);
    int thermocyclingOperation(const nlohmann::json & thermocyclingObj) throw(std::invalid_argument);
    int measurementOperation(const nlohmann::json & measurementObj) throw(std::invalid_argument);
    int sangerSequencingOperation(const nlohmann::json & sangerSequencingObj) throw(std::invalid_argument);
    int oligosynthesizeOperation(const nlohmann::json & oligosynthesizeObj) throw(std::invalid_argument);
    int colonyPickingOperation(const nlohmann::json & colonyPickingObj) throw(std::invalid_argument);
    int cellSpreadingOperation(const nlohmann::json & cellSpreadingObj) throw(std::invalid_argument);
    int flashFreezeOperation(const nlohmann::json & flashFreezeObj) throw(std::invalid_argument);
    int mixOperation(const nlohmann::json & mixObj) throw(std::invalid_argument);
    int turbidostatOperation(const nlohmann::json & turObj) throw(std::invalid_argument);
    int flowCitometryOperation(const nlohmann::json & mixObj) throw(std::invalid_argument);
};

#endif // OPERATIONSBLOCKS_H
