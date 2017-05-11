#include "bioblockstranslator.h"

typedef ProtocolMathF MF;
typedef ProtocolBoolF BF;

using json = nlohmann::json;

BioBlocksTranslator::BioBlocksTranslator(units::Time timeSlice, const std::string & jsonFilepath) :
    filePath(jsonFilepath)
{
    this->timeSliceValue = timeSlice;
}

BioBlocksTranslator::~BioBlocksTranslator() {

}    

std::shared_ptr<ProtocolGraph> BioBlocksTranslator::translateFile() throw(std::invalid_argument) {
    std::ifstream in(filePath);
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

            if (lastBlockProcess != NULL) { //variable set can go in initialization
                protocolEndTime = MF::max(protocolEndTime, lastBlockProcess->getEndVariable());
            }
        }
        makeProtocolGraph();

        return ptrGraph;
    } catch (std::exception & e) {
        throw(std::invalid_argument("BioBlocksTranslator::translateFile. Exception ocurred " + std::string(e.what())));
    }
}

void BioBlocksTranslator::makeProtocolGraph() {
    setTimeStep();
    initActualTimeVar();

    for(int op: initializationOps) {
        ptrGraph->appendOperations(op);
    }

    processLoadContainers();

    ptrGraph->startLoopBlock(BF::lessEq(ptrGraph->getTimeVariable(),MF::add(protocolEndTime, timeSlice)));

    for(const std::shared_ptr<BlockPOJOInterface> & logicBlock : freelogicOps) {
        logicBlock->appendOperationsToGraphs(ptrGraph);
    }

    for(const std::shared_ptr<BlockPOJOInterface> & opBlock : blocksOps) {
        opBlock->appendOperationsToGraphs(ptrGraph);
    }

    for(const std::shared_ptr<BlockPOJOInterface> & logicBlock : linkedlogicOps) {
        logicBlock->appendOperationsToGraphs(ptrGraph);
    }

    int timeStep = ptrGraph->emplaceTimeStep();
    ptrGraph->appendOperations(timeStep);

    ptrGraph->endLoopBlock();
}

void BioBlocksTranslator::setTimeStep() {
    double value = timeSliceValue.to(units::s);
    int timeStep = ptrGraph->emplaceSetTimeStep(MF::getNum(value), units::s);
    timeSlice = MF::getNum(value);
    ptrGraph->setStartNode(timeStep);
}

void BioBlocksTranslator::initActualTimeVar() {
    int initActualTime = ptrGraph->emplaceAssignation(ptrGraph->getTimeVariable()->toString(), MF::getNum(0));
    ptrGraph->appendOperations(initActualTime);
}

void BioBlocksTranslator::processLoadContainers() {
    const std::unordered_map<std::string, std::shared_ptr<VirtualContainer>> & vcMap = ptrGraph->getVContainerMap();
    for(const auto & vcPair : vcMap) {
        const std::string & name = vcPair.first;
        std::shared_ptr<VirtualContainer> vcontainer = vcPair.second;

        units::Volume initiVol = vcontainer->getInitialVolume();
        int loadOp = ptrGraph->emplaceLoadContainer(name, MF::getNum(initiVol.to(units::ml)), units::ml);
        ptrGraph->appendOperations(loadOp);
    }
}

void BioBlocksTranslator::resetAttributes(const std::string & protocolName) {
    ptrGraph = std::make_shared<ProtocolGraph>(protocolName);

    initializationOps.clear();
    blocksOps.clear();
    linkedlogicOps.clear();
    freelogicOps.clear();

    lastBlockProcess = NULL;
    protocolEndTime = MF::getNum(0);
    logicSerial.reset();
}

void BioBlocksTranslator::processLinkedBlocks(
        nlohmann::json blockObj,
        const OperationsBlocks & blocksTrans)
    throw(std::invalid_argument)
{
    for(json::iterator it = blockObj.begin(); it != blockObj.end(); ++it) {
        processBlock(*it, blocksTrans);
    }
}

