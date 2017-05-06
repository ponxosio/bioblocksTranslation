#ifndef CPUBLOCKPOJO_H
#define CPUBLOCKPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>
#include <protocolGraph/operables/comparison/protocolboolf.h>

#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"
#include "bioblocksTranslation/blocks/blocksutils.h"

class CpuBlockPOJO : public BlockPOJOInterface
{
public:
    CpuBlockPOJO();
    CpuBlockPOJO(const CpuBlockPOJO & bbpojo);
    CpuBlockPOJO(int opId,
                 std::shared_ptr<MathematicOperable> initTime,
                 std::shared_ptr<VariableEntry> endIfVar = NULL);

    virtual ~CpuBlockPOJO();

    inline std::shared_ptr<MathematicOperable> getInitTime() const {
        return initTime;
    }

    inline std::shared_ptr<VariableEntry> getEndIfVar() const {
        return endIfVar;
    }

    inline virtual std::shared_ptr<MathematicOperable> getEndVariable() const {
        return initTime;
    }

    virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const;

protected:
    int opId;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> endIfVar;
};

#endif // CPUBLOCKPOJO_H
