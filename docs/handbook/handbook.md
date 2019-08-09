# Piranha Language Handbook

## Table of Contents
* [1 Introduction](#1)
  * [1.1 Literals](#1.1)
    * [1.1.1 Booleans](#1.1.1)
    * [1.1.2 Integers](#1.1.2)
    * [1.1.3 String](#1.1.3)
    * [1.1.4 Floating-point Values](#1.1.4)
  * [1.2 Nodes](#1.1)
    * [1.2.1 Node Definitions](#1.2.1)
    * [1.2.2 Node Instances](#1.2.2)
    * [1.2.3 Default Inputs](#1.2.3)
    * [1.2.4 Node Visibility](#1.2.4)
    * [1.2.5 Immediate Node Instances](#1.2.5)
    * [1.2.6 Nested Nodes](#1.2.6)
<br/>

# <a name="1"/>1 Introduction

Piranha is a configurable programming language that allows for easy object-oriented binding to C++ classes. It was designed primarily for use in [MantaRay](https://github.com/ange-yaghi/manta-ray) as a high speed SDL (scene description langauge). Is is better therefore to think of Piranha as a specification langauge, although it has the ability to perform sequential operations as well.

## <a name="1.1"/>1.1 Literals

Piranha defines a few fundamental literal types which are automatically parsed. It should be noted that these literals don't have any intrinsic meaning to the compiler and must be given definitions in order to "do" anything. This will be discussed later in this manual.

### <a name="1.1.1"/>1.1.1 Booleans

Boolean values are specified just as they would be in C/C++ using `true` or `false`.

<pre>
<b>node</b> example {
  <b>output</b> boolean: <b>true</b>;
}
</pre>

### <a name="1.1.2"/>1.1.2 Integers

Integer values can be specified in either hexadecimal or decimal form. 

<pre>
<b>node</b> example {
  <b>output</b> int0: 0x01;
  <b>output</b> int1: 1;
  <b>output</b> int2: 01;
}
</pre>

### <a name="1.1.3"/>1.1.3 Strings

String values are written as textual information enclosed in double quotes. Special characters like the newline character '\n' are written as they would be in C. Strings are also automatically concatenated if placed side by side (as in C).

<pre>
<b>node</b> example {
  <b>output</b> string: "String 1" "String 2";
}
</pre>

### <a name="1.1.4"/>1.1.4 Floating-point Values

Floating point values are numbers with a decimal. If the decimal is not present the compiler will treat the number as an integer literal.

<pre>
<b>node</b> example {
  <b>output</b> int: 50;      // Interpreted as an integer
  <b>output</b> float: 50.0;  // Interpreted as a floating point value
}
</pre>

## <a name="1.2"/>1.2 Nodes

### <a name="1.2.1"/>1.2.1 Node Definitions

The fundamental building block of any Piranha program is the **node**. A node is a structure which may have inputs, outputs and nested nodes. 

The syntax of a node *template* (hereby called a node definition) generally follows this pattern:

<pre>
<b>node</b> node_name {
  // Inputs/outputs
  // ...
  
  // Nested nodes
  // ...
}
</pre>

A hypothetical example is given below in the form of a simple adder:

<pre>
<b>node</b> adder {
  <b>input</b> left;
  <b>input</b> right;
  <b>output</b> out: left + right;
}
</pre>

Intuitively, this node could be thought of as a virtual "circuit" that accepts two arbitrary input channels and then adds them.

### <a name="1.2.2"/>1.2.2 Node Instances

A node definition by itself is not particularly useful. Analogous to a C++ class, the node definition must be *instantiated*. Piranha uses a syntax very similar to C++ for instantiating nodes.

To instantiate the adder defined above, a possible approach would be:

<pre>
<b>adder</b> adder_name (
  left: 5.0,
  right: 20.0
)
</pre>

A node instantiated in this way is a **full** instantiation because it has all the following parts:
* All input names (`left`, `right`, etc)
* A node name (`adder_name`)

Having all these parts is not necessary when instantiating a node. Piranha can infer the input name based on the position of the argument in the argument list. If desired some parameters can be specified using positional notation and others can be specified with the full name. 

In addition to not requiring all inputs be labelled, a node may be instantiated without a name. This is called an **unnamed** node instance. For example, the following is valid:

<pre>
<b>adder</b>(5.0, 20.0)
</pre>

as is the following:

<pre>
<b>adder</b>(left: 5.0, 20.0)
</pre>

**Warning**: Mixing positional and named notation in this way can become unwieldy if used haphazardly. For instance, the following will give a syntax error when compiled:

<pre>
<b>adder</b>(5.0, left: 20.0)
</pre>

This is because the `left` input parameter was set multiple times, once through positional notation and once directly by name.

### <a name="1.2.3"/>1.2.3 Default Inputs

Defining default values for inputs is done through the following syntax:

<pre>
<b>node</b> adder {
  <b>input</b> left: 5.0;
  <b>input</b> right;
  <b>output</b> out: left + right;
}
</pre>

With the code above, if the `left` parameter is not specified, it will take on the value of 5.0. Piranha also allows for default values to be equal to the value of an input. For example, the following is valid:

<pre>
<b>node</b> adder {
  <b>input</b> left: right;
  <b>input</b> right;
  <b>output</b> out: left + right;
}
</pre>

### <a name="1.2.4"/>1.2.4 Node Visibility

Access to a particular node definition can be limited to a single file using node visibility keywords. Valid visibility keywords are `public` and `private`. Nodes are `public` by default. For example:

<pre>
<b>private</b> <b>node</b> adder {
  <b>input</b> left: right;
  <b>input</b> right;
  <b>output</b> out: left + right;
}
</pre>

Since this node is declared as `private` it will only be visible within the file in which it is defined. 

### <a name="1.2.5"/>1.2.5 Immediate Node Instances

In some cases it can be useful to instantiate a node immediately after declaring it. Rather than write out a full instantiation, one can use the following shorthand:

<pre>
<b>node</b> adder {
  <b>input</b> left;
  <b>input</b> right;
  <b>output</b> out: left + right;
} (5.0, 5.0) -> <b>auto</b>
</pre>

The `auto` keyword will make the new instance have the same name as the node definition it is instantiating. For a node with no inputs, the first part `( ... )` can be omitted. The `node` keyword can also be replaced by an arbitrary node name. For instance, the following is valid:

<pre>
<b>node</b> adder {
  <b>input</b> left: 0.0;
  <b>input</b> right: 0.0;
  <b>output</b> out: left + right;
} -> zero_adder
</pre>

### <a name="1.2.6"/>1.2.6 Nested Nodes

Nodes can be instantiated inside of other nodes. This can be thought of as aggregating nodes into larger nodes with more complex outputs. For instance, take the following hypothetical node definition:

<pre>
<b>node</b> multiply_add {
 <b>input</b> a;
 <b>input</b> b;
 <b>input</b> c;
 <b>output</b> out: a * b + c;

<b>node</b> calculate {
  <b>input</b> a;
  <b>input</b> b;
  <b>input</b> c;
  <b>input</b> scale;
  <b>output</b> out: madd.out * scale;
  
  <b>multiply_add</b> madd(
   a: a,
   b: b,
   c: c
  )
}
</pre>

Using the `.` operator, outputs from a node can be referenced by name. In this case the output of the `calculate` node is the `out` output from the `multiply_add` node nested inside the `calculate` node multiplied by the `scale` input.

Similarly it's also possible to instantiate nodes in an inline fashion like so:

<pre>
<b>node</b> calculate {
  <b>input</b> a;
  <b>input</b> b;
  <b>input</b> c;
  <b>input</b> scale;
  <b>output</b> out: <b>multiply_add</b>(a, b, c).out * scale;
}
</pre>

**UNDER CONSTRUCTION**
