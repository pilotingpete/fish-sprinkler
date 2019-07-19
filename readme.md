# Ryan's Water Sculpture Controller

## About

This is code for an arduino to control an art installation project for Ryan.

## State Machine Diagram

```plantuml

(*) --> "Water Fill"
"Water Fill" --> "Idle"

if "Motion Detected" then
  ->[yes] "Activate Air Solenoid"
  -> "Activate Water Solenoid"
  -> "Timeout"
  -> "Water Fill"
  
else
  ->[no] "Idle"
endif

```