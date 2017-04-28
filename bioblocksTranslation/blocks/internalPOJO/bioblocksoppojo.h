#ifndef BIOBLOCKSOPPOJO_H
#define BIOBLOCKSOPPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry>
#include <utils/units.h>

class BioBlocksOpPOJO
{
public:
    BioBlocksOpPOJO() {
        opId = -1;
        duration = -1*units::s;
        initTime = NULL;
        finishOpVar = NULL;
        endIfVar = NULL;
    }

    BioBlocksOpPOJO(const BioBlocksOpPOJO & bbpojo) {
        opId = bbpojo.opId;
        duration = bbpojo.duration;
        initTime = bbpojo.initTime;
        finishOpVar = bbpojo.finishOpVar;
        endIfVar = bbpojo.finishOpVar;
    }

    BioBlocksOpPOJO(
            int opId,
            units::Time duration,
            std::shared_ptr<MathematicOperable> initTime,
            std::shared_ptr<VariableEntry> finishOpVar,
            std::shared_ptr<VariableEntry> endIfVar = NULL)
    {
        this->opId = opId;
        this->duration = duration;
        this->initTime = initTime;
        this->finishOpVar = finishOpVar;
        this->endIfVar = endIfVar;
    }

    virtual ~BioBlocksOpPOJO() {}

    inline int getOpId() const {
        return opId;
    }

    inline units::Time getDuration() const {
        return duration;
    }

    inline std::shared_ptr<MathematicOperable> getInitTime() const {
        return initTime;
    }

    inline std::shared_ptr<VariableEntry> getFinishOpVar() const {
        return finishOpVar;
    }

    inline std::shared_ptr<VariableEntry> getEndIfVar() const {
        return endIfVar;
    }

protected:
    int opId;
    units::Time duration;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> finishOpVar;
    std::shared_ptr<VariableEntry> endIfVar;


};

#endif // BIOBLOCKSOPPOJO_H
