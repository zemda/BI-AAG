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

/**
 * @brief Reconstructs the word from a given sequence of rule indices.
 * 
 * The function creates a lambda function that applies a rule from the grammar.
 * If the rule derives a terminal symbol, the function adds this symbol to the word.
 * If the rule derives a nonterminal symbol, the function recursively calls itself with the next rule in the sequence.
 * After defining the lambda function, the function iterates over the sequence of rule indices and applies each rule using the lambda function.
 * 
 * @param grammar The context-free grammar used for parsing.
 * @param trace A sequence of indices of rules in the grammar that can derive the word.
 * @return The word derived from the sequence of rule indices.
 */
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
    
    /**
     * @brief Key is nonTerminal, value is 2D vector of size n x n, 
     * where each cell contains index of rule that generated substring
     */
    std::map<Symbol, std::vector<std::vector<int>>> table; 

    /**
     * @brief Initialize table with -1, which means that substring cannot be generated
     */
    for (const auto& nonTerminal : grammar.m_Nonterminals)
        table[nonTerminal] = std::vector<std::vector<int>>(n, std::vector<int>(n, -1));
    
    /**
     * @brief Iterates over each character in the word and each rule in the grammar 
     * and  fills in the diagonal of the table
     * 
     *  If a rule is a terminal rule (i.e., it derives a single terminal symbol) and the terminal symbol 
     *  matches the current character in the word, the function stores the index of this rule in the table 
     *  for the nonterminal symbol of the rule and the substring of the word consisting of the current character.
     * 
     */
    for (size_t charIndex = 0; charIndex < n; ++charIndex)
        for (size_t ruleIndex = 0; ruleIndex < grammar.m_Rules.size(); ++ruleIndex){
            auto& [nonTerminal, ruleRightSide] = grammar.m_Rules[ruleIndex];
            if (ruleRightSide.size() == 1 && ruleRightSide[0] == word[charIndex])
                table[nonTerminal][charIndex][charIndex] = ruleIndex;
        }

    /**
     * @brief Fills in the table for all substrings of length 2 or more.
     * 
     * The function iterates over all possible lengths of substrings, starting positions, split positions, and rules.
     * If a rule is a nonterminal rule (i.e., it derives two nonterminal symbols) and the rule 
     * can be used to derive the current substring (the two parts), the function saves the index of this 
     * rule in the table for the nonterminal symbol of the rule and the current substring.
     * 
     * @param len The length of the current substring.
     * @param startPos The starting position of the current substring in the word.
     * @param splitPos The position where the current substring is split into two parts.
     * @param ruleIndex The index of the current rule in the list of grammar rules.
     */
    for (size_t len = 2; len <= n; ++len)
        for (size_t startPos = 0; startPos <= n - len; ++startPos)
            for (size_t splitPos = startPos; splitPos < startPos + len - 1; ++splitPos)
                for (size_t ruleIndex = 0; ruleIndex < grammar.m_Rules.size(); ++ruleIndex){
                    auto& [nonTerminal, ruleRightSide] = grammar.m_Rules[ruleIndex];
                    if (ruleRightSide.size() == 2 &&
                        table[ruleRightSide[0]][startPos][splitPos] != -1 && 
                        table[ruleRightSide[1]][splitPos + 1][startPos + len - 1] != -1)
                            table[nonTerminal][startPos][startPos + len - 1] = ruleIndex;
                    
                }       
    /**
     * @brief Backtracks through the table to find a sequence of rule indices that can derive the word.
     * 
     * The function first checks if the initial symbol can derive the entire word. If it can, the function creates a lambda function for backtracking.
     * The lambda function takes a nonterminal symbol and the start and end indices of a substring of the word as parameters.
     * If the start and end indices are the same, the function adds the index of the rule used to derive the terminal symbol at this position to the result vector.
     * If the start and end indices are different, the function iterates over all possible split positions within the substring.
     * If it finds a split position where the two parts of the substring can be derived by two nonterminal symbols according to a rule in the grammar,
     * the function adds the index of this rule to the result vector and recursively calls itself for the two parts of the substring.
     * After defining the lambda function, the function calls it for the initial symbol and the entire word.
     * 
     * @return A vector of indices of rules in the grammar that can derive the word in the order they are applied
     */
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