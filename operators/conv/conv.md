# Contents

- **Conv** operator for type [real](#real)
- **Conv** operator for types [float16, float, double](#float)

Based on ONNX documentation [Conv version 22](https://onnx.ai/onnx/operators/onnx__Conv.html#conv-22).

<a id="real"></a>
# **Conv** (real,real,[real])

## Signature
$Y = \textbf{Conv}(X,W,[B])$
 
where:
- $X$: input tensor
- $W$: convolution kernel
- $B$: optional bias
- $Y$: output tensor

## Restrictions

[General restrictions](./../common/general_restrictions.md) are applicable.

The following specific restrictions apply to the **Conv** operator:

| Restriction    | Statement | Origin |
| -------- | ------- | ------- |
| `R1` | Input tensor $X$ has 2 spatial axes | Transient |
| `R2` | Attribute $\text{auto\_pad}$ is set to `NOTSET`  | [No default values](../../../deliverables/reqs/reqs.md#no_default_value) |
| `R3` | Attribute $\text{group}$ is set to 1 (standard convolution)| Transient |

 ## Informal specification
  
Operator **Conv** computes the convolution of the input tensor $X$ with the kernel $W$ and adds bias $B$ to the result.

### Standard convolution
A _standard convolution_ applies a kernel (also called "filter") to the input tensor, aggregating information accross both spatial axes and channels. For a given output channel, the kernel operates accross all input channels and all contributions are summed to produce the output. This corresponds to the case where attribute $\text{group}= 1$. 

The mathematical definition of the operator is given hereafter:

$$
\begin{gathered}
    Y[b, c, y_h, y_w] = \sum_{i=0}^{dW_1-1} \sum_{j=0}^{dW_2-1} \sum_{k=0}^{dW_3-1} \\ (X_p[b,i, y_h \cdot \text{strides}[0]+ j * \text{dilations}[0] , y_w \cdot \text{strides}[1]+ k * \text{dilations}[1] ] \cdot W_d[c, i, j, k]) \\ + B_b[c]
\end{gathered}
$$

Where
- $b \in [0,dY_0-1]$ is the batch index. $dY_0$ is the batch size of output $Y$
- $c \in [0,dY_1-1]$ is the data channel. $dY_1$ is the number of data channels of output $Y$
- $y_h \in [0,dY_2-1]$ is the index of the first spatial axis of output $Y$
- $y_w \in [0,dY_3-1]$ is the index of the second spatial axis of output $Y$
- $dW_1$ is the number of feature maps of kernel $W$
- $dW_2$ is the size of the first spatial axis of kernel $W$
- $dW_3$ is the size of the second spatial axis of kernel $W$
- $strides$ is an attribute of the operator. It will be described later in this section.
- $X_{p} = \text{pad}(X,pads)$ is the padded version of the input tensor $X$. Function $\text{pad}$ applies zero-padding as specified by the pads attribute (see ONNX **Pad** operator).
- $W_{d} = \text{dilation}(W,dilations)$ is the dilated version of the kernel $W$. Function $\text{dilation}$ expands the kernel by inserting spaces between its elements as specified by the dilations attribute. Its definition is given later.
- $B_{b} = \text{broadcast}(B,(dY_0 , dY_1 , dY_2 , dY_3))$ is the broadcasted version of bias $B$.  Function $\text{broadcast}$ replicates the bias value across the spatial dimensions and batch dimension of the output $Y$. It takes as argument the bias $B$ and the shape of output $Y$. Its definition is given later.

The effect of the operator is illustrated on the following figure. In this example
- shape of $Y$ is ($1, 1, 4, 4$) (batch size is 1, number of data channels is 1)
- shape of $X$ is ($1, 1, 8, 8$) (batch size is 1, number of data channels is 1)
- shape of $W$ is ($1, 1, 2, 3$)  (number of data channels is 1)
- shape of $B$ is ($1$)
- $\text{pads}$ is  set to (2,1,2,2) (2 rows on the top, 1 column on the left, 2 rows on the bottom , 2 columns on the right)
- $\text{dilations}$ is set to (2,2)
- $\text{strides}$ is set to (3,2)

The following figure illustrates $\text{pad}$ function applied to the input tensor $X$:
<img src="./assets/imgs/onnx_conv_padop2.png" alt="drawing" width="100%"/>


The following figure illustrates $\text{dilation}$ function applied to the kernel $W$:

<img src="./assets/imgs/onnx_conv_dilationop.png" alt="drawing" width="50%"/>


Finally, the following figure illustrates operator **Conv** applied on input $X$ with kernel $W$ and bias $B$:
<img src="./assets/imgs/convwithoperators2.png" alt="drawing" width="100%"/>

The following figure shows the case where the number of channels of $X$ is 3. In this example:
- shape of $Y$ is ($1, 1, 4, 4$) 
- shape of $X$ is ($1, 3, 8, 8$)
- shape of $W$ is ($1, 3, 2, 3$) 
- shape of $B$ is $1$
- $\text{groups}$ is  set to 1 
- the other attributes have the same values as in the previous figure.

<img src="./assets/imgs/3channels_std_conv_modified.png" alt="drawing" width="100%"/>

### Output shape calculation
The shape of the output tensor $Y$ is determined by the shape of the input tensor $X$, the shape of the kernel $W$ and the values of attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$.

$$
Y_\textbf{shape} = \left(dX_0, \frac{dW_0}{\text{groups}}, dY_2, dY_3\right)
$$

Where:

- $dY_2 =  \displaystyle\left\lfloor{\frac{\alpha-\theta}{\text{strides}[0]}} \right\rfloor + 1$

    <br>

    - $\displaystyle\alpha = dX_2 + \text{pads}[0] + \text{pads}[2]$

    <br>

    - $\displaystyle\theta = \text{dilations}[0] * (dW_2 - 1) + 1$

<br>

- $dY_3 =  \displaystyle\left\lfloor{\frac{\beta-\gamma}{\text{strides}[1]}} \right\rfloor + 1$

    <br>

    - $\displaystyle\beta = dX_3 + \text{pads}[1] + \text{pads}[3]$

    <br>

    - $\displaystyle\gamma = \text{dilations}[1] * (dW_3 - 1) + 1$

## Error conditions
No error condition.

## Attributes

### $\text{strides}$: `list of int`

Attribute $\text{strides}$ determines how the kernel is applied on tensor $X$ during the convolution.

For instance, with $\text{strides}[0]=3$ and $\text{strides}[1]=2$, the kernel is applied to data 3 units down in the first spatial axis and to data 2 units on right in the second spatial axis at each step of the convolution.

The effect of the $\text{strides}$ attribute is illustrated on the following figure. In this example, $\text{strides}=(3,2)$.

<img src="./assets/imgs/conv_stride3.png" width="300" />

#### Constraints
- `[C1]` <a id="C1rattr1"></a> Value domain
    - Statement: $\text{strides}$ is a list of strictly positive integers.
    - Rationale: Stride values represent the number of applications of the kernel in the two spatial dimensions
- `[C2]` <a id="C2rattr1"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$
    - Statement: 
        *  $dY_2 > 0$
         
      and
      
        * $dY_3 > 0$
    - Rationale: Convolution is only performed when the output dimensions are positive.

### $\text{auto\_pad}$: `string`

The $\text{auto\_pad}$ attribute determines if and how automatic padding is done for the input tensor $X$.

#### Constraints
- `[C1]` Value domain 
    - Statement: $\text{auto\_pad}$ shall be in set {`NOTSET`, `VALID`, `SAME_UPPER`, `SAME_LOWER`}.
- `[C2]` Explicit padding 
    - Statement: $\text{auto\_pad}$ shall be set to `NOTSET` `[R2]`
    - Rationale: The SONNX profile imposes explicit padding.

### $\text{pads}$: `list of int`

Attribute $\text{pads}$ determines the padding at the beginning and end along each spatial axis of the input tensor $X$.

$\text{pads}$ is a list of the form (`x1_begin`, `x2_begin`,..., `x1_end`, `x2_end`,...), where `xi_begin` is the number of elements (possibly zero) added at the beginning of axis $i$ and `xi_end` is the number of elements added at the end of axis $i$.

The padding value is 0.

The effect of the $\text{pads}$ attribute is illustrated on the following figure. In this example,  $\text{pads}=(2,1,2,2)$.
<img src="./assets/imgs/conv_pad2.png" width="300" />

#### Constraints
 - `[C1]` <a id="C1rattr2"></a> Value domain
    - Statement: $\text{pads}$ is a list of positive or null integers.
    - Rationale: A padding value gives a *number of elements* to be added to some spatial axis.
 - `[C2]` <a id="C2rattr2"></a> Consistency between the shape of $X$ and the length of $\text{pads}$
    - Statement: The length of the $\text{pads}$ list is two times the number of spatial axes of $X$
    - Rationale: Padding shall be given for all spatial axes, and a begining value and an end value must be given for each axis.
 - `[C3]` <a id="C3rattr2"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$  
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1) on attribute $\text{strides}$.

### $\text{dilations}$: `list of int`

Attribute $\text{dilations}$ specifies the spacing between the kernel elements for each spatial axis of the filter $W$. The ith value in the list gives the dilation factor for spatial axis $i$. If the dilation factor is greater than 1 for axis $i$, then the kernel elements are spaced out by the dilation factor for that axis. 

The value of the elements introduced by the dilation is 0.

The effect of the $\text{dilations}$ attribute for a tensor with two spatial axes is depicted on the following figure. In this example, $\text{dilations}=(2,2)$. 

<img src="./assets/imgs/dilation.png" width="300" />


#### Constraints
 - `[C1]` <a id="C1rattr3"></a> Value domain
    - Statement: $\text{dilations}$ is a list of strictly positive integers
    - Rationale: The dilation is a *factor of expansion* along a certain axis. 
 - `[C2]` <a id="C2rattr3"></a> Relation between $\text{dilations}$ and $W$ 
    - Statement: The length of the $\text{dilations}$ list is equal to number of spatial axes of $W$.
    - Rationale: Dilation is defined for all spatial axes of $W$.
 - `[C3]` <a id="C3rattr3"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$  
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1) on attribute $\text{strides}$.

### $\text{group}$: `int` 

This attribute specifies the number of groups the input channels and output channels are divided into. When $\text{group}=1$, a standard convolution is performed. When $\text{group}$ is greater than 1, convolution is computed for each group separately with a specific set of filters.

The effect of the $\text{group}$ attribute for a tensor with two spatial axes is depicted on the following figure. In this example $\text{group}=3$.

<img src="./assets/imgs/grouped_convolution.png" width="300" />

(Taken from https://eli.thegreenplace.net/2018/depthwise-separable-convolutions-for-machine-learning)

In the example, with $\text{group}$ set to 3 and an input $\text{X}$ and an output $\text{Y}$ with 3 channels, the input and output channels will be divided into 3 groups of 1 channel.

#### Constraints
 - `[C1]` <a id="C1rattr4"></a> Value domain
    - Statement: $\text{group}$ is a strictly positive value.
    - Rationale: $\text{group}$ represent a grouping factor. 
 - `[C2]` <a id="C2rattr3"></a> Consistency between number of channels and groups
    - Statement: 
      - $dX1 \text{ mod}$ $\text{group}= 0$
      - $dY1 \text{ mod}$ $\text{group}= 0$
    - Rationale: Each group shall have the same number of input channels and the shall be the same number of output channels assigned to each group.
- `[C3]` <a id="C3rattr3"></a> Support for standard and depthwise convolutions
    - Statement: $\text{group}=1$ `[R3]`

### $\text{kernel\_shape}$: `list of int`

This parameter specifies the shape of the convolution kernel $W$.

#### Constraints

- `[C1]` <a id="C1rattr4"></a> Value domain
    - Statement: $\text{kernel\_shape}$ is a list of strictly positive integers
    - Rationale: A dimension is always positive and cannot be null.
- `[C2]` <a id="C2rattr4"></a> Consistency between $W$ and $\text{kernel\_shape}$
    - Statement:  The size of $W$ for an axis must be equal to the value of $\text{kernel\_shape}$ for that axis
   - Rationale: $\text{kernel\_shape}$ represents the shape of $W$, where $\text{kernel\_shape}[0] = dW_3$ and $\text{kernel\_shape}[1] = dW_2$.


## Inputs

###  $\text{X}$: `real tensor`

Tensor $X$ is the input tensor on which convolution with kernel $W$ is computed.

The shape of tensor $X$ is $(dX_0 , dX_1 , dX_2 , dX_3)$

Where
- $dX_0$ is the batch size of input $X$.
- $dX_1$ is the number of data channels of input $X$.
- $dX_2$ and $dX_3$ are the sizes of the input for the two spatial axes (height and width).

#### Constraints

- `[C1]` <a id="C1rx"></a> Number of spatial axes of tensor $X$
    - Statement: The number of spatial axes of tensor $X$ is 2. `R1`
    - Rationale: This restriction is introduced to reduce the specification effort. It matches the industrial use cases considered in the profule.
- `[C2]` <a id="C2rx"></a> Consistency between the number of channels of $X$ and $W$
    - Statement:  $dW_1=\frac{dX_1}{group}$
- `[C3]` <a id="C3rx"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1) on attribute $\text{strides}$.
- `[C4]` <a id="C4rx"></a> Axis denotations 
    - Statement: If axis denotation is in effect, the operation expects input data tensor to have axis denotation \[`DATA_BATCH`, `DATA_CHANNEL`, `DATA_FEATURE`, `DATA_FEATURE`\].
    - Rationale: Denotation convention

### $\text{W}$: `real tensor`

Tensor $W$ is the convolution kernel.

The shape of tensor $W$ is $(dW_0 , dW_1 , dW_2 , dW_3)$

Where
- $dW_0$ is the number of output channels or number of feature maps
- $dW_1$ is the number of input channels
- $dW_2$ and $dW_3$ are the sizes of the kernel for the two spatial axes.

#### Constraints
- `[C1]` <a id="C1rw"></a> Consistency between the number of channels of $X$ and $W$
   - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rx) on tensor $X$.
- `[C2]` <a id="C2rw"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1) on attribute $\text{strides}$.
- `[C3]` <a id="C3rw"></a> Consistency between $W$ and $\text{kernel\_shape}$
   - Statement:  The size of $W$ for an axis must be equal to the value of $\text{kernel\_shape}$ for that axis
   - Rationale: $\text{kernel\_shape}$ represents the shape of $W$, where $\text{kernel\_shape}[0] = dW_3$ and $\text{kernel\_shape}[1] = dW_2$.
