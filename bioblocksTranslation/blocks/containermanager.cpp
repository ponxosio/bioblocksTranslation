#include "containermanager.h"

using json = nlohmann::json;

ContainerManager::ContainerManager(std::shared_ptr<ProtocolGraph> protocolPtr) {
    this->protocolPtr = protocolPtr;
}

ContainerManager::~ContainerManager() {

}

void ContainerManager::processContainerBlock(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    if(!containerObj["block_type"].is_null()) {
        std::string contStr = containerObj["block_type"];

        if (contStr.compare(SINGLE_CONATINER_BLOCK_STR) == 0) {
            processContainerBlock(containerObj);
        } else if (contStr.compare(LIST_CONATINER_BLOCK_STR) == 0) {
            processContainerList(containerObj);
        } else {
            throw(std::invalid_argument("ContainerManager::processContainerBlock. Unknow block type: \"" + opStr + "\""));
        }
    } else {
        throw(std::invalid_argument("ContainerManager::processContainerBlock. " +
                                    BlocksUtils::generateNoPropertyErrorMsg(containerObj,"block_type")));
    }
}

void ContainerManager::processSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerName","type","destiny"}, containerObj);

        std::string name = containerObj["containerName"];
        if (!protocolPtr->hasVContainer(name)) {
            units::Temperature store = getStoreTemperature(containerObj["destiny"]);
            std::tuple<units::Volume,VirtualContainer::ContainerType> typeTuple = parseType(containerObj["type"]);

            units::Volume initialVolume = -1 * units::ml;
            if (std::get<1>(typeTuple) == VirtualContainer::tube) {
                BlocksUtils::checkPropertiesExists(std::vector<std::string>{"initialVolume","initialVolumeUnits"}, containerObj);
                initialVolume = std::atoi(containerObj["initialVolume"]) * BlocksUtils::getVolumeUnits(containerObj["initialVolumeUnits"]);
            }
            protocolPtr->addVContainer(name, std::get<1>(typeTuple),std::get<0>(typeTuple),initialVolume, store);
        }
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("ContainerManager::processSingleContainer." + std::string(e.what())));
    }
}

void ContainerManager::processContainerList(const nlohmann::json & containerObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"containerList"}, containerObj);
        json contList = containerObj["containerList"];
        for (json::iterator it = contList.begin(); it != contList.end(); ++it) {
            processContainerBlock(*it);
        }
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
    if (capStr.compare(EPPENDORF_TUBE_2_ML_STR) == 0) {
        maxVol = 2.0 * units::ml;
        type = VirtualContainer::tube;
    } else if (capStr.compare(EPPENDORF_TUBE_6_ML_STR) == 0) {
        maxVol = 6.0 * units::ml;
        type = VirtualContainer::tube;
    } else if (capStr.compare(AGAROSE_GEL_STR) == 0) {
        maxVol = 0.03 * units::ml;
        type = VirtualContainer::gel;
    } else if (capStr.compare(PETRI_DISH_STR) == 0) {
        maxVol = 50.0 * units::ml;
        type = VirtualContainer::petri_dish;
    } else if (capStr.compare(WELL_PLATE_96_STR) == 0) {
        maxVol = 0.2 * units::ml;
        type = VirtualContainer::well;
    } else if (capStr.compare(WELL_PLATE_192_STR) == 0) {
        maxVol = 0.2 * units::ml;
        type = VirtualContainer::well;
    } else if (capStr.compare(BEAKER_150_ML_STR) == 0) {
        maxVol = 150.0 * units::ml;
        type = VirtualContainer::tube;
    } else if (capStr.compare(FLASK_150_ML_STR) == 0) {
        maxVol = 150.0 * units::ml;
        type = VirtualContainer::tube;
    } else {
        throw(std::invalid_argument("Unknow conatiner type value : \"" + capStr + "\""));
    }
    return std::make_tuple(maxVol, type);
}
