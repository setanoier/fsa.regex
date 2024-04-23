## Task
Given an FSA description in the *input.txt* (see input file format) your program should output to console an error description (see validation errors) OR a regular expression that corresponds to the given FSA. Assume that deterministic and non-deterministic FSAs may be inserted. However, epsilon-moves are guaranteed NOT to be used as inputs for any FSA types. The regular expression should be built according to a slightly modified version of the **Kleene's algorithm**.

## Input
| Input                      | Description                                                  |
| -------------------------- | ------------------------------------------------------------ |
| type=[t]                   | t $\in$ {deterministic, non-deterministic}                   |
| states=[s1, s2, ...]       | s1, s2 $\in$ latin letters, words and numbers                |
| alphabet=[a1, a2, ...]     | a1, a2 $\in$ latin letters, words, numbers and character `_` |
| initial=[s]                | s $\in$ states                                               |
| accepting=[s1, s2, ...]    | s1, s2 $\in$ states                                          |
| transitions=[s1>a>s2, ...] | s1, s2 $\in$ states; a $\in$ alphabet                        |
