/**
 * @brief A simple drake system with updates to an external class
 *
 * Representative use case for algorithms, with their own state,
 * that exist outside a drake diagram but need to be updated
 * by the drake diagram.
 *
 * e.g. dynamic part of maliput traffic agents, delphyne mhdm agent.
 **/
/*****************************************************************************
** Disable check
*****************************************************************************/

#include <iostream>

#include <drake/systems/framework/context.h>
#include <drake/systems/framework/diagram.h>
#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/framework/vector_base.h>

/*****************************************************************************
 * Methods
 ****************************************************************************/

/*****************************************************************************
 * Main
 ****************************************************************************/

int main(int /*argc*/, char** /*argv*/) {

  std::cout << std::endl;
  std::cout << "***********************************************************" << std::endl;
  std::cout << "                  External Update" << std::endl;
  std::cout << "***********************************************************" << std::endl;
  std::cout << std::endl;

  return 0;
}

