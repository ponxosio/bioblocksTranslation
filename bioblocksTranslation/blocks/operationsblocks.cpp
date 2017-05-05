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
            opsIds = incubateOperation(blockObj);
        } else if (blockStr.compare(BIOBLOCKS_CENTRIFUGATION_STR) == 0) {
            opsIds = centrifugationOperation(blockObj);
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

            int transferId = graphPtr->getNextAvailableNodeId();
            opsIds.push_back(graphPtr->emplaceTransfer(sourceName, destinationName, volume, volumeUnits, BlocksUtils::generateDurationOpVar(transferId)));
            break;
        } case 2: {//one to many
            ContainerManager::VolumeMap volumeMap = ContainerManager::extractVolume(pipetteObj["destination"]);

            const std::string & sourceName = sourceContainers[0];
            for(const std::string & destinationName : destinationContainers) {
                std::shared_ptr<MathematicalOperable> volume = std::get<0>(volumeMap[destinationName]);
                units::Volume volumeUnits = std::get<1>(volumeMap[destinationName]);

                int transferId = graphPtr->getNextAvailableNodeId();
                opsIds.push_back(graphPtr->emplaceTransfer(sourceName, destinationName, volume, volumeUnits, BlocksUtils::generateDurationOpVar(transferId)));
            }
            break;
        } case 3: {//many to one
            ContainerManager::VolumeMap volumeMap = ContainerManager::extractVolume(pipetteObj["source"]);

            const std::string & sourceName = destinationContainers[0];
            for(const std::string & destinationName : sourceContainers) {
                std::shared_ptr<MathematicalOperable> volume = std::get<0>(volumeMap[destinationName]);
                units::Volume volumeUnits = std::get<1>(volumeMap[destinationName]);

                int transferId = graphPtr->getNextAvailableNodeId();
                opsIds.push_back(graphPtr->emplaceTransfer(sourceName, destinationName, volume, volumeUnits, BlocksUtils::generateDurationOpVar(transferId)));
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
    try {
        checkPropertiesExists(std::vector<std::string>{"continuosflow_type", "source", "destination", "duration", "duration_units"}, continuousflowObj);

        std::vector<int> opsIds;

        std::vector<std::string> sourceContainers = vcManager->processContainerBlock(continuousflowObj["source"]);
        std::vector<std::string> destinationContainers = vcManager->processContainerBlock(continuousflowObj["destination"]);

        int flowType = std::atoi(continuousflowObj["continuosflow_type"]);
        switch (flowType) {
        case 1: {//one to one
            ContainerManager::RateMap rateMap = ContainerManager::extractRate(continuousflowObj["destination"]);

            const std::string & sourceName = sourceContainers[0];
            const std::string & destinationName = destinationContainers[0];

            std::shared_ptr<MathematicalOperable> rate = std::get<0>(rateMap[destinationName]);
            units::Volumetric_Flow rateUnits = std::get<1>(rateMap[destinationName]);

            opsIds.push_back(graphPtr->emplaceSetContinuousFlow(sourceName, destinationName, rate, rateUnits));
            break;
        } case 2: {//one to many
            ContainerManager::VolumeMap rateMap = ContainerManager::extractRate(continuousflowObj["destination"]);

            const std::string & sourceName = sourceContainers[0];
            for(const std::string & destinationName : destinationContainers) {
                std::shared_ptr<MathematicalOperable> rate = std::get<0>(rateMap[destinationName]);
                units::Volumetric_Flow rateUnits = std::get<1>(rateMap[destinationName]);

                opsIds.push_back(graphPtr->emplaceSetContinuousFlow(sourceName, destinationName, rate, rateUnits));
            }
            break;
        } case 3: {//many to one
            ContainerManager::VolumeMap rateMap = ContainerManager::extractRate(continuousflowObj["source"]);

            const std::string & sourceName = sourceContainers[0];
            for(const std::string & destinationName : destinationContainers) {
                std::shared_ptr<MathematicalOperable> rate = std::get<0>(rateMap[destinationName]);
                units::Volume rateUnits = std::get<1>(rateMap[destinationName]);

                opsIds.push_back(graphPtr->emplaceSetContinuousFlow(sourceName, destinationName, rate, rateUnits));
            }
            break;
        } default:
            throw(std::invalid_argument("unknow pippete type: " + continuousflowObj["continuosflow_type"]));
            break;
        }
        return opsIds;
    } catch (std::exception & e) {
        throw(std::invalid_argument("OperationsBlocks::continuousflowOperation. " + std::string(e.what())));
    }
}

int OperationsBlocks::electrophoresisOperation(const nlohmann::json & electrophoresisObj) throw(std::invalid_argument) {
    try {
        checkPropertiesExists(std::vector<std::string>{
                                  "source",
                                  "ladder",
                                  "field_strength",
                                  "field_strength_units_v",
                                  "field_strength_units_l",
                                  "data_reference"}, electrophoresisObj);

        std::string sourceContainer = vcManager->processContainerBlock(electrophoresisObj["source"])[0];
        std::shared_ptr<MathematicOperable> fieldStrength = mathBlocks->translateMathBlock(electrophoresisObj["field_strength"]);

        units::ElectricField fieldUnits = BlocksUtils::getElectricPotentialUnits(electrophoresisObj["field_strength_units_v"]) /
                                            BlocksUtils::getLengthUnits(electrophoresisObj["field_strength_units_l"]);

        std::shared_ptr<VariableEntry> variableEntry =
                std::dynamic_pointer_cast<VariableEntry>(mathBlocks->translateMathBlock(electrophoresisObj["data_reference"]));

        return graphPtr->emplaceElectrophoresis(sourceContainer, fieldStrength, fieldUnits, variableEntry);
    } catch (std::exception & e) {
        throw(std::invalid_argument("OperationsBlocks::electrophoresisOperation. " + std::string(e.what())));
    }
}

std::vector<int> OperationsBlocks::incubateOperation(const nlohmann::json & incubateObj) throw(std::invalid_argument) {
    try {
        checkPropertiesExists(std::vector<std::string>{
                                  "source",
                                  "temperature",
                                  "temperature_units",
                                  "c02_percent",
                                  "shaking_speed",
                                  "shaking_speed_units"}, incubateObj);

        std::string sourceContainer = vcManager->processContainerBlock(incubateObj["source"])[0];

        std::shared_ptr<MathematicOperable> shakingSpeed = mathBlocks->translateMathBlock(incubateObj["shaking_speed"]);
        units::Frequency shakingSpeedUnits = BlocksUtils::getTemperatureUnits(incubateObj["shaking_speed_units"]);

        std::shared_ptr<MathematicOperable> temperature = mathBlocks->translateMathBlock(incubateObj["temperature"]);
        units::Temperature temperatureUnits = BlocksUtils::getTemperatureUnits(incubateObj["temperature_units"]);

        int heatOp = graphPtr->emplaceApplyTemperature(sourceContainer, temperature, temperatureUnits);
        int stirOp = graphPtr->emplaceShake(sourceContainer, shakingSpeed, shakingSpeedUnits);

        return std::vector<int>{heatOp, stirOp};
    } catch (std::exception & e) {
        throw(std::invalid_argument("OperationsBlocks::incubateOperation. " + std::string(e.what())));
    }
}

std::vector<int> OperationsBlocks::centrifugationOperation(const nlohmann::json & centrifugationObj) throw(std::invalid_argument) {
    try {
        checkPropertiesExists(std::vector<std::string>{
                                  "source",
                                  "speed",
                                  "speed_units",
                                  "temperature",
                                  "temperature_units"}, centrifugationObj);

        std::string sourceContainer = vcManager->processContainerBlock(centrifugationObj["source"])[0];

        std::shared_ptr<MathematicOperable> speed = mathBlocks->translateMathBlock(centrifugationObj["speed"]);
        units::Frequency speedUnits = BlocksUtils::getTemperatureUnits(centrifugationObj["speed_units"]);

        std::shared_ptr<MathematicOperable> temperature = mathBlocks->translateMathBlock(centrifugationObj["temperature"]);
        units::Temperature temperatureUnits = BlocksUtils::getTemperatureUnits(centrifugationObj["temperature_units"]);

        int heatOp = graphPtr->emplaceApplyTemperature(sourceContainer, temperature, temperatureUnits);
        int centrifugateOp = graphPtr->emplaceCentrifugate(sourceContainer, speed, speedUnits);

        return std::vector<int>{heatOp, centrifugateOp};
    } catch (std::exception & e) {
        throw(std::invalid_argument("OperationsBlocks::centrifugationOperation. " + std::string(e.what())));
    }
}

int OperationsBlocks::measurementOperation(const nlohmann::json & measurementObj) throw(std::invalid_argument) {
    try {
        checkPropertiesExists(std::vector<std::string>{
                                  "source",
                                  "measurement_type",
                                  "measurement_frequency",
                                  "unit_frequency",
                                  "data_reference"}, measurementObj);

        std::string sourceContainer = vcManager->processContainerBlock(measurementObj["source"])[0];

        std::shared_ptr<MathematicOperable> measureFrequency = mathBlocks->translateMathBlock(measurementObj["measurement_frequency"]);
        units::Frequency measureUnits = BlocksUtils::getTemperatureUnits(measurementObj["unit_frequency"]);

        std::shared_ptr<VariableEntry> dataReference = std::dynamic_pointer_cast<VariableEntry>
                                                        (mathBlocks->translateMathBlock(measurementObj["data_reference"]));

        int opId = -1;
        int measureType = std::atoi(measurementObj["measurement_type"]);
        switch (measureType) {
        case 1: { //absorbance
            checkPropertiesExists(std::vector<std::string>{"wavelengthnum","wavelengthnum_units"}, measurementObj);

            std::shared_ptr<MathematicOperable> waveLength = mathBlocks->translateMathBlock(measurementObj["wavelengthnum"]);
            units::Length waveLengthUnits = BlocksUtils::getLengthUnits(measurementObj["wavelengthnum_units"]);

            opId = graphPtr->emplaceMeasureOD(sourceContainer, dataReference->toString(),
                                              measureFrequency, measureUnits, waveLength, waveLengthUnits);
            break;
        } case 2: {//fluorescence
            checkPropertiesExists(std::vector<std::string>{
                                      "excitation",
                                      "excitation_units",
                                      "emission",
                                      "emission_units"
                                  }, measurementObj);

            std::shared_ptr<MathematicOperable> excitation = mathBlocks->translateMathBlock(measurementObj["excitation"]);
            units::Length excitationUnits = BlocksUtils::getLengthUnits(measurementObj["excitation_units"]);

            std::shared_ptr<MathematicOperable> emission = mathBlocks->translateMathBlock(measurementObj["emission"]);
            units::Length emissionUnits = BlocksUtils::getLengthUnits(measurementObj["emission_units"]);

            opId = graphPtr->emplaceMeasureFluorescence(sourceContainer, dataReference->toString(),
                                              measureFrequency, measureUnits, excitation, excitationUnits, emission, emissionUnits);
            break;
        } case 3: { //luminiscence
            opId = graphPtr->emplaceMeasureLuminiscence(sourceContainer, dataReference->toString(), measureFrequency, measureUnits);
            break;
        } case 4: { //volume
            opId = graphPtr->emplaceMeasureVolume(sourceContainer, dataReference->toString(), measureFrequency, measureUnits);
            break;
        } case 5: { //temperature
            opId = graphPtr->emplaceMeasureTemperature(sourceContainer, dataReference->toString(), measureFrequency, measureUnits);
            break;
        } default:
            throw(std::invalid_argument("Unknow measurement type: " + std::to_string(measureType)));
            break;
        }
        return opId;
    } catch (std::exception & e) {
        throw(std::invalid_argument("OperationsBlocks::measurementOperation. " + std::string(e.what())));
    }
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
    try {
        checkPropertiesExists(std::vector<std::string>{
                                  "source",
                                  "mix_speed",
                                  "mix_speed_units",
                                  "type"}, mixObj);

        std::string sourceContainer = vcManager->processContainerBlock(mixObj["source"])[0];

        std::shared_ptr<MathematicOperable> speed = mathBlocks->translateMathBlock(mixObj["mix_speed"]);
        units::Frequency speedUnits = BlocksUtils::getTemperatureUnits(mixObj["mix_speed_units"]);

        int opId = -1;
        int mixType = std::atoi(measurementObj["type"]);
        switch (mixType) {
        case 1: { //vortex
            opId = graphPtr->emplaceStir(sourceContainer, speed, speedUnits);
            break;
        } case 2: { //shake
            opId = graphPtr->emplaceShake(sourceContainer, speed, speedUnits);
            break;
        } default:
            throw(std::invalid_argument("Unknow mix type: " + std::to_string(mixType)));
            break;
        }
        return opId;
    } catch (std::exception & e) {
        throw(std::invalid_argument("OperationsBlocks::mixOperation. " + std::string(e.what())));
    }
}

int OperationsBlocks::turbidostatOperation(const nlohmann::json & turObj) throw(std::invalid_argument) {

}

int OperationsBlocks::flowCitometryOperation(const nlohmann::json & mixObj) throw(std::invalid_argument) {

}
