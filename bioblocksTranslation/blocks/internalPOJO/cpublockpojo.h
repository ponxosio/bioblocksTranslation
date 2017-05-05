#ifndef CPUBLOCKPOJO_H
#define CPUBLOCKPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"

class CpuBlockPOJO
{
public:
    CpuBlockPOJO() {
        opIds = -1;
        initTime = NULL;
    }

    CpuBlockPOJO(const CpuBlockPOJO & bbpojo) {
        opIds = bbpojo.opIds;
        initTime = bbpojo.initTime;
    }

    CpuBlockPOJO(
            int opId,
            std::shared_ptr<MathematicOperable> initTime)
    {
        this->initTime = initTime;
        this->opIds = opId;
    }

    virtual ~CpuBlockPOJO() {}


    inline std::shared_ptr<MathematicOperable> getInitTime() const {
        return initTime;
    }

    inline virtual std::shared_ptr<VariableEntry> getEndVariable() const {
        return initTime;
    }

protected:
    int opIds;
    std::shared_ptr<VariableEntry> initTime;
};

#endif // CPUBLOCKPOJO_H
