# ![](./doc/img/duoscript32.png) DuoScript

Scripting engine for Counter-Strike 2 on Metamod.

| ⚠️ | This is under heavy development and is missing most features. Use at your own risk! |
|----|-------------------------------------------------------------------------------------|

## To-Do List
There is a pretty long to-do list, as DuoScript currently is only a MVP
that is barely capable of loading plugins.

- Source 2
  - Schemas
  - Entities
  - Events
- Networking
- Databases
- Compiler / Plugin Build Tool
- Typechecking

## Goals
- **Zero module dependencies:** Modules should be able to load and run while
  only depending on AMTL, No-SDK Metamod, and our `public/` folder. Logic modules, such as database libraries
  or generic utilities, should run exactly the same acrosss HL2SDK releases.
- **Sandboxed:** Plugins should be able to run completely independently of other
  plugins, only interacting with others when they choose to. C++ controllers should be
  fully aware of the fiber they are executing in and deny access if they so choose.
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
  sandboxing features that are battle-tested.
- **Coroutines/LibUV:** We use interruptable coroutines to make fully pausable/resumable
  "fibers" on Luau, while a backend LibUV event loop handles I/O polling and scheduling.

## License
DuoScript itself is licensed as Affero GPL. This prevents users from maintaining private
forks of DuoScript; all derivatives of DuoScript must be open source. In other words,
any player on a server running any of the artifacts contained within src/ must be able to
obtain the source code of that artifact.

However, all "header" files are instead licensed under LGPL3, and all "module definition" files
will also be licensed under LGPL3 once they have been written. This allows plugins
written by other developers to remain closed source, even across distribution.

I do not believe in code obfuscation as an anti-piracy measure. As such, we will take
actions to prevent obfuscated code from running on DuoScript if we find obfuscated 
DuoScript plugins in the wild. (IE, get/setfenv is disabled pre-emptively for other reasons)
