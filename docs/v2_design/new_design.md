### Piranha V2 Design

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
