#include "bioblockstranslator.h"

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
        std::shared_ptr<ProtocolGraph> ptrGraph = std::make_shared(protocolName);

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

void BioBlocksTranslator::processLinkedBlocksList(const nlohmann::json & blockObj, std::shared_ptr<OperationsBlocks> blocksTrans) {

}

void BioBlocksTranslator::processLinkedBlocks(const nlohmann::json & blockObj, std::shared_ptr<OperationsBlocks> blocksTrans) {

}
