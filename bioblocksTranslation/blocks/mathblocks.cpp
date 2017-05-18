#include "bioblocksTranslation/blocks/logicblocks.h"
#include "mathblocks.h"

typedef ProtocolMathF MF;

using json = nlohmann::json;


MathBlocks::MathBlocks(std::shared_ptr<ProtocolGraph> protocolPtr, std::shared_ptr<LogicBlocks> logicTrans)
{
    this->protocolPtr = protocolPtr;
    this->logicTrans = logicTrans;
}

MathBlocks::~MathBlocks() {

}

std::shared_ptr<MathematicOperable> MathBlocks::translateMathBlock(const nlohmann::json & mathJSONObj) throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("block_type", mathJSONObj)) {
        std::string opStr = mathJSONObj["block_type"];

        std::shared_ptr<MathematicOperable> op = NULL;
        if(opStr.compare(MATH_NUMBER_STR) == 0) {
            op = mathNumberOperation(mathJSONObj);
        } else if (opStr.compare(MATH_ARITHMETIC_STR) == 0) {
            op = mathArithmeticOperation(mathJSONObj);
        } else if (opStr.compare(MATH_SINGLE_STR) == 0) {
            op = mathSingleOperation(mathJSONObj);
        } else if (opStr.compare(MATH_TRIG_STR) == 0) {
            op = mathSingleOperation(mathJSONObj);
        } else if (opStr.compare(MATH_CONSTANT_STR) == 0) {
            op = mathConstantOperation(mathJSONObj);
        } else if (opStr.compare(MATH_ROUND_STR) == 0) {
            op = mathSingleOperation(mathJSONObj);
        } else if (opStr.compare(MATH_ON_LIST_STR) == 0) {
            op = mathOnListOperation(mathJSONObj);
        } else if (opStr.compare(MATH_MODULO_STR) == 0) {
            op = mathModuloOperation(mathJSONObj);
        } else if (opStr.compare(MATH_CONSTRAIN_STR) == 0) {
            op = mathConstrainOperation(mathJSONObj);
        } else if (opStr.compare(MATH_RANDOM_INT_STR) == 0) {
            op = mathRandomIntOperation(mathJSONObj);
        } else if (opStr.compare(MATH_RANDOM_FLOAT_STR) == 0) {
            op = mathRandomFloatOperation();
        } else if (opStr.compare(VARIABLES_GET_STR) == 0) {
            op = variableGetOperation(mathJSONObj);
        } else if (opStr.compare(LOGICTERNARY_STR) == 0) {
            op = logicTernaryOperation(mathJSONObj);
        } else {
            throw(std::invalid_argument("MathBlocks::translateMathBlock. Unknow type " + opStr));
        }

        return op;
    } else {
        throw(std::invalid_argument("MathBlocks::translateMathBlock." + BlocksUtils::generateNoPropertyErrorMsg(mathJSONObj, "block_type") ));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathNumberOperation(const json & mathNumberObj) throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("value", mathNumberObj)) {
        std::string valueStr = mathNumberObj["value"];
        double value = std::atof(valueStr.c_str());
        return MF::getNum(value);
    } else {
        throw(std::invalid_argument("MathBlocks::mathNumberOperation." + BlocksUtils::generateNoPropertyErrorMsg(mathNumberObj, "value")));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathArithmeticOperation(const json & mathArithmeticObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"left","rigth","op"}, mathArithmeticObj);

        std::shared_ptr<MathematicOperable> leftPtr = translateMathBlock(mathArithmeticObj["left"]);
        std::shared_ptr<MathematicOperable> rightPtr = translateMathBlock(mathArithmeticObj["rigth"]);
        ArithmeticOperation::ArithmeticOperator op = getArithmeticOperator(mathArithmeticObj["op"]);
        return std::make_shared<ArithmeticOperation>(leftPtr, op, rightPtr);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("MathBlocks::mathArithmeticOperation." + std::string(e.what())));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathSingleOperation(const json & mathSingleObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"value","op"}, mathSingleObj);

        std::shared_ptr<MathematicOperable> valuePtr = translateMathBlock(mathSingleObj["value"]);
        ProtocolUnaryOperation::UnaryOperator op = getUnaryOperator(mathSingleObj["op"]);
        return std::make_shared<ProtocolUnaryOperation>(valuePtr, op);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("MathBlocks::mathArithmeticOperation." + std::string(e.what())));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathConstantOperation(const json & constantOperationObj) throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("constant", constantOperationObj)) {
        std::string valueStr = constantOperationObj["constant"];
        double value = getNumericConstant(valueStr);
        return MF::getNum(value);
    } else {
        throw(std::invalid_argument("MathBlocks::mathConstantOperation." + BlocksUtils::generateNoPropertyErrorMsg(constantOperationObj, "constant")));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathOnListOperation(const json & mathOnListObj) throw(std::invalid_argument) {
    //TODO
    return NULL;
}

std::shared_ptr<MathematicOperable> MathBlocks::mathModuloOperation(const json & mathModuloObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"left","rigth"}, mathModuloObj);

        std::shared_ptr<MathematicOperable> leftPtr = translateMathBlock(mathModuloObj["left"]);
        std::shared_ptr<MathematicOperable> rightPtr = translateMathBlock(mathModuloObj["rigth"]);

        return std::make_shared<ArithmeticOperation>(leftPtr, ArithmeticOperation::module, rightPtr);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("MathBlocks::mathModuloOperation." + std::string(e.what())));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathConstrainOperation(const json & mathConstrainObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"value","low", "high"}, mathConstrainObj);

        std::shared_ptr<MathematicOperable> valuePtr = translateMathBlock(mathConstrainObj["value"]);
        std::shared_ptr<MathematicOperable> lowPtr = translateMathBlock(mathConstrainObj["low"]);
        std::shared_ptr<MathematicOperable> highPtr = translateMathBlock(mathConstrainObj["high"]);

        return MF::min(MF::max(valuePtr, lowPtr), highPtr);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("MathBlocks::mathConstrainOperation." + std::string(e.what())));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathRandomIntOperation(const json & mathRandomIntObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"from","to"}, mathRandomIntObj);

        std::shared_ptr<MathematicOperable> fromPtr = translateMathBlock(mathRandomIntObj["from"]);
        std::shared_ptr<MathematicOperable> toPtr = translateMathBlock(mathRandomIntObj["to"]);

        return MF::random(fromPtr, toPtr);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("MathBlocks::mathRandomIntOperation." + std::string(e.what())));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::mathRandomFloatOperation() {
    return MF::random();
}

