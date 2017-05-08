#include "bioblocksTranslation/blocks/mathblocks.h"
#include "logicblocks.h"

typedef ProtocolBoolF BF;
typedef ProtocolMathF MF;

using json = nlohmann::json;

LogicBlocks::LogicBlocks(std::shared_ptr<ProtocolGraph> protocolPtr, std::shared_ptr<MathBlocks> mathTrans)
{
    this->protocolPtr = protocolPtr;
    this->mathTrans = mathTrans;
}

LogicBlocks::~LogicBlocks() {

}

std::shared_ptr<ComparisonOperable> LogicBlocks::translateLogicBlock(const nlohmann::json & logicJSONObj)  throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("block_type", logicJSONObj)) {
        std::shared_ptr<ComparisonOperable> translatedBlock = NULL;

        std::string opStr = logicJSONObj["block_type"];
        if (opStr.compare(LOGIC_COMPARE_STR) == 0) {
            translatedBlock = logicCompareOperation(logicJSONObj);
        } else if (opStr.compare(LOGIC_OPERATION_STR) == 0) {
            translatedBlock = logicOperationOperation(logicJSONObj);
        } else if (opStr.compare(LOGIC_NEGATE_STR) == 0) {
            translatedBlock = logicNegateOperation(logicJSONObj);
        } else if (opStr.compare(LOGIC_BOOLEAN_STR) == 0) {
            translatedBlock = logicBooleanOperation(logicJSONObj);
        } else if (opStr.compare(LOGIC_NULL_STR) == 0) {
            translatedBlock = logicNullOperation(logicJSONObj);
        } else if (opStr.compare(MATH_NUMBER_PROPERTY_STR) == 0) {
            translatedBlock = mathNumberPropertyOperation(logicJSONObj);
        } else {
            throw(std::invalid_argument("LogicBlocks::translateLogicBlock. Unknow block type: \"" + opStr + "\""));
        }
        return translatedBlock;
    } else {
        throw(std::invalid_argument("LogicBlocks::translateLogicBlock. " +
                                    BlocksUtils::generateNoPropertyErrorMsg(logicJSONObj,"block_type")));
    }
}

std::shared_ptr<ComparisonOperable> LogicBlocks::logicCompareOperation(const nlohmann::json & logicCompareObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"left","rigth","op"}, logicCompareObj);

        std::shared_ptr<MathematicOperable> leftPtr = mathTrans->translateMathBlock(logicCompareObj["left"]);
        std::shared_ptr<MathematicOperable> rightPtr = mathTrans->translateMathBlock(logicCompareObj["rigth"]);
        SimpleComparison::ComparisonOperator op = getComparisonOperator(logicCompareObj["op"]);

        return std::make_shared<SimpleComparison>(false,leftPtr, op, rightPtr);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("LogicBlocks::logicCompareOperation." + std::string(e.what())));
    }
}

std::shared_ptr<ComparisonOperable> LogicBlocks::logicOperationOperation(const nlohmann::json & logicOperationObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"left","rigth","op"}, logicOperationObj);

        std::shared_ptr<ComparisonOperable> leftPtr = translateLogicBlock(logicOperationObj["left"]);
        std::shared_ptr<ComparisonOperable> rightPtr = translateLogicBlock(logicOperationObj["rigth"]);
        BooleanComparison::BooleanOperator op = getBooleanOperator(logicOperationObj["op"]);

        return std::make_shared<BooleanComparison>(false,leftPtr, op, rightPtr);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("LogicBlocks::logicOperationOperation." + std::string(e.what())));
    }
}

std::shared_ptr<ComparisonOperable> LogicBlocks::logicNegateOperation(const nlohmann::json & logicNegateObj) throw(std::invalid_argument) {
    if (BlocksUtils::hasProperty("bool", logicNegateObj)) {
        std::shared_ptr<ComparisonOperable> boolPtr(translateLogicBlock(logicNegateObj["bool"])->clone());
        boolPtr->negate();
        return boolPtr;
    } else {
        throw(std::invalid_argument("LogicBlocks::logicNegateOperation." +
                                    BlocksUtils::generateNoPropertyErrorMsg(logicNegateObj, "bool")));
    }
}

std::shared_ptr<ComparisonOperable> LogicBlocks::logicBooleanOperation(const nlohmann::json & logicBooleanObj) throw(std::invalid_argument) {
    if (BlocksUtils::hasProperty("value", logicBooleanObj)) {
        return getBoolConstant(logicBooleanObj["value"]);
    } else {
        throw(std::invalid_argument("LogicBlocks::logicBooleanOperation." +
                                    BlocksUtils::generateNoPropertyErrorMsg(logicBooleanObj, "value")));
    }
}

std::shared_ptr<ComparisonOperable> LogicBlocks::logicNullOperation(const nlohmann::json & logicNullObj) throw(std::invalid_argument) {
    return NULL;
}

