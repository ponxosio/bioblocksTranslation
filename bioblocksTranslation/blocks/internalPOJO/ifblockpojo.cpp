#include "ifblockpojo.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

IfBlockPOJO::IfBlockPOJO() {
    this->id = -1;
    this->linked = false;
    this->timeSlice = -1*units::s;

    this->initTime = NULL;
    this->endIfVar = NULL;
    this->elseVar = NULL;
    this->endWhileExecutingVar = NULL;
}

IfBlockPOJO::IfBlockPOJO(const IfBlockPOJO & obj) :
    branchesVector(obj.branchesVector), otherIfEnds(obj.otherIfEnds)
{
    this->id = obj.id;
    this->linked = obj.linked;
    this->timeSlice = obj.timeSlice;

    this->initTime = obj.initTime;
    this->endIfVar = obj.endIfVar;
    this->elseVar = obj.elseVar;
    this->endWhileExecutingVar = obj.endWhileExecutingVar;
}

IfBlockPOJO::IfBlockPOJO(
        int id,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar,
        std::vector<std::shared_ptr<VariableEntry>> otherIfEnds,
        std::shared_ptr<VariableEntry> elseVar,
        std::shared_ptr<VariableEntry> endWhileExecutingVar) :
     otherIfEnds(otherIfEnds)
{
    this->id = id;
    this->linked = false;
    this->timeSlice = -1*units::s;

    this->initTime = initTime;
    this->endIfVar = endIfVar;
    this->elseVar = elseVar;
    this->endWhileExecutingVar = endWhileExecutingVar;
}

IfBlockPOJO::IfBlockPOJO(
        int id,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar,
        std::vector<std::shared_ptr<VariableEntry>> otherIfEnds,
        units::Time timeSlice,
        std::shared_ptr<VariableEntry> elseVar,
        std::shared_ptr<VariableEntry> endWhileExecutingVar) :
    otherIfEnds(otherIfEnds)
{
    this->id = id;
    this->linked = true;
    this->timeSlice = timeSlice;

    this->initTime = initTime;
    this->endIfVar = endIfVar;
    this->elseVar = elseVar;
    this->endWhileExecutingVar = endWhileExecutingVar;
}

IfBlockPOJO::~IfBlockPOJO() {

}

void IfBlockPOJO::appendOperationsToGraphs(std::shared_ptr<ProtocolGraph> graphPtr, std::shared_ptr<LogicBlocksManager> logicManager) const throw(std::runtime_error) {
    if (!branchesVector.empty()) {
        std::shared_ptr<MathematicOperable> timeVar = graphPtr->getTimeVariable();;
        std::shared_ptr<ComparisonOperable> timeCondition = BlocksUtils::makeTimeCondition(timeVar,initTime);

        if (linked) {
            double timeValue = Utils::toDefaultUnits(timeSlice);
            timeVar = MF::add(timeVar, MF::getNum(Utils::pround(timeValue, 5))); //set the precision to 5 decimal to avoid floating point representation errors
        }

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

        int logicBlockStartId = graphPtr->getNextAvailableNodeId();
        if (logicManager != NULL) {
            logicManager->addIfLogicBlockStartId(logicBlockStartId);
            logicManager->addIfBranchTriggerVar(logicBlockStartId, branchTrigerred);
            logicManager->addIfExecutingFlagVar(logicBlockStartId, std::dynamic_pointer_cast<VariableEntry>(triggeredFlag));
            if (branchCondition->isPhysical()) {
                logicManager->setLogicBlockIdPhyscal(logicBlockStartId);
            }
        }

        graphPtr->startIfBlock(branchCondition);
        graphPtr->appendOperations(setBranchTrigerred);
        ++it;

        while (it != branchesVector.end()) {
            branchCondition = std::get<0>(*it);
            branchTrigerred = std::get<1>(*it);

            if (logicManager != NULL) {
                logicManager->addIfBranchTriggerVar(logicBlockStartId, branchTrigerred);
                if (branchCondition->isPhysical()) {
                    logicManager->setLogicBlockIdPhyscal(logicBlockStartId);
                }
            }

            int setActualBranchTrigerred = graphPtr->emplaceAssignation(branchTrigerred->toString(), timeVar);
            graphPtr->startElIfBlock(branchCondition);
            graphPtr->appendOperations(setActualBranchTrigerred);

            ++it;
        }

        graphPtr->startElseBlock();
        if (elseVar != NULL) {
            int setElseTrigerred = graphPtr->emplaceAssignation(elseVar->toString(), timeVar);
            graphPtr->appendOperations(setElseTrigerred);

            if(logicManager != NULL) {
                logicManager->addIfBranchTriggerVar(logicBlockStartId, elseVar);
            }

        } else {
            int setEnfIfTime = graphPtr->emplaceAssignation(endIfVar->toString(), timeVar);
            graphPtr->appendOperations(setEnfIfTime);

            for(std::shared_ptr<VariableEntry> otherIf : otherIfEnds) {
                int setOtherIfEndTime = graphPtr->emplaceAssignation(otherIf->toString(), timeVar);
                graphPtr->appendOperations(setOtherIfEndTime);

                if(logicManager != NULL) {
                    logicManager->addIfEndVar(logicBlockStartId, otherIf);
                }
            }
        }

        if(logicManager != NULL) {
            logicManager->addIfEndVar(logicBlockStartId, endIfVar);
        }

        if (endWhileExecutingVar != NULL) {
            int unsetExecutingWhile = graphPtr->emplaceAssignation(endWhileExecutingVar->toString(), MF::getNum(0));
            graphPtr->appendOperations(unsetExecutingWhile);

            if(logicManager != NULL) {
                logicManager->addIfEndVar(logicBlockStartId, endWhileExecutingVar);
            }
        }

        graphPtr->endIfBlock();
        graphPtr->endIfBlock();
    } else {
        throw(std::runtime_error("IfBlockPOJO::appendOperationsToGraphs. No branches to process"));
    }
}

