std::shared_ptr<MathematicOperable> MathBlocks::logicTernaryOperation(const json & logicTernaryObj) throw(std::invalid_argument) {
    try {
        BlocksUtils::checkPropertiesExists(std::vector<std::string>{"if","then","else"}, logicTernaryObj);

        std::shared_ptr<ComparisonOperable> conditionPtr = logicTrans->translateLogicBlock(logicTernaryObj["if"]);
        std::shared_ptr<MathematicOperable> truePtr = translateMathBlock(logicTernaryObj["then"]);
        std::shared_ptr<MathematicOperable> falsePtr = translateMathBlock(logicTernaryObj["else"]);

        return MF::test(conditionPtr, truePtr, falsePtr);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("MathBlocks::logicTernaryOperation." + std::string(e.what())));
    }
}

std::shared_ptr<MathematicOperable> MathBlocks::variableGetOperation(const nlohmann::json & variableGetObj) throw(std::invalid_argument) {
    if(BlocksUtils::hasProperty("variable", variableGetObj)) {
        std::string valueStr = variableGetObj["variable"];
        return protocolPtr->getVariable(valueStr);
    } else {
        throw(std::invalid_argument("MathBlocks::variableGetOperation." + BlocksUtils::generateNoPropertyErrorMsg(variableGetObj, "variable")));
    }
}

