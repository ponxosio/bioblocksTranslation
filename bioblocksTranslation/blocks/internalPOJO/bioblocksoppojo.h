#ifndef BIOBLOCKSOPPOJO_H
#define BIOBLOCKSOPPOJO_H

#include <memory>
#include <string>

#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/VariableEntry.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>
#include <protocolGraph/operables/comparison/protocolboolf.h>

#include <utils/units.h>

#include "bioblocksTranslation/blocks/internalPOJO/blockpojointerface.h"
#include "bioblocksTranslation/blocks/blocksutils.h"

class BioBlocksOpPOJO : public BlockPOJOInterface
{
    typedef ProtocolMathF MF;
public:
    BioBlocksOpPOJO();
    BioBlocksOpPOJO(const BioBlocksOpPOJO & bbpojo);
    BioBlocksOpPOJO(int opId,
                    std::shared_ptr<MathematicOperable> duration,
                    std::shared_ptr<MathematicOperable> initTime,
                    std::vector<std::shared_ptr<VariableEntry>> endIfVector = std::vector<std::shared_ptr<VariableEntry>>{});
    BioBlocksOpPOJO(std::vector<int> opIds,
                    std::shared_ptr<MathematicOperable> duration,
                    std::shared_ptr<MathematicOperable> initTime,
                    std::vector<std::shared_ptr<VariableEntry>> endIfVector = std::vector<std::shared_ptr<VariableEntry>>{});

    virtual ~BioBlocksOpPOJO();

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

    virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const;

protected:
    std::vector<int> opIds;
    std::shared_ptr<MathematicOperable> duration;
    std::shared_ptr<MathematicOperable> initTime;
    std::vector<std::shared_ptr<VariableEntry>> endIfVector;


};

#endif // BIOBLOCKSOPPOJO_H
