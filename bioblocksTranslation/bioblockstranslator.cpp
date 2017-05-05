#include "bioblockstranslator.h"

typedef ProtocolMathF MF;

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

        std::shared_ptr<MathBlocks> mathBlocks = std::make_shared(ptrGraph);
        std::shared_ptr<LogicBlocks> logicBlocks = std::make_shared(ptrGraph);
        mathBlocks->setLogicTrans(logicBlocks);
        logicBlocks->setMathTrans(mathBlocks);

        std::shared_ptr<ContainerManager> contBlocks = std::make_shared<ContainerManager>(ptrGraph, mathBlocks);
        std::shared_ptr<OperationsBlocks> opBlocks = std::make_shared<OperationsBlocks>(ptrGraph, contBlocks, logicBlocks, mathBlocks);

        json blocksList = js["linkedBlocks"];
        processLinkedBlocksList(blocksList, opBlocks);

        return ptrGraph;
    } catch (std::exception & e) {
        throw(std::invalid_argument("BioBlocksTranslator::translateFile. Exception ocurred " + std::string(e.what())));
    }
}

void BioBlocksTranslator::resetAttributes(const std::string & protocolName) {
    ptrGraph = std::make_shared(protocolName);

    initializationOps.clear();
    blocksOps.clear();
    logicOps.clear();

    lastBlockProcess = NULL;
}

void BioBlocksTranslator::processLinkedBlocksList(const nlohmann::json & blockObj, std::shared_ptr<OperationsBlocks> blocksTrans) {

}

void BioBlocksTranslator::processLinkedBlocks(const nlohmann::json & blockObj, std::shared_ptr<OperationsBlocks> blocksTrans) {

}

void BioBlocksTranslator::variablesSet(const nlohmann::json & variableSetObj, std::shared_ptr<OperationsBlocks> blocksTrans)
    throw(std::invalid_argument)
{
    try {
        units::Time initTime;
        units::Time duration;

        BlocksUtils::fillTimeSetting(variableSetObj, initTime, duration);

        checkPropertiesExists(std::vector<std::string>{"variable", "value"}, variableSetObj);

        std::string varName = variableSetObj["variable"];
        std::shared_ptr<MathematicOperable> varValue = blocksTrans->getMathBlocks()->translateMathBlock(variableSetObj["value"]);
        int op = ptrGraph->emplaceAssignation(varName, varValue);

        if (initTime() == 0) {
            initializationOps.push_back(op);
        } else if (initTime() < 0) {
            if (lastBlockProcess != NULL) {
                std::shared_ptr<MathematicOperable> initTime = lastBlockProcess->getEndVariable();
                std::shared_ptr<BlockPOJOInterface> nextBlock = std::make_shared<CpuBlockPOJO>(op, initTime);

                lastBlockProcess = nextBlock;
                blocksOps.push_back(nextBlock);
            } else {
                throw(std::invalid_argument("linked block: " + BlocksUtils::jsonObjToStr(variableSetObj) + " has a NULL lastBlockProcessed"));
            }
        } else {
            throw(std::invalid_argument("variable set op not linked or with init time 0"));
        }

    } catch (std::exception & e) {
        throw(std::invalid_argument("BioBlocksTranslator::variablesSet. " + std::string(e.what())));
    }
}

void BioBlocksTranslator::processBioBlocksOp(
        const nlohmann::json & bioblocksObj,
        std::shared_ptr<OperationsBlocks> blocksTrans,
        std::shared_ptr<VariableEntry> endIfVar)
    throw(std::invalid_argument)
{
    units::Time initTime;
    units::Time duration;

    BlocksUtils::fillTimeSetting(bioblocksObj, initTime, duration);
    std::vector<int> ops = blocksTrans->processBlock(bioblocksObj);

    std::shared_ptr<MathematicOperable> initValue;
    if (initTime() < 0) {
        if (lastBlockProcess != NULL) {
            initValue = lastBlockProcess->getEndVariable();
        } else {
            throw(std::invalid_argument("BioBlocksTranslator::processBioBlocksOp. linked block: " + BlocksUtils::jsonObjToStr(bioblocksObj) +
                                        " has a NULL block on top. Unable to get init time"));
        }
    } else {
        initValue = MF::getNum(Utils::toDefaultUnits(initTime));
    }

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

    std::shared_ptr<BlockPOJOInterface> nextBlock = std::make_shared<BioBlocksOpPOJO>(ops, durationValue, initValue, endIfVar);
    blocksOps.push_back(nextBlock);
    lastBlockProcess = nextBlock;
}


























