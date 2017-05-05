#ifndef BLOCKPOJOINTERFACE_H
#define BLOCKPOJOINTERFACE_H

#include <memory>

#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/ProtocolGraph.h>

class BlockPOJOInterface
{
public:
    virtual ~BlockPOJOInterface(){}

    virtual std::shared_ptr<MathematicOperable> getEndVariable() const = 0;
    virtual void appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const = 0;
};

#endif // BLOCKPOJOINTERFACE_H
