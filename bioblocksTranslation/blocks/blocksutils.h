#ifndef BLOCKSUTILS_H
#define BLOCKSUTILS_H

#define UNITS_VOLUME_L "l"
#define UNITS_VOLUME_ML "ml"
#define UNITS_VOLUME_UL "ul"
#define UNITS_VOLUME_NL "nl"

#define UNITS_FREQUENCY_HZ "hz"
#define UNITS_FREQUENCY_KHZ "khz"
#define UNITS_FREQUENCY_MHZ "mhz"

#define UNITS_LENGTH_NM "nm"
#define UNITS_LENGTH_M "m"
#define UNITS_LENGTH_CM "cm"

#define UNITS_TIME_MS "ms"
#define UNITS_TIME_S "s"
#define UNITS_TIME_MINUTE "minute"
#define UNITS_TIME_HR "hr"

#define UNITS_LUMINOUS_CD "cd"

#define UNITS_TEMPERATURE_C "c"
#define UNITS_TEMPERATURE_K "k"

#define UNITS_ELECTRICPOTENTIAL_V "V"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

//lib
#include <json.hpp>

//local
#include <protocolGraph/operables/comparison/ComparisonOperable.h>
#include <protocolGraph/operables/comparison/protocolboolf.h>
#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>

#include <utils/units.h>


class BlocksUtils
{
public:
    static void checkPropertiesExists(const std::vector<std::string> & properties, const nlohmann::json & objJSON) throw(std::invalid_argument);
    static std::string generateNoPropertyErrorMsg(const nlohmann::json & objJSON, const std::string & property);
    static std::string jsonObjToStr(const nlohmann::json & objJSON);

    static units::Volume getVolumeUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Temperature getTemperatureUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Frequency getFrequencyUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Length getLengthUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::Time getTimeUnits(const std::string & unitsStr) throw(std::invalid_argument);
    static units::ElectricPotential getElectricPotentialUnits(const std::string & unitsStr) throw(std::invalid_argument);

    static void fillTimeSetting(const nlohmann::json & objJSON, units::Time & initT, units::Time & duration);

    static std::string generateFinishOpVar(int op);
    static std::string generateDurationOpVar(int op);

    static std::string generateEndIfVar(int op);
    static std::string generateTriggeredIfVar(int op, int branchNum);
    static std::string generateElseIfVar(int op);
    static std::string generateTrigerredIfFlagVar(int op);

    static std::string generateTriggerWhileVar(int op);
    static std::string generateEndWhileVar(int op);

    static std::string generateExecutingVar(const std::vector<int> & ops);

    virtual ~BlocksUtils(){}
private:
    BlocksUtils(){}
};

#endif // BLOCKSUTILS_H
