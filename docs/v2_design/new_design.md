# Piranha V2 Design

## Types

Name | WireDefinition
---- | --------------
TypeId | String
AdaptorDefinition | NodeDefinition \| NULL

---

Name | Wire
---- | ------------
Definition | WireDefinition
Adaptor | Node \| NULL

---

Name | Conduit
---- | -------
Node | Node \| NULL
Wire | Wire \| NULL

---

Name | Port
---- | ----
Type | INPUT \| OUTPUT
Default | Conduit

---

Name | NodeDefinition
---- | --------------
InputPorts | Port[]
OutputPorts | Port[]
AliasPort | Port \| NULL
Nodes | NodeInstance[]

---

Name | Context
---- | -------
ConnectionMap | Port -> (Conduit \| NULL \| UNKNOWN)
Node | Node | NULL

---

Name | Node
---- | ------------
Context | Context
NodeDefinition | NodeDefinition

## Stages

1. Parsing
2. Name Resolution
3. Operator Expansion
4. Access Resolution
5. Type Resolution
6. Optimization
