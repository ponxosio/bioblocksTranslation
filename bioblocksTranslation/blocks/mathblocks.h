#ifndef MATHBLOCKS_H
#define MATHBLOCKS_H

#define MATH_NUMBER_STR "math_number"
#define MATH_ARITHMETIC_STR "math_arithmetic"
#define MATH_SINGLE_STR "math_single"
#define MATH_TRIG_STR "math_trig"
#define MATH_CONSTANT_STR "math_constant"
#define MATH_ROUND_STR "math_round"
#define MATH_ON_LIST_STR "math_on_list"
#define MATH_MODULO_STR "math_modulo"
#define MATH_CONSTRAIN_STR "math_constrain"
#define MATH_RANDOM_INT_STR "math_random_int"
#define MATH_RANDOM_FLOAT_STR "math_random_float"
#define VARIABLES_GET_STR "variables_get"
#define LOGICTERNARY_STR "logic_ternary"

#define BLOCKLY_PLUS_STR "ADD"
#define BLOCKLY_MINUS_STR "MINUS"
#define BLOCKLY_MULTIPLY_STR "MULTIPLY"
#define BLOCKLY_DIVIDE_STR "DIVIDE"
#define BLOCKLY_POWER_STR "POWER"

#define BLOCKLY_ABSOLUTE_VALUE_STRING "ABS"
#define BLOCKLY_FLOOR_VALUE_STRING "ROUNDDOWN"
#define BLOCKLY_ROUND_VALUE_STRING "ROUND"
#define BLOCKLY_CEILING_VALUE_STRING "ROUNDUP"
#define BLOCKLY_SQRT_VALUE_STRING "ROOT"
#define BLOCKLY_LN_VALUE_STRING "LN"
#define BLOCKLY_LOG10_VALUE_STRING "LOG10"
#define BLOCKLY_EXP_E_VALUE_STRING "EXP"
#define BLOCKLY_EXP_10_VALUE_STRING "POW10"
#define BLOCKLY_MINUS_VALUE_STRING "NEG"
#define BLOCKLY_SIN_VALUE_STRING "SIN"
#define BLOCKLY_COS_VALUE_STRING "COS"
#define BLOCKLY_TAN_VALUE_STRING "TAN"
#define BLOCKLY_ASIN_VALUE_STRING "ASIN"
#define BLOCKLY_ACOS_VALUE_STRING "ACOS"
#define BLOCKLY_ATAN_VALUE_STRING "ATAN"

#define BLOCKLY_PI_STR "PI"
#define BLOCKLY_E_STR "E"
#define BLOCKLY_GOLDEN_RATIO_STR "GOLDEN_RATIO"
#define BLOCKLY_SQRT2_STR "SQRT2"
#define BLOCKLY_SQRT1_2_STR "SQRT1_2"
#define BLOCKLY_INFINITY_STR "INFINITY"

#include <memory>
#include <sstream>
#include <stdexcept>

//lib
#include <boost/math/constants/constants.hpp>
#include <json.hpp>

//local
#include <protocolGraph/operables/mathematics/MathematicOperable.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>
#include <protocolGraph/ProtocolGraph.h>

#include "bioblocksTranslation/blocks/blocksutils.h"

class LogicBlocks;

class MathBlocks
{
public:
    MathBlocks(std::shared_ptr<ProtocolGraph> protocolPtr, std::shared_ptr<LogicBlocks> logicTrans = NULL);
    virtual ~MathBlocks();

    std::shared_ptr<MathematicOperable> translateMathBlock(const nlohmann::json & mathJSONObj) throw(std::invalid_argument);

    inline void setLogicTrans(std::shared_ptr<LogicBlocks> logicTrans) {
        this->logicTrans = logicTrans;
    }

protected:
    std::shared_ptr<ProtocolGraph> protocolPtr;
    std::shared_ptr<LogicBlocks> logicTrans;

    std::shared_ptr<MathematicOperable> mathNumberOperation(const nlohmann::json & mathNumberObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathArithmeticOperation(const nlohmann::json & mathArithmeticObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathSingleOperation(const nlohmann::json & mathSingleObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathConstantOperation(const nlohmann::json & constantOperationObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathOnListOperation(const nlohmann::json & mathOnListObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathModuloOperation(const nlohmann::json & mathModuloObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathConstrainOperation(const nlohmann::json & mathConstrainObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathRandomIntOperation(const nlohmann::json & mathRandomIntObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> mathRandomFloatOperation();

    std::shared_ptr<MathematicOperable> logicTernaryOperation(const nlohmann::json & logicTernaryObj) throw(std::invalid_argument);
    std::shared_ptr<MathematicOperable> variableGetOperation(const nlohmann::json & variableGetObj) throw(std::invalid_argument);

    ArithmeticOperation::ArithmeticOperator getArithmeticOperator(const std::string & aopStr) throw(std::invalid_argument);
    UnaryOperation::UnaryOperator getUnaryOperator(const std::string & uopStr) throw(std::invalid_argument);
    double getNumericConstant(const std::string & constantStr) throw(std::invalid_argument);
};

#endif // MATHBLOCKS_H
