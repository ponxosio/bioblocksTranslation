#ifndef WHILEBLOCKPOJO_H
#define WHILEBLOCKPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/comparison/ComparisonOperable.h>
#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"

class WhileBlockPOJO : public BlockPOJOInterface
{
public:
    WhileBlockPOJO() {
        this->id = -1;
        this->condition = NULL;
        this->initTime = NULL;
        this->initVar = NULL;
        this->endVar = NULL;
    }

    WhileBlockPOJO(const WhileBlockPOJO & obj) {
        this->id = obj.id;
        this->condition = obj.condition;
        this->initTime = obj.initTime;
        this->initVar = obj.initVar;
        this->endVar = obj.endVar;
    }

    WhileBlockPOJO(
            int id,
            std::shared_ptr<ComparisonOperable> condition,
            std::shared_ptr<MathematicOperable> initTime,
            std::shared_ptr<VariableEntry> initVar,
            std::shared_ptr<VariableEntry> endVar)
    {
        this->id = id;
        this->condition = condition;
        this->initTime = initTime;
        this->initVar = initVar;
        this->endVar = endVar;
    }

    virtual ~WhileBlockPOJO() {}

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

protected:
    int id;
    std::shared_ptr<ComparisonOperable> condition;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> initVar;
    std::shared_ptr<VariableEntry> endVar;
};

#endif // WHILEBLOCKPOJO_H
