# Piranha Language Handbook

## Table of Contents
* [1 Introduction](#1)
  * [1.1 Literals](#1.1)
    * [1.1.1 Booleans](#1.1.1)
    * [1.1.2 Integers](#1.1.2)
    * [1.1.3 String](#1.1.3)
    * [1.1.4 Floating-point Values](#1.1.4)
  * [1.2 Nodes](#1.2)
    * [1.2.1 Node Definitions](#1.2.1)
    * [1.2.2 Node Instances](#1.2.2)
    * [1.2.3 Default Inputs](#1.2.3)
    * [1.2.4 Node Visibility](#1.2.4)
    * [1.2.5 Immediate Node Instances](#1.2.5)
    * [1.2.6 Nested Nodes](#1.2.6)
<br/>

# <a name="1"></a>1 Introduction

Piranha is a configurable programming language that allows for easy object-oriented binding to C++ classes. It was designed primarily for use in [MantaRay](https://github.com/ange-yaghi/manta-ray) as a high speed SDL (scene description langauge). Is is better therefore to think of Piranha as a specification langauge, although it has the ability to perform sequential operations as well.

## <a name="1.1"></a>1.1 Literals

Piranha defines a few fundamental literal types which are automatically parsed. It should be noted that these literals don't have any intrinsic meaning to the compiler and must be given definitions in order to "do" anything. This will be discussed later in this manual.

### <a name="1.1.1"></a>1.1.1 Booleans

Boolean values are specified just as they would be in C/C++ using `true` or `false`.

<pre>
<b>node</b> example {
  <b>output</b> boolean: <b>true</b>;
}
</pre>

### <a name="1.1.2"></a>1.1.2 Integers

Integer values can be specified in either hexadecimal or decimal form. 

<pre>
<b>node</b> example {
  <b>output</b> int0: 0x01;
  <b>output</b> int1: 1;
  <b>output</b> int2: 01;
}
</pre>

### <a name="1.1.3"></a>1.1.3 Strings

String values are written as textual information enclosed in double quotes. Special characters like the newline character '\n' are written as they would be in C. Strings are also automatically concatenated if placed side by side (as in C).

<pre>
<b>node</b> example {
  <b>output</b> string: "String 1" "String 2";
}
</pre>

### <a name="1.1.4"></a>1.1.4 Floating-point Values

Floating point values are numbers with a decimal. If the decimal is not present the compiler will treat the number as an integer literal.

<pre>
<b>node</b> example {
  <b>output</b> int: 50;      // Interpreted as an integer
  <b>output</b> float: 50.0;  // Interpreted as a floating point value
}
</pre>

## <a name="1.2"></a>1.2 Nodes

### <a name="1.2.1"></a>1.2.1 Node Definitions

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

### <a name="1.2.2"></a>1.2.2 Node Instances

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

### <a name="1.2.3"></a>1.2.3 Default Inputs

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

### <a name="1.2.4"></a>1.2.4 Node Visibility

Access to a particular node definition can be limited to a single file using node visibility keywords. Valid visibility keywords are `public` and `private`. Nodes are `public` by default. For example:

<pre>
<b>private</b> <b>node</b> adder {
  <b>input</b> left: right;
  <b>input</b> right;
  <b>output</b> out: left + right;
}
</pre>

Since this node is declared as `private` it will only be visible within the file in which it is defined. 

### <a name="1.2.5"></a>1.2.5 Immediate Node Instances

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

### <a name="1.2.6"></a>1.2.6 Nested Nodes

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

### <a name="1.2.7"></a>1.2.7 Toggle Inputs

Sometimes you may want to disable a node. This is done using a *toggle* port. The following node has a toggle port attached:

<pre>
<b>node</b> calculate {
  <b>input</b> a;
  <b>input</b> b;
  <b>toggle</b> enable: <b>true</b>;
  <b>output</b> c: a + b;
}
</pre>

Inputs to toggle ports must evaluate to boolean values. If the toggle input for a node evaluates to false, the node is disabled. Furthermore, any node that is dependent upon any output of the disabled node is also disabled.

### <a name="1.2.8"></a>1.2.8 Output Aliasing

It is often convenient to not have to explicitely specify an output by name, especially when a node only has one output. To simplify syntax, *output aliasing* can be used. For example:

<pre>
<b>node</b> calculate {
  <b>input</b> a;
  <b>input</b> b;
  <b>alias</b> <b>output</b> c: a + b;
}

<b>calculate</b>(
  a: <b>calculate</b>(1, 2),
  b: <b>calculate</b>(3, 4)
)
</pre>

In the example above, the calculate nodes aliases the output called `c`. Nodes with an alias output must have only one output.

### <a name="1.2.9"></a>1.2.9 Input Type Enforcement

If desired, the inputs to a node can be constrained to a particular type. For example:

<pre>
<b>node</b> calculate {
  <b>input</b> a;
  <b>input</b> b;
  <b>output</b> c: a + b;
}

<b>node</b> post_process {
  <b>input</b> a [<b>calculate</b>];
  <b>input</b> b [<b>calculate</b>];
  <b>output</b> c: a.c + b.c;
}
</pre>

In this exampe, the `post_process` node requires that both inputs be references to a `calculate` instance. This will result in the following behavior:

<pre>
<b>node</b> deception { <b>output</b> c: 10; } 

// Will fail because the input types don't match
<b>post_process</b>(5, 10)

// Will fail even though the "deception" type has the required output "c"
<b>post_process</b>(
  <b>deception</b>(), 
  <b>deception</b>()
)

// Okay
<b>post_process</b> (
  a: <b>calculate</b>(1, 2),
  b: <b>calculate</b>(2, 3)
)
</pre>

### <a name="1.2.9"></a>1.2.9 Binding to Native Code

Through the following notation, a Piranha node definition can be made to point to an underlying C++ type. This type of node definition is called a *builtin type*. When binding to a native type, every input and output must have type enforcement tags present. See [1.2.9](#1.2.9) for details. The following is a hypothetical node definition that aliases a C++ class:

<pre>
<b>node</b> adder => cpp_adder_class {
  <b>input</b> a [<b>float</b>]: 0.0;
  <b>input</b> b [<b>float</b>]: 0.0;
  <b>output</b> c [<b>float</b>];
}
</pre>

The C++ class is referenced through a label `cpp_adder_class`. This reference is established when the language rules are generated (see TODO reference). As can be seen default inputs can be specified for builtin types as well so long as they match the enforced input type.

## <a name="1.3"></a>1.3 Files

### <a name="1.3.1"></a>1.3.1 Importing Files
A file in Piranha is called a *compilation unit*. You can't access node definitions in another compilation unit unless you explicitely import that file. This is done with the following notation:

<pre>
<b>import</b> "filename.pr"
</pre>

All node definitions within this file that are public will now be visible.

### <a name="1.3.2"></a>1.3.2 Import Visibility

Imports are private by default. This means that if file *A* imports file *B*, then by default if another file *C* imports *A*, then the definitions inside *B* will not be available to *C*.

To ensure that node definitions imported from another file are visible to outside files, the import statement must be preceded by the `public` keyword as follows:

`file_a.pr`:
<pre>
<b>public</b> <b>import</b> "file_b.pr"
</pre>

`file_b.pr`:
<pre>
<b>public</b> <b>node</b> pub { ... }
<b>private</b> <b>node</b> priv { ... }
</pre>

`file_c.pr`:
<pre>
// Okay
<b>pub</b>( ... )

// Error: Unresolved node
<b>priv</b>( ... )
</pre>

### <a name="1.3.3"></a>1.3.3 Name Safety

To distinguish between two node definitions with the same name but from different libraries, Piranha provides a feature called *import naming*. To name a file that is being imported, the following syntax is used:

<pre>
<b>import</b> "some_library.pr" <b>as</b> lib
</pre>

To instantiate a node type that is defined in "some_lib.pr" one could then write:

<pre>
lib::<b>some_type</b>( ... )
</pre>

If the type being accessed is within a public import inside `some_lib.pr` then it is still accessed through the library name `lib`. At this time Piranha doesn't support nested namespaces in the same way that C++ does.

## <a name="1.4"></a>1.4 Execution Model

### <a name="1.4.1"></a>1.4.1 Introduction

Piranha differs from other programming languages considerably when it comes to its execution model; that is, the order and manner in which statements are executed. Consider the following simple program:

<pre>
<b>print_to_console</b>(result)
<b>add</b> result(10, 30)
</pre>

For those familiar with most sequential programming languages, this program would appear to be written backwards. In fact, it would also be logical to assume that such a program would result in a syntax error since the node *result* is used before it's even instantiated. 

In Piranha, however, such a program is completely valid. All of the dependencies of a node are *evaluated* before the node itself. So even though the add operation is instantiated after the print node textually, it is evaluated first.

The default execution direction is *down*. For example, consider this program:

<pre>
<b>print_to_console</b>("1")
<b>print_to_console</b>("2")
<b>print_to_console</b>("3")
</pre>

This program will print the numbers in the order printed and Piranha will *guarantee* this order since these nodes don't have any dependencies. 

### <a name="1.4.2"></a>1.4.2 Node Evaluation

When a node "runs" it is called *node evaluation*. The Piranha interpreter will ensure that any node will be evaluated one time only. For example:

<pre>
<b>print_to_console</b>(result)
<b>print_to_console</b>(result + 1)
<b>add</b> result(2, 7)
</pre>

In this example, the node `result` is referenced twice in two `print_to_console` nodes. Despite this it is only evaluated once when the first print node is instantiated.

All of a node's dependencies will be evaluated before it is evaluated. For instance, in the example below the node `a` is executed first followed by `b` and then `c`.

<pre>
<b>add</b> c(a, b)
<b>add</b> b(1, 2)
<b>add</b> a(3, 4)
</pre>

### <a name="1.4.3"></a>1.4.3 Composite Node Evaluation

In previous examples, all nodes were builtin types and so did not have any nested nodes. Evaluation of composite nodes follows the same execution model as described previously with the additional stipulation that inputs are evaluated first and outputs are evaluated last. For example:

<pre>
<b>node</b> example {
  <b>input</b> a: <b>add</b>(1, 2);
  <b>input</b> b;
  <b>output</b> result;

  <b>print_to_console</b>(result)
  <b>add</b> result(a, b)
}

<b>example</b>(b: 5)

</pre>

One could equivalently express the above code in *execution order* as:

<pre>
<b>add</b> node0(1, 2)
<b>add</b> result(node0, 5)
<b>print_to_console</b>(result)
</pre>

The example below further demonstrates how composite node dependencies are treated:

<pre>
<b>node</b> get_value {
  <b>alias</b> <b>output</b> out: 5;

  <b>print_to_console</b>("Returning 5")
}

<b>node</b> example {
  <b>input</b> a;
  <b>input</b> b;
  <b>output</b> result;

  <b>print_to_console</b>("The result is: ")
  <b>print_to_console</b>(result)
  <b>add</b> result(a, b)
}

<b>example</b>(value, 10)
<b>get_value</b> value()

</pre>

The above program outputs:

<pre>
Returning 5
The result is:
15
</pre>
