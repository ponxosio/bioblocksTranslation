#include "cpublockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

CpuBlockPOJO::CpuBlockPOJO() {
    opId = -1;
    initTime = NULL;
    endIfVar = NULL;
}

CpuBlockPOJO::CpuBlockPOJO(const CpuBlockPOJO & bbpojo) {
    opId = bbpojo.opId;
    initTime = bbpojo.initTime;
    endIfVar = bbpojo.endIfVar;
}

CpuBlockPOJO::CpuBlockPOJO(
        int opId,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
{
    this->initTime = initTime;
    this->opId = opId;
    this->endIfVar = endIfVar;
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
    graphPtr->endIfBlock();
}
