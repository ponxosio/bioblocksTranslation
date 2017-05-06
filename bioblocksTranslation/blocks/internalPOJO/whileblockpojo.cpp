#include "whileblockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

WhileBlockPOJO::WhileBlockPOJO() {
    this->id = -1;
    this->condition = NULL;
    this->initTime = NULL;
    this->initVar = NULL;
    this->endVar = NULL;
    this->endIfVar = NULL;
}

WhileBlockPOJO::WhileBlockPOJO(const WhileBlockPOJO & obj) {
    this->id = obj.id;
    this->condition = obj.condition;
    this->initTime = obj.initTime;
    this->initVar = obj.initVar;
    this->endVar = obj.endVar;
    this->endIfVar = obj.endIfVar;
}

WhileBlockPOJO::WhileBlockPOJO(
        int id,
        std::shared_ptr<ComparisonOperable> condition,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> initVar,
        std::shared_ptr<VariableEntry> endVar,
        std::shared_ptr<VariableEntry> endIfVar)
{
    this->id = id;
    this->condition = condition;
    this->initTime = initTime;
    this->initVar = initVar;
    this->endVar = endVar;
    this->endIfVar = endIfVar;
}

WhileBlockPOJO::~WhileBlockPOJO() {

}

void WhileBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BF::makeAnd(BF::bigEq(timeVar, initTime),BF::lessEq(timeVar, getEndVariable()));

    graphPtr->startIfBlock(timeCondition);
    graphPtr->startIfBlock(condition);

    int setInitWhileTime = graphPtr->emplaceAssignation(initVar->toString(), timeVar);
    graphPtr->appendOperations(setInitWhileTime);

    graphPtr->startElseBlock();

    int setEndWhileTime = graphPtr->emplaceAssignation(endVar->toString(), timeVar);
    graphPtr->appendOperations(setEndWhileTime);

    if (endIfVar != NULL) {
        int setOtherIfEndTime = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
        graphPtr->appendOperations(setOtherIfEndTime);
    }

    graphPtr->endIfBlock();
    graphPtr->endIfBlock();
}
