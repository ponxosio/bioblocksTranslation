#ifndef CONTAINERMANAGER_H
#define CONTAINERMANAGER_H

#define SINGLE_CONATINER_BLOCK_STR "container"
#define LIST_CONATINER_BLOCK_STR "containerList"

#define STORE_AMBIENT_STR "Ambient"
#define STORE_MINUS80_STR "Minus80"
#define STORE_MINUS20_STR "Minus20"
#define STORE_ZERO_STR "Zero"
#define STORE_FOUR_STR "Four"

#define DISCARD_BIO_WASTE_STR "Bio-Waste"
#define DISCARD_CHEMICAL_WASTE_STR "Chemical-Waste"
#define DISCARD_REGULAR_WASTE_STR "Regular-Waste"

#define EPPENDORF_TUBE_2_ML_STR 1
#define EPPENDORF_TUBE_6_ML_STR 2
#define AGAROSE_GEL_STR 201
#define PETRI_DISH_STR 202
#define WELL_PLATE_96_STR 301
#define WELL_PLATE_192_STR 302
#define BEAKER_150_ML_STR 4
#define FLASK_150_ML_STR 5

#include <memory>
#include <stdexcept>
#include <tuple>

//lib
#include <json.hpp>

//local
#include <protocolGraph/ProtocolGraph.h>

#include "bioblocksTranslation/blocks/blocksutils.h"

class ContainerManager
{
public:
    ContainerManager(std::shared_ptr<ProtocolGraph> protocolPtr);
    virtual ~ContainerManager();

    void processContainerBlock(const nlohmann::json & containerObj) throw(std::invalid_argument);

protected:
    std::shared_ptr<ProtocolGraph> protocolPtr;

    void processSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument);
    void processContainerList(const nlohmann::json & containerObj) throw(std::invalid_argument);

    units::Temperature getStoreTemperature(const std::string & storeStr) throw(std::invalid_argument);
    std::tuple<units::Volume,VirtualContainer::ContainerType> parseType(const std::string & typeStr) throw(std::invalid_argument);
};

#endif // CONTAINERMANAGER_H
