### Piranha V2 Design

Name | WireDefinition
---- | --------------
TypeId | String
AdaptorDefinition | NodeDefinition \| NULL

---

Name | WireInstance
---- | ------------
Definition | WireDefinition
Adaptor | NodeInstance \| NULL

---

Name | Conduit
---- | -------
NodeInstance | NodeInstance \| NULL
WireInstance | WireInstance \| NULL

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
NodeInstance | NodeInstance | NULL

---

Name | NodeInstance
---- | ------------
Context | Context
NodeDefinition | NodeDefinition
```