ArithmeticOperation::ArithmeticOperator MathBlocks::getArithmeticOperator(const std::string & aopStr) throw(std::invalid_argument) {
    ArithmeticOperation::ArithmeticOperator op;
    if(aopStr.compare(BLOCKLY_PLUS_STR) == 0) {
        op = ArithmeticOperation::plus;
    } else if(aopStr.compare(BLOCKLY_MINUS_STR) == 0) {
        op = ArithmeticOperation::minus;
    } else if(aopStr.compare(BLOCKLY_MULTIPLY_STR) == 0) {
        op = ArithmeticOperation::multiply;
    } else if(aopStr.compare(BLOCKLY_DIVIDE_STR) == 0) {
        op = ArithmeticOperation::divide;
    } else if(aopStr.compare(BLOCKLY_POWER_STR) == 0) {
        op = ArithmeticOperation::power;
    } else {
        throw(std::invalid_argument("MathBlocks::getArithmeticOperator. Unknow op " + aopStr));
    }
    return op;
}

ProtocolUnaryOperation::UnaryOperator MathBlocks::getUnaryOperator(const std::string & uopStr) {
    ProtocolUnaryOperation::UnaryOperator op;
    if( uopStr.compare(BLOCKLY_ABSOLUTE_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::absoluteValue;
    } else if( uopStr.compare(BLOCKLY_FLOOR_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::floor;
    } else if( uopStr.compare(BLOCKLY_ROUND_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::round;
    } else if( uopStr.compare(BLOCKLY_CEILING_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::ceiling;
    } else if( uopStr.compare(BLOCKLY_SQRT_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::sqrt;
    } else if( uopStr.compare(BLOCKLY_LN_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::ln;
    } else if( uopStr.compare(BLOCKLY_LOG10_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::log10;
    } else if( uopStr.compare(BLOCKLY_EXP_E_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::exp_e;
    } else if( uopStr.compare(BLOCKLY_EXP_10_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::exp_10;
    } else if( uopStr.compare(BLOCKLY_MINUS_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::minus;
    } else if( uopStr.compare(BLOCKLY_SIN_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::sin_op;
    } else if( uopStr.compare(BLOCKLY_COS_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::cos_op;
    } else if( uopStr.compare(BLOCKLY_TAN_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::tan_op;
    } else if( uopStr.compare(BLOCKLY_ASIN_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::asin_op;
    } else if( uopStr.compare(BLOCKLY_ACOS_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::acos_op;
    } else if( uopStr.compare(BLOCKLY_ATAN_VALUE_STRING ) == 0) {
        op = ProtocolUnaryOperation::atan_op;
    } else {
        throw(std::invalid_argument("MathBlocks::getUnaryOperator. Unknow op" + uopStr));
    }
    return op;
}

double MathBlocks::getNumericConstant(const std::string & constantStr) {
    double value = 0.0;
    if (constantStr.compare(BLOCKLY_PI_STR) == 0) {
        value = boost::math::constants::pi<double>();
    } else if(constantStr.compare(BLOCKLY_E_STR) == 0) {
        value = boost::math::constants::e<double>();
    } else if(constantStr.compare(BLOCKLY_GOLDEN_RATIO_STR) == 0) {
        value = boost::math::constants::phi<double>();
    } else if(constantStr.compare(BLOCKLY_SQRT2_STR) == 0) {
        value = boost::math::constants::root_two<double>();
    } else if(constantStr.compare(BLOCKLY_SQRT1_2_STR) == 0) {
        value = 1.0 / boost::math::constants::root_two<double>();
    } else if(constantStr.compare(BLOCKLY_INFINITY_STR) == 0) {
        value = std::numeric_limits<double>::max();
    } else {
        throw(std::invalid_argument("MathBlocks::getNumericConstant. Unknow constant " + constantStr));
    }
    return value;
}
