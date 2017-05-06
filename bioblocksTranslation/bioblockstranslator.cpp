#include "bioblockstranslator.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

using json = nlohmann::json;

BioBlocksTranslator::BioBlocksTranslator()
{

}
BioBlocksTranslator::~BioBlocksTranslator() {

}

std::shared_ptr<ProtocolGraph> BioBlocksTranslator::translateFile(const std::string & path) throw(std::invalid_argument) {
    std::ifstream in(path);
    json js;
    try {
        in >> js;

        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"tittle", "linkedBlocks"}, js);

        std::string protocolName = js["tittle"];
        resetAttributes(protocolName);

        std::shared_ptr<MathBlocks> mathBlocks = std::make_shared<MathBlocks>(ptrGraph);
        std::shared_ptr<LogicBlocks> logicBlocks = std::make_shared<LogicBlocks>(ptrGraph);
        mathBlocks->setLogicTrans(logicBlocks);
        logicBlocks->setMathTrans(mathBlocks);

        std::shared_ptr<ContainerManager> contBlocks = std::make_shared<ContainerManager>(ptrGraph, mathBlocks);
        OperationsBlocks opBlocks(ptrGraph, contBlocks, mathBlocks, logicBlocks);

        json blocksList = js["linkedBlocks"];
        for(json::iterator it = blocksList.begin(); it != blocksList.end(); ++it) {
            processLinkedBlocks(*it, opBlocks);
            protocolEndTime = MF::max(protocolEndTime, lastBlockProcess->getEndVariable());
        }
        makeProtocolGraph();

        return ptrGraph;
    } catch (std::exception & e) {
        throw(std::invalid_argument("BioBlocksTranslator::translateFile. Exception ocurred " + std::string(e.what())));
    }
}

void BioBlocksTranslator::makeProtocolGraph() {
    setTimeStep();

    for(int op: initializationOps) {
        ptrGraph->appendOperations(op);
    }

    ptrGraph->startLoopBlock(BF::lessEq(ptrGraph->getTimeVariable(),protocolEndTime));

    for(const std::shared_ptr<BlockPOJOInterface> & logicBlock : logicOps) {
        logicBlock->appendOperationsToGraphs(ptrGraph);
    }

    for(const std::shared_ptr<BlockPOJOInterface> & opBlock : blocksOps) {
        opBlock->appendOperationsToGraphs(ptrGraph);
    }

    ptrGraph->endLoopBlock();
}

void BioBlocksTranslator::setTimeStep() {
    //TODO:
}

void BioBlocksTranslator::resetAttributes(const std::string & protocolName) {
    ptrGraph = std::make_shared<ProtocolGraph>(protocolName);

    initializationOps.clear();
    blocksOps.clear();
    logicOps.clear();

    lastBlockProcess = NULL;
    protocolEndTime = MF::getNum(0);
    logicSerial.reset();
}

void BioBlocksTranslator::processLinkedBlocks(
        nlohmann::json blockObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
    throw(std::invalid_argument)
{
    for(json::iterator it = blockObj.begin(); it != blockObj.end(); ++it) {
        json blockOp = *it;
        try {
            BlocksUtils::checkPropertiesExists(std::vector<std::string>{"block_type"}, blockOp);
            std::string blockType = blockOp["block_type"];

            if (blockType.compare(BIOBLOCKS_IF_STR) == 0) {
                bioblocksIfOperation(blockOp, blocksTrans, initTime, endIfVar);
            } else if (blockType.compare(BIOBLOCKS_WHILE_STR) == 0) {
                bioblocksWhileOperation(blockOp, blocksTrans, initTime, endIfVar);
            } else if (blockType.compare(BIOBLOCKS_VARIABLE_SET_STR) == 0) {
                variablesSet(blockOp, blocksTrans, initTime, endIfVar);
            } else if (blockType.compare(BIOBLOCKS_THERMOCYCLING_STR) == 0) {
                thermocyclingOperation(blockOp, blocksTrans, initTime, endIfVar);
            } else {
                processBioBlocksOp(blockOp, blocksTrans, initTime, endIfVar);
            }
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("BioBlocksTranslator::processLinkedBlocks. " + std::string(e.what())));
        }
    }
}

void BioBlocksTranslator::thermocyclingOperation(
        const nlohmann::json & thermocyclingObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
    throw(std::invalid_argument)
{
    //TODO:
}

void BioBlocksTranslator::variablesSet(
        const nlohmann::json & variableSetObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
    throw(std::invalid_argument)
{
    try {
        units::Time initTimeValue;
        units::Time duration;

        BlocksUtils::fillTimeSetting(variableSetObj, initTimeValue, duration);

        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"variable", "value"}, variableSetObj);

        std::string varName = variableSetObj["variable"];
        std::shared_ptr<MathematicOperable> varValue = blocksTrans.getMathBlocks()->translateMathBlock(variableSetObj["value"]);
        int op = ptrGraph->emplaceAssignation(varName, varValue);

        if (initTimeValue() == 0) {
            initializationOps.push_back(op);
        } else {
            std::shared_ptr<MathematicOperable> initTimeVar = initTime != NULL ? initTime : processIniTime(initTimeValue);
            std::shared_ptr<BlockPOJOInterface> nextBlock = std::make_shared<CpuBlockPOJO>(op, initTimeVar, endIfVar);

            lastBlockProcess = nextBlock;
            blocksOps.push_back(nextBlock);
            initializeVarToZero(BlocksUtils::generateExecutingVar(std::vector<int>{op}));
        }

    } catch (std::exception & e) {
        throw(std::invalid_argument("BioBlocksTranslator::variablesSet. " + std::string(e.what())));
    }
}

