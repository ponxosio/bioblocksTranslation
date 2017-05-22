#include "cpublockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

CpuBlockPOJO::CpuBlockPOJO() {
    opId = -1;
    initTime = NULL;
    timeStep = NULL;
    endWhileExecutingVar = NULL;    
}

CpuBlockPOJO::CpuBlockPOJO(const CpuBlockPOJO & bbpojo) :
    endIfVector(bbpojo.endIfVector)
{
    opId = bbpojo.opId;
    initTime = bbpojo.initTime;
    timeStep = bbpojo.timeStep;
    endWhileExecutingVar = bbpojo.endWhileExecutingVar;
}

CpuBlockPOJO::CpuBlockPOJO(
        int opId,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<MathematicOperable> timeStep,
        std::vector<std::shared_ptr<VariableEntry>> endIfVector,
        std::shared_ptr<VariableEntry> endWhileExecutingVar) :
    endIfVector(endIfVector)
{
    this->initTime = initTime;
    this->timeStep = timeStep;
    this->opId = opId;
    this->endWhileExecutingVar = endWhileExecutingVar;
}

CpuBlockPOJO::~CpuBlockPOJO(){

}

void CpuBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const throw(std::runtime_error) {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BlocksUtils::makeTimeCondition(timeVar, initTime, MF::add(initTime, timeStep));

    graphPtr->startIfBlock(timeCondition);
    graphPtr->appendOperations(opId);

    for(const std::shared_ptr<VariableEntry> & endIfVar : endIfVector) {
        int setEndIfTime = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
        graphPtr->appendOperations(setEndIfTime);
    }

    if (endWhileExecutingVar != NULL) {
        int unsetExecutingWhile = graphPtr->emplaceAssignation(endWhileExecutingVar->toString(), MF::getNum(0));
        graphPtr->appendOperations(unsetExecutingWhile);
    }

    graphPtr->endIfBlock();
}
