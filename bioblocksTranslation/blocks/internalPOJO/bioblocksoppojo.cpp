# include "bioblocksoppojo.h"

typedef ProtocolBoolF BF;

BioBlocksOpPOJO::BioBlocksOpPOJO() {
    duration = NULL;
    initTime = NULL;

}

BioBlocksOpPOJO::BioBlocksOpPOJO(const BioBlocksOpPOJO & bbpojo) :
    endIfVector(bbpojo.endIfVector)
{
    opIds = bbpojo.opIds;
    duration = bbpojo.duration;
    initTime = bbpojo.initTime;
}

BioBlocksOpPOJO::BioBlocksOpPOJO(
        int opId,
        std::shared_ptr<MathematicOperable> duration,
        std::shared_ptr<MathematicOperable> initTime,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar) :
    endIfVector(endIfVar)
{
    this->duration = duration;
    this->initTime = initTime;

    opIds.push_back(opId);
}

BioBlocksOpPOJO::BioBlocksOpPOJO(
        std::vector<int> opIds,
        std::shared_ptr<MathematicOperable> duration,
        std::shared_ptr<MathematicOperable> initTime,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar) :
    opIds(opIds), endIfVector(endIfVar)
{
    this->duration = duration;
    this->initTime = initTime;
}

BioBlocksOpPOJO::~BioBlocksOpPOJO(){

}

void BioBlocksOpPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const throw(std::runtime_error) {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BlocksUtils::makeTimeCondition(timeVar, initTime, getEndVariable());

    std::string executingFlagName = BlocksUtils::generateExecutingVar(opIds);
    std::shared_ptr<MathematicOperable> executingFlag = graphPtr->getVariable(executingFlagName);
    std::shared_ptr<ComparisonOperable> notExecuting = BF::equal(executingFlag, MF::getNum(0));
    std::shared_ptr<ComparisonOperable> executing = BF::equal(executingFlag, MF::getNum(1));

    int setExecutingFlag = graphPtr->emplaceAssignation(executingFlagName, MF::getNum(1));
    int unsetExecutingFlag = graphPtr->emplaceAssignation(executingFlagName, MF::getNum(0));

    graphPtr->startIfBlock(timeCondition);

    graphPtr->startIfBlock(notExecuting);
    graphPtr->appendOperations(setExecutingFlag);

    std::vector<int> finishOpIds;
    for(int op: opIds) {
        int finishOp = graphPtr->emplaceFinishOperation(op);
        finishOpIds.push_back(finishOp);
        graphPtr->appendOperations(op);
    }

    graphPtr->endIfBlock();

    graphPtr->startElIfBlock(executing);
    graphPtr->appendOperations(unsetExecutingFlag);

    for(int op: finishOpIds) {
        graphPtr->appendOperations(op);
    }

    if (!endIfVector.empty()) {
        for(const std::shared_ptr<VariableEntry> & endIfVar : endIfVector) {
            int setEndIfVar = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
            graphPtr->appendOperations(setEndIfVar);
        }
    }
    graphPtr->endIfBlock();
}