void BioBlocksTranslator::bioblocksIfOperation(
        const nlohmann::json & bioblocksIfObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
    throw(std::invalid_argument)
{
    try {
        units::Time initTimeValue;
        units::Time duration;

        BlocksUtils::fillTimeSetting(bioblocksIfObj, initTimeValue, duration);
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"branches", "numberOfBranches"}, bioblocksIfObj);

        json branches = bioblocksIfObj["branches"];
        json::iterator it = branches.begin();
        if (it != branches.end()) {
            int id = logicSerial.getNextValue();
            std::shared_ptr<MathematicOperable> initValueVar = initTime != NULL ? initTime : processIniTime(initTimeValue);

            std::string endIfName = BlocksUtils::generateEndIfVar(id);
            initializeVarToInfinite(endIfName);
            std::shared_ptr<VariableEntry> actualIfEndVar = ptrGraph->getVariable(endIfName);

            std::string triggerIfName = BlocksUtils::generateTrigerredIfFlagVar(id);
            initializeVarToZero(triggerIfName);

            std::shared_ptr<VariableEntry> elseVar = NULL;
            if(!bioblocksIfObj["else"].is_null()) {
                std::string elseTriggerName = BlocksUtils::generateElseIfVar(id);
                initializeVarToInfinite(elseTriggerName);
                elseVar = ptrGraph->getVariable(elseTriggerName);

                json elseJson = bioblocksIfObj["else"];
                json::iterator elseIt = elseJson.begin();
                if (elseIt != elseJson.end()) {
                    for(; elseIt != elseJson.end(); ++it) {
                        std::shared_ptr<MathematicOperable> tempElseTime = NULL;
                        if (elseIt == elseJson.begin() + 1) {//first element
                            tempElseTime = elseVar;
                        }

                        std::shared_ptr<VariableEntry> tempElseEndVar = NULL;
                        if (elseIt == elseJson.end() - 1) {//last element
                            tempElseEndVar = actualIfEndVar;
                        }
                        processLinkedBlocks(*elseIt, blocksTrans, tempElseTime, tempElseEndVar);
                    }
                } else {
                    throw(std::invalid_argument("empty else " + BlocksUtils::jsonObjToStr(bioblocksIfObj)));
                }
            }
            std::shared_ptr<IfBlockPOJO> ifBlock = std::make_shared<IfBlockPOJO>(id, initValueVar, actualIfEndVar, endIfVar, elseVar);

            AutoEnumerate branchSerial;
            for(;it != branches.end(); ++it) {
                json branchJson = *it;
                BlocksUtils::checkPropertiesExists(std::vector<std::string>{"condition", "nestedOp"}, branchJson);

                std::shared_ptr<ComparisonOperable> condition = blocksTrans.getLogicBlocks()->translateLogicBlock(branchJson["condition"]);

                json nestedOpJSON = branchJson["nestedOp"];
                json::iterator nestedOpIt = nestedOpJSON.begin();
                if (nestedOpIt != nestedOpJSON.end()) {
                    for(; nestedOpIt != nestedOpJSON.end(); ++nestedOpIt) {

                        std::shared_ptr<VariableEntry> trigerredVar = NULL;
                        if (nestedOpIt == nestedOpJSON.begin() + 1) { //first element
                            int branchId = branchSerial.getNextValue();

                            std::string trigerredVarName = BlocksUtils::generateTriggeredIfVar(id, branchId);
                            initializeVarToInfinite(trigerredVarName);
                            trigerredVar = ptrGraph->getVariable(trigerredVarName);

                            ifBlock->pushBranchTuple(condition, trigerredVar);
                        }

                        std::shared_ptr<VariableEntry> tempEndIfVar = NULL;
                        if (nestedOpIt == nestedOpJSON.end() - 1) {//last element
                            tempEndIfVar = actualIfEndVar;
                        }

                        processLinkedBlocks(*nestedOpIt, blocksTrans, trigerredVar, tempEndIfVar);
                    }
                } else {
                    throw(std::invalid_argument("empty nestedOps " + BlocksUtils::jsonObjToStr(bioblocksIfObj)));
                }
            }
            logicOps.push_back(ifBlock);
            lastBlockProcess = ifBlock;
        } else {
            throw(std::invalid_argument("empty branches " + BlocksUtils::jsonObjToStr(bioblocksIfObj)));
        }
    } catch (std::exception & e) {
        throw(std::invalid_argument("BioBlocksTranslator::bioblocksIfOperation. " + std::string(e.what())));
    }
}

