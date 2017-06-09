#include "logicblocksmanager.h"

LogicBlocksManager::LogicBlocksManager() {
    mainLoopId = -1;
}

LogicBlocksManager::~LogicBlocksManager() {

}

void LogicBlocksManager::addIfLogicBlockStartId(int id) {
    logicIfBlockStartIds.insert(id);
}

void LogicBlocksManager::addWhileLogicBlockStartId(int id) {
    logicWhileBlockStartIds.insert(id);
}

void LogicBlocksManager::setLogicBlockIdPhyscal(int id) {
    physicalLogicBlocks.insert(id);
}

bool LogicBlocksManager::isStartLogicIfBlock(int id) const {
    auto finded = logicIfBlockStartIds.find(id);
    return (finded != logicIfBlockStartIds.end());
}

bool LogicBlocksManager::isStartLogicWhileBlock(int id) const {
    return (logicWhileBlockStartIds.find(id) != logicWhileBlockStartIds.end());
}

bool LogicBlocksManager::isPhysicalLogicBlock(int id) const {
    return (physicalLogicBlocks.find(id) != physicalLogicBlocks.end());
}

void LogicBlocksManager::addIfBranchTriggerVar(int id, std::shared_ptr<VariableEntry> varEntry) {
    auto finded = ifBranchesTrigeredVarMap.find(id);
    if (finded != ifBranchesTrigeredVarMap.end()) {
        finded->second.push_back(varEntry);
    } else {
        std::vector<std::shared_ptr<VariableEntry>> vector{varEntry};
        ifBranchesTrigeredVarMap.insert(std::make_pair(id, vector));
    }
}

void LogicBlocksManager::addWhileTriggerVar(int id, std::shared_ptr<VariableEntry> varEntry) {
    whilesTrigeredVarMap[id] = varEntry;
}

void LogicBlocksManager::addIfEndVar(int id, std::shared_ptr<VariableEntry> varEntry) {
    auto finded = ifsEndVarsMap.find(id);
    if (finded != ifsEndVarsMap.end()) {
        finded->second.push_back(varEntry);
    } else {
        std::vector<std::shared_ptr<VariableEntry>> vector{varEntry};
        ifsEndVarsMap.insert(std::make_pair(id, vector));
    }
}

void LogicBlocksManager::addWhileEndVar(int id, std::shared_ptr<VariableEntry> varEntry) {
    auto finded = whilesEndVarsMap.find(id);
    if (finded != whilesEndVarsMap.end()) {
        finded->second.push_back(varEntry);
    } else {
        std::vector<std::shared_ptr<VariableEntry>> vector{varEntry};
        whilesEndVarsMap.insert(std::make_pair(id, vector));
    }
}

void LogicBlocksManager::addIfExecutingFlagVar(int id, std::shared_ptr<VariableEntry> varEntry) {
    ifsExecutigFlagMap[id] = varEntry;
}

void LogicBlocksManager::addWhileExecutingFlagVar(int id, std::shared_ptr<VariableEntry> varEntry) {
    whilesExecutigFlagMap[id] = varEntry;
}

const std::vector<std::shared_ptr<VariableEntry>> & LogicBlocksManager::getBranchesTriggeredVars(int id) const throw(std::invalid_argument) {
    auto finded = ifBranchesTrigeredVarMap.find(id);
    if (finded != ifBranchesTrigeredVarMap.end()) {
        return finded->second;
    } else {
        throw(std::invalid_argument("LogicBlocksManager::getBranchesTriggeredVars." + std::to_string(id) + " is not on ifBranchesTrigeredVarMap"));
    }
}

std::shared_ptr<VariableEntry> LogicBlocksManager::getWhileTrigeredVar(int id) const throw(std::invalid_argument) {
    auto finded = whilesTrigeredVarMap.find(id);
    if (finded != whilesTrigeredVarMap.end()) {
        return finded->second;
    } else {
        throw(std::invalid_argument("LogicBlocksManager::getWhileTrigeredVar." + std::to_string(id) + " is not on whilesTrigeredVarMap"));
    }
}

