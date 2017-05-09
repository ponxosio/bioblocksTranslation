#include "containermanager.h"

using json = nlohmann::json;

ContainerManager::ContainerManager(std::shared_ptr<ProtocolGraph> protocolPtr, std::shared_ptr<MathBlocks> mathBlocks) {
    this->protocolPtr = protocolPtr;
    this->mathBlocks = mathBlocks;
}

ContainerManager::~ContainerManager() {

}

std::vector<std::string> ContainerManager::processContainerBlock(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("block_type", containerObj)) {
        std::string contStr = containerObj["block_type"];

        std::vector<std::string> containerList;
        if (contStr.compare(SINGLE_CONATINER_BLOCK_STR) == 0) {
            containerList.push_back(processSingleContainer(containerObj));
        } else if (contStr.compare(LIST_CONATINER_BLOCK_STR) == 0) {
            containerList = processContainerList(containerObj);
        } else {
            throw(std::invalid_argument("ContainerManager::processContainerBlock. Unknow block type: \"" + contStr + "\""));
        }
        return containerList;
    } else {
        throw(std::invalid_argument("ContainerManager::processContainerBlock. " +
                                    BlocksUtils::generateNoPropertyErrorMsg(containerObj,"block_type")));
    }
}

ContainerManager::VolumeMap  ContainerManager::extractVolume(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("block_type", containerObj)) {
        std::string contStr = containerObj["block_type"];

        VolumeMap volumeList;
        if (contStr.compare(SINGLE_CONATINER_BLOCK_STR) == 0) {
            volumeList.insert(extractVolumeSingleContainer(containerObj));
        } else if (contStr.compare(LIST_CONATINER_BLOCK_STR) == 0) {
            volumeList = extractVolumeListContainer(containerObj);
        } else {
            throw(std::invalid_argument("ContainerManager::extractVolume. Unknow block type: \"" + contStr + "\""));
        }
        return volumeList;
    } else {
        throw(std::invalid_argument("ContainerManager::extractVolume. " +
                                    BlocksUtils::generateNoPropertyErrorMsg(containerObj,"block_type")));
    }
}

ContainerManager::RateMap  ContainerManager::extractRate(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("block_type", containerObj)) {
        std::string contStr = containerObj["block_type"];

        RateMap rateList;
        if (contStr.compare(SINGLE_CONATINER_BLOCK_STR) == 0) {
            rateList.insert(extractRateSingleContainer(containerObj));
        } else if (contStr.compare(LIST_CONATINER_BLOCK_STR) == 0) {
            rateList = extractRateListContainer(containerObj);
        } else {
            throw(std::invalid_argument("ContainerManager::extractRate. Unknow block type: \"" + contStr + "\""));
        }
        return rateList;
    } else {
        throw(std::invalid_argument("ContainerManager::extractRate. " +
                                    BlocksUtils::generateNoPropertyErrorMsg(containerObj,"block_type")));
    }
}

ContainerManager::PCRStepVector ContainerManager::extractPCRSteps(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"steps"}, containerObj);
        PCRStepVector steps;

        std::string stepsStr = containerObj["steps"];
        int stepsNum = std::stoi(stepsStr);
        for(int i = 0; i < stepsNum; i++) {
            BlocksUtils::checkPropertiesExists(std::vector<std::string>{
                                               "temperature" + std::to_string(i),
                                               "temperature_units" + std::to_string(i),
                                               "duration" + std::to_string(i),
                                               "duration_units" + std::to_string(i)}, containerObj);

            units::Temperature temperatureUnits = BlocksUtils::getTemperatureUnits(containerObj["temperature_units" + std::to_string(i)]);
            std::string tempStr = containerObj["temperature" + std::to_string(i)];
            double temperature = std::stof(tempStr);

            units::Time durationUnits = BlocksUtils::getTimeUnits(containerObj["duration_units" + std::to_string(i)] );
            std::string durationStr =  containerObj["duration" + std::to_string(i)];
            int duration = std::stoi(durationStr);

            steps.push_back(std::make_tuple(std::make_tuple(temperature, temperatureUnits),
                                            std::make_tuple(duration , durationUnits)));
        }
        return steps;
    } catch (std::exception & e) {
        throw(std::invalid_argument("ContainerManager::extractVolumeSingleContainer." + std::string(e.what())));
    }
}

std::pair<std::string, ContainerManager::VolumeTuple> ContainerManager::extractVolumeSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerName", "volume","unit_volume"}, containerObj);

        std::string name = containerObj["containerName"];
        units::Volume volumeUnits = BlocksUtils::getVolumeUnits(containerObj["unit_volume"]);
        std::shared_ptr<MathematicOperable> volume = mathBlocks->translateMathBlock(containerObj["volume"]);

        return std::make_pair(name, std::make_tuple(volume, volumeUnits));
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("ContainerManager::extractVolumeSingleContainer." + std::string(e.what())));
    }
}

ContainerManager::VolumeMap ContainerManager::extractVolumeListContainer(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerList"}, containerObj);
        json contList = containerObj["containerList"];

        VolumeMap volumeMap;
        for (json::iterator it = contList.begin(); it != contList.end(); ++it) {
            VolumeMap localMap = extractVolume(*it);
            for(const auto & pair : localMap) {
                volumeMap.insert(pair);
            }
        }
        return volumeMap;
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("ContainerManager::processSingleContainer." + std::string(e.what())));
    }
}