void BioBlocksTranslator::processBlock(
        nlohmann::json blockOp,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endWhileExecutingVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar)
    throw(std::invalid_argument)
{
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"block_type"}, blockOp);
        std::string blockType = blockOp["block_type"];

        if (blockType.compare(BIOBLOCKS_IF_STR) == 0) {
            bioblocksIfOperation(blockOp, blocksTrans, initTime, endWhileExecutingVar, endIfVar);
        } else if (blockType.compare(BIOBLOCKS_WHILE_STR) == 0) {
            bioblocksWhileOperation(blockOp, blocksTrans, initTime, endWhileExecutingVar, endIfVar);
        } else if (blockType.compare(BIOBLOCKS_VARIABLE_SET_STR) == 0) {
            variablesSet(blockOp, blocksTrans, initTime, endWhileExecutingVar, endIfVar);
        } else if (blockType.compare(BIOBLOCKS_THERMOCYCLING_STR) == 0) {
            thermocyclingOperation(blockOp, blocksTrans, initTime, endWhileExecutingVar, endIfVar);
        } else {
            processBioBlocksOp(blockOp, blocksTrans, initTime, endWhileExecutingVar, endIfVar);
        }
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("BioBlocksTranslator::processLinkedBlocks. " + std::string(e.what())));
    }
}

void BioBlocksTranslator::thermocyclingOperation(
        const nlohmann::json & thermocyclingObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endWhileExecutingVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar)
    throw(std::invalid_argument)
{
    try {
        units::Time initTimeValue;
        units::Time duration;
        BlocksUtils::fillTimeSetting(thermocyclingObj, initTimeValue, duration);

        std::shared_ptr<MathematicOperable> initTimeVar = initTime != NULL ? initTime : processIniTime(initTimeValue);
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"cycles", "source"}, thermocyclingObj);
        std::string sourceName = blocksTrans.getVCManager()->processContainerBlock(thermocyclingObj["source"])[0];

        //make while
        int id = logicSerial.getNextValue();

        std::shared_ptr<MathematicOperable> cycles = blocksTrans.getMathBlocks()->translateMathBlock(thermocyclingObj["cycles"]);
        std::string cyclesCounterName = BlocksUtils::generateThermocyclingCounter(id);
        initializeVarToZero(cyclesCounterName);

        std::shared_ptr<ComparisonOperable> condition = BF::less(ptrGraph->getVariable(cyclesCounterName), cycles);

        std::string triggerWhileName = BlocksUtils::generateTriggerWhileVar(id);
        initializeVarToInfinite(triggerWhileName);
        std::shared_ptr<VariableEntry> trigerredVar = ptrGraph->getVariable(triggerWhileName);

        std::string endWhileName = BlocksUtils::generateEndWhileVar(id);
        initializeVarToInfinite(endWhileName);
        std::shared_ptr<VariableEntry> endWhileVar = ptrGraph->getVariable(endWhileName);

        std::string executingWhileName = BlocksUtils::generateExecutingWhileVar(id);
        initializeVarToZero(executingWhileName);

        std::shared_ptr<BlockPOJOInterface> whileBlock =
                std::make_shared<WhileBlockPOJO>(
                    id,
                    condition,
                    initTimeVar,
                    trigerredVar,
                    endWhileVar,
                    endIfVar,
                    endWhileExecutingVar);

        if (initTimeValue() < 0) { //if the block is linked
            linkedlogicOps.push_back(whileBlock);
        } else { //if the block is free
            freelogicOps.push_back(whileBlock);
        }

        lastBlockProcess = whileBlock;

        //process steps
        ContainerManager::PCRStepVector stepsVector = blocksTrans.getVCManager()->extractPCRSteps(thermocyclingObj["source"]);
        for(auto it = stepsVector.begin(); it != stepsVector.end(); ++it) {
            const ContainerManager::PCRStepTuple & actualTuple = *it;

            std::shared_ptr<MathematicOperable> actualInitTime = NULL;
            if (it == stepsVector.begin()) {//first element
                actualInitTime = trigerredVar;
            } else {
                actualInitTime = lastBlockProcess->getEndVariable();
            }

            const ContainerManager::TemperatureTuple & tempTuple = std::get<0>(actualTuple);
            const ContainerManager::TimeTuple & timeTuple = std::get<1>(actualTuple);

            units::Time duration = std::get<0>(timeTuple) * std::get<1>(timeTuple);
            int opHeat = ptrGraph->emplaceApplyTemperature(sourceName, MF::getNum(std::get<0>(tempTuple)), std::get<1>(tempTuple));

            std::string executingOpName = BlocksUtils::generateExecutingVar(std::vector<int>{opHeat});
            initializeVarToZero(executingOpName);

            std::shared_ptr<BlockPOJOInterface> nextBlock =
                    std::make_shared<BioBlocksOpPOJO>(opHeat,
                                                      MF::getNum(Utils::toDefaultUnits(duration)),
                                                      actualInitTime);

            blocksOps.push_back(nextBlock);
            lastBlockProcess = nextBlock;
        }

        //update cycles
        int op = ptrGraph->emplaceAssignation(cyclesCounterName, MF::add(ptrGraph->getVariable(cyclesCounterName), MF::getNum(1)));
        std::shared_ptr<BlockPOJOInterface> nextBlock =
                std::make_shared<CpuBlockPOJO>(op,
                                               lastBlockProcess->getEndVariable(),
                                               endIfVar,
                                               ptrGraph->getVariable(executingWhileName),
                                               false);

        lastBlockProcess = nextBlock;
        blocksOps.push_back(nextBlock);
        initializeVarToZero(BlocksUtils::generateExecutingVar(std::vector<int>{op}));

        lastBlockProcess = whileBlock;

    }catch (std::invalid_argument & e) {
        throw(std::invalid_argument("BioBlocksTranslator::thermocyclingOperation. " + std::string(e.what())));
    }
}