- `[C4]` <a id="C4rw"></a> Compliance with axis denotations
    - Statement: If axis denotation is in effect, the operation expects the weight tensor to have axis denotation \[`FILTER_OUT_CHANNEL`, `FILTER_IN_CHANNEL`, `FILTER_SPATIAL`, `FILTER_SPATIAL`\].
- `[C5]` <a id="C5rw"></a> Consistency between output channels and group
     - Statement: $dW_0 \mod \text{group} == 0$

### $\text{B}$: `real tensor` (optional)

Tensor $B$ is the bias. 

The shape of tensor $B$ is $dB_0$.

#### Constraints
- `[C1]` <a id="C1rb"></a> Consistency between the number of channels of $B$ and $W$
    - Statement:  $dB_0 = dW_0$.


## Outputs

### $\text{Y}$ : `real tensor`

The shape of the output $Y$ will be $(dY_0 , dY_1 , dY_2 , dY_3)$ 

Where
- $dY_0$ is the number of batches
- $dY_1$ is the number of channels
- $dY_2$ and $dY_3$ are the sizes of the output for the two spatial axes

#### Constraints.
- `[C1]` <a id="C1ry"></a> Consistency between the shape of tensors $X$, $W$, $Y$, attributes $\text{pads}$ and  $\text{strides}$
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1) on attribute $\text{strides}$.

