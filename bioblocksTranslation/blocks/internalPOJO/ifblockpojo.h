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

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"
#include "bioblocksTranslation/blocks/blocksutils.h"

class IfBlockPOJO : public BlockPOJOInterface
{
public:
    typedef std::tuple<std::shared_ptr<ComparisonOperable>,std::shared_ptr<VariableEntry>> BranchTuple;

    IfBlockPOJO();
    IfBlockPOJO(const IfBlockPOJO & obj);
    IfBlockPOJO(int id,
                std::shared_ptr<MathematicOperable> initTime,
                std::shared_ptr<VariableEntry> endIfVar,
                std::shared_ptr<VariableEntry> otherIfEnds,
                std::shared_ptr<VariableEntry> elseVar = NULL);

    virtual ~IfBlockPOJO();

    inline int getId() const {
        return id;
    }

    inline const std::vector<BranchTuple> & getBranches() const {
        return branchesVector;
    }

    inline std::shared_ptr<VariableEntry> getOtherIfEnds() const {
        return otherIfEnds;
    }

    inline virtual std::shared_ptr<MathematicOperable> getEndVariable() const {
        return endIfVar;
    }


    inline void pushBranchTuple(std::shared_ptr<ComparisonOperable> condition, std::shared_ptr<VariableEntry> trigerVariable) {
        branchesVector.push_back(std::make_tuple(condition, trigerVariable));
    }

    virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const;

protected:
    int id;
    std::vector<BranchTuple> branchesVector;
    std::shared_ptr<VariableEntry> elseVar;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> endIfVar;
    std::shared_ptr<VariableEntry> otherIfEnds;
};

#endif // IFBLOCKPOJO_H