std::shared_ptr<VariableEntry> LogicBlocksManager::getIfExecutingFlagVar(int id) const throw(std::invalid_argument) {
    auto finded = ifsExecutigFlagMap.find(id);
    if (finded != ifsExecutigFlagMap.end()) {
        return finded->second;
    } else {
        throw(std::invalid_argument("LogicBlocksManager::getIfExecutingFlagVar." + std::to_string(id) + " is not on ifsExecutigFlagMap"));
    }
}

std::shared_ptr<VariableEntry> LogicBlocksManager::getWhileExecutingFlagVar(int id) const throw(std::invalid_argument) {
    auto finded = whilesExecutigFlagMap.find(id);
    if (finded != whilesExecutigFlagMap.end()) {
        return finded->second;
    } else {
        throw(std::invalid_argument("LogicBlocksManager::getWhileExecutingFlagVar." + std::to_string(id) + " is not on whilesExecutigFlagMap"));
    }
}

const std::vector<std::shared_ptr<VariableEntry>> & LogicBlocksManager::getIfEndVars(int id) const throw(std::invalid_argument) {
    auto finded = ifsEndVarsMap.find(id);
    if (finded != ifsEndVarsMap.end()) {
        return finded->second;
    } else {
        throw(std::invalid_argument("LogicBlocksManager::getIfEndVars." + std::to_string(id) + " is not on ifsEndVarsMap"));
    }
}

const std::vector<std::shared_ptr<VariableEntry>> & LogicBlocksManager::getWhilesEndVars(int id) const throw(std::invalid_argument) {
    auto finded = whilesEndVarsMap.find(id);
    if (finded != whilesEndVarsMap.end()) {
        return finded->second;
    } else {
        throw(std::invalid_argument("LogicBlocksManager::getWhilesEndVars." + std::to_string(id) + " is not on whilesEndVarsMap"));
    }
}

std::string LogicBlocksManager::toString() const {
    std::stringstream stream;

    stream << "ifs:[";
    for(int id : logicIfBlockStartIds) {
        stream << id << ",";
    }
    stream << "];";

    stream << "whiles:[";
    for(int id : logicWhileBlockStartIds) {
        stream << id << ",";
    }
    stream << "];";

    stream << "physical:[";
    for(int id : physicalLogicBlocks) {
        stream << id << ",";
    }
    stream << "];";

    stream << "triggeredWhileVar:[";
    for(auto pair : whilesTrigeredVarMap) {
        stream << pair.first << " : " << pair.second->toString() << ",";
    }
    stream << "];";

    stream << "ifExecutingFlag:[";
    for(auto pair : ifsExecutigFlagMap) {
        stream << pair.first << " : " << pair.second->toString() << ",";
    }
    stream << "];";

    stream << "triggeredIfVar:[";
    for(auto pair : ifBranchesTrigeredVarMap) {
        stream << pair.first << " : ";

        const std::vector<std::shared_ptr<VariableEntry>> & vector = pair.second;
        stream << "[";
        for(auto var : vector) {
            stream << var->toString() << ",";
        }
        stream << "],";
    }
    stream << "];";

    stream << "endIfVar:[";
    for(auto pair : ifsEndVarsMap) {
        stream << pair.first << " : ";

        const std::vector<std::shared_ptr<VariableEntry>> & vector = pair.second;
        stream << "[";
        for(auto var : vector) {
            stream << var->toString() << ",";
        }
        stream << "],";
    }
    stream << "];";

    stream << "endWhileVar:[";
    for(auto pair : whilesEndVarsMap) {
        stream << pair.first << " : ";

        const std::vector<std::shared_ptr<VariableEntry>> & vector = pair.second;
        stream << "[";
        for(auto var : vector) {
            stream << var->toString() << ",";
        }
        stream << "],";
    }
    stream << "];";

    return stream.str();
}
