# Drake Systems

## Internal Storage - Abstract State / AbstractValue

It is a type-erased container. What does this mean to our use case?

When we declare abstract state via:

```
    this->DeclareAbstractState(drake::systems::AbstractValue::Make(Foo())); // std::make_unique<AbstractValue>());
```

we are actually sending a copy/clone of that to the state's storage. **The original instance
will not be updated.**

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

## External Storage

If you need to utilise external storage, just pass in a pointer to the system instance and utilise that in the update step.