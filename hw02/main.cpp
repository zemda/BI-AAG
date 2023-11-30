#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <vector>

using Symbol = char;
using Word = std::vector<Symbol>;

struct Grammar {
    std::set<Symbol> m_Nonterminals;
    std::set<Symbol> m_Terminals;
    std::vector<std::pair<Symbol, std::vector<Symbol>>> m_Rules;
    Symbol m_InitialSymbol;
};


Word reconstructWord(const Grammar& grammar, std::vector<size_t> trace) {
    Word word;
    std::function<void(size_t)> applyRule = [&](size_t ruleIndex) mutable {
        const auto& rule = grammar.m_Rules[ruleIndex];
        for (const auto& symbol : rule.second) {
            if (grammar.m_Terminals.count(symbol))
                word.push_back(symbol);
            else 
                if (!trace.empty()) {
                    size_t nextRuleIndex = trace.front();
                    trace.erase(trace.begin());
                    applyRule(nextRuleIndex);
                }
            
        }
    };
    while (!trace.empty()) {
        size_t ruleIndex = trace.front();
        trace.erase(trace.begin());
        applyRule(ruleIndex);
    }
    return word;
}

std::vector<size_t> trace(const Grammar& grammar, const Word& word) {
    if (word.empty()) {
        for (size_t i = 0; i < grammar.m_Rules.size(); ++i) 
            if (grammar.m_Rules[i].first == grammar.m_InitialSymbol && grammar.m_Rules[i].second.empty())
                return {i};
            
        return {};
    }
    size_t n = word.size();
    std::map<Symbol, std::vector<std::vector<int>>> table;

    for (const auto& nonTerminal : grammar.m_Nonterminals)
        table[nonTerminal] = std::vector<std::vector<int>>(n, std::vector<int>(n, -1));
    

    for (size_t charIndex = 0; charIndex < n; ++charIndex) //for each char in word
        for (size_t ruleIndex = 0; ruleIndex < grammar.m_Rules.size(); ++ruleIndex){ // for each rule
            auto& rule = grammar.m_Rules[ruleIndex];
            if (rule.second.size() == 1 && rule.second[0] == word[charIndex]) // if rule is terminal and if matches curr char
                table[rule.first][charIndex][charIndex] = ruleIndex;
        }

    for (size_t len = 2; len <= n; ++len) // for each length of substring
        for (size_t startPos = 0; startPos <= n - len; ++startPos) // for each possible starting pos of substring
            for (size_t splitPos = startPos; splitPos < startPos + len - 1; ++splitPos) // for each possible split position within substring
                for (size_t ruleIndex = 0; ruleIndex < grammar.m_Rules.size(); ++ruleIndex){ // for each rule
                    auto& rule = grammar.m_Rules[ruleIndex];
                    if (rule.second.size() == 2 && 
                        table[rule.second[0]][startPos][splitPos] != -1 && 
                        table[rule.second[1]][splitPos + 1][startPos + len - 1] != -1) //if rule iss nonTerminal and generates substring
                            table[rule.first][startPos][startPos + len - 1] = ruleIndex;
                    
                }       
            
    std::vector<size_t> result;
    if (table[grammar.m_InitialSymbol][0][n - 1] != -1){ // if initial symbol generates whole word
        std::function<void(Symbol, size_t, size_t)> backtrack = [&](Symbol current, size_t i, size_t j) {
            if (i == j){ // if current symbol is terminal, base case
                result.push_back(table[current][i][j]);
                return;
            }
            for (size_t k = i; k < j; ++k) // for each split pos within substring
                if (table[grammar.m_Rules[table[current][i][j]].second[0]][i][k] != -1 && // if 2 nonTerminals generate substring
                    table[grammar.m_Rules[table[current][i][j]].second[1]][k + 1][j] != -1){ //can generate two parts of substring at split k
                    result.push_back(table[current][i][j]);
                    backtrack(grammar.m_Rules[table[current][i][j]].second[0], i, k);
                    backtrack(grammar.m_Rules[table[current][i][j]].second[1], k + 1, j);
                    return;
                }
        };
        backtrack(grammar.m_InitialSymbol, 0, n - 1);
    }
    return result;
}