## Formal specification

[See Why3 specification](./formal/conv.mlw).

## Numerical Accuracy
*(To be completed)*

<a id="float"></a>
# **Conv** (float, float, [float])
where float is in {float16, float, double}


## Signature

## Signature
$Y = \textbf{Conv}(X,W,[B])$
 
where:
- $X$: input tensor
- $W$: convolution kernel
- $B$: optional bias
- $Y$: output tensor

## Restrictions
The following restrictions apply to the **Conv** operator for the SONNX profile:

[General restrictions](./../common/general_restrictions.md) are applicable.

| Restriction    | Statement | Origin |
| -------- | ------- | ------- |
| `R1` | Input tensor $X$ has 2 spatial axes | Transient |
| `R2` | Attribute $\text{auto\_pad}$ is set to `NOTSET`  | [No default values](../../../deliverables/reqs/reqs.md#no_default_value) |
| `R3` | Attribute $\text{group}$ is set to 1 (standard convolution)| Transient |

 ## Informal specification
  
Operator **Conv** computes the convolution of the input tensor $X$ with the kernel $W$ and adds bias $B$ to the result.

### Standard convolution
A _standard convolution_ applies a kernel (also called "filter") to the input tensor, aggregating information accross both spatial axes and channels. For a given output channel, the kernel operates accross all input channels and all contributions are summed to produce the output, according to IEEE 754 floating-point semantics. This corresponds to the case where attribute $\text{group}= 1$. 

The mathematical definition of the operator is given hereafter:

$$
\begin{gathered}
    Y[b, c, y_h, y_w] = \sum_{i=0}^{dW_1-1} \sum_{j=0}^{dW_2-1} \sum_{k=0}^{dW_3-1} \\ (X_p[b,i, y_h \cdot \text{strides}[0]+ j * \text{dilations}[0] , y_w \cdot \text{strides}[1]+ k * \text{dilations}[1] ] \cdot W_d[c, i, j, k]) \\ + B_b[c]
\end{gathered}
$$

Where
- $b \in [0,dY_0-1]$ is the batch index. $dY_0$ is the batch size of output $Y$
- $c \in [0,dY_1-1]$ is the data channel. $dY_1$ is the number of data channels of output $Y$
- $y_h \in [0,dY_2-1]$ is the index of the first spatial axis of output $Y$
- $y_w \in [0,dY_3-1]$ is the index of the second spatial axis of output $Y$
- $dW_1$ is the number of feature maps of kernel $W$
- $dW_2$ is the size of the first spatial axis of kernel $W$
- $dW_3$ is the size of the second spatial axis of kernel $W$
- $strides$ is an attribute of the operator. It will be described later in this section.
- $X_{p} = \text{pad}(X,pads)$ is the padded version of the input tensor $X$. Function $\text{pad}$ applies zero-padding as specified by the pads attribute (see ONNX **Pad** operator).
- $W_{d} = \text{dilation}(W,dilations)$ is the dilated version of the kernel $W$. Function $\text{dilation}$ expands the kernel by inserting spaces between its elements as specified by the dilations attribute. Its definition is given later.
- $B_{b} = \text{broadcast}(B,(dY_0 , dY_1 , dY_2 , dY_3))$ is the broadcasted version of bias $B$.  Function $\text{broadcast}$ replicates the bias value across the spatial dimensions and batch dimension of the output $Y$. It takes as argument the bias $B$ and the shape of output $Y$. Its definition is given later.

The effect of the operator is illustrated on the following figure. In this example
- shape of $Y$ is ($1, 1, 4, 4$) (batch size is 1, number of data channels is 1)
- shape of $X$ is ($1, 1, 8, 8$) (batch size is 1, number of data channels is 1)
- shape of $W$ is ($1, 1, 2, 3$)  (number of data channels is 1)
- shape of $B$ is ($1$)
- $\text{pads}$ is  set to (2,1,2,2) (2 rows on the top, 1 column on the left, 2 rows on the bottom , 2 columns on the right)
- $\text{dilations}$ is set to (2,2)
- $\text{strides}$ is set to (3,2)

The following figure illustrates $\text{pad}$ function applied to the input tensor $X$:
<img src="./assets/imgs/onnx_conv_padop2.png" alt="drawing" width="100%"/>


The following figure illustrates $\text{dilation}$ function applied to the kernel $W$:

<img src="./assets/imgs/onnx_conv_dilationop.png" alt="drawing" width="50%"/>


Finally, the following figure illustrates operator **Conv** applied on input $X$ with kernel $W$ and bias $B$:
<img src="./assets/imgs/convwithoperators2.png" alt="drawing" width="100%"/>

The following figure shows the case where the number of channels of $X$ is 3. In this example:
- shape of $Y$ is ($1, 1, 4, 4$) 
- shape of $X$ is ($1, 3, 8, 8$)
- shape of $W$ is ($1, 3, 2, 3$) 
- shape of $B$ is $1$
- $\text{groups}$ is  set to 1 
- the other attributes have the same values as in the previous figure.

<img src="./assets/imgs/3channels_std_conv_modified.png" alt="drawing" width="100%"/>

### Output shape calculation
The shape of the output tensor $Y$ is determined by the shape of the input tensor $X$, the shape of the kernel $W$ and the values of attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$.

$$
Y_\textbf{shape} = \left(dX_0, \frac{dW_0}{\text{groups}}, dY_2, dY_3\right)
$$

Where:

- $dY_2 =  \displaystyle\left\lfloor{\frac{\alpha-\theta}{\text{strides}[0]}} \right\rfloor + 1$

    <br>

    - $\displaystyle\alpha = dX_2 + \text{pads}[0] + \text{pads}[2]$

    <br>

    - $\displaystyle\theta = \text{dilations}[0] * (dW_2 - 1) + 1$

<br>

- $dY_3 =  \displaystyle\left\lfloor{\frac{\beta-\gamma}{\text{strides}[1]}} \right\rfloor + 1$

    <br>

    - $\displaystyle\beta = dX_3 + \text{pads}[1] + \text{pads}[3]$

    <br>

    - $\displaystyle\gamma = \text{dilations}[1] * (dW_3 - 1) + 1$

## Error conditions
No error condition.

## Attributes

### $\text{strides}$: `list of int`

Attribute $\text{strides}$ determines how the kernel is applied on tensor $X$ during the convolution.

For instance, with $\text{strides}[0]=3$ and $\text{strides}[1]=2$, the kernel is applied to data 3 units down in the first spatial axis and to data 2 units on right in the second spatial axis at each step of the convolution.

The effect of the $\text{strides}$ attribute is illustrated on the following figure. In this example, $\text{strides}=(3,2)$.

<img src="./assets/imgs/conv_stride3.png" width="300" />

#### Constraints
- `[C1]` <a id="C1rattr1f"></a> Value domain
    - Statement: $\text{strides}$ is a list of strictly positive integers.
    - Rationale: Stride values represent the number of applications of the kernel in the two spatial dimensions
- `[C2]` <a id="C2rattr1f"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$
    - Statement: 
        *  $dY_2 > 0$
         
      and
      
        * $dY_3 > 0$
    - Rationale: Convolution is only performed when the output dimensions are positive.

### $\text{auto\_pad}$: `string`

The $\text{auto\_pad}$ attribute determines if and how automatic padding is done for the input tensor $X$.

#### Constraints
- `[C1]` Value domain 
    - Statement: $\text{auto\_pad}$ shall be in set {`NOTSET`, `VALID`, `SAME_UPPER`, `SAME_LOWER`}.
- `[C2]` Explicit padding 
    - Statement: $\text{auto\_pad}$ shall be set to `NOTSET` `[R2]`
    - Rationale: The SONNX profile imposes explicit padding.

### $\text{pads}$: `list of int`

Attribute $\text{pads}$ determines the padding at the beginning and end along each spatial axis of the input tensor $X$.

$\text{pads}$ is a list of the form (`x1_begin`, `x2_begin`,..., `x1_end`, `x2_end`,...), where `xi_begin` is the number of elements (possibly zero) added at the beginning of axis $i$ and `xi_end` is the number of elements added at the end of axis $i$.

The padding value is 0.

The effect of the $\text{pads}$ attribute is illustrated on the following figure. In this example,  $\text{pads}=(2,1,2,2)$.
<img src="./assets/imgs/conv_pad2.png" width="300" />

#### Constraints
 - `[C1]` <a id="C1rattr2f"></a> Value domain
    - Statement: $\text{pads}$ is a list of positive or null integers.
    - Rationale: A padding value gives a *number of elements* to be added to some spatial axis.
 - `[C2]` <a id="C2rattr2f"></a> Consistency between the shape of $X$ and the length of $\text{pads}$
    - Statement: The length of the $\text{pads}$ list is two times the number of spatial axes of $X$
    - Rationale: Padding shall be given for all spatial axes, and a begining value and an end value must be given for each axis.
 - `[C3]` <a id="C3rattr2f"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$  
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1f) on attribute $\text{strides}$.

