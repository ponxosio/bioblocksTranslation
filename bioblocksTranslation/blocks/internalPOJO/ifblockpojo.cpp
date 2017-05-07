#include "ifblockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

IfBlockPOJO::IfBlockPOJO() {
    this->id = -1;
    this->initTime = NULL;
    this->endIfVar = NULL;
    this->elseVar = NULL;
}

IfBlockPOJO::IfBlockPOJO(const IfBlockPOJO & obj) :
    branchesVector(obj.branchesVector), otherIfEnds(obj.otherIfEnds)
{
    this->id = obj.id;
    this->initTime = obj.initTime;
    this->endIfVar = obj.endIfVar;
    this->elseVar = obj.elseVar;
}

IfBlockPOJO::IfBlockPOJO(
        int id,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar,
        std::vector<std::shared_ptr<VariableEntry>> otherIfEnds,
        std::shared_ptr<VariableEntry> elseVar) :
    otherIfEnds(otherIfEnds)
{
    this->id = id;
    this->initTime = initTime;
    this->endIfVar = endIfVar;
    this->elseVar = elseVar;
}

IfBlockPOJO::~IfBlockPOJO() {

}

void IfBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BF::bigEq(timeVar, initTime);

    std::string triggeredFlagName = BlocksUtils::generateTrigerredIfFlagVar(id);
    std::shared_ptr<MathematicOperable> triggeredFlag = graphPtr->getVariable(triggeredFlagName);
    std::shared_ptr<ComparisonOperable> notTrigerred = BF::equal(triggeredFlag, MF::getNum(0));

    int settrigerredFlagOp = graphPtr->emplaceAssignation(triggeredFlagName, MF::getNum(1));

    graphPtr->startIfBlock(BF::makeAnd(timeCondition, notTrigerred));
    graphPtr->appendOperations(settrigerredFlagOp);

    auto it = branchesVector.begin();
    std::shared_ptr<ComparisonOperable> branchCondition = std::get<0>(*it);
    std::shared_ptr<VariableEntry> branchTrigerred = std::get<1>(*it);

    int setBranchTrigerred = graphPtr->emplaceAssignation(branchTrigerred->toString(), timeVar);
    graphPtr->startIfBlock(branchCondition);
    graphPtr->appendOperations(setBranchTrigerred);
    ++it;

    while (it != branchesVector.end()) {
        branchCondition = std::get<0>(*it);
        branchTrigerred = std::get<1>(*it);

        int setActualBranchTrigerred = graphPtr->emplaceAssignation(branchTrigerred->toString(), timeVar);
        graphPtr->startElIfBlock(branchCondition);
        graphPtr->appendOperations(setActualBranchTrigerred);

        ++it;
    }

    graphPtr->startElseBlock();
    if (elseVar != NULL) {
        int setElseTrigerred = graphPtr->emplaceAssignation(elseVar->toString(), timeVar);
        graphPtr->appendOperations(setElseTrigerred);
    } else {
        int setEnfIfTime = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
        graphPtr->appendOperations(setEnfIfTime);

        for(std::shared_ptr<VariableEntry> otherIf : otherIfEnds) {
            int setOtherIfEndTime = graphPtr->emplaceAssignation(otherIf->toString(), timeVar);
            graphPtr->appendOperations(setOtherIfEndTime);
        }
    }
    graphPtr->endIfBlock();
    graphPtr->endIfBlock();
}

















