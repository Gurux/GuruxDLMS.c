# dlms_cosem

`dlms_cosem` is a Rust implementation of the DLMS/COSEM protocol.
The crate mirrors the original C feature switches (`DLMS_IGNORE_*`) via Cargo
features so that downstream users can build highly configurable embedded and
server applications.

## Goals

- Offer a modular Rust API for building DLMS/COSEM clients, servers and security
  services.
- Preserve the extensive feature gating capabilities exposed by the
  `DLMS_IGNORE_*` preprocessor switches.
- Integrate seamlessly with the repository's build system and documentation
  pipelines.

## Workspace integration

The crate lives under `development/dlms_cosem_rs` and builds as a standard Rust
library (an `rlib`) when invoked through Cargo:

```shell
cargo build --manifest-path development/dlms_cosem_rs/Cargo.toml
```

Downstream components that still require native artifacts can request them on
demand without modifying the manifest by forcing the desired crate type during
the build:

```shell
cargo rustc --manifest-path development/dlms_cosem_rs/Cargo.toml --release -- --crate-type staticlib
cargo rustc --manifest-path development/dlms_cosem_rs/Cargo.toml --release -- --crate-type cdylib
```

Formatting, linting, testing and documentation generation have corresponding
Makefile targets so that the existing automation can invoke the Rust toolchain:

```shell
make -C development cargo-fmt
make -C development cargo-clippy
make -C development cargo-test
make -C development cargo-doc
```

## Next steps

The current code base only contains stubs describing the intended module layout.
Future iterations will flesh out the transport layers, security suites and
object models while maintaining feature parity with the legacy implementation.
