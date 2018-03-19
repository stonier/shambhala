/**
 * @brief A simple drake system that updates an external object.
 *
 * This example demonstrates use of a drake system to be responsible
 * for updating, but not the storage of the object under consideration.
 *
 * One example of this would be traffic light entities in a simulation
 * that are maintained by the simulation ground truth layer but passed
 * to the update engine (potentially drake or other) for switching
 * the state. The actual traffic light object must be accessible at
 * the ground truth level to share data with the visual rendering
 * engine as well as provide details to a ground truth api for other
 * parts of the simulation (e.g. logging or the ego car software).
 *
 * It tests the following capabilities:
 *
 *  - initialising from and publishing to external storage
 *  - updating periodically
 *  - TODO: updating on an external trigger
 *    (e.g. human-interactive trigger for development workflows)
 *  - TODO: auto-differentiable handling
 *
 * How (key point is in how to handle speculative updates):
 *
 *  - A pointer to the external object is shared with the leaf system
 *    (for minimality: this could be structs of the external object's data)
 *  - A copy of the external object is added to the system's Abstract State
 *    (this copy is effectively 'cache' the state of speculative updates
 *  - Unrestricted updates are called to update the internal copy
 *    (these are the speculative updates)
 *  - If an unrestricted update is confirmed, it triggers a publishing event
 *  - The external copy is updated from the abstract state
 *    (up to the user to handle multi-thread get/set concerns if necessary)
 *
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
	void setCount(const int& count) { counter = count;}
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
  FooSystem(Foo& foo_ptr);
private:
  typedef drake::systems::PublishEvent<double>::PublishEvent::PublishCallback PublishCallback;

  void DoPublish(
      const drake::systems::Context<double>& context,
      const std::vector<const drake::systems::PublishEvent<double>*>& events) const override;

  void DoPublishEventUpdate(
      const drake::systems::Context<double>& context,
      const drake::systems::PublishEvent<double>& event);

  void DoCalcUnrestrictedUpdate(
      const drake::systems::Context<double>& context,
      const std::vector<const drake::systems::UnrestrictedUpdateEvent<double>*>& events,
      drake::systems::State<double>* state) const override;

  Foo& foo_ptr;
};

FooSystem::FooSystem(Foo& foo_ptr) : foo_ptr(foo_ptr) {
  std::cout << "FooSystem::FooSystem()" << std::endl;

  this->set_name("foo");

  // Use this to periodically update the system's internal copy of foo.
  // Note: unrestricted gives permission to update arbitrary values on the state.
  double period = 1.0;
  this->DeclarePeriodicUnrestrictedUpdate(period);

  // No need to declare a periodic publish update since that will automagically
  // trigger if a speculative unrestricted update is accepted
  // this->DeclarePeriodicPublish(period);

  // shift a copy of Foo over onto the state, this will get used for speculative updates
  this->DeclareAbstractState(drake::systems::AbstractValue::Make(foo_ptr));
}

void FooSystem::DoPublish(
    const drake::systems::Context<double>& context,
    const std::vector<const drake::systems::PublishEvent<double>*>& /* events */) const
{
  // If the event is a consequence of an unrestricted update, it will be of type kForced
  //
  // Q: Is there a way to work directly with the publish event callback system?

  std::cout << "Do Publish Callback" << std::endl;

  const drake::systems::AbstractValues& foo_absolute_values = context.get_abstract_state();
  const Foo& foo = foo_absolute_values.get_value(0).template GetValue<Foo>();
  int count = foo_ptr.count();
  foo_ptr.setCount(foo.count());
  std::cout << "FooPtr: " << count << "->" << foo_ptr.count() << std::endl;
}

void FooSystem::DoCalcUnrestrictedUpdate(
    const drake::systems::Context<double>& /* context */,
    const std::vector<const drake::systems::UnrestrictedUpdateEvent<double>*>& /* events */,
    drake::systems::State<double>* state) const
{
  // This can be a speculative update! Don't update external storage here, it may be rewound
  std::cout << "Unrestricted update event" << std::endl;
  drake::systems::AbstractValues& foo_absolute_values = state->get_mutable_abstract_state();

  Foo& foo = foo_absolute_values.get_mutable_value(0).template GetMutableValue<Foo>();
  int count = foo.count();
  foo.increment();
  std::cout << "Foo: " << count << "->" << foo.count() << std::endl;
}

class Diagram : public drake::systems::Diagram<double> {
public:
  explicit Diagram(Foo& foo_ptr);

  std::unique_ptr<drake::systems::Context<double>> CreateContext() const;
};

Diagram::Diagram(Foo& foo_ptr) {
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
  auto diagram = std::make_unique<Diagram>(*foo_ptr);
  auto context = diagram->CreateContext();
  auto simulator = std::make_unique<drake::systems::Simulator<double>>(*diagram, std::move(context));
  simulator->set_target_realtime_rate(1.0);
  simulator->Initialize();
  simulator->StepTo(10.0);

  std::cout << "External FooPtr: " << foo_ptr->count() << std::endl;
  return 0;
}