### $\text{dilations}$: `list of int`

Attribute $\text{dilations}$ specifies the spacing between the kernel elements for each spatial axis of the filter $W$. The ith value in the list gives the dilation factor for spatial axis $i$. If the dilation factor is greater than 1 for axis $i$, then the kernel elements are spaced out by the dilation factor for that axis. 

The value of the elements introduced by the dilation is 0.

The effect of the $\text{dilations}$ attribute for a tensor with two spatial axes is depicted on the following figure. In this example, $\text{dilations}=(2,2)$. 

<img src="./assets/imgs/dilation.png" width="300" />


#### Constraints
 - `[C1]` <a id="C1rattr3f"></a> Value domain
    - Statement: $\text{dilations}$ is a list of strictly positive integers
    - Rationale: The dilation is a *factor of expansion* along a certain axis. 
 - `[C2]` <a id="C2rattr3f"></a> Relation between $\text{dilations}$ and $W$ 
    - Statement: The length of the $\text{dilations}$ list is equal to number of spatial axes of $W$.
    - Rationale: Dilation is defined for all spatial axes of $W$.
 - `[C3]` <a id="C3rattr3f"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$  
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1f) on attribute $\text{strides}$.

### $\text{group}$: `int` 

This attribute specifies the number of groups the input channels and output channels are divided into. When $\text{group}=1$, a standard convolution is performed. When $\text{group}$ is greater than 1, convolution is computed for each group separately with a specific set of filters.

