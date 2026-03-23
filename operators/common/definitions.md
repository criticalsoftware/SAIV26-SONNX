# Definitions

## Tensors

### Classification of tensors

A **tensor** is a multi-dimensional array of values. Based on its shape it can be devided in:

- **Scalar**: A 0-rank tensor (empty list of dimensions).

- **Vector**: A 1-rank tensor (list of dimensions with lenght 1).

- **Matrix**: A 2-rank tensor (list of dimensions with lenght 2). 

- **Empty/Null Tensor**: A multi-dimensional array with at least one null dimension.

### Rank of a tensor

The rank of a tensor is the number of dimensions it has. For instance, the rank of a given tensor `X` is `rX`.

### Shape of a tensor

The shape of a tensor is a list of its dimensions. 

The list is **empty** for a **scalar tensor** and **non-empty** for any other tensor.

A given tensor $X$ is said to have shape $[dX_0, dX_1, ..., dX_{rX-1}]$ where $dX_i$ is the size of the $i^{th}$ dimension.

### Tensor data

Constants `minfloat16`,  `minfloat`,  `mindouble` (resp. `maxfloat16`, `maxfloat`, `maxdouble`) represent the minimum (resp. maximum) values for `float16`, `float`, `double`, `respectively`.

## Helper functions

At this section, these helper functions will be described in the same way operators are described in the formal spec **textually**, **mathematically** and **example-based**.

### Coordinates and offsets

### Size

Takes a tensor shape as input and produces the total number of elements in the tensor.

$$\mathit{size (\mathit{X\_shape})} = \prod_{i=0}^{rX-1} dX_i$$

Where:
- $\mathit{X\_shape}$ is the shape of the tensor $X$.

#### Offset

Takes both a list of coordinates (to access a tensor) and a tensor shape as input and produces the flattened equivalent coordinate of the tensor.

$$\mathit{offset (\mathit{coords}, \mathit{X\_shape})} = \sum_{i=0}^{rX-1} \left ( c_i \cdot \prod_{j=i+1}^{rX-1} dX_j \right ) $$

Where:

- $\mathit{coords}$ is the list of coordinates to access the tensor.

- $\mathit{X\_shape}$ is the shape of the tensor $X$.

>Note that the product over an empty range/interval is defined to be 1.

#### Index

Takes both a flattened coordinate and a tensor shape as input and produces the equivalent list of coordinates to access the tensor.

$$\mathit{index (\mathit{offset}, \mathit{X\_shape})} = [c_0, c_1, ..., c_{rX-1}]$$

Where:

- $\mathit{offset}$ is the flattened coordinate to access the tensor.

- $\mathit{X\_shape}$ is the shape of the tensor $X$.

- $c_i = \left ( \displaystyle\frac{\mathit{offset}}{\displaystyle\prod_{j=i+1}^{rX-1} dX_j} \right ) \mod dX_i$