int main(){
    Grammar g0{
        {'A', 'B', 'C', 'S'},
        {'a', 'b'},
        {
            {'S', {'A', 'B'}},
            {'S', {'B', 'C'}},
            {'A', {'B', 'A'}},
            {'A', {'a'}},
            {'B', {'C', 'C'}},
            {'B', {'b'}},
            {'C', {'A', 'B'}},
            {'C', {'a'}},
        },
        'S'};

    assert(reconstructWord(g0, trace(g0, {'b', 'a', 'a', 'b', 'a'})) == Word({'b', 'a', 'a', 'b', 'a'}));
    assert(reconstructWord(g0, trace(g0, {'b'})) == Word({}));
    assert(reconstructWord(g0, trace(g0, {'a'})) == Word({}));
    assert(reconstructWord(g0, trace(g0, {})) == Word({}));
    assert(reconstructWord(g0, trace(g0, {'a', 'a', 'a', 'a', 'a'})) == Word({'a', 'a', 'a', 'a', 'a'}));
    assert(reconstructWord(g0, trace(g0, {'a', 'b'})) == Word({'a', 'b'}));
    assert(reconstructWord(g0, trace(g0, {'b', 'a'})) == Word({'b', 'a'}));
    assert(reconstructWord(g0, trace(g0, {'c', 'a'})) == Word({}));

    Grammar g1{
        {'A', 'B'},
        {'x', 'y'},
        {
            {'A', {}},
            {'A', {'x'}},
            {'B', {'x'}},
            {'A', {'B', 'B'}},
            {'B', {'B', 'B'}},
        },
        'A'};
    assert(reconstructWord(g1, trace(g1, {})) == Word({}));
    assert(reconstructWord(g1, trace(g1, {'x'})) == Word({'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x'})) == Word({'x', 'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x', 'x'})) == Word({'x', 'x', 'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x', 'x', 'x'})) == Word({'x', 'x', 'x', 'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x', 'x', 'x', 'x'})) == Word({'x', 'x', 'x', 'x', 'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x', 'x', 'x', 'x', 'x'})) == Word({'x', 'x', 'x', 'x', 'x', 'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x'})) == Word({'x', 'x', 'x', 'x', 'x', 'x', 'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'})) == Word({'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}));
    assert(reconstructWord(g1, trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'})) == Word({'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}));

    Grammar g2{
        {'A', 'B'},
        {'x', 'y'},
        {
            {'A', {'x'}},
            {'B', {'x'}},
            {'A', {'B', 'B'}},
            {'B', {'B', 'B'}},
        },
        'B'};
    assert(reconstructWord(g2, trace(g2, {})) == Word({}));
    assert(reconstructWord(g2, trace(g2, {'x'})) == Word({'x'}));
    assert(reconstructWord(g2, trace(g2, {'x', 'x'})) == Word({'x', 'x'}));
    assert(reconstructWord(g2, trace(g2, {'x', 'x', 'x'})) == Word({'x', 'x', 'x'}));

    Grammar g3{
        {'A', 'B', 'C', 'D', 'E', 'S'},
        {'a', 'b'},
        {
            {'S', {'A', 'B'}},
            {'S', {'S', 'S'}},
            {'S', {'a'}},
            {'A', {'B', 'S'}},
            {'A', {'C', 'D'}},
            {'A', {'b'}},
            {'B', {'D', 'D'}},
            {'B', {'b'}},
            {'C', {'D', 'E'}},
            {'C', {'b'}},
            {'C', {'a'}},
            {'D', {'a'}},
            {'E', {'S', 'S'}},
        },
        'S'};
    assert(reconstructWord(g3, trace(g3, {})) == Word({}));
    assert(reconstructWord(g3, trace(g3, {'a', 'b', 'a', 'a', 'b'})) == Word({'a', 'b', 'a', 'a', 'b'}));
    assert(reconstructWord(g3, trace(g3, {'a', 'b', 'a', 'a', 'b', 'a', 'b', 'a', 'b', 'a', 'a'})) == Word({'a', 'b', 'a', 'a', 'b', 'a', 'b', 'a', 'b', 'a', 'a'}));
}