std::shared_ptr<ComparisonOperable> LogicBlocks::mathNumberPropertyOperation(const nlohmann::json & mathNumberPropertyObj)
    throw(std::invalid_argument)
{
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"op","value"}, mathNumberPropertyObj);

        std::shared_ptr<ComparisonOperable> cop = NULL;

        std::shared_ptr<MathematicOperable> valuePtr = mathTrans->translateMathBlock(mathNumberPropertyObj["value"]);
        std::string opStr = mathNumberPropertyObj["op"];

        if ((opStr.compare(BLOCKLY_EVEN_STR) == 0) ||
                (opStr.compare(BLOCKLY_ODD_STR) == 0) ||
                (opStr.compare(BLOCKLY_PRIME_STR) == 0) ||
                (opStr.compare(BLOCKLY_WHOLE_STR) == 0) ||
                (opStr.compare(BLOCKLY_POSITIVE_STR) == 0) ||
                (opStr.compare(BLOCKLY_NEGATIVE_STR) == 0))
        {
            CharacteristicCheck::CheckOperations op = getCheckOperator(opStr);
            cop = std::make_shared<CharacteristicCheck>(false, valuePtr, op);

        } else if (opStr.compare(BLOCKLY_DIVISIBLE_BY_STR) == 0) {

            BlocksUtils::checkPropertiesExists(std::vector<std::string>{"divisor"}, mathNumberPropertyObj);

            std::shared_ptr<MathematicOperable> divisorPtr = mathTrans->translateMathBlock(mathNumberPropertyObj["divisor"]);
            std::shared_ptr<MathematicOperable> modulePtr = MF::module(valuePtr, divisorPtr);
            cop = std::make_shared<SimpleComparison>(false, modulePtr, SimpleComparison::equal, MF::getNum(0));
        } else {
            throw(std::invalid_argument("LogicBlocks::mathNumberPropertyOperation. Unknow operation \"" + opStr + "\""));
        }
        return cop;
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("LogicBlocks::mathNumberPropertyOperation." + std::string(e.what())));
    }
}

SimpleComparison::ComparisonOperator LogicBlocks::getComparisonOperator(const std::string & opStr) throw(std::invalid_argument) {
    SimpleComparison::ComparisonOperator op;
    if (opStr.compare(BLOCKLY_EQ_STR) == 0) {
        op = SimpleComparison::equal;
    } else if (opStr.compare(BLOCKLY_NEQ_STR) == 0) {
        op = SimpleComparison::not_equal;
    } else if (opStr.compare(BLOCKLY_LT_STR) == 0) {
        op = SimpleComparison::less;
    } else if (opStr.compare(BLOCKLY_LTE_STR) == 0) {
        op = SimpleComparison::less_equal;
    } else if (opStr.compare(BLOCKLY_GT_STR) == 0) {
        op = SimpleComparison::greater;
    } else if (opStr.compare(BLOCKLY_GTE_STR) == 0) {
        op = SimpleComparison::greater_equal;
    } else {
        throw(std::invalid_argument("LogicBlocks::getComparisonOperator. Unknow operator: \"" + opStr + "\""));
    }
    return op;
}

BooleanComparison::BooleanOperator LogicBlocks::getBooleanOperator(const std::string & opStr) throw(std::invalid_argument) {
    BooleanComparison::BooleanOperator op;
    if (opStr.compare(BLOCKLY_AND_STR) == 0) {
        op = BooleanComparison::conjunction;
    } else if (opStr.compare(BLOCKLY_OR_STR) == 0) {
        op = BooleanComparison::disjunction;
    } else {
        throw(std::invalid_argument("LogicBlocks::getBooleanOperator. Unknow operator: \"" + opStr + "\""));
    }
    return op;
}

CharacteristicCheck::CheckOperations LogicBlocks::getCheckOperator(const std::string & opStr) throw(std::invalid_argument) {
    CharacteristicCheck::CheckOperations op;
    if (opStr.compare(BLOCKLY_EVEN_STR) == 0) {
        op = CharacteristicCheck::even;
    } else if (opStr.compare(BLOCKLY_ODD_STR) == 0) {
        op = CharacteristicCheck::odd;
    } else if (opStr.compare(BLOCKLY_PRIME_STR) == 0) {
        op = CharacteristicCheck::prime;
    } else if (opStr.compare(BLOCKLY_WHOLE_STR) == 0) {
        op = CharacteristicCheck::whole;
    } else if (opStr.compare(BLOCKLY_POSITIVE_STR) == 0) {
        op = CharacteristicCheck::positive;
    } else if (opStr.compare(BLOCKLY_NEGATIVE_STR) == 0) {
        op = CharacteristicCheck::negative;
    } else {
        throw(std::invalid_argument("LogicBlocks::getCheckOperator. Unknow operator : \"" + opStr + "\""));
    }
    return op;
}

std::shared_ptr<Tautology> LogicBlocks::getBoolConstant(const std::string & opStr) throw(std::invalid_argument) {
    std::shared_ptr<Tautology> op = NULL;
    if (opStr.compare(BLOCKLY_TRUE_STR) == 0) {
        op = std::make_shared<Tautology>();
    } else if (opStr.compare(BLOCKLY_FALSE_STR) == 0) {
        op = std::make_shared<Tautology>();
        op->negate();
    } else {
        throw(std::invalid_argument("LogicBlocks::getBoolConstant. Unknow constant: \"" + opStr + "\""));
    }
    return op;
}
