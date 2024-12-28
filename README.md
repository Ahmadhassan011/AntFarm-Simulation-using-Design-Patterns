```markdown
# AntFarm Simulation

## Description
The AntFarm Simulation is a console-based program that simulates the life and battles of ant colonies. Users can create different ant farms, manage resources, and observe the interactions between competing colonies. With a focus on strategy, players can assign warriors, manage food supply, and engage in battles until only one colony remains.

## Live Demo or Installation Instructions
To run the AntFarm Simulation, follow these steps:

1. Copy the code into a file named `main.cpp`.
2. Compile the code using a C++ compiler, e.g.:
   ```bash
   g++ -o AntFarmSimulation main.cpp
   ```
3. Execute the compiled program:
   ```bash
   ./AntFarmSimulation
   ```
4. Follow the on-screen instructions to interact with the simulation.

## Features
- **Colonies Management**: Create and manage multiple ant colonies.
- **Battle System**: Engage in battles between ant colonies utilizing warrior ants.
- **Resource Management**: Provide food, worker ants, or warrior ants to your colonies.
- **Survival Mechanics**: Stay alive by managing the food supply and defending against rivals.
- **Battle Tracking**: Keep track of ant kills, colony kills, and survival ticks.
- **Single Instance Management**: Utilize a singleton pattern for managing the meadow where colonies exist.

## Usage
After starting the simulation, you can use the following commands:
- `spawn`: Create a new ant colony by specifying its coordinates and species.
- `give`: Provide resources (food, worker, warrior) to an existing colony based on its ID.
- `tick`: Advance the simulation by a specified number of ticks (default is 1).
- `summary`: Display the current status of a specified colony using its ID.
- `exit`: Exit the simulation.
- `help`: Display a list of available commands.

### Example of Commands
```plaintext
> spawn
Enter X coordinate: 5
Enter Y coordinate: 10
Enter species name: FireAnts

> give
Enter colony ID: 0
Enter resource type (food/worker/warrior): food
Enter amount: 50

> tick
Enter number of ticks to advance (default 1): 3

> summary
Enter colony ID: 0
```

Feel free to explore and strategize in the vibrant world of the AntFarm Simulation!
```