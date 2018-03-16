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
#include <drake/systems/framework/event.h>
#include <drake/systems/framework/diagram_builder.h>
#include <drake/systems/framework/leaf_system.h>

/*****************************************************************************
 * Methods
 ****************************************************************************/

// Whatever

class Foo {
public:
	Foo();
	void increment();
	int count() const { return counter; }
private:
	int counter;
};

Foo::Foo() : counter(0) {
	std::cout << "Foo::Foo()" << std::endl;
}

void Foo::increment() {
	counter = counter+1;
}

class FooSystem : public drake::systems::LeafSystem<double> {
public:
  DRAKE_NO_COPY_NO_MOVE_NO_ASSIGN(FooSystem)
  FooSystem(const std::shared_ptr<Foo>& foo_ptr);
private:
  typedef drake::systems::PublishEvent<double>::PublishEvent::PublishCallback PublishCallback;
//  typedef std::function<void(const drake::systems::Context<double>&, const drake::systems::PublishEvent<T>&)> PublishCallback;

  void DoCalcUnrestrictedUpdate(
      const drake::systems::Context<double>& context,
      const std::vector<const drake::systems::UnrestrictedUpdateEvent<double>*>& events,
      drake::systems::State<double>* state) const;

  std::shared_ptr<Foo> foo_ptr;
};

FooSystem::FooSystem(const std::shared_ptr<Foo>& foo_ptr) : foo_ptr(foo_ptr) {
  std::cout << "FooSystem::FooSystem()" << std::endl;
  // unrestricted is not a continuous/discrete time system, lets you access and update a mutable State
  this->DeclarePeriodicUnrestrictedUpdate(1.0);
  this->DeclareAbstractState(drake::systems::AbstractValue::Make(*foo_ptr));

  PublishCallback callback = [this](
      const drake::systems::Context<double>& c,
      const drake::systems::PublishEvent<double>&)
  {
    std::cout << "Publish Event Callback" << std::endl;
  };
}

/**
 * This can be a speculative update!
 * You don't want to update external storage here, since it may need to get rewound.
 */
void FooSystem::DoCalcUnrestrictedUpdate(
          const drake::systems::Context<double>& /* context */,
          const std::vector<const drake::systems::UnrestrictedUpdateEvent<double>*>& /* events */,
          drake::systems::State<double>* state) const
{
	std::cout << "Unrestricted update event" << std::endl;
	drake::systems::AbstractValues& foo_absolute_values = state->get_mutable_abstract_state();

	// Don't trust auto, it will do Foo foo, which means changes won't last beyond this scope
	Foo& foo = foo_absolute_values.get_mutable_value(0).template GetMutableValue<Foo>();
	int count = foo.count();
	foo.increment();
	std::cout << "Foo: " << count << "->" << foo.count() << std::endl;
	count = foo_ptr->count();
	foo_ptr->increment();
	std::cout << "FooPtr: " << count << "->" << foo_ptr->count() << std::endl;
}

class Diagram : public drake::systems::Diagram<double> {
public:
	explicit Diagram(const std::shared_ptr<Foo>& foo_ptr);

	std::unique_ptr<drake::systems::Context<double>> CreateContext() const;
};

Diagram::Diagram(const std::shared_ptr<Foo>& foo_ptr) {
	std::cout << "Diagram::Diagram();" << std::endl;
	drake::systems::DiagramBuilder<double> builder;
	builder.template AddSystem<FooSystem>(foo_ptr);
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

  std::shared_ptr<Foo> foo_ptr = std::make_shared<Foo>();
  auto diagram = std::make_unique<Diagram>(foo_ptr);
  auto context = diagram->CreateContext();
  auto simulator = std::make_unique<drake::systems::Simulator<double>>(*diagram, std::move(context));
  simulator->set_target_realtime_rate(1.0);
  simulator->Initialize();
  simulator->StepTo(10.0);

  std::cout << "External FooPtr: " << foo_ptr->count() << std::endl;
  return 0;
}

