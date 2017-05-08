#include "whileblockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

WhileBlockPOJO::WhileBlockPOJO() {
    this->id = -1;
    this->condition = NULL;
    this->initTime = NULL;
    this->initVar = NULL;
    this->endVar = NULL;
}

WhileBlockPOJO::WhileBlockPOJO(const WhileBlockPOJO & obj) {
    this->id = obj.id;
    this->condition = obj.condition;
    this->initTime = obj.initTime;
    this->initVar = obj.initVar;
    this->endVar = obj.endVar;
}

WhileBlockPOJO::WhileBlockPOJO(
        int id,
        std::shared_ptr<ComparisonOperable> condition,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> initVar,
        std::shared_ptr<VariableEntry> endVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVector) :
    endIfVector(endIfVector)
{
    this->id = id;
    this->condition = condition;
    this->initTime = initTime;
    this->initVar = initVar;
    this->endVar = endVar;
}

WhileBlockPOJO::~WhileBlockPOJO() {

}

void WhileBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr) const throw(std::runtime_error) {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BlocksUtils::makeTimeCondition(timeVar, initVar, getEndVariable());

    graphPtr->startIfBlock(timeCondition);
    graphPtr->startIfBlock(condition);

    int setInitWhileTime = graphPtr->emplaceAssignation(initVar->toString(), timeVar);
    graphPtr->appendOperations(setInitWhileTime);

    graphPtr->startElseBlock();

    int setEndWhileTime = graphPtr->emplaceAssignation(endVar->toString(), timeVar);
    graphPtr->appendOperations(setEndWhileTime);

    for(const auto & endIfVar : endIfVector) {
        int setOtherIfEndTime = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
        graphPtr->appendOperations(setOtherIfEndTime);
    }

    graphPtr->endIfBlock();
    graphPtr->endIfBlock();
}
