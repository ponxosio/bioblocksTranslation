#include "whileblockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

WhileBlockPOJO::WhileBlockPOJO() {
    this->id = -1;
    this->linked = false;
    this->timeSlice = -1*units::s;

    this->condition = NULL;
    this->initTime = NULL;
    this->initVar = NULL;
    this->endVar = NULL;
    this->endWhileExecutingVar = NULL;
}

WhileBlockPOJO::WhileBlockPOJO(const WhileBlockPOJO & obj) {
    this->id = obj.id;
    this->linked = obj.linked;
    this->timeSlice = obj.timeSlice;

    this->condition = obj.condition;
    this->initTime = obj.initTime;
    this->initVar = obj.initVar;
    this->endVar = obj.endVar;
    this->endWhileExecutingVar = obj.endWhileExecutingVar;
}

WhileBlockPOJO::WhileBlockPOJO(
        int id,
        std::shared_ptr<ComparisonOperable> condition,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> initVar,
        std::shared_ptr<VariableEntry> endVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVector,
        std::shared_ptr<VariableEntry> endWhileExecutingVar) :
    endIfVector(endIfVector)
{
    this->id = id;
    this->linked = false;
    this->timeSlice = -1*units::s;

    this->condition = condition;
    this->initTime = initTime;
    this->initVar = initVar;
    this->endVar = endVar;
    this->endWhileExecutingVar = endWhileExecutingVar;
}

WhileBlockPOJO::WhileBlockPOJO(
        int id,
        std::shared_ptr<ComparisonOperable> condition,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> initVar,
        std::shared_ptr<VariableEntry> endVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVector,
        units::Time timeSlice,
        std::shared_ptr<VariableEntry> endWhileExecutingVar) :
    endIfVector(endIfVector)
{
    this->id = id;
    this->linked = true;
    this->timeSlice = timeSlice;

    this->condition = condition;
    this->initTime = initTime;
    this->initVar = initVar;
    this->endVar = endVar;
    this->endWhileExecutingVar = endWhileExecutingVar;
}

WhileBlockPOJO::~WhileBlockPOJO() {

}

void WhileBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr, std::shared_ptr<LogicBlocksManager> logicManager) const throw(std::runtime_error) {
    std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();
    std::shared_ptr<ComparisonOperable> timeCondition = BlocksUtils::makeTimeCondition(timeVar, initTime, getEndVariable());

    if (linked) {
        double timeValue = Utils::toDefaultUnits(timeSlice);
        timeVar = MF::add(timeVar, MF::getNum(Utils::pround(timeValue, 5))); //set the precision to 5 decimal to avoid floating point representation errors
    }

    std::string executingWhileName = BlocksUtils::generateExecutingWhileVar(id);
    std::shared_ptr<ComparisonOperable> executingIsUnset = BF::equal(graphPtr->getVariable(executingWhileName), MF::getNum(0));

    graphPtr->startIfBlock(timeCondition);

    int logicBlockStartId = graphPtr->getNextAvailableNodeId();
    if (logicManager != NULL) {
        logicManager->addWhileLogicBlockStartId(logicBlockStartId);
        logicManager->addWhileTriggerVar(logicBlockStartId, initVar);
        logicManager->addWhileEndVar(logicBlockStartId, endVar);
        logicManager->addWhileExecutingFlagVar(logicBlockStartId, graphPtr->getVariable(executingWhileName));
        if (condition->isPhysical()) {
            logicManager->setLogicBlockIdPhyscal(logicBlockStartId);
        }
    }

    graphPtr->startIfBlock(condition);

    graphPtr->startIfBlock(executingIsUnset);

    int setExecuting = graphPtr->emplaceAssignation(executingWhileName, MF::getNum(1));
    graphPtr->appendOperations(setExecuting);

    int setInitWhileTime = graphPtr->emplaceAssignation(initVar->toString(), timeVar);
    graphPtr->appendOperations(setInitWhileTime);

    graphPtr->endIfBlock();

    graphPtr->startElseBlock();

    int setEndWhileTime = graphPtr->emplaceAssignation(endVar->toString(), timeVar);
    graphPtr->appendOperations(setEndWhileTime);

    for(const auto & endIfVar : endIfVector) {
        int setOtherIfEndTime = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
        graphPtr->appendOperations(setOtherIfEndTime);

        if (logicManager != NULL) {
            logicManager->addWhileEndVar(logicBlockStartId, endIfVar);
        }
    }

    if (endWhileExecutingVar != NULL) {
        int unsetExecutingWhile = graphPtr->emplaceAssignation(endWhileExecutingVar->toString(), MF::getNum(0));
        graphPtr->appendOperations(unsetExecutingWhile);

        if (logicManager != NULL) {
            logicManager->addWhileEndVar(logicBlockStartId, endWhileExecutingVar);
        }
    }

    graphPtr->endIfBlock();
    graphPtr->endIfBlock();
}
