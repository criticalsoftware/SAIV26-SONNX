# SAIV26 / Verified Tensor Operators for Safety-Critical Machine Learning

This repository serves as the companion artifact for a paper submitted to the [SAIV'26](https://aiverification.org/2026/) Symposium.

This work is a specific and concrete fork of a much broader initiative called [Safe ONNX](https://github.com/ericjenn/working-groups/tree/ericjenn-srpwg-wg1/safety-related-profile/sonnx) (SONNX), whose goal is to develop a **safe profile for Machine Learning**, based on the [ONNX](https://onnx.ai/) framework.

While the SONNX project covers the full scope of building a verified ML inference stack, this repository focuses on a well-defined subset of operators and presents the formal verification methodology and results discussed in the paper.

## Table of Contents

- [Repository Structure](#repository-structure)
- [Informal Specification](#informal-specification)
- [Formal Specification](#formal-specification)
    - [Abstract Level](#abstract-level)
    - [Concrete Level](#concrete-level)
- [Foundation Modules - Tensors](#foundation-modules---tensors)
- [Running the Proofs](#running-the-proofs)
    - [Prerequisites](#prerequisites)
    - [Formal Verification](#formal-verification)
    - [Code Extraction](#code-extraction)

## Repository Structure

The repository is organized into two main directories:

```
├── tensor/                      # Core tensor library (shared across all operators)
│   ├── code/                    # Extracted C code (ctensor, cindex)
│   ├── drivers/                 # Why3 extraction drivers (.drv)
│   └── tensor/                  # Why3 formal specifications (.mlw)
│
|── operators/                   # Neural network operator specifications
|   ├── add/                     # Addition
|       ├── code/                # C operator extracted code
|       ├── formal/              # Why3 formal specifications (.mlw)
|       └── add.md               # Informal specification (Markdown)
|   ├── sigmoid/                 
|       ├── code/                
|       ├── formal/              
|       └── sigmoid.md           
|   └── (...)                    # Other operators
|
└── network/                     # Network specifications
    └── logisticRegression.mlw   # Logistic regression formal specification
```

## Informal Specification

Each [ONNX operator](https://onnx.ai/onnx/operators/) has a Markdown document (e.g., `add.md`, `conv.md`, `matmul.md`) that serves as its **informal specification**. These documents provide a description of the operator at three levels of detail: **textual**, **mathematical**, and **example-based**. 

It also presents the operator's **inputs** and **attributes**, whose restrictions are explicitly specified according both to the ONNX specification and to the SONNX profile.

These specifications are written for human consumption and serve as the reference from which the formal specifications are derived.

## Formal Specification

The formal specifications are written in [Why3](https://www.why3.org/) (`.mlw` files) and follow a **two-level architecture**.

### Abstract Level

The abstract level defines the **mathematical semantics** of each operator.

It neglects implementation details and focuses on mirroring the descriptions presented at the informal specification.

### Concrete Level

The concrete level provides an **imperative implementation** that operates on `C`-compatible data structures (`ctensor`) and is proven to refine the abstract specification.

Implementation details and design choices are made explicit at this level, and it serves as the basis for code extraction.

## Foundation Modules - Tensors

Provides the core definitions for the formalization:

- `tensor.mlw` - provides the abstract tensor model, including its properties and constructors.

- `layout.mlw` - provides row-major layout to reason about multi-dimensional indices.

- `libtensor.mlw` - provides the concrete `ctensor` structure.

- `libvector.mlw` - provides reasoning on arrays boundaries.

- `std.mlw` - provides standard mathematical definitions, reasoning on arrays accesses and the creation of the `float` datatype.

## Running the Proofs

### Prerequisites

To replicate the proofs you should have the following tools installed (please take into consideration that the solvers and Why3 versions are important to ensure the proofs can be replayed without issues):

- [Why3](https://www.why3.org/) - version 1.8.0

- SMT solvers:

    - [Alt-Ergo](https://alt-ergo.ocamlpro.com/) - version 2.6.2

    - [CVC4](https://cvc4.github.io/getting-started.html) - version 1.8

    - [CVC5](https://cvc5.github.io/docs/latest/installation/installation.html) - version 1.2.1

    - [Z3](https://github.com/Z3Prover/z3) - version 4.13.4

- [Just](https://github.com/casey/just) - version >=1.42.3

Once Why3 is installed, locate your `.why3.conf` file and add `/absolute/path/to/saiv/tensor/tensor` to the `loadpath` section. Replace `/absolute/path/to/` with the actual path to the repository on your system.

### Formal Verification

To open a `.mlw` file in the Why3 IDE and inspect/replay its proofs:

```bash
# From the operator's formal/ directory, e.g. Add operator:
cd operators/add/formal

# Launch the Why3 IDE
why3 ide add.mlw
```

The IDE will load the file, display all proof obligations (goals), and show the status of each goal proved because proofs are already uploaded in the folder `operators/add/formal/add`.

To replay a given operator proofs you can either launch the IDE and select the command *Reset Proofs* under the *Tools* menu, or you can delete the `add` folder (`operators/add/formal/add`) and re-run the command.

**Note:** When replaying proofs, you may need to increase the solver timeout. This can be configured in Why3 under *File > Preferences*, and then applying the desired solver to the corresponding verification condition (VC). Additionally, some proofs rely on specific transformations and instantiations applied through the Why3 IDE. To successfully replay these proofs, the same transformations must be reapplied. The required transformations are recorded in the `why3session.xml` file located in the proof folder of each operator.

### Code Extraction

You can extract C code using the Makefiles available in each operator's `formal/` directory and in the tensor module.

To perform the extraction:

```bash
# Navigate to the formal directory of an operator or the tensor module
cd tensor/tensor/        # For the tensor library
# or
cd operators/add/formal/ # For a specific operator

# Run the extraction
make
```

The extraction process removes any existing `code/` directory and generates C code from the Why3 modules using the configured drivers. For the tensor library, this includes modules such as `libvector.CIndex`, `libtensor.CTensor`, and `libtensor.CTensorInt64`.

The generated C files are then compiled with `gcc` to verify syntactic correctness, and all output files are listed for inspection in the `code/` directory.

To clean generated files:

```bash
make clean
```

### Using Just

The repository includes a `justfile` with recipes to run proofs, extract code, and clean artifacts across all operators at once.

```bash
just prove              # run Why3 proofs for all operators
just prove add          # run proofs for a single operator

just extract            # extract C code for all operators
just extract sigmoid    # extract C code for a single operator

just clean              # remove generated C code for all operators
just clean tensor       # remove generated code for the tensor library only
```

All recipes accept an optional operator name; omitting it defaults to running across all supported operators.

The default prover is `alt-ergo`. To use a different prover, pass it via `--set`:

```bash
just --set prover z3 prove
just --set prover cvc5 prove add
```