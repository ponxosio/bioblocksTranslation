#include "operationsblocks.h"

OperationsBlocks::OperationsBlocks(
        std::shared_ptr<ProtocolGraph> graphPtr,
        std::shared_ptr<ContainerManager> vcManager,
        std::shared_ptr<MathBlocks> mathBlocks,
        std::shared_ptr<LogicBlocks> logicBlocks)
{
    this->graphPtr = graphPtr;
    this->vcManager = vcManager;
    this->mathBlocks = mathBlocks;
    this->logicBlocks = logicBlocks;
}

OperationsBlocks::~OperationsBlocks() {

}

void OperationsBlocks::processBlock(const nlohmann::json & blockObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"block_type"}, blockObj);
        std::string blockStr = blockObj["block_type"];

        if (blockStr.compare(BIOBLOCKS_PIPETTE_STR) == 0) {
            pipetteOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_CONTINUOUS_FLOW_STR) == 0) {
            continuousflowOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_ELECTROPHORESIS_STR) == 0) {
            electrophoresisOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_INCUBATE_STR) == 0) {
            incubateOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_CENTRIFUGATION_STR) == 0) {
            centrifugationOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_THERMOCYCLING_STR) == 0) {
            thermocyclingOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_MEASUREMENT_STR) == 0) {
            measurementOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_SANGERSEQUENCING_STR) == 0) {
            sangerSequencingOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_OLIGOSYNTHESIZE_STR) == 0) {
            oligosynthesizeOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_COLONYPICKING_STR) == 0) {
            colonyPickingOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_CELLSPREADING_STR) == 0) {
            cellSpreadingOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_FLASHFREEZE_STR) == 0) {
            flashFreezeOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_MIX_STR) == 0) {
            mixOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_TURBIDOSTAT_STR) == 0) {
            turbidostatOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_FLOWCITOMETRY_STR) == 0) {
            flowCitometryOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_VARIABLE_SET_STR) == 0) {
            variablesSet(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_IF_STR) == 0) {
            bioblocksIfOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_WHILE_STR) == 0) {
            bioblocksWhileOperation(blockObj);
        } else {
            throw(std::invalid_argument("Unknow block type: \"" + blockStr + "\""));
        }
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("OperationsBlocks::processBlock. " + std::string(e.what())));
    }
}

void OperationsBlocks::pipetteOperation(const nlohmann::json & pipetteObj) throw(std::invalid_argument) {

}

void OperationsBlocks::continuousflowOperation(const nlohmann::json & continuousflowObj) throw(std::invalid_argument) {

}

void OperationsBlocks::electrophoresisOperation(const nlohmann::json & electrophoresisObj) throw(std::invalid_argument) {

}

void OperationsBlocks::incubateOperation(const nlohmann::json & incubateObj) throw(std::invalid_argument) {

}

void OperationsBlocks::centrifugationOperation(const nlohmann::json & centrifugationObj) throw(std::invalid_argument) {

}

void OperationsBlocks::thermocyclingOperation(const nlohmann::json & thermocyclingObj) throw(std::invalid_argument) {

}

void OperationsBlocks::measurementOperation(const nlohmann::json & measurementObj) throw(std::invalid_argument) {

}

void OperationsBlocks::sangerSequencingOperation(const nlohmann::json & sangerSequencingObj) throw(std::invalid_argument) {

}

void OperationsBlocks::oligosynthesizeOperation(const nlohmann::json & oligosynthesizeObj) throw(std::invalid_argument) {

}

void OperationsBlocks::colonyPickingOperation(const nlohmann::json & colonyPickingObj) throw(std::invalid_argument) {

}

void OperationsBlocks::cellSpreadingOperation(const nlohmann::json & cellSpreadingObj) throw(std::invalid_argument) {

}

void OperationsBlocks::flashFreezeOperation(const nlohmann::json & flashFreezeObj) throw(std::invalid_argument) {

}

void OperationsBlocks::mixOperation(const nlohmann::json & mixObj) throw(std::invalid_argument) {

}

void OperationsBlocks::turbidostatOperation(const nlohmann::json & turObj) throw(std::invalid_argument) {

}

void OperationsBlocks::flowCitometryOperation(const nlohmann::json & mixObj) throw(std::invalid_argument) {

}

void OperationsBlocks::variablesSet(const nlohmann::json & variableSetObj) throw(std::invalid_argument) {

}

void OperationsBlocks::bioblocksIfOperation(const nlohmann::json & bioblocksIfObj) throw(std::invalid_argument) {

}

void OperationsBlocks::bioblocksWhileOperation(const nlohmann::json & bioblocksWhileObj) throw(std::invalid_argument) {

}
