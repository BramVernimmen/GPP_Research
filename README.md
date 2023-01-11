# Traffic Simulation
## Description
I will be trying to simulate traffic using a Direcitonal Graph and a Behavior Tree.

The code is made using a c++ framework made by our teachers. 

All PathFinding is done using A*.

### Result
Image of result coming soon


## Implementation
### Step 1 - Creating the grid

We want to start off with creating a grid where our cars can travel on.

For this first simulation a grid is good enough, we just want to achieve basic movement/decision making.
Using a grid will make our city look like New York, so why not.

When launching the program right now we will be getting this screen.
![Startup_Grid_Image](/Assets/Images/Gifs/Step1_DefaultGrid.gif)

This however is code that was given to us by our teachers.

We have a Start and End point that we can move and a path that will be calculated using A*.
The code for A* is something that we wrote ourselves during class.
It's nice that we have a grid ready to use, but there are some things missing.

The grid makes a graph, but is by default an Undirected Graph.
When working with streets, we don't want this.


If we take a look at the types of streets there are, we can deduce 2 different sorts.

1: a One-Way street
  - This is easy, just one lane and cars cannot go the opposite direction.
  
2: a Two-Way street
  - This one has 2 lanes (not visible everytime), going in the opposite direction.
  - Legally, on a Two-Way street, it's not allowed to just turn around and go to the other direction. (An exception could be an intersection.)

So a Directed Graph suits this program a lot better.
We will have a clear view of each street and Two-Way streets will be bigger.

Let's get started on creating this grid!


But before that; what should the connection cost be of the Graph?

I decided to go with a value that could be used in km/h. This might change the speed of our car later on.

After making connections and adding building to our grid, we have this problem.
![First_Grid_Problem_Image](/Assets/Images/Gifs/Step1_FirstGridProblem.gif)

At first sight, this looks good. But when looking closer, the path always uses a U turn.

I would prefer it when the cars take a path that doesn't use any U turns.

We can easily fix this; we need to change the costs of the Intersection Connections.

Doing this will massively increase the cost of every Intersection taken, so our path will have to go around.

Obviously we don't want to keep this cost change, so we only increase it before calculating the new path, and instantly decrease it after calculating it.


Doing so will result in this! It looks a lot cleaner now.

![First_Grid_Problem_Image](/Assets/Images/Gifs/Step1_FirstGrid.gif)

At times our path will contain a U turn, but this only happens when the path is really small.

### Step 2 - Adding a car