void BioBlocksTranslator::variablesSet(
        const nlohmann::json & variableSetObj,
        const OperationsBlocks & blocksTrans,
        std::shared_ptr<MathematicOperable> initTime,
        std::shared_ptr<VariableEntry> endWhileExecutingVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar)
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
            std::shared_ptr<BlockPOJOInterface> nextBlock = std::make_shared<CpuBlockPOJO>(op, initTimeVar, endIfVar, endWhileExecutingVar);

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
        std::shared_ptr<VariableEntry> endWhileExecutingVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar)
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
            if(BlocksUtils::hasProperty("else", bioblocksIfObj)) {
                std::string elseTriggerName = BlocksUtils::generateElseIfVar(id);
                initializeVarToInfinite(elseTriggerName);
                elseVar = ptrGraph->getVariable(elseTriggerName);
            }

            std::shared_ptr<IfBlockPOJO> ifBlock = NULL;std::make_shared<IfBlockPOJO>(id, initValueVar, actualIfEndVar, endIfVar, elseVar, endWhileExecutingVar);

            if (initTimeValue() < 0) { //if the block is linked
                ifBlock = std::make_shared<IfBlockPOJO>(id, initValueVar, actualIfEndVar, endIfVar, timeSliceValue, elseVar, endWhileExecutingVar);
                linkedlogicOps.push_back(ifBlock);
            } else { //if the block is free
                ifBlock = std::make_shared<IfBlockPOJO>(id, initValueVar, actualIfEndVar, endIfVar, elseVar, endWhileExecutingVar);
                freelogicOps.push_back(ifBlock);
            }

            lastBlockProcess = ifBlock;

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
                        if (nestedOpIt == nestedOpJSON.begin()) { //first element
                            int branchId = branchSerial.getNextValue();

                            std::string trigerredVarName = BlocksUtils::generateTriggeredIfVar(id, branchId);
                            initializeVarToInfinite(trigerredVarName);
                            trigerredVar = ptrGraph->getVariable(trigerredVarName);

                            ifBlock->pushBranchTuple(condition, trigerredVar);
                        }

                        std::shared_ptr<VariableEntry> tempEndWhileVar = NULL;
                        std::vector<std::shared_ptr<VariableEntry>> tempEndIfVar;
                        if (nestedOpIt == nestedOpJSON.end() - 1) {//last element
                            tempEndIfVar.reserve(endIfVar.size());
                            tempEndIfVar.insert(tempEndIfVar.begin(), endIfVar.begin(), endIfVar.end());
                            tempEndIfVar.push_back(actualIfEndVar);

                            tempEndWhileVar = endWhileExecutingVar;
                        }
                        processBlock(*nestedOpIt, blocksTrans, trigerredVar, tempEndWhileVar, tempEndIfVar);
                    }
                } else {
                    throw(std::invalid_argument("empty nestedOps " + BlocksUtils::jsonObjToStr(bioblocksIfObj)));
                }
            }

            if (elseVar != NULL) {
                json elseJson = bioblocksIfObj["else"];
                json::iterator elseIt = elseJson.begin();
                if (elseIt != elseJson.end()) {
                    for(; elseIt != elseJson.end(); ++elseIt) {
                        std::shared_ptr<MathematicOperable> tempElseTime = NULL;
                        if (elseIt == elseJson.begin()) {//first element
                            tempElseTime = elseVar;
                        }

                        std::shared_ptr<VariableEntry> tempEndWhileVar = NULL;
                        std::vector<std::shared_ptr<VariableEntry>> tempElseEndVar;
                        if (elseIt == elseJson.end() - 1) {//last element
                            tempElseEndVar.reserve(endIfVar.size());
                            tempElseEndVar.insert(tempElseEndVar.begin(), endIfVar.begin(), endIfVar.end());
                            tempElseEndVar.push_back(actualIfEndVar);

                            tempEndWhileVar = endWhileExecutingVar;
                        }
                        processBlock(*elseIt, blocksTrans, tempElseTime, tempEndWhileVar, tempElseEndVar);
                    }
                } else {
                    throw(std::invalid_argument("empty else " + BlocksUtils::jsonObjToStr(bioblocksIfObj)));
                }
            }

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
        std::shared_ptr<VariableEntry> endWhileExecutingVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar)
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

            std::string executingWhileName = BlocksUtils::generateExecutingWhileVar(id);
            initializeVarToZero(executingWhileName);

            std::shared_ptr<BlockPOJOInterface> whileBlock = NULL;
            if (initTimeValue() < 0) { //if the block is linked
                whileBlock = std::make_shared<WhileBlockPOJO>(id,condition,initTimeVar,trigerredVar,endWhileVar,endIfVar,timeSliceValue,endWhileExecutingVar);
                linkedlogicOps.push_back(whileBlock);
            } else { //if the block is free
                whileBlock = std::make_shared<WhileBlockPOJO>(id,condition,initTimeVar,trigerredVar,endWhileVar,endIfVar,endWhileExecutingVar);
                freelogicOps.push_back(whileBlock);
            }

            lastBlockProcess = whileBlock;

            for(; it != branches.end(); ++it) {
                std::shared_ptr<MathematicOperable> tempInitTime = NULL;
                if (it == branches.begin()) { //first element
                    tempInitTime = trigerredVar;
                }

                std::shared_ptr<VariableEntry> tempEndWhileVar = NULL;
                std::vector<std::shared_ptr<VariableEntry>> tempElseEndVar;
                if (it == branches.end() - 1) {//last element
                    tempElseEndVar.reserve(endIfVar.size());
                    tempElseEndVar.insert(tempElseEndVar.begin(), endIfVar.begin(), endIfVar.end());

                    tempEndWhileVar = ptrGraph->getVariable(BlocksUtils::generateExecutingWhileVar(id));
                }
                processBlock(*it, blocksTrans, tempInitTime, tempEndWhileVar, tempElseEndVar);
            }
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
        std::shared_ptr<VariableEntry> endWhileExecutingVar,
        std::vector<std::shared_ptr<VariableEntry>> endIfVar)
{
    units::Time initTimeValue;
    units::Time duration;

    BlocksUtils::fillTimeSetting(bioblocksObj, initTimeValue, duration);
    std::vector<int> ops = blocksTrans.processBlock(bioblocksObj);
    initializeVarToZero(BlocksUtils::generateExecutingVar(ops));

    std::shared_ptr<MathematicOperable> initTimeVar = initTime != NULL ? initTime : processIniTime(initTimeValue);
    std::shared_ptr<MathematicOperable> durationValue = processDuration(duration, ops);

    std::shared_ptr<BlockPOJOInterface> nextBlock = std::make_shared<BioBlocksOpPOJO>(ops, durationValue, initTimeVar, endIfVar, endWhileExecutingVar);
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

void BioBlocksTranslator::initializeVarToValue(const std::string & name, std::shared_ptr<MathematicOperable> value) {
    int op = ptrGraph->emplaceAssignation(name, value);
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

























