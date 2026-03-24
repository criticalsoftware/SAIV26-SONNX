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

<a id="subshape"></a>
**Subshape**

If one wants to specify a subshape of the a given tensor $X$, starting from the $i^{th}$ dimension to the end of the list, it can be denoted as $X\_shape_i$ where:

$$X\_shape_i = [dX_i, dX_{i+1}, ..., dX_{rX-1}]$$

### Tensor data

Constants `minfloat16`,  `minfloat`,  `mindouble` (resp. `maxfloat16`, `maxfloat`, `maxdouble`) represent the minimum (resp. maximum) values for `float16`, `float`, `double`, `respectively`.

<a id="tensor_index"></a>

### Tensor access 
**Tensor index** (or **multi-index**): For an $n$-dimensional tensor $T$, a single element is addressed by an index tuple $i=(i_0,i_1,\dots,i_{n-1})$ where $i_k$ is the index along axis $k$.


## Helper functions

At this section, these helper functions will be described in the same way operators are described in the formal spec **textually**, **mathematically** and **example-based**.

### Coordinates and offsets

### Size

Takes a tensor shape as input and produces the total number of elements in the tensor.

$$\mathit{size (\mathit{X\_shape})} = \prod_{i=0}^{rX-1} dX_i$$

Where:
- $\mathit{X\_shape}$ is the shape of the tensor $X$.

> Note that the product over an empty range/interval is defined to be 1.

#### Examples

```math
\mathit{size}([2, 3]) = 6
```

```math
\mathit{size}([4, 5, 6]) = 120
```


#### Offset

Takes both a list of coordinates (to access a tensor) and a tensor shape as input and produces the flattened equivalent coordinate of the tensor.

$$\mathit{offset (\mathit{coords}, \mathit{X\_shape})} = \sum_{i=0}^{rX-1} \left ( c_i \cdot \mathit{size (X\_shape_{i+1})} \right ) $$

Where:

- $\mathit{coords}$ is the list of coordinates to access the tensor.

- $\mathit{X\_shape}$ is the shape of the tensor $X$.

- $\mathit{X\_shape_{i+1}}$ is a [subshape](#subshape) of the tensor $X$

> Note that the product over an empty range/interval is defined to be 1.

#### Examples

```math
\mathit{offset}([0, 0], [2, 3]) = 0
```
```math
\mathit{offset}([0, 1], [2, 3]) = 1
```
```math
\mathit{offset}([1, 0], [2, 3]) = 3
```
```math
\mathit{offset}([1, 2], [2, 3]) = 5
```
#### Index

Takes both a flattened coordinate and a tensor shape as input and produces the equivalent list of coordinates to access the tensor.

$$\mathit{index (\mathit{flat\_coord}, \mathit{X\_shape})} = [c_0, c_1, ..., c_{rX-1}]$$

$$c_i = \displaystyle\frac{\displaystyle\mathit{k}}{\displaystyle\mathit{size (X\_shape_{i+1})}}$$

$$k = \mathit{flat\_coord} - \displaystyle\sum_{j=0}^{i-1} \left( c_j \cdot \displaystyle\mathit{size (X\_shape_{j+1})}\right) $$

Where:

- $\mathit{flat\_coord}$ is the flattened coordinate to access the tensor.

- $\mathit{X\_shape}$ is the shape of the tensor $X$.

- $\mathit{X\_shape_{i+1}}$ is a [subshape](#subshape) of the tensor $X$

- $\mathit{X\_shape_{j+1}}$ is a [subshape](#subshape) of the tensor $X$

> Note that the sum over an empty range/interval is defined to be 0.

> Note that the product over an empty range/interval is defined to be 1.


#### Examples

```math
\mathit{index}(0, [2, 3]) = [0, 0]
```
```math
\mathit{index}(1, [2, 3]) = [0, 1]
```
```math
\mathit{index}(3, [2, 3]) = [1, 0]
```
```math
\mathit{index}(5, [2, 3]) = [1, 2]
```