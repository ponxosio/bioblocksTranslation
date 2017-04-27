#include "blocksutils.h"

using json = nlohmann::json;

void BlocksUtils::checkPropertiesExists(const std::vector<std::string> & properties, const nlohmann::json & objJSON) throw(std::invalid_argument) {
    for(const std::string & property : properties) {
        if (objJSON[property].is_null()) {
            throw(std::invalid_argument(generateNoPropertyErrorMsg(objJSON, property)));
        }
    }
}

std::string BlocksUtils::generateNoPropertyErrorMsg(const nlohmann::json & objJSON, const std::string & property) {
    std::stringstream stream;
    stream << objJSON;
    return stream.str() + "JSON has no property \"" + property + "\"";
}

units::Volume BlocksUtils::getVolumeUnits(const std::string & unitsStr) throw(std::invalid_argument) {
    units::Volume vol;
    if (unitsStr.compare(UNITS_VOLUME_L) == 0) {
        vol = units::l;
    } else if (unitsStr.compare(UNITS_VOLUME_ML) == 0) {
        vol = units::ml;
    } else if (unitsStr.compare(UNITS_VOLUME_UL) == 0) {
        vol = units::ul;
    } else if (unitsStr.compare(UNITS_VOLUME_NL) == 0) {
        vol = units::nl;
    } else {
        throw(std::invalid_argument("Unknow volume units \"" + unitsStr + "\""));
    }
    return vol;
}
