#include <metaSMT/support/default_visitation_unrolling_limit.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/API/Stack.hpp>
#include <metaSMT/API/SymbolTable.hpp>
#include <metaSMT/backend/SWORD_Backend.hpp>
#include "options.hpp"
typedef metaSMT::DirectSolver_Context<
  metaSMT::IgnoreSymbolTable<
    metaSMT::Stack<metaSMT::solver::SWORD_Backend>
  >
> ContextType;
OptionMap options;
#include "main.cpp"
