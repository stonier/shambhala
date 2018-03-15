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
#include <memory>

#include <drake/systems/analysis/simulator.h>
#include <drake/systems/framework/value.h>
#include <drake/systems/framework/context.h>
#include <drake/systems/framework/diagram.h>
#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/framework/leaf_system.h>
//#include <drake/systems/framework/vector_base.h>

/*****************************************************************************
 * Methods
 ****************************************************************************/

// Whatever

class Foo {
public:
	Foo();
	void update();
private:
	int counter;
};

Foo::Foo() : counter(0) {
	std::cout << "Foo::Foo()" << std::endl;
}

void Foo::update() {
	counter++;
}

class FooSystem : public drake::systems::LeafSystem<double> {
public:
//	DRAKE_NO_COPY_NO_MOVE_NO_ASSIGN(FooSystem)
    FooSystem();
private:
    void DoCalcUnrestrictedUpdate(
          const drake::systems::Context<double>& context,
          const std::vector<const drake::systems::UnrestrictedUpdateEvent<double>*>& events,
          drake::systems::State<double>* state) const;
};

FooSystem::FooSystem() {
    std::cout << "FooSystem::FooSystem()" << std::endl;
    // unrestricted is not a continuous/discrete time system, lets you access and update a mutable State
    this->DeclarePeriodicUnrestrictedUpdate(1.0);
    //int DeclareAbstractState(std::unique_ptr<AbstractValue> abstract_state) {
    this->DeclareAbstractState(drake::systems::AbstractValue::Make(Foo())); // std::make_unique<AbstractValue>());

    // this->DeclareInputPort(drake::systems::kVectorValued, 1);
    // Adding one generalized position and one generalized velocity.
    // A 2D output vector for position and velocity.
    // this->DeclareVectorOutputPort(drake::systems::BasicVector<T>(2),
    //                                 &Particle::CopyStateOut);
}

void FooSystem::DoCalcUnrestrictedUpdate(
          const drake::systems::Context<double>& context,
          const std::vector<const drake::systems::UnrestrictedUpdateEvent<double>*>& events,
          drake::systems::State<double>* state) const
{
	std::cout << "Unrestricted update event" << std::endl;
	auto foo_absolute_values = &state->get_mutable_abstract_state();
	// auto foo_value = state->get_mutable_abstract_state().get_mutable_value(0);
    // do i need to register events?
	// Plan A: bypass events and get foo into state
	// Plan B: figure out wtf is going on with events

}

class Diagram : public drake::systems::Diagram<double> {
public:
	explicit Diagram();

	std::unique_ptr<drake::systems::Context<double>> CreateContext() const;
};

Diagram::Diagram() {
	std::cout << "Diagram::Diagram();" << std::endl;
	drake::systems::DiagramBuilder<double> builder;
	builder.template AddSystem<FooSystem>();
	builder.BuildInto(this);
}

std::unique_ptr<drake::systems::Context<double>> Diagram::CreateContext() const {
	std::cout << "Diagram::CreateContext();" << std::endl;
	auto context = this->AllocateContext();
	return context;
}

/*****************************************************************************
 * Main
 ****************************************************************************/

int main(int /*argc*/, char** /*argv*/) {

  std::cout << std::endl;
  std::cout << "***********************************************************" << std::endl;
  std::cout << "                  External Update" << std::endl;
  std::cout << "***********************************************************" << std::endl;
  std::cout << std::endl;

  auto diagram = std::make_unique<Diagram>();
  auto context = diagram->CreateContext();
  auto simulator = std::make_unique<drake::systems::Simulator<double>>(*diagram, std::move(context));
  simulator->set_target_realtime_rate(1.0);
  simulator->Initialize();
  simulator->StepTo(10.0);
  return 0;
}