void BioBlocksTranslator::bioblocksWhileOperation(
        const nlohmann::json & bioblocksWhileObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
    throw(std::invalid_argument)
{
    try {
        units::Time initTimeValue;
        units::Time duration;

        BlocksUtils::fillTimeSetting(bioblocksWhileObj, initTimeValue, duration);
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"branches", "condition"}, bioblocksWhileObj);

        std::shared_ptr<MathematicOperable> initTimeVar = initTime != NULL ? initTime : processIniTime(initTimeValue);
        std::shared_ptr<ComparisonOperable> condition = blocksTrans.getLogicBlocks()->translateLogicBlock(bioblocksWhileObj["condition"]);

        json branches = bioblocksWhileObj["branches"];
        json::iterator it = branches.begin();
        if (it != branches.end()) {
            int id = logicSerial.getNextValue();

            std::string triggerWhileName = BlocksUtils::generateTriggerWhileVar(id);
            initializeVarToInfinite(triggerWhileName);
            std::shared_ptr<VariableEntry> trigerredVar = ptrGraph->getVariable(triggerWhileName);

            std::string endWhileName = BlocksUtils::generateEndWhileVar(id);
            initializeVarToInfinite(endWhileName);
            std::shared_ptr<VariableEntry> endWhileVar = ptrGraph->getVariable(endWhileName);

            for(; it != branches.end(); ++it) {
                std::shared_ptr<MathematicOperable> tempInitTime = NULL;
                if (it == branches.begin() + 1) { //first element
                    tempInitTime = trigerredVar;
                }
                processLinkedBlocks(*it, blocksTrans, tempInitTime);
            }

            std::shared_ptr<BlockPOJOInterface> whileBlock = std::make_shared<WhileBlockPOJO>(id, condition, initTimeVar, trigerredVar, endWhileVar, endIfVar);
            logicOps.push_back(whileBlock);
            lastBlockProcess = whileBlock;
        } else {
            throw(std::invalid_argument("empty branches " + BlocksUtils::jsonObjToStr(bioblocksWhileObj)));
        }
    } catch (std::exception & e) {
        throw(std::invalid_argument("BioBlocksTranslator::bioblocksWhileOperation. " + std::string(e.what())));
    }
}

void BioBlocksTranslator::processBioBlocksOp(
        const nlohmann::json & bioblocksObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endIfVar)
{
    units::Time initTimeValue;
    units::Time duration;

    BlocksUtils::fillTimeSetting(bioblocksObj, initTimeValue, duration);
    std::vector<int> ops = blocksTrans.processBlock(bioblocksObj);
    initializeVarToZero(BlocksUtils::generateExecutingVar(ops));

    std::shared_ptr<MathematicOperable> initTimeVar = initTime != NULL ? initTime : processIniTime(initTimeValue);
    std::shared_ptr<MathematicOperable> durationValue = processDuration(duration, ops);

    std::shared_ptr<BlockPOJOInterface> nextBlock = std::make_shared<BioBlocksOpPOJO>(ops, durationValue, initTimeVar, endIfVar);
    blocksOps.push_back(nextBlock);
    lastBlockProcess = nextBlock;
}

void BioBlocksTranslator::initializeVarToInfinite(const std::string & name) {
    int op = ptrGraph->emplaceAssignation(name, ConstantNumber::getInfinite());
    initializationOps.push_back(op);
}

void BioBlocksTranslator::initializeVarToZero(const std::string & name) {
    int op = ptrGraph->emplaceAssignation(name, MF::getNum(0));
    initializationOps.push_back(op);
}

std::shared_ptr<MathematicOperable> BioBlocksTranslator::processIniTime(units::Time initTime) throw(std::invalid_argument)
{
    std::shared_ptr<MathematicOperable> initValue = NULL;
    if (initTime() < 0) {
        if (lastBlockProcess != NULL) {
            initValue = lastBlockProcess->getEndVariable();
        } else {
            throw(std::invalid_argument("Has a NULL block on top. Unable to get init time"));
        }
    } else {
        initValue = MF::getNum(Utils::toDefaultUnits(initTime));
    }
    return initValue;
}

std::shared_ptr<MathematicOperable> BioBlocksTranslator::processDuration(units::Time duration, const std::vector<int> & ops) {
    std::shared_ptr<MathematicOperable> durationValue = NULL;
    if(duration() < 0) {
        for(int op : ops) {
            std::string durationVar = BlocksUtils::generateDurationOpVar(op);
            initializeVarToInfinite(durationVar);

            durationValue = MF::add(durationValue, ptrGraph->getVariable(durationVar));
        }
    } else {
        durationValue = MF::getNum(Utils::toDefaultUnits(duration));
    }
    return durationValue;
}

























