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

#define EPPENDORF_TUBE_2_ML_STR "1"
#define EPPENDORF_TUBE_6_ML_STR "2"
#define AGAROSE_GEL_STR "201"
#define PETRI_DISH_STR "202"
#define WELL_PLATE_96_STR "301"
#define WELL_PLATE_192_STR "302"
#define BEAKER_150_ML_STR "4"
#define FLASK_150_ML_STR "5"

#include <memory>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

//lib
#include <json.hpp>

//local
#include <protocolGraph/ProtocolGraph.h>
#include <utils/utilsjson.h>

#include "bioblocksTranslation/blocks/blocksutils.h"
#include "bioblocksTranslation/blocks/mathblocks.h"

class ContainerManager
{
public:
    typedef std::tuple<double, units::Temperature> TemperatureTuple;
    typedef std::tuple<double, units::Time> TimeTuple;
    typedef std::tuple<TemperatureTuple, TimeTuple> PCRStepTuple;

    typedef std::tuple<std::shared_ptr<MathematicOperable>, units::Volume>  VolumeTuple;
    typedef std::tuple<std::shared_ptr<MathematicOperable>, units::Volumetric_Flow>  RateTuple;

    typedef std::unordered_map<std::string,VolumeTuple>  VolumeMap;
    typedef std::unordered_map<std::string,RateTuple>  RateMap;
    typedef std::vector<PCRStepTuple> PCRStepVector;

    ContainerManager(std::shared_ptr<ProtocolGraph> protocolPtr, std::shared_ptr<MathBlocks> mathBlocks);
    virtual ~ContainerManager();

    std::vector<std::string> processContainerBlock(const nlohmann::json & containerObj) throw(std::invalid_argument);

    VolumeMap  extractVolume(const nlohmann::json & containerObj) throw(std::invalid_argument);
    RateMap  extractRate(const nlohmann::json & containerObj) throw(std::invalid_argument);
    PCRStepVector extractPCRSteps(const nlohmann::json & containerObj) throw(std::invalid_argument);

protected:
    std::shared_ptr<ProtocolGraph> protocolPtr;
    std::shared_ptr<MathBlocks> mathBlocks;

    std::pair<std::string, VolumeTuple> extractVolumeSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument);
    VolumeMap extractVolumeListContainer(const nlohmann::json & containerObj) throw(std::invalid_argument);

    std::pair<std::string, RateTuple> extractRateSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument);
    RateMap extractRateListContainer(const nlohmann::json & containerObj) throw(std::invalid_argument);

    std::string processSingleContainer(const nlohmann::json & containerObj) throw(std::invalid_argument);
    std::vector<std::string> processContainerList(const nlohmann::json & containerObj) throw(std::invalid_argument);

    units::Temperature getStoreTemperature(const std::string & storeStr) throw(std::invalid_argument);
    std::tuple<units::Volume,VirtualContainer::ContainerType> parseType(const std::string & typeStr) throw(std::invalid_argument);
};

#endif // CONTAINERMANAGER_H
