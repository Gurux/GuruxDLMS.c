# dlms_cosem_rs

`dlms_cosem_rs` provides a Rust centric façade for the Gurux DLMS/COSEM toolchain.
The crate mirrors the original C feature switches (`DLMS_IGNORE_*`) via Cargo
features so that downstream users can gradually migrate embedded and server
applications without losing configurability.

## Goals

- Offer a modular Rust API for building DLMS/COSEM clients, servers and security
  services that interoperate with the existing Gurux C libraries.
- Preserve the extensive feature gating capabilities exposed by the
  `DLMS_IGNORE_*` preprocessor switches.
- Integrate seamlessly with the repository's build system and documentation
  pipelines.

## Workspace integration

The crate lives under `development/dlms_cosem_rs` and can be built directly as
a static or dynamic library for linking into the existing C toolchain:

```shell
cargo build --manifest-path development/dlms_cosem_rs/Cargo.toml
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
