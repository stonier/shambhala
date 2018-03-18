# Drake Systems

## General Notes

### Abstract State

Abstract values are used for unrestricted updates when you're playing around with whatever
tickles your fancy. It is a type-erased container, bit like `std::any`.

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

### Speculative Updates

Drake has the ability to rewind update steps if necessary. To enable this, it first makes a speculative update.
Any Continuous/Discrete/Unrestricted update is a speculative update. If there are tasks you need to do that
cannot be rewound, e.g. updating external storage or sending a message out on middleware, use the publishing
system. Publishing events are automagically forced upon the acceptance of speculative updates. e.g. If
`DoCalcUnrestrictedUpdates` is processed and accepted, then it will automatically trigger a publishing event
that can be processed in `DoPublish`.

### Update Types - Continuous / Discrete / Unrestricted

* Continous: specify the system in terms of continuous derivatives and let the integrator worry about the machinery
* Discrete: specify the system in terms of discrete equations and let the integrator worry about the machinery

In both cases, there are specific state types representative of the kind of system.

* Unrestricted: save anything as the state and provide your own update mechanism

## Use Cases

### Updating External Objects

* [demo_external_object](src/external_object.cpp)
