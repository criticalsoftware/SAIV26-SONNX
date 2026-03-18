# SAIV26

This repository serves as the companion artifact for a paper to be published at the **Conference XXXXX**.

This work is a specific and concrete fork of a much broader initiative called [SONNX](https://github.com/ericjenn/working-groups/tree/ericjenn-srpwg-wg1/safety-related-profile/sonnx), whose goal is to develop a **safe profile for Machine Learning**, based on the [ONNX](https://onnx.ai/) framework. While the SONNX project covers the full scope of building a verified ML inference stack, this repository focuses on a well-defined subset of operators and presents the formal verification methodology and results discussed in the paper.

---

## Repository Structure

The repository is organized into two main directories:

```
SAIV26/
├── tensor/                 # Core tensor library (shared across all operators)
│   ├── code/               # Extracted C code (ctensor, cindex)
│   ├── drivers/            # Why3 extraction drivers (.drv)
│   └── tensor/             # Why3 formal specifications (.mlw)
│
|── operators/              # Neural network operator specifications
|   ├── add/                # Addition
|       ├── code/           # C operator extracted code
|       ├── formal/         # Why3 formal specifications (.mlw)
|       └── add.md          # Informal specification (Markdown)
|   ├── sigmoid/
|       ├── code/
|       ├── formal/
|       └── sigmoid.md
|   ├── (...)               # Other operators
|   └── general_definitions # General definitions
|
└── networks/               # Network specifications
    ├── lR                  # Logistic regression informal specification
    └── logisticRegression/ # Logistic regression formal specification
```
---

## Informal Specification

Each operator has a Markdown document (e.g., `add.md`, `conv.md`, `matmul.md`) that serves as its **informal specification**. These documents provide a description of the operator at three levels of detail: **textual, mathematical, and example-based**. 

It also presents the operator's **inputs** and **attributes**, whose restrictions are explicitly specified according both to the ONNX specification and to the SOONX profile.

These specifications are written for human consumption and serve as the reference from which the formal specifications are derived.

---
## Formal Specification

The formal specifications are written in [Why3](https://www.why3.org/) (`.mlw` files) and follow a **two-level architecture**: an abstract level and a concrete level.

### Abstract Level

The abstract level defines the **mathematical semantics** of each operator.

It neglects implementation details and focuses on mirroring the descriptions presented at the informal spec.

### Concrete Level

The concrete level provides an **imperative implementation** that operates on C-compatible data structures (`ctensor`) and is proven to refine the abstract specification.

Implementation details and design choices are made explicit at this level, and it serves as the basis for code extraction.

---

## Foundation Modules — Tensors

Provides the core definitions for the formalization. Thses modules include:

- `tensor.mlw` — Provides the abstract tensor model, including its properties and constructors.

- `layout.mlw` — Provides row-major layout to reason about multi-dimensional indices.

- `libtensor.mlw` — Provides the concrete `ctensor` structure.

- `libvector.mlw` — Provides reasoning on arrays boundaries.

- `std.mlw` — Provides standard mathematical definitions, reasoning on arrays accesses and the creation of the `float` datatype.

## Running the Proofs

### Prerequisites

To replicate the proofs you should have the following tools installed (please take into consideration that the solvers and why3 versions are important to ensure the proofs can be replayed without issues):

- [Why3](https://www.why3.org/) - version 1.8.0

- SMT solvers:

    - [Alt-Ergo](https://alt-ergo.ocamlpro.com/) - version 2.6.2

    - [CVC4](https://cvc4.github.io/getting-started.html) - version 1.8

    - [CVC5](https://cvc5.github.io/docs/latest/installation/installation.html) - version 1.2.1

    - [Z3](https://github.com/Z3Prover/z3) - version 4.13.4

### Verifying a `.mlw` File

To open a `.mlw` file in the Why3 IDE and inspect/replay its proofs:

```bash
# From the operator's formal/ directory, e.g.:
cd operators/add/formal

# Launch the Why3 IDE
why3 ide -L ../../tensor/tensor add.mlw

```

The path that follows the `-L` flag points to the directory containing the foundation modules (e.g., `tensor.mlw`, `layout.mlw`, etc.) that are required to load the operator's specification.

The IDE will load the file, display all proof obligations (goals), and show the status of each goal proved because proofs are already uploaded in the folder `operators/add/formal/add`.

To replay all proofs from the command line without the IDE:

### Extracting Verified C Code

Each operator (and the tensor library) includes a `Makefile` for extracting verified C code:

```bash
# From the operator's formal/ directory:
cd operators/add/formal
make lib
```

This will:
1. Extract the shared tensor library modules (`CTensor`, `CIndex`) to C.
2. Extract the operator-specific module (e.g., `COPAdd`) to C using the operator's driver file.
3. Compile the extracted C files with `gcc` to verify they are syntactically valid.
4. List the generated files in the `code/` directory.

To clean generated files:

```bash
make clean
```