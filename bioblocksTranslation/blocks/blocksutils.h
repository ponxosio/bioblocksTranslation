#ifndef BLOCKSUTILS_H
#define BLOCKSUTILS_H

#define UNITS_VOLUME_L "l"
#define UNITS_VOLUME_ML "ml"
#define UNITS_VOLUME_UL "ul"
#define UNITS_VOLUME_NL "nl"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

//lib
#include <json.hpp>

//local
#include <utils/units.h>

class BlocksUtils
{
public:
    static void checkPropertiesExists(const std::vector<std::string> & properties, const nlohmann::json & objJSON) throw(std::invalid_argument);
    static std::string generateNoPropertyErrorMsg(const nlohmann::json & objJSON, const std::string & property);

    static units::Volume getVolumeUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Temperature getTemperatureUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Frequency getFrequencyUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Length getLengthUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Time getTimeUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::ElectricPotential getElectricPotentialUnits(const std::string & unitsStr) throw(std::invalid_argument);

    static void fillTimeSetting(const nlohmann::json & objJSON, units::Time & initT);
    static void fillTimeSetting(const nlohmann::json & objJSON, units::Time & initT, units::Time & duration);

    virtual ~BlocksUtils(){}
private:
    BlocksUtils(){}
};

#endif // BLOCKSUTILS_H
