#ifndef IFBLOCKPOJO_H
#define IFBLOCKPOJO_H

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <protocolGraph/operables/comparison/ComparisonOperable.h>
#include <protocolGraph/operables/comparison/protocolboolf.h>
#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>

#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/logicblockpojointerface.h"
#include "bioblocksTranslation/blocks/blocksutils.h"

class IfBlockPOJO : public LogicBlockPOJOInterface
{
public:
    typedef std::tuple<std::shared_ptr<ComparisonOperable>,std::shared_ptr<VariableEntry>> BranchTuple;

    IfBlockPOJO();
    IfBlockPOJO(const IfBlockPOJO & obj);
    IfBlockPOJO(int id,
                std::shared_ptr<MathematicOperable> initTime,
                std::shared_ptr<VariableEntry> endIfVar,
                std::vector<std::shared_ptr<VariableEntry>> otherIfEnds,
                std::shared_ptr<VariableEntry> elseVar = NULL,
                std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL);

    IfBlockPOJO(int id,
                std::shared_ptr<MathematicOperable> initTime,
                std::shared_ptr<VariableEntry> endIfVar,
                std::vector<std::shared_ptr<VariableEntry>> otherIfEnds,
                units::Time timeSlice,
                std::shared_ptr<VariableEntry> elseVar = NULL,
                std::shared_ptr<VariableEntry> endWhileExecutingVar = NULL);


    virtual ~IfBlockPOJO();

    inline int getId() const {
        return id;
    }

    inline const std::vector<BranchTuple> & getBranches() const {
        return branchesVector;
    }

    inline virtual std::shared_ptr<MathematicOperable> getEndVariable() const {
        return endIfVar;
    }


    inline void pushBranchTuple(std::shared_ptr<ComparisonOperable> condition, std::shared_ptr<VariableEntry> trigerVariable) {
        branchesVector.push_back(std::make_tuple(condition, trigerVariable));
    }

    virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr, std::shared_ptr<LogicBlocksManager> logicManager) const throw(std::runtime_error);

protected:
    int id;
    bool linked;
    units::Time timeSlice;

    std::vector<BranchTuple> branchesVector;
    std::shared_ptr<VariableEntry> elseVar;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> endIfVar;
    std::vector<std::shared_ptr<VariableEntry>> otherIfEnds;
    std::shared_ptr<VariableEntry> endWhileExecutingVar;
};

#endif // IFBLOCKPOJO_H
