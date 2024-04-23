# FSA to Regular Expression translator
An example of input/output\ 
Input:
```
type=[non-deterministic]
states=[q0,q1] 
alphabet=[0,1]
initial=[q0]
accepting=[q1]
transitions=[q0>1>q0,q0>0>q1]
```
Output:
```
(((1|eps)(1|eps)*(0)|(0))(({})(1|eps)*(0)|(eps))*(({})(1|eps)*(0)|(eps))|((1|eps)(1|eps)*(0)|(0)))
```
