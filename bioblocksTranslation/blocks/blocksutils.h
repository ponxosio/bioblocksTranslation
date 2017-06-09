#ifndef BLOCKSUTILS_H
#define BLOCKSUTILS_H

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
#include <utils/utilsjson.h>

class BlocksUtils
{
public:
    static void fillTimeSetting(const nlohmann::json & objJSON, units::Time & initT, units::Time & duration);

    static std::string generateFinishOpVar(int op);
    static std::string generateDurationOpVar(int op);

    static std::string generateEndIfVar(int op);
    static std::string generateTriggeredIfVar(int op, int branchNum);
    static std::string generateElseIfVar(int op);
    static std::string generateTrigerredIfFlagVar(int op);

    static std::string generateTriggerWhileVar(int op);
    static std::string generateEndWhileVar(int op);
    static std::string generateExecutingWhileVar(int op);

    static std::string generateExecutingVar(const std::vector<int> & ops);

    static std::string generateThermocyclingCounter(int op);

    static std::shared_ptr<ComparisonOperable> makeTimeCondition(std::shared_ptr<MathematicOperable> timeVar,
                                                                 std::shared_ptr<MathematicOperable> initTime,
                                                                 std::shared_ptr<MathematicOperable> endTime = NULL);

    virtual ~BlocksUtils(){}
private:
    BlocksUtils(){}
};

#endif // BLOCKSUTILS_H
