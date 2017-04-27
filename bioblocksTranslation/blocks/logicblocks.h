#ifndef LOGICBLOCKS_H
#define LOGICBLOCKS_H

#define LOGIC_COMPARE_STR "logic_compare"
#define LOGIC_OPERATION_STR "logic_operation"
#define LOGIC_NEGATE_STR "logic_negate"
#define LOGIC_BOOLEAN_STR "logic_boolean"
#define LOGIC_NULL_STR "logic_null"
#define MATH_NUMBER_PROPERTY_STR "math_number_property"

#define BLOCKLY_EQ_STR "EQ"
#define BLOCKLY_NEQ_STR "NEQ"
#define BLOCKLY_LT_STR "LT"
#define BLOCKLY_LTE_STR "LTE"
#define BLOCKLY_GT_STR "GT"
#define BLOCKLY_GTE_STR "GTE"

#define BLOCKLY_AND_STR "AND"
#define BLOCKLY_OR_STR "OR"

#define BLOCKLY_TRUE_STR "TRUE"
#define BLOCKLY_FALSE_STR "FALSE"

#define BLOCKLY_EVEN_STR "EVEN"
#define BLOCKLY_ODD_STR "ODD"
#define BLOCKLY_PRIME_STR "PRIME"
#define BLOCKLY_WHOLE_STR "WHOLE"
#define BLOCKLY_POSITIVE_STR "POSITIVE"
#define BLOCKLY_NEGATIVE_STR "NEGATIVE"
#define BLOCKLY_DIVISIBLE_BY_STR "DIVISIBLE_BY"

#include <memory>
#include <stdexcept>

//lib
#include <json.hpp>

//local
#include <protocolGraph/ProtocolGraph.h>
#include <protocolGraph/operables/comparison/ComparisonOperable.h>
#include <protocolGraph/operables/comparison/BooleanComparison.h>
#include <protocolGraph/operables/comparison/SimpleComparison.h>
#include <protocolGraph/operables/comparison/characteristiccheck.h>
#include <protocolGraph/operables/comparison/protocolboolf.h>
#include <protocolGraph/operables/mathematics/protocolmathf.h>

#include "bioblocksTranslation/blocks/blocksutils.h"

class MathBlocks;

class LogicBlocks {
public:
    LogicBlocks(std::shared_ptr<ProtocolGraph> protocolPtr, std::shared_ptr<MathBlocks> mathTrans = NULL);
    virtual ~LogicBlocks();

    std::shared_ptr<ComparisonOperable> translateLogicBlock(const nlohmann::json & logicJSONObj) throw(std::invalid_argument);

    inline void setMathTrans(std::shared_ptr<MathBlocks> mathTrans) {
        this->mathTrans = mathTrans;
    }

protected:
    std::shared_ptr<ProtocolGraph> protocolPtr;
    std::shared_ptr<MathBlocks> mathTrans;

    std::shared_ptr<ComparisonOperable> logicCompareOperation(const nlohmann::json & logicCompareObj) throw(std::invalid_argument);
    std::shared_ptr<ComparisonOperable> logicOperationOperation(const nlohmann::json & logicOperationObj) throw(std::invalid_argument);
    std::shared_ptr<ComparisonOperable> logicNegateOperation(const nlohmann::json & logicNegateObj) throw(std::invalid_argument);
    std::shared_ptr<ComparisonOperable> logicBooleanOperation(const nlohmann::json & logicBooleanObj) throw(std::invalid_argument);
    std::shared_ptr<ComparisonOperable> logicNullOperation(const nlohmann::json & logicNullObj) throw(std::invalid_argument);
    std::shared_ptr<ComparisonOperable> mathNumberPropertyOperation(const nlohmann::json & mathNumberPropertyObj) throw(std::invalid_argument);

    SimpleComparison::ComparisonOperator getComparisonOperator(const std::string & opStr) throw(std::invalid_argument);
    BooleanComparison::BooleanOperator getBooleanOperator(const std::string & opStr) throw(std::invalid_argument);
    CharacteristicCheck::CheckOperations getCheckOperator(const std::string & opStr) throw(std::invalid_argument);
    std::shared_ptr<Tautology> getBoolConstant(const std::string & opStr) throw(std::invalid_argument);
    std::shared_ptr<ComparisonOperable> getNumberCheckOp(const std::string & opStr) throw(std::invalid_argument);

};

#endif // LOGICBLOCKS_H