std::pair<std::string, ContainerManager::RateTuple> ContainerManager::extractRateSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerName", "rate", "rate_volume_units", "rate_time_units"}, containerObj);

        std::string name = containerObj["containerName"];
        units::Volumetric_Flow rateUnits = ( BlocksUtils::getVolumeUnits(containerObj["rate_volume_units"]) / BlocksUtils::getTimeUnits(containerObj["rate_time_units"]));
        std::shared_ptr<MathematicOperable> rate = mathBlocks->translateMathBlock(containerObj["rate"]);

        return std::make_pair(name, std::make_tuple(rate, rateUnits));
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("ContainerManager::extractVolumeSingleContainer." + std::string(e.what())));
    }
}

ContainerManager::RateMap ContainerManager::extractRateListContainer(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerList"}, containerObj);
        json contList = containerObj["containerList"];

        RateMap rateMap;
        for (json::iterator it = contList.begin(); it != contList.end(); ++it) {
            RateMap localMap = extractRate(*it);
            for(const auto & pair : localMap) {
                rateMap.insert(pair);
            }
        }
        return rateMap;
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("ContainerManager::processSingleContainer." + std::string(e.what())));
    }
}

std::string ContainerManager::processSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerName","type","destiny"}, containerObj);

        std::string name = containerObj["containerName"];
        if (!protocolPtr->hasVContainer(name)) {
            units::Temperature store = getStoreTemperature(containerObj["destiny"]);
            std::tuple<units::Volume,VirtualContainer::ContainerType> typeTuple = parseType(containerObj["type"]);

            units::Volume initialVolume = -1 * units::ml;
            if (std::get<1>(typeTuple) == VirtualContainer::tube) {
                BlocksUtils::checkPropertiesExists(std::vector<std::string>{"initialVolume","initialVolumeUnits"}, containerObj);

                std::string initialVolStr = containerObj["initialVolume"];
                initialVolume = std::stod(initialVolStr) * BlocksUtils::getVolumeUnits(containerObj["initialVolumeUnits"]);
            }
            protocolPtr->addVContainer(name, std::get<1>(typeTuple),std::get<0>(typeTuple),initialVolume, store);
        }
        return name;
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("ContainerManager::processSingleContainer." + std::string(e.what())));
    }
}

std::vector<std::string> ContainerManager::processContainerList(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerList"}, containerObj);
        json contList = containerObj["containerList"];

        std::vector<std::string> containerList;
        for (json::iterator it = contList.begin(); it != contList.end(); ++it) {
            std::vector<std::string> localList = processContainerBlock(*it);
            containerList.reserve(containerList.size() + localList.size());
            containerList.insert(containerList.end(), localList.begin(), localList.end());
        }
        return containerList;
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("ContainerManager::processSingleContainer." + std::string(e.what())));
    }
}

units::Temperature ContainerManager::getStoreTemperature(const std::string & storeStr) throw(std::invalid_argument) {
    units::Temperature temp;
    if (storeStr.compare(STORE_AMBIENT_STR) == 0
       (storeStr.compare(DISCARD_BIO_WASTE_STR) == 0) ||
       (storeStr.compare(DISCARD_CHEMICAL_WASTE_STR) == 0) ||
       (storeStr.compare(DISCARD_REGULAR_WASTE_STR) == 0))
    {
        temp = 26.85 * units::C;
    } else if (storeStr.compare(STORE_MINUS80_STR) == 0) {
        temp = -80.0 * units::C;
    } else if (storeStr.compare(STORE_MINUS20_STR) == 0) {
        temp = -20.0 * units::C;
    } else if (storeStr.compare(STORE_ZERO_STR) == 0) {
        temp = 0 * units::C;
    } else if (storeStr.compare(STORE_FOUR_STR) == 0) {
        temp = 4 * units::C;
    } else {
        throw(std::invalid_argument("Unknow store value : \"" + storeStr + "\""));
    }
    return temp;
}

std::tuple<units::Volume,VirtualContainer::ContainerType> ContainerManager::parseType(const std::string & typeStr) throw(std::invalid_argument) {
    units::Volume maxVol;
    VirtualContainer::ContainerType type;
    if (typeStr.compare(EPPENDORF_TUBE_2_ML_STR) == 0) {
        maxVol = 2.0 * units::ml;
        type = VirtualContainer::tube;
    } else if (typeStr.compare(EPPENDORF_TUBE_6_ML_STR) == 0) {
        maxVol = 6.0 * units::ml;
        type = VirtualContainer::tube;
    } else if (typeStr.compare(AGAROSE_GEL_STR) == 0) {
        maxVol = 0.03 * units::ml;
        type = VirtualContainer::gel;
    } else if (typeStr.compare(PETRI_DISH_STR) == 0) {
        maxVol = 50.0 * units::ml;
        type = VirtualContainer::petri_dish;
    } else if (typeStr.compare(WELL_PLATE_96_STR) == 0) {
        maxVol = 0.2 * units::ml;
        type = VirtualContainer::well;
    } else if (typeStr.compare(WELL_PLATE_192_STR) == 0) {
        maxVol = 0.2 * units::ml;
        type = VirtualContainer::well;
    } else if (typeStr.compare(BEAKER_150_ML_STR) == 0) {
        maxVol = 150.0 * units::ml;
        type = VirtualContainer::tube;
    } else if (typeStr.compare(FLASK_150_ML_STR) == 0) {
        maxVol = 150.0 * units::ml;
        type = VirtualContainer::tube;
    } else {
        throw(std::invalid_argument("Unknow conatiner type value : \"" + typeStr + "\""));
    }
    return std::make_tuple(maxVol, type);
}
