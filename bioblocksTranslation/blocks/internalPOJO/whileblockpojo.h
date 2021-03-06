#ifndef WHILEBLOCKPOJO_H
#define WHILEBLOCKPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/comparison/ComparisonOperable.h>
#include <protocolGraph/operables/comparison/protocolboolf.h>
#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>

#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/logicblockpojointerface.h"
#include "bioblocksTranslation/blocks/blocksutils.h"


class WhileBlockPOJO : public LogicBlockPOJOInterface
{
public:
    WhileBlockPOJO();
    WhileBlockPOJO(const WhileBlockPOJO & obj);
    WhileBlockPOJO(int id,
            std::shared_ptr<ComparisonOperable> condition,
            std::shared_ptr<MathematicOperable> initTime,
            std::shared_ptr<VariableEntry> initVar,
            std::shared_ptr<VariableEntry> endVar,
            std::vector<std::shared_ptr<VariableEntry>> endIfVector,
            std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL);
    WhileBlockPOJO(int id,
            std::shared_ptr<ComparisonOperable> condition,
            std::shared_ptr<MathematicOperable> initTime,
            std::shared_ptr<VariableEntry> initVar,
            std::shared_ptr<VariableEntry> endVar,
            std::vector<std::shared_ptr<VariableEntry>> endIfVector,
            units::Time timeSlice,
            std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL);

    virtual ~WhileBlockPOJO();

    inline int getId() const {
        return id;
    }

    inline std::shared_ptr<const ComparisonOperable> getCondition() const {
        return condition;
    }

    inline std::shared_ptr<VariableEntry> getInitVar() const {
        return initVar;
    }

    inline virtual std::shared_ptr<MathematicOperable> getEndVariable() const {
        return endVar;
    }

    virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr, std::shared_ptr<LogicBlocksManager> logicManager) const throw(std::runtime_error);

protected:
    int id;
    bool linked;
    units::Time timeSlice;

    std::shared_ptr<ComparisonOperable> condition;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> initVar;
    std::shared_ptr<VariableEntry> endVar;
    std::vector<std::shared_ptr<VariableEntry>> endIfVector;
    std::shared_ptr<VariableEntry> endWhileExecutingVar;
};

#endif // WHILEBLOCKPOJO_H
