#ifndef LOGICBLOCKSMANAGER_H
#define LOGICBLOCKSMANAGER_H

#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <protocolGraph/operables/mathematics/VariableEntry.h>

#include "bioblocksTranslation/bioblockstranslator_global.h"

class LOGICBLOCKSMANAGER_EXPORT LogicBlocksManager
{
public:
    LogicBlocksManager();
    virtual ~LogicBlocksManager();

    void addIfLogicBlockStartId(int id);
    void addWhileLogicBlockStartId(int id);
    void setLogicBlockIdPhyscal(int id);

    bool isStartLogicIfBlock(int id) const;
    bool isStartLogicWhileBlock(int id) const;
    bool isPhysicalLogicBlock(int id) const;

    void addIfBranchTriggerVar(int id, std::shared_ptr<VariableEntry> varEntry);
    void addWhileTriggerVar(int id, std::shared_ptr<VariableEntry> varEntry);

    void addIfEndVar(int id, std::shared_ptr<VariableEntry> varEntry);
    void addWhileEndVar(int id, std::shared_ptr<VariableEntry> varEntry);

    void addIfExecutingFlagVar(int id, std::shared_ptr<VariableEntry> varEntry);
    void addWhileExecutingFlagVar(int id, std::shared_ptr<VariableEntry> varEntry);

    const std::vector<std::shared_ptr<VariableEntry>> & getBranchesTriggeredVars(int id) const throw(std::invalid_argument);
    std::shared_ptr<VariableEntry> getWhileTrigeredVar(int id) const throw(std::invalid_argument);

    std::shared_ptr<VariableEntry> getIfExecutingFlagVar(int id) const throw(std::invalid_argument);
    std::shared_ptr<VariableEntry> getWhileExecutingFlagVar(int id) const throw(std::invalid_argument);

    const std::vector<std::shared_ptr<VariableEntry>> & getIfEndVars(int id) const throw(std::invalid_argument);
    const std::vector<std::shared_ptr<VariableEntry>> & getWhilesEndVars(int id) const throw(std::invalid_argument);

    std::string toString() const;

protected:
    std::unordered_set<int> logicIfBlockStartIds;
    std::unordered_set<int> logicWhileBlockStartIds;

    std::unordered_set<int> physicalLogicBlocks;

    std::unordered_map<int, std::shared_ptr<VariableEntry>> ifsExecutigFlagMap;
    std::unordered_map<int, std::shared_ptr<VariableEntry>> whilesExecutigFlagMap;

    std::unordered_map<int, std::vector<std::shared_ptr<VariableEntry>>> ifBranchesTrigeredVarMap;
    std::unordered_map<int, std::shared_ptr<VariableEntry>> whilesTrigeredVarMap;

    std::unordered_map<int, std::vector<std::shared_ptr<VariableEntry>>> whilesEndVarsMap;
    std::unordered_map<int, std::vector<std::shared_ptr<VariableEntry>>> ifsEndVarsMap;
};

#endif // LOGICBLOCKSMANAGER_H
