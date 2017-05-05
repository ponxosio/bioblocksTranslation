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

std::vector<int> OperationsBlocks::processBlock(const nlohmann::json & blockObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"block_type"}, blockObj);
        std::string blockStr = blockObj["block_type"];

        std::vector<int> opsIds;
        if (blockStr.compare(BIOBLOCKS_PIPETTE_STR) == 0) {
            opsIds = pipetteOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_CONTINUOUS_FLOW_STR) == 0) {
            opsIds = continuousflowOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_ELECTROPHORESIS_STR) == 0) {
            opsIds.push_back(electrophoresisOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_INCUBATE_STR) == 0) {
            opsIds.push_back(incubateOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_CENTRIFUGATION_STR) == 0) {
            opsIds.push_back(centrifugationOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_THERMOCYCLING_STR) == 0) {
            opsIds.push_back(thermocyclingOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_MEASUREMENT_STR) == 0) {
            opsIds.push_back(measurementOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_SANGERSEQUENCING_STR) == 0) {
            opsIds.push_back(sangerSequencingOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_OLIGOSYNTHESIZE_STR) == 0) {
            opsIds.push_back(oligosynthesizeOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_COLONYPICKING_STR) == 0) {
            opsIds.push_back(colonyPickingOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_CELLSPREADING_STR) == 0) {
            opsIds.push_back(cellSpreadingOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_FLASHFREEZE_STR) == 0) {
            opsIds.push_back(flashFreezeOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_MIX_STR) == 0) {
            opsIds.push_back(mixOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_TURBIDOSTAT_STR) == 0) {
            opsIds.push_back(turbidostatOperation(blockObj));
        } else if (blockStr.compare(BIOBLOCKS_FLOWCITOMETRY_STR) == 0) {
            opsIds.push_back(flowCitometryOperation(blockObj));
        } else {
            throw(std::invalid_argument("Unknow block type: \"" + blockStr + "\""));
        }
        return opsIds;
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("OperationsBlocks::processBlock. " + std::string(e.what())));
    }
}

std::vector<int> OperationsBlocks::pipetteOperation(const nlohmann::json & pipetteObj) throw(std::invalid_argument) {
    units::Time initTime;
    BlocksUtils::fillTimeSetting(pipetteObj, initTime);
    try {
        checkPropertiesExists(std::vector<std::string>{"pipetteTypeName", "source", "destination", "aspirationspeed", "dispensespeed", "mixafterbefore"}, pipetteObj);

        std::vector<int> opsIds;

        std::vector<std::string> sourceContainers = vcManager->processContainerBlock(pipetteObj["source"]);
        std::vector<std::string> destinationContainers = vcManager->processContainerBlock(pipetteObj["destination"]);

        int pipetteType = std::atoi(pipetteObj["pipetteTypeName"]);
        switch (pipetteType) {
        case 1: {//one to one
            ContainerManager::VolumeMap volumeMap = ContainerManager::extractVolume(pipetteObj["destination"]);

            const std::string & sourceName = sourceContainers[0];
            const std::string & destinationName = destinationContainers[0];

            std::shared_ptr<MathematicalOperable> volume = std::get<0>(volumeMap[destinationName]);
            units::Volume volumeUnits = std::get<1>(volumeMap[destinationName]);

            opsIds.push_back(graphPtr->emplaceTransfer(sourceName, destinationName, volume, volumeUnits));
            break;
        } case 2: {//one to many
            ContainerManager::VolumeMap volumeMap = ContainerManager::extractVolume(pipetteObj["destination"]);

            const std::string & sourceName = sourceContainers[0];
            for(const std::string & destinationName : destinationContainers) {
                std::shared_ptr<MathematicalOperable> volume = std::get<0>(volumeMap[destinationName]);
                units::Volume volumeUnits = std::get<1>(volumeMap[destinationName]);

                opsIds.push_back(graphPtr->emplaceTransfer(sourceName, destinationName, volume, volumeUnits));
            }
            break;
        } case 3: {//many to one
            ContainerManager::VolumeMap volumeMap = ContainerManager::extractVolume(pipetteObj["source"]);

            const std::string & sourceName = destinationContainers[0];
            for(const std::string & destinationName : sourceContainers) {
                std::shared_ptr<MathematicalOperable> volume = std::get<0>(volumeMap[destinationName]);
                units::Volume volumeUnits = std::get<1>(volumeMap[destinationName]);

                opsIds.push_back(graphPtr->emplaceTransfer(sourceName, destinationName, volume, volumeUnits));
            }
            break;
        } default:
            throw(std::invalid_argument("unknow pippete type: " + pipetteObj["pipetteTypeName"]));
            break;
        }
        return opsIds;
    } catch (std::exception & e) {
        throw(std::invalid_argument("OperationsBlocks::pipetteOperation. " + std::string(e.what())));
    }
}

std::vector<int> OperationsBlocks::continuousflowOperation(const nlohmann::json & continuousflowObj) throw(std::invalid_argument) {

}

int OperationsBlocks::electrophoresisOperation(const nlohmann::json & electrophoresisObj) throw(std::invalid_argument) {

}

int OperationsBlocks::incubateOperation(const nlohmann::json & incubateObj) throw(std::invalid_argument) {

}

int OperationsBlocks::centrifugationOperation(const nlohmann::json & centrifugationObj) throw(std::invalid_argument) {

}

int OperationsBlocks::thermocyclingOperation(const nlohmann::json & thermocyclingObj) throw(std::invalid_argument) {

}

int OperationsBlocks::measurementOperation(const nlohmann::json & measurementObj) throw(std::invalid_argument) {

}

int OperationsBlocks::sangerSequencingOperation(const nlohmann::json & sangerSequencingObj) throw(std::invalid_argument) {

}

int OperationsBlocks::oligosynthesizeOperation(const nlohmann::json & oligosynthesizeObj) throw(std::invalid_argument) {

}

int OperationsBlocks::colonyPickingOperation(const nlohmann::json & colonyPickingObj) throw(std::invalid_argument) {

}

int OperationsBlocks::cellSpreadingOperation(const nlohmann::json & cellSpreadingObj) throw(std::invalid_argument) {

}

int OperationsBlocks::flashFreezeOperation(const nlohmann::json & flashFreezeObj) throw(std::invalid_argument) {

}

int OperationsBlocks::mixOperation(const nlohmann::json & mixObj) throw(std::invalid_argument) {

}

int OperationsBlocks::turbidostatOperation(const nlohmann::json & turObj) throw(std::invalid_argument) {

}

int OperationsBlocks::flowCitometryOperation(const nlohmann::json & mixObj) throw(std::invalid_argument) {

}

int OperationsBlocks::variablesSet(const nlohmann::json & variableSetObj) throw(std::invalid_argument) {

}

int OperationsBlocks::bioblocksIfOperation(const nlohmann::json & bioblocksIfObj) throw(std::invalid_argument) {

}

int OperationsBlocks::bioblocksWhileOperation(const nlohmann::json & bioblocksWhileObj) throw(std::invalid_argument) {

}
