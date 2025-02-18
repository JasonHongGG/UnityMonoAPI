#pragma once
#include <iostream>
#include <memory>
#include "SearchStrategyInterface.h"
#include "NormalSearchStrategy.h"
#include "PatternSearchStrategy.h"
#include "SundaySearchStrategy.h"

class SearchStrategyFactory {
public:
    static std::shared_ptr<ISearchStrategy> CreateStrategy(int iSearchMode, bool alignSearch = false) {
        switch (iSearchMode) {
        case 0:
            return std::make_shared<NormalSearchStrategy>();
        case 1:
            return std::make_shared<SundaySearchStrategy>();
        case 2:
            return std::make_shared<PatternSearchStrategy>(alignSearch);
        default:
            throw std::invalid_argument("Unsupported search mode");
        }
    }
};