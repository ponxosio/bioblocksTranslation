#include "cpublockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

CpuBlockPOJO::CpuBlockPOJO() {
    opId = -1;
    initTime = NULL;
    endWhileExecutingVar = NULL;
    executedOnlyOnce = true;
}

CpuBlockPOJO::CpuBlockPOJO(const CpuBlockPOJO & bbpojo) :
    endIfVector(bbpojo.endIfVector)
{
    opId = bbpojo.opId;
    initTime = bbpojo.initTime;
    endWhileExecutingVar = bbpojo.endWhileExecutingVar;
    executedOnlyOnce = bbpojo.executedOnlyOnce;
}

CpuBlockPOJO::CpuBlockPOJO(
        int opId,
        std::shared_ptr<MathematicOperable> initTime,
        std::vector<std::shared_ptr<VariableEntry>> endIfVector,
        std::shared_ptr<VariableEntry> endWhileExecutingVar,
        bool executedOnlyOnce) :
    endIfVector(endIfVector)
{
    this->initTime = initTime;
    this->opId = opId;
    this->endWhileExecutingVar = endWhileExecutingVar;
    this->executedOnlyOnce = executedOnlyOnce;
}

CpuBlockPOJO::~CpuBlockPOJO(){

}

void CpuBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const throw(std::runtime_error) {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BlocksUtils::makeTimeCondition(timeVar, initTime);

    std::string executingFlagName = BlocksUtils::generateExecutingVar(std::vector<int>{opId});
    std::shared_ptr<MathematicOperable> executingFlag = graphPtr->getVariable(executingFlagName);
    std::shared_ptr<ComparisonOperable> notExecuting =  executedOnlyOnce ? BF::equal(executingFlag, MF::getNum(0)) : NULL;

    graphPtr->startIfBlock(BF::makeAnd(timeCondition, notExecuting));
    graphPtr->appendOperations(opId);

    if (executedOnlyOnce) {
        int setExecutingFlagOp = graphPtr->emplaceAssignation(executingFlagName, MF::getNum(1));
        graphPtr->appendOperations(setExecutingFlagOp);
    }

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
