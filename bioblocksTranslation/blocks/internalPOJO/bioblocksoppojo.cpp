# include "bioblocksoppojo.h"

typedef ProtocolBoolF BF;

BioBlocksOpPOJO::BioBlocksOpPOJO() {
    duration = NULL;
    initTime = NULL;
    endIfVar = NULL;
}

BioBlocksOpPOJO::BioBlocksOpPOJO(const BioBlocksOpPOJO & bbpojo) {
    opIds = bbpojo.opIds;
    duration = bbpojo.duration;
    initTime = bbpojo.initTime;
    endIfVar = bbpojo.endIfVar;
}

BioBlocksOpPOJO::BioBlocksOpPOJO(
        int opId,
        std::shared_ptr<MathematicOperable> duration,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
{
    this->duration = duration;
    this->initTime = initTime;
    this->endIfVar = endIfVar;

    opIds.push_back(opId);
}

BioBlocksOpPOJO::BioBlocksOpPOJO(
        std::vector<int> opIds,
        std::shared_ptr<MathematicOperable> duration,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar) :
    opIds(opIds)
{
    this->duration = duration;
    this->initTime = initTime;
    this->endIfVar = endIfVar;
}

BioBlocksOpPOJO::~BioBlocksOpPOJO(){

}

void BioBlocksOpPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BF::makeAnd(BF::bigEq(timeVar, initTime),BF::lessEq(timeVar, getEndVariable()));

    std::string executingFlagName = BlocksUtils::generateExecutingVar(opIds);
    std::shared_ptr<MathematicOperable> executingFlag = graphPtr->getVariable(executingFlagName);
    std::shared_ptr<ComparisonOperable> notExecuting = BF::equal(executingFlag, MF::getNum(0));
    std::shared_ptr<ComparisonOperable> executing = BF::equal(executingFlag, MF::getNum(1));

    std::vector<int> finishOpIds;
    for(int op: opIds) {
        int finishOp = graphPtr->emplaceFinishOperation(op);
        finishOpIds.push_back(finishOp);
    }

    int setExecutingFlag = graphPtr->emplaceAssignation(executingFlagName, MF::getNum(1));
    int unsetExecutingFlag = graphPtr->emplaceAssignation(executingFlagName, MF::getNum(0));

    graphPtr->startIfBlock(BF::makeAnd(timeCondition, notExecuting));
    graphPtr->appendOperations(setExecutingFlag);
    graphPtr->appendOperations(opIds);

    graphPtr->startElIfBlock(executing);
    graphPtr->appendOperations(unsetExecutingFlag);
    graphPtr->appendOperations(finishOpIds);

    if (endIfVar != NULL) {
        int setEndIfVar = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
        graphPtr->appendOperations(setEndIfVar);
    }
    graphPtr->endIfBlock();
}
