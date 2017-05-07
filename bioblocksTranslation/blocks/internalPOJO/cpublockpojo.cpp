#include "cpublockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

CpuBlockPOJO::CpuBlockPOJO() {
    opId = -1;
    initTime = NULL;
}

CpuBlockPOJO::CpuBlockPOJO(const CpuBlockPOJO & bbpojo) :
    endIfVector(bbpojo.endIfVector)
{
    opId = bbpojo.opId;
    initTime = bbpojo.initTime;
}

CpuBlockPOJO::CpuBlockPOJO(
        int opId,
        std::shared_ptr<MathematicOperable> initTime,
        std::vector<std::shared_ptr<VariableEntry>> endIfVector) :
    endIfVector(endIfVector)
{
    this->initTime = initTime;
    this->opId = opId;
}

CpuBlockPOJO::~CpuBlockPOJO(){

}

void CpuBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BF::bigEq(timeVar, initTime);

    std::string executingFlagName = BlocksUtils::generateExecutingVar(std::vector<int>{opId});
    std::shared_ptr<MathematicOperable> executingFlag = graphPtr->getVariable(executingFlagName);
    std::shared_ptr<ComparisonOperable> notExecuting = BF::equal(executingFlag, MF::getNum(0));

    int setExecutingFlagOp = graphPtr->emplaceAssignation(executingFlagName, MF::getNum(1));

    graphPtr->startIfBlock(BF::makeAnd(timeCondition, notExecuting));
    graphPtr->appendOperations(opId);
    graphPtr->appendOperations(setExecutingFlagOp);

    for(const std::shared_ptr<VariableEntry> & endIfVar : endIfVector) {
        int setEndIfTime = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
        graphPtr->appendOperations(setEndIfTime);
    }

    graphPtr->endIfBlock();
}