The effect of the $\text{group}$ attribute for a tensor with two spatial axes is depicted on the following figure. In this example $\text{group}=3$.

<img src="./assets/imgs/grouped_convolution.png" width="300" />

(Taken from https://eli.thegreenplace.net/2018/depthwise-separable-convolutions-for-machine-learning)

In the example, with $\text{group}$ set to 3 and an input $\text{X}$ and an output $\text{Y}$ with 3 channels, the input and output channels will be divided into 3 groups of 1 channel.

#### Constraints
 - `[C1]` <a id="C1rattr4f"></a> Value domain
    - Statement: $\text{group}$ is a strictly positive value.
    - Rationale: $\text{group}$ represent a grouping factor. 
 - `[C2]` <a id="C2rattr4f"></a> Consistency between number of channels and groups
    - Statement: 
      - $dX1 \text{ mod}$ $\text{group}= 0$
      - $dY1 \text{ mod}$ $\text{group}= 0$
    - Rationale: Each group shall have the same number of input channels and the shall be the same number of output channels assigned to each group.
- `[C3]` <a id="C3rattr4f"></a> Support for standard and depthwise convolutions
    - Statement: $\text{group}=1$ `[R3]`

### $\text{kernel\_shape}$: `list of int`

This parameter specifies the shape of the convolution kernel $W$.

#### Constraints

- `[C1]` <a id="C1rattr5f"></a> Value domain
    - Statement: $\text{kernel\_shape}$ is a list of strictly positive integers
    - Rationale: A dimension is always positive and cannot be null.
- `[C2]` <a id="C2rattr5f"></a> Consistency between $W$ and $\text{kernel\_shape}$
    - Statement:  The size of $W$ for an axis must be equal to the value of $\text{kernel\_shape}$ for that axis
   - Rationale: $\text{kernel\_shape}$ represents the shape of $W$, where $\text{kernel\_shape}[0] = dW_3$ and $\text{kernel\_shape}[1] = dW_2$.


## Inputs

###  $\text{X}$: `floating-point tensor`

Tensor $X$ is the input tensor on which convolution with kernel $W$ is computed.

The shape of tensor $X$ is $(dX_0 , dX_1 , dX_2 , dX_3)$

Where
- $dX_0$ is the batch size of input $X$.
- $dX_1$ is the number of data channels of input $X$.
- $dX_2$ and $dX_3$ are the sizes of the input for the two spatial axes (height and width).

#### Constraints

- `[C1]` <a id="C1rxf"></a> Number of spatial axes of tensor $X$
    - Statement: The number of spatial axes of tensor $X$ is 2. `R1`
    - Rationale: This restriction is introduced to reduce the specification effort. It matches the industrial use cases considered in the profule.
- `[C2]` <a id="C2rxf"></a> Consistency between the number of channels of $X$ and $W$
    - Statement:  $dW_1=\frac{dX_1}{group}$
- `[C3]` <a id="C3rxf"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1f) on attribute $\text{strides}$.
- `[C4]` <a id="C4rxf"></a> Axis denotations 
    - Statement: If axis denotation is in effect, the operation expects input data tensor to have axis denotation \[`DATA_BATCH`, `DATA_CHANNEL`, `DATA_FEATURE`, `DATA_FEATURE`\].
    - Rationale: Denotation convention

