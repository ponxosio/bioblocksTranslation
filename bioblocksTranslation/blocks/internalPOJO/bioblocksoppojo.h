#ifndef BIOBLOCKSOPPOJO_H
#define BIOBLOCKSOPPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>
#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"

class BioBlocksOpPOJO : public BlockPOJOInterface
{
    typedef ProtocolMathF MF;
public:
    BioBlocksOpPOJO() {
        opIds = -1;
        duration = NULL;
        initTime = NULL;
        endIfVar = NULL;
    }

    BioBlocksOpPOJO(const BioBlocksOpPOJO & bbpojo) {
        opIds = bbpojo.opIds;
        duration = bbpojo.duration;
        initTime = bbpojo.initTime;
        endIfVar = bbpojo.finishOpVar;
    }

    BioBlocksOpPOJO(
            int opId,
            std::shared_ptr<MathematicOperable> duration,
            std::shared_ptr<MathematicOperable> initTime,
            std::shared_ptr<VariableEntry> endIfVar = NULL)
    {
        this->duration = duration;
        this->initTime = initTime;
        this->endIfVar = endIfVar;

        opIds.push_back(opId);
    }

    BioBlocksOpPOJO(
            std::vector<int> opIds,
            std::shared_ptr<MathematicOperable> duration,
            std::shared_ptr<MathematicOperable> initTime,
            std::shared_ptr<VariableEntry> endIfVar = NULL) :
        opIds(opIds)
    {
        this->duration = duration;
        this->initTime = initTime;
        this->endIfVar = endIfVar;
    }

    virtual ~BioBlocksOpPOJO() {}

    inline const std::vector<int> & getOpIds() const {
        return opIds;
    }

    inline std::shared_ptr<MathematicOperable> getDuration() const {
        return duration;
    }

    inline std::shared_ptr<MathematicOperable> getInitTime() const {
        return initTime;
    }

    inline virtual std::shared_ptr<MathematicOperable> getEndVariable() const {
        return MF::add(initTime,duration);
    }

    inline std::shared_ptr<VariableEntry> getEndIfVar() const {
        return endIfVar;
    }

protected:
    std::vector<int> opIds;
    std::shared_ptr<MathematicOperable> duration;
    std::shared_ptr<MathematicOperable> initTime;
    std::shared_ptr<VariableEntry> endIfVar;


};

#endif // BIOBLOCKSOPPOJO_H
