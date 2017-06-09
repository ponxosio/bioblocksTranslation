#include "blocksutils.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

using json = nlohmann::json;

void BlocksUtils::fillTimeSetting(const nlohmann::json & objJSON, units::Time & initT, units::Time & duration) {
    try {
        UtilsJSON::checkPropertiesExists(std::vector<std::string>{"timeOfOperation", "timeOfOperation_units"}, objJSON);

        std::string timeStr = objJSON["timeOfOperation"];
        initT = std::stof(timeStr) * UtilsJSON::getTimeUnits(objJSON["timeOfOperation_units"]);

        if (UtilsJSON::hasProperty("duration", objJSON)) {
            UtilsJSON::checkPropertiesExists(std::vector<std::string>{"duration_units"}, objJSON);

            std::string durationStr = objJSON["duration"];
            duration = std::stof(durationStr) * UtilsJSON::getTimeUnits(objJSON["duration_units"]);
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