### $\text{W}$: `floating-point tensor`

Tensor $W$ is the convolution kernel.

The shape of tensor $W$ is $(dW_0 , dW_1 , dW_2 , dW_3)$

Where
- $dW_0$ is the number of output channels or number of feature maps
- $dW_1$ is the number of input channels
- $dW_2$ and $dW_3$ are the sizes of the kernel for the two spatial axes.

#### Constraints
- `[C1]` <a id="C1rwf"></a> Consistency between the number of channels of $X$ and $W$
   - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rxf) on tensor $X$.
- `[C2]` <a id="C2rwf"></a> Consistency between the shape of tensors $X$, $W$, $Y$ and  attributes $\text{pads}$, $\text{dilations}$ and $\text{strides}$
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1f) on attribute $\text{strides}$.
- `[C3]` <a id="C3rwf"></a> Consistency between $W$ and $\text{kernel\_shape}$
   - Statement:  The size of $W$ for an axis must be equal to the value of $\text{kernel\_shape}$ for that axis
   - Rationale: $\text{kernel\_shape}$ represents the shape of $W$, where $\text{kernel\_shape}[0] = dW_3$ and $\text{kernel\_shape}[1] = dW_2$.
- `[C4]` <a id="C4rwf"></a> Compliance with axis denotations
    - Statement: If axis denotation is in effect, the operation expects the weight tensor to have axis denotation \[`FILTER_OUT_CHANNEL`, `FILTER_IN_CHANNEL`, `FILTER_SPATIAL`, `FILTER_SPATIAL`\].
- `[C5]` <a id="C5rwf"></a> Consistency between output channels and group
     - Statement: $dW_0 \mod \text{group} == 0$

### $\text{B}$: `floating-point tensor` (optional)

Tensor $B$ is the bias. 

The shape of tensor $B$ is $dB_0$.

#### Constraints
- `[C1]` <a id="C1rbf"></a> Consistency between the number of channels of $B$ and $W$
    - Statement:  $dB_0 = dW_0$.


## Outputs

### $\text{Y}$ : `floating-point tensor`

The size of the output $Y$ will be $(dY_0 , dY_1 , dY_2 , dY_3)$ 

Where
- $dY_0$ is the number of batches
- $dY_1$ is the number of channels
- $dY_2$ and $dY_3$ are the sizes of the output for the two spatial axes

#### Constraints.
- `[C1]` <a id="C1ryf"></a> Consistency between the shape of tensors $X$, $W$, $Y$, attributes $\text{pads}$ and  $\text{strides}$
    - Statement: see constraint [<b><span style="font-family: 'Courier New', monospace">[C2]</span></b>](#C2rattr1f) on attribute $\text{strides}$.

## Formal specification

[See Why3 specification](./formal/conv.mlw).

## Numerical Accuracy
*(To be completed)*