# DuoScript

Scripting engine for Counter-Strike 2 on Metamod.

## Goals
- **Zero module dependencies:** Modules should be able to load and run while
  only depending on AMTL, No-SDK Metamod, and our `public/` folder. Logic modules, such as database libraries
  or generic utilities, should run exactly the same acrosss HL2SDK releases.
- **Sandboxed:** Plugins should be able to run completely independently of other
  plugins, only interacting with others when they choose to.
- **Concurrent:** Both modules and plugins should run fully concurrently; only blocking
  the main thread when executing business logic. Plugins should not have an exposed async/await interface;
  rather this should be handled behind the scenes by the scripting engine
- **Collaborative:** Unlike other Metamod-based platforms, we should expose ourself to
  Metamod and expose Metamod to ourselves. 
- **Intuitive:** Developing on or for duoscript should be intuitive and fun.

## Implementation
- **Sinks:** Modules will have all their resources tracked automatically, and they will be freed
  upon module unload. Modules will only be responsible for closing their own handles, such as
  database connections.
- **Luau:** The Luau scripting engine developed by roblox has several powerful type-checking and
  sandboxing features that are battle-tested. Luau is very high performance, and includes a basic
  function-level JIT compiler.
- **Coroutines/LibUV:** We use interruptable coroutines to make fully pausable/resumable
  "fibers" on Luau, while a backend LibUV event loop handles I/O polling and scheduling.