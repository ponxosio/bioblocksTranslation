#ifndef IFBLOCKPOJO_H
#define IFBLOCKPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/comparison/ComparisonOperable.h>
#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"

class IfBlockPOJO : public BlockPOJOInterface
{
public:
    IfBlockPOJO() {
        this->id = -1;
        this->condition = NULL;
        this->initTime = NULL;
        this->trigeredVar = NULL;
        this->endIfVar = NULL;
    }

    IfBlockPOJO(const IfBlockPOJO & obj) {
        this->id = obj.id;
        this->condition = obj.condition;
        this->initTime = obj.initTime;
        this->trigeredVar = obj.trigeredVar;
        this->endIfVar = obj.endIfVar;
    }

    IfBlockPOJO(
            int id,
            std::shared_ptr<ComparisonOperable> condition,
            std::shared_ptr<MathematicOperable> initTime,
            std::shared_ptr<VariableEntry> trigeredVar,
            std::shared_ptr<VariableEntry> endIfVar)
    {
        this->id = id;
        this->condition = condition;
        this->initTime = initTime;
        this->trigeredVar = trigeredVar;
        this->endIfVar = endIfVar;
    }
    virtual ~IfBlockPOJO() {}

    inline int getId() const {
        return id;
    }

    inline std::shared_ptr<const ComparisonOperable> getCondition() const {
        return condition;
    }

    inline std::shared_ptr<VariableEntry> getTrigeredVar() const {
        return trigeredVar;
    }

    inline virtual std::shared_ptr<MathematicOperable> getEndVariable() const {
        return endIfVar;
    }

protected:
    int id;
    std::shared_ptr<ComparisonOperable> condition;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> trigeredVar;
    std::shared_ptr<VariableEntry> endIfVar;
};

#endif // IFBLOCKPOJO_H
