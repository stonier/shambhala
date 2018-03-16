# Drake Systems

## General Notes

### Continuous / Discrete / Unrestricted

* Continous: specify the system in terms of continuous derivatives and let the integrator worry about the machinery
* Discrete: specify the system in terms of discrete equations and let the integrator worry about the machinery

In both cases, there are specific state types representative of the kind of system.

* Unrestricted: save anything as the state and provide your own update mechanism


### Internal Storage - Abstract State / AbstractValue

This is oft used as the storage for unrestricted updates (because you're playing around with whatever
the hell you want).

It is a type-erased container, bit like `std::any`.

When we declare abstract state via:

```
    this->DeclareAbstractState(drake::systems::AbstractValue::Make(Foo())); // std::make_unique<AbstractValue>());
```

we are actually sending a copy/clone of that to the state's storage (can also move a unique pointer in).
**The original instance will not be updated.**

```
  // This overload is for copyable T
  template <typename Arg1, typename... Args,>
  explicit Value(Arg1&& arg1, Args&&... args)
      : value_{std::forward<Arg1>(arg1), std::forward<Args>(args)...} {}

  // This overload is for cloneable T; we move a unique_ptr into our Storage.
  template <typename Arg1, typename... Args,>
  explicit Value(Arg1&& arg1, Args&&... args)
      : value_{std::make_unique<T>(
            std::forward<Arg1>(arg1), std::forward<Args>(args)...)} {}

  // moves a unique ptr to storage
  explicit Value(std::unique_ptr<T> v)
      : value_{Traits::to_storage(std::move(v))} {}
```

## Use Cases

### External Storage / Proxy'ing

* Shared pointer into your leaf system
* Copy the data structure part of that in as AbstractValue state.
* Use `DoCalcUnrestrictedUpdate` to update the internal state. This is speculative and may get unwound!
* Use some publishing machinery to pass the internal storage back to the external storage via the pointer
