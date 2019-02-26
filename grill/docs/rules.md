# Game rules

|                       | Player B stays silent | Player B betrays      |
|-----------------------|-----------------------|-----------------------|
| **Player A stays silent** | A: 2 years B: 2 years | A: 6 years B: 1 year  |
| **Player A betrays**      | A: 1 year B: 6 years  | A: 4 years B: 4 years |


# Protocol

## Connection

- S: waits for 2 connections
- C(s): join through join-request
- S: notify Cs about game start
- For rounds:
    - S: waits for 2 decisions
    - C(s): Send decision
    - S: notify clients about result