#include "blocksutils.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

using json = nlohmann::json;

void BlocksUtils::checkPropertiesExists(const std::vector<std::string> & properties, const nlohmann::json & objJSON) throw(std::invalid_argument) {
    for(const std::string & property : properties) {
        if (!hasProperty(property, objJSON)) {
            throw(std::invalid_argument(generateNoPropertyErrorMsg(objJSON, property)));
        }
    }
}

bool BlocksUtils::hasProperty(const std::string & property, const nlohmann::json & objJSON) {
    return (objJSON.find(property) != objJSON.end());
}

std::string BlocksUtils::generateNoPropertyErrorMsg(const nlohmann::json & objJSON, const std::string & property) {
    return jsonObjToStr(objJSON) + "JSON has no property \"" + property + "\"";
}

std::string BlocksUtils::jsonObjToStr(const nlohmann::json & objJSON) {
    std::stringstream stream;
    stream << objJSON;
    return stream.str();
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

units::Temperature BlocksUtils::getTemperatureUnits(const std::string & unitsStr) throw(std::invalid_argument) {
    units::Temperature temp;
    if (unitsStr.compare(UNITS_TEMPERATURE_C) == 0) {
        temp = units::C;
    } else if (unitsStr.compare(UNITS_TEMPERATURE_K) == 0) {
        temp = units::K;
    } else {
        throw(std::invalid_argument("Unknow Temperature units \"" + unitsStr + "\""));
    }
    return temp;
}

units::Frequency BlocksUtils::getFrequencyUnits(const std::string & unitsStr) throw(std::invalid_argument) {
    units::Frequency freq;
    if (unitsStr.compare(UNITS_FREQUENCY_HZ) == 0) {
        freq = units::Hz;
    } else if (unitsStr.compare(UNITS_FREQUENCY_KHZ) == 0) {
        freq = units::kHz;
    } else if (unitsStr.compare(UNITS_FREQUENCY_KHZ) == 0) {
        freq = units::MHz;
    } else {
        throw(std::invalid_argument("Unknow frequency units \"" + unitsStr + "\""));
    }
    return freq;
}

units::Length BlocksUtils::getLengthUnits(const std::string & unitsStr) throw(std::invalid_argument) {
    units::Length len;
    if (unitsStr.compare(UNITS_LENGTH_CM) == 0) {
        len = units::cm;
    } else if (unitsStr.compare(UNITS_LENGTH_M) == 0) {
        len = units::m;
    } else if (unitsStr.compare(UNITS_LENGTH_NM) == 0) {
        len = units::nm;
    } else {
        throw(std::invalid_argument("Unknow length units \"" + unitsStr + "\""));
    }
    return len;
}

units::Time BlocksUtils::getTimeUnits(const std::string & unitsStr) throw(std::invalid_argument) {
    units::Time time;
    if (unitsStr.compare(UNITS_TIME_HR) == 0) {
        time = units::hr;
    } else if (unitsStr.compare(UNITS_TIME_MINUTE) == 0) {
        time = units::minute;
    } else if (unitsStr.compare(UNITS_TIME_MS) == 0) {
        time = units::ms;
    } else if (unitsStr.compare(UNITS_TIME_S) == 0) {
        time = units::s;
    } else {
        throw(std::invalid_argument("Unknow time units \"" + unitsStr + "\""));
    }
    return time;
}

units::ElectricPotential BlocksUtils::getElectricPotentialUnits(const std::string & unitsStr) throw(std::invalid_argument) {
    units::ElectricPotential ep;
    if (unitsStr.compare(UNITS_ELECTRICPOTENTIAL_V) == 0) {
        ep = units::V;
    } else {
        throw(std::invalid_argument("Unknow electric potential units \"" + unitsStr + "\""));
    }
    return ep;
}

void BlocksUtils::fillTimeSetting(const nlohmann::json & objJSON, units::Time & initT, units::Time & duration) {
    try {
        checkPropertiesExists(std::vector<std::string>{"timeOfOperation", "timeOfOperation_units"}, objJSON);

        std::string timeStr = objJSON["timeOfOperation"];
        initT = std::stof(timeStr) * getTimeUnits(objJSON["timeOfOperation_units"]);

        if (hasProperty("duration", objJSON)) {
            checkPropertiesExists(std::vector<std::string>{"duration_units"}, objJSON);

            std::string durationStr = objJSON["duration"];
            duration = std::stof(durationStr) * getTimeUnits(objJSON["duration_units"]);
        } else {
            duration = -1*units::s;
        }
    } catch (std::exception & e) {
        throw(std::invalid_argument("BlocksUtils::fillTimeSetting. " + std::string(e.what())));
    }
}

std::string BlocksUtils::generateFinishOpVar(int op) {
    return "finish_op" + std::to_string(op);
}

std::string BlocksUtils::generateDurationOpVar(int op) {
    return "duration_op" + std::to_string(op);
}

std::string BlocksUtils::generateEndIfVar(int op) {
    return "endIf_" + std::to_string(op);
}

std::string BlocksUtils::generateTriggeredIfVar(int op, int branchNum) {
    return "triggerTimeIf_" + std::to_string(op) + "_" + std::to_string(branchNum);
}

std::string BlocksUtils::generateElseIfVar(int op) {
    return "elseTriggerTime_" + std::to_string(op);
}

std::string BlocksUtils::generateTrigerredIfFlagVar(int op) {
    return "triggerIfFlag_" + std::to_string(op);
}

std::string BlocksUtils::generateTriggerWhileVar(int op) {
    return "triggeredWhile_" + std::to_string(op);
}

std::string BlocksUtils::generateEndWhileVar(int op) {
    return "endWhile_" + std::to_string(op);
}

std::string BlocksUtils::generateExecutingWhileVar(int op) {
    return "executingWhile_" + std::to_string(op);
}

std::string BlocksUtils::generateExecutingVar(const std::vector<int> & ops) {
    std::stringstream stream;
    stream << "executing";
    for(int op : ops) {
        stream << "_";
        stream << op;
    }
    return stream.str();
}

std::string BlocksUtils::generateThermocyclingCounter(int op) {
    return "thermocycling_cycles_count_" + std::to_string(op);
}

std::shared_ptr<ComparisonOperable> BlocksUtils::makeTimeCondition(
        std::shared_ptr<MathematicOperable> timeVar,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<MathematicOperable> endTime)
{
    std::shared_ptr<ComparisonOperable> timeBig = BF::bigEq(timeVar, initTime);
    std::shared_ptr<ComparisonOperable> timeLess = NULL;
    if (endTime != NULL) {
        timeLess = BF::less(timeVar, endTime);
    }
    return BF::makeAnd(timeBig, timeLess);
}








