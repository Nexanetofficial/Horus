# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libhorus_cli*         | RPC client functionality used by *horus-cli* executable |
| *libhorus_common*      | Home for common functionality shared by different executables and libraries. Similar to *libhorus_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libhorus_consensus*   | Stable, backwards-compatible consensus functionality used by *libhorus_node* and *libhorus_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *libhorusconsensus*    | Shared library build of static *libhorus_consensus* library |
| *libhorus_kernel*      | Consensus engine and support library used for validation by *libhorus_node* and also exposed as a [shared library](../shared-libraries.md). |
| *libhorusqt*           | GUI functionality used by *horus-qt* and *horus-gui* executables |
| *libhorus_ipc*         | IPC functionality used by *horus-node*, *horus-wallet*, *horus-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *libhorus_node*        | P2P and RPC server functionality used by *horusd* and *horus-qt* executables. |
| *libhorus_util*        | Home for common functionality shared by different executables and libraries. Similar to *libhorus_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libhorus_wallet*      | Wallet functionality used by *horusd* and *horus-wallet* executables. |
| *libhorus_wallet_tool* | Lower-level wallet functionality used by *horus-wallet* executable. |
| *libhorus_zmq*         | [ZeroMQ](../zmq.md) functionality used by *horusd* and *horus-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *libhorus_consensus* and *libhorus_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`libhorus_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libhorus_node* code lives in `src/node/` in the `node::` namespace
  - *libhorus_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libhorus_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libhorus_util* code lives in `src/util/` in the `util::` namespace
  - *libhorus_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

horus-cli[horus-cli]-->libhorus_cli;

horusd[horusd]-->libhorus_node;
horusd[horusd]-->libhorus_wallet;

horus-qt[horus-qt]-->libhorus_node;
horus-qt[horus-qt]-->libhorusqt;
horus-qt[horus-qt]-->libhorus_wallet;

horus-wallet[horus-wallet]-->libhorus_wallet;
horus-wallet[horus-wallet]-->libhorus_wallet_tool;

libhorus_cli-->libhorus_util;
libhorus_cli-->libhorus_common;

libhorus_common-->libhorus_consensus;
libhorus_common-->libhorus_util;

libhorus_kernel-->libhorus_consensus;
libhorus_kernel-->libhorus_util;

libhorus_node-->libhorus_consensus;
libhorus_node-->libhorus_kernel;
libhorus_node-->libhorus_common;
libhorus_node-->libhorus_util;

libhorusqt-->libhorus_common;
libhorusqt-->libhorus_util;

libhorus_wallet-->libhorus_common;
libhorus_wallet-->libhorus_util;

libhorus_wallet_tool-->libhorus_wallet;
libhorus_wallet_tool-->libhorus_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class horus-qt,horusd,horus-cli,horus-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libhorus_wallet* and *libhorus_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libhorus_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libhorus_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *libhorus_common* should serve a similar function as *libhorus_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *libhorus_util* and *libhorus_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for horus-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *libhorus_kernel* is not supposed to depend on *libhorus_common*, only *libhorus_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *libhorus_kernel* should only depend on *libhorus_util* and *libhorus_consensus*.

- The only thing that should depend on *libhorus_kernel* internally should be *libhorus_node*. GUI and wallet libraries *libhorusqt* and *libhorus_wallet* in particular should not depend on *libhorus_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libhorus_consensus*, *libhorus_common*, and *libhorus_util*, instead of *libhorus_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libhorusqt*, *libhorus_node*, *libhorus_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *libhorus_node* to *libhorus_kernel* as part of [The libhoruskernel Project #24303](https://github.com/horus/horus/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/horus/horus/issues/15732